[CmdletBinding()]
param([switch]$Apply)

$ErrorActionPreference = 'Stop'

if ($Apply) {
    $setupIdentity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $setupPrincipal = [Security.Principal.WindowsPrincipal]::new($setupIdentity)
    if (-not $setupPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        throw 'Open PowerShell as administrator, then run this script with -Apply.'
    }
}

$setupNames = @('Path', 'INCLUDE', 'LIB', 'LIBPATH')
$setupPrevious = [ordered]@{}
foreach ($setupName in $setupNames) {
    $setupPrevious[$setupName] = [Environment]::GetEnvironmentVariable($setupName, 'Machine')
}

foreach ($setupName in @('INCLUDE', 'LIB', 'LIBPATH')) {
    if ([Environment]::GetEnvironmentVariable($setupName, 'User')) {
        throw "User-level $setupName overrides the system value. Review that existing variable before applying system defaults."
    }
    # Do not copy temporary or user-specific search directories into system defaults.
    [Environment]::SetEnvironmentVariable($setupName, $setupPrevious[$setupName], 'Process')
}

$setupVswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path -LiteralPath $setupVswhere)) { throw 'vswhere.exe was not found.' }
$setupInstall = & $setupVswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $setupInstall) { throw 'An installed MSVC C++ toolchain was not found.' }

& (Join-Path $setupInstall 'Common7\Tools\Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation | Out-Null
if ($env:VSCMD_ARG_TGT_ARCH -ne 'x64' -or -not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir) {
    throw 'The x64 MSVC developer environment was not initialized.'
}

$setupCompilerBin = [IO.Path]::GetFullPath((Join-Path $env:VCToolsInstallDir 'bin\Hostx64\x64'))
$setupSdkBin = [IO.Path]::GetFullPath((Join-Path $env:WindowsSdkDir ("bin\{0}\x64" -f $env:WindowsSDKVersion.TrimEnd('\'))))
$setupNinjaBin = Join-Path $setupInstall 'Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja'
foreach ($setupExecutable in @(
    (Join-Path $setupCompilerBin 'cl.exe'),
    (Join-Path $setupCompilerBin 'link.exe'),
    (Join-Path $setupSdkBin 'rc.exe'),
    (Join-Path $setupSdkBin 'mt.exe'),
    (Join-Path $setupNinjaBin 'ninja.exe')
)) {
    if (-not (Test-Path -LiteralPath $setupExecutable -PathType Leaf)) {
        throw "Required tool was not found: $setupExecutable"
    }
}

$setupPathAdditions = @($setupCompilerBin, $setupSdkBin, $setupNinjaBin)
$setupNewPath = [Collections.Generic.List[string]]::new()
foreach ($setupEntry in ($setupPathAdditions + @($setupPrevious['Path'] -split ';'))) {
    if ($setupEntry -and $setupNewPath -notcontains $setupEntry) { $setupNewPath.Add($setupEntry) }
}
$setupValues = [ordered]@{
    Path = $setupNewPath -join ';'
    INCLUDE = $env:INCLUDE
    LIB = $env:LIB
    LIBPATH = $env:LIBPATH
}
foreach ($setupName in @('INCLUDE', 'LIB', 'LIBPATH')) {
    if ([string]::IsNullOrWhiteSpace($setupValues[$setupName])) {
        throw "The developer environment returned an empty $setupName."
    }
}

if (-not $Apply) {
    [pscustomobject]@{
        Scope = 'Machine'
        Compiler = Join-Path $setupCompilerBin 'cl.exe'
        SdkVersion = $env:WindowsSDKVersion.TrimEnd('\')
        PathAdditions = $setupPathAdditions
        Values = $setupValues
    }
    return
}

$setupBackupDirectory = Join-Path $PSScriptRoot '..\build\environment-backups'
New-Item -ItemType Directory -Path $setupBackupDirectory -Force | Out-Null
$setupBackupFile = Join-Path $setupBackupDirectory ("msvc-system-{0}.json" -f (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[pscustomobject]@{ Scope = 'Machine'; Variables = $setupPrevious } |
    ConvertTo-Json -Depth 3 | Set-Content -LiteralPath $setupBackupFile -Encoding UTF8

try {
    foreach ($setupName in $setupNames) {
        [Environment]::SetEnvironmentVariable($setupName, $setupValues[$setupName], 'Machine')
    }
    foreach ($setupName in $setupNames) {
        if ([Environment]::GetEnvironmentVariable($setupName, 'Machine') -ne $setupValues[$setupName]) {
            throw "System variable verification failed: $setupName"
        }
    }
} catch {
    $setupFailure = $_
    foreach ($setupName in $setupNames) {
        try { [Environment]::SetEnvironmentVariable($setupName, $setupPrevious[$setupName], 'Machine') }
        catch { Write-Warning "Could not restore $setupName. Backup: $setupBackupFile" }
    }
    throw $setupFailure
}

Write-Host 'Saved x64 MSVC and Ninja defaults to Windows system environment variables.'
Write-Host "Backup: $([IO.Path]::GetFullPath($setupBackupFile))"
Write-Host 'Completely close terminals and their host apps, then open them again from the Start menu.'
