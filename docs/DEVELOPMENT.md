# 编辑器无关的开发环境

用户偏好 VS Code 或 Vim/Neovim，并希望方便切换 Windows/Linux。教学主线采用 C++20、CMake 3.25+、Ninja 和命令行。源码、构建描述与编辑器设置分开管理。

## 各工具负责什么

| 工具层 | 作用 | 本项目选择 |
| --- | --- | --- |
| 编辑器 | 编辑文件、查看诊断、发起调试 | VS Code 或 Vim/Neovim，由用户选择 |
| 语言服务 | 补全、跳转、静态诊断 | 可选 clangd，读取实际编译配置 |
| 构建描述与生成 | 描述 target、依赖和编译条件 | CMake |
| 构建执行 | 根据依赖判断哪些文件需要重新编译 | Ninja |
| 编译与链接工具链 | 将源代码生成目标平台程序 | Windows 当前使用 MSVC；Linux 使用 GCC/Clang；macOS 使用 Apple Clang |

MSVC 编译器可以从终端调用，Visual Studio IDE 不参与课程操作。切到 Linux 后使用相应编译器和系统 SDK，公共 C++ 与 CMake 代码继续复用。跨平台需要各平台实际重新构建和验证，不表示同一个可执行文件到处运行。

## 当前 Linux 环境（2026-11-14 实测）

用户已切换到 Ubuntu 24.04.5 LTS / x86_64，当前 shell 为 `/usr/bin/zsh`。下列构建命令也适用于 Bash。本课所需工具已可用，无需安装或修改环境变量。

| 项目 | 本机结果 |
| --- | --- |
| CMake | 4.4.2，`/usr/local/bin/cmake` |
| Ninja | 1.11.1，`/usr/bin/ninja` |
| 当前编译器 | GCC 13.3.0，CMake 选择 `/usr/bin/c++` |
| 备选编译器 / 语言服务 | Clang 18.1.3 / clangd 18.1.3 |
| 调试器 / 版本控制 | GDB 15.1 / Git 2.43.0 |
| 编辑器入口 | 已发现 `code` 与 `nvim`；未检查编辑器插件配置 |
| CPU / 内存 | Intel Core i7-1185G7，4 核 8 线程；系统报告内存 15 GiB |

已从新构建目录运行 `cmake --preset linux-debug`、`cmake --build --preset linux-debug` 和 `./build/linux-debug/bin/vle_sandbox`：参考工程编译、链接、三行输出均成功，退出码为 0，构建输出未见 warning/error。

用户练习采用独立目录：

```sh
cmake -S practice -B build/lesson00-linux-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/lesson00-linux-debug
```

练习已成功配置并生成 Ninja 规则与编译数据库。初次构建的头文件路径问题已由用户修正，`main.cpp` 也已补齐；最新 review 在新目录 `build/lesson00-linux-review` 中编译链接成功，但 `engine_name()` 缺少返回值，GCC 报告 `-Wreturn-type` 警告，仍待用户修正后验收输出。当前 Linux 的编译器 ABI 探测成功，Windows 旧问题的原因仍未定位。

后续 Vulkan 环境的只读检查：`vulkaninfo --summary` 成功；loader 报告 Instance Version 1.4.313，系统头文件版本为 1.4.313，Khronos validation layer 可枚举，`glslc --version` 可执行并报告 shaderc v2023.8。硬件设备为 Intel Iris Xe Graphics（TGL GT2），设备 API 1.4.318，Mesa 25.2.8；另有 CPU 软件设备 llvmpipe。`VULKAN_SDK` 当前为空，不影响上述系统工具运行。未测试本项目 Vulkan 初始化、绘制、mesh shader 或光追，不将枚举结果当作功能验证，也不将这些独立组件版本合称为一个已验证的 SDK 版本。

当前先使用 GCC，不为 L00 额外安排 Clang 构建矩阵。切换编译器时使用独立构建目录；Windows 缓存与二进制不复制到 Linux 复用。

## Windows 配置记录（切回时参考）

用户已明确选择 Windows 系统级持久环境配置：普通 Git Bash、PowerShell 和 cmd 共同继承，不为每种 shell 单独配置启动脚本，也无需每次手动初始化。此处以已安装的 x64 MSVC 和 Windows SDK 为默认工具链。

当前核实的目录：

| 用途 | 目录 |
| --- | --- |
| cl.exe、link.exe | `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64` |
| rc.exe、mt.exe | `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64` |
| Ninja | `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja` |

MSVC 构建需要 PATH、INCLUDE 和 LIB 等配套环境。[微软说明](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#path-and-environment-variables-for-command-line-builds) 按用户的持久配置偏好，准备了 [一次性配置脚本](../tools/configure-msvc-system.ps1)：它运行官方开发环境初始化脚本获取本机实际值，然后仅保存以下四个系统变量。

| 系统变量 | 内容 |
| --- | --- |
| Path | 前置编译器、Windows SDK 工具和 Ninja 三个目录，保留其他系统 Path 条目；不复制当前进程或用户的完整 Path |
| INCLUDE | MSVC、C++ 标准库和 Windows SDK 头文件搜索目录 |
| LIB | x64 MSVC 与 Windows SDK 链接库搜索目录 |
| LIBPATH | MSVC 开发环境提供的元数据搜索目录 |

这会将普通终端的默认工具链固定为当前 x64 版本。升级 MSVC 或 Windows SDK 后重新运行脚本以更新实际目录；切换其他目标架构时再使用对应的开发环境。项目源码与 CMake 中仍不硬编码本机路径。

### 一次性安装

从开始菜单以“管理员身份运行”打开 PowerShell，执行：

```powershell
powershell.exe -NoProfile -File "D:\Code-dx12\tools\configure-msvc-system.ps1" -Apply
```

`-Apply` 将配置写入 Windows 的 Machine 系统环境变量，对新启动的两种终端均生效。脚本会先将旧值备份至项目的 `build/environment-backups/`，写入后读回核对；遇到写入错误会尝试恢复旧值。它不会修改 Bash 或 PowerShell 的启动配置。

运行成功后，完全退出 PowerShell、Git Bash，以及承载终端的 Windows Terminal 或 VS Code，再从开始菜单重新打开。已运行的父进程可能继续保存旧环境；如仍继承旧值，注销 Windows 后重新登录。[持久环境变量及变更通知](https://learn.microsoft.com/en-us/dotnet/api/system.environment.setenvironmentvariable?view=netframework-4.8.1#remarks)

只查看配置时，省略 `-Apply`；预览无需管理员权限，也不写持久环境：

```powershell
powershell.exe -NoProfile -File "D:\Code-dx12\tools\configure-msvc-system.ps1"
```

### 新终端验证

PowerShell 中执行：

```powershell
Get-Command cl, ninja
ninja --version
```

Git Bash 中执行：

```bash
command -v cl ninja
ninja --version
```

预期 `cl` 位于上表的 x64 编译器目录，Ninja 显示 `1.12.1`。环境配置后直接使用后文的 CMake 构建命令。无需再运行旧的嵌套 Bash 启动命令。

若还在旧的 `--noprofile --norc` 会话中遇到 `__git_ps1` 错误，可先执行 `PS1='\u@\h:\w> '`；之后正常重新打开 Git Bash，让其按现有配置加载提示符。

### 已验证范围与恢复

已通过脚本语法检查及预览，并在助手的临时进程中应用生成的四项配置，验证工具查找、Ninja 版本、现有参考程序的 cl 编译、链接和运行。随后在继续 L00 时读取系统配置，已确认 Machine Path 中的三个工具目录，以及 INCLUDE、LIB、LIBPATH 均已写入，并找到安装备份。助手没有管理员权限，未直接执行系统写入。用户新终端的完整构建结果仍待反馈；助手此前启动 Git Bash 遇到执行环境限制。

需要撤销时，在管理员 PowerShell 中使用对应的备份文件，恢复上述四项旧值：

```powershell
$msvcBackup = Get-Content -LiteralPath 'D:\Code-dx12\build\environment-backups\实际备份文件名.json' -Raw | ConvertFrom-Json
foreach ($msvcVariable in @('Path', 'INCLUDE', 'LIB', 'LIBPATH')) {
    [Environment]::SetEnvironmentVariable($msvcVariable, $msvcBackup.Variables.$msvcVariable, 'Machine')
}
```

恢复后同样重新启动终端宿主。课程实现仍由用户完成；此脚本仅处理用户明确要求的本机工具环境。

## 其他 Linux / macOS 机器

准备 CMake、Ninja 和支持 C++20 的编译器。Linux 可以使用发行版包管理器提供的 GCC 或 Clang；macOS 使用 Command Line Tools 提供的工具链。版本和安装命令在目标机器上核验，不要求现在就安装另一个平台。

```sh
cmake --version
ninja --version
c++ --version
```

## 共同构建入口

在根目录参考工程上使用：

```sh
cmake -S . -B build/dev-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/dev-debug
```

Windows 运行 `./build/dev-debug/bin/vle_sandbox.exe`；Linux/macOS 运行 `./build/dev-debug/bin/vle_sandbox`。用户的 L00 练习项目有独立 source/build 路径，遵循课文中的命令。

`-S` 指向源码目录，`-B` 指向生成文件目录，`-G Ninja` 选择生成器。普通 Ninja 是单配置生成器，Debug 在配置时指定。

切换生成器、编译器或平台时使用新的构建目录；已有 `build/windows-vs2022` 不能直接改作 Ninja 构建目录。不要共享 `CMakeCache.txt`、`compile_commands.json` 或生成的二进制来代替重新配置。

现有 `CMakePresets.json` 的 Windows Visual Studio 预设属于初始化参考，尚未由助手改写。L00 中由用户理解命令后自行加入 Ninja 预设；目前以上直接命令不依赖预设。

## 编辑器读取构建信息

开启 `CMAKE_EXPORT_COMPILE_COMMANDS` 后，CMake 的 Ninja 生成器会在构建目录生成 `compile_commands.json`，其中包括各源文件的 include 路径、宏和编译参数。该选项不适用于 Visual Studio generator。[CMake 官方说明](https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html)

clangd 可通过 VS Code、Vim/Neovim 的 LSP 客户端提供语言服务。当前 Linux 已安装 clangd，但尚未核验编辑器是否启用对应 LSP。应指向当前构建目录的编译数据库；例如根目录 `.clangd` 中的 `CompileFlags` → `CompilationDatabase` 可设为 `build/lesson00-linux-debug`，对应当前练习。参考工程则使用 `build/linux-debug`，两者的源文件不同。这只是配置提示，本次未改写编辑器配置。[clangd 使用说明](https://clangd.llvm.org/installation)、[编译数据库配置](https://clangd.llvm.org/config#compilationdatabase)

编译器切换和数据库位置由用户实际构建决定。编辑器中的语法标记与命令行构建结果分别检查；不靠 IDE 自动补全隐式增加 include 路径或链接库。
