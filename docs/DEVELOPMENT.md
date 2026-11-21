# 编辑器无关的开发环境

用户偏好 VS Code 或 Vim/Neovim，并希望方便切换 Windows/Linux。教学主线采用 C++20、CMake 3.25+、Ninja 和命令行。源码、构建描述与编辑器设置分开管理。

当前入口（2026-11-21）：Windows Git Bash + MSVC + Ninja / x64 / Debug；练习预设已使用 `build/lesson00-windows-debug` 并通过用户运行反馈。缓存确认 MSVC `19.44.35222.0`（工具集 `14.44.35207`）。持久环境沿用既有配置；下方环境路径与版本按各自日期作为历史记录。课程现直接进入 [Vulkan L04.1](lessons/04-vulkan-bootstrap.md)，本文仅在用户询问或具体接入遇到问题时查阅，不作为必修前置。

## 各工具负责什么

| 工具层 | 作用 | 本项目选择 |
| --- | --- | --- |
| 编辑器 | 编辑文件、查看诊断、发起调试 | VS Code 或 Vim/Neovim，由用户选择 |
| 语言服务 | 补全、跳转、静态诊断 | 可选 clangd，读取实际编译配置 |
| 构建描述与生成 | 描述 target、依赖和编译条件 | CMake |
| 构建执行 | 根据依赖判断哪些文件需要重新编译 | Ninja |
| 编译与链接工具链 | 将源代码生成目标平台程序 | Windows 当前使用 MSVC；Linux 使用 GCC/Clang；macOS 使用 Apple Clang |

MSVC 编译器可以从终端调用，Visual Studio IDE 不参与课程操作。切到 Linux 后使用相应编译器和系统 SDK，公共 C++ 与 CMake 代码继续复用。跨平台需要各平台实际重新构建和验证，不表示同一个可执行文件到处运行。

## Linux 历史环境（2026-11-14 实测）

用户当时使用 Ubuntu 24.04.5 LTS / x86_64，shell 为 `/usr/bin/zsh`。下列命令记录该平台验证，也适用于 Linux Bash；当前 Windows 命令见后续小节与课文。

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

当时使用 GCC，未为 L00 额外安排 Clang 构建矩阵。切换编译器时使用独立构建目录；Windows 缓存与二进制不复制到 Linux 复用。

## Windows 配置记录（2026-11-14 本机复验）

用户已明确选择 Windows 系统级持久环境配置：普通 Git Bash、PowerShell 和 cmd 共同继承，不为每种 shell 单独配置启动脚本，也无需每次手动初始化。此处以已安装的 x64 MSVC 和 Windows SDK 为默认工具链。

当前核实的目录：

| 用途 | 目录 |
| --- | --- |
| cl.exe、link.exe | `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64` |
| rc.exe、mt.exe | `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64` |
| Ninja | `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja` |

本次在 `D:\dx12_code\IceShell` 所在 Windows 机器实测：配置前 Machine Path 缺少上述三个目录，Machine INCLUDE、LIB、LIBPATH 为空。已运行现有配置脚本并读回确认四项系统变量写入成功；原值备份为 `build/environment-backups/msvc-system-20261114-235517-086.json`。本机官方开发环境选择 MSVC 工具集 `14.43.34808`，编译器报告 `19.43.34810.0`，Ninja 为 `1.12.1`，不要套用此前机器的工具集版本。

助手仅将读回的系统配置和用户 Path 装入验证进程，分别在 PowerShell 与 Git Bash 登录 shell 中从新目录构建根目录参考工程：`build/windows-env-powershell-20261114`、`build/windows-env-gitbash-20261114`。两者的 CMake + Ninja / x64 / Debug 配置、编译、链接、三行输出均成功，退出码 0，输出未见 warning/error，编译数据库已生成。当时没有复现历史上的 ABI 探测停滞，但未定位历史问题原因。2026-11-15 用户另提供 Git Bash 中练习工程成功构建与运行的日志，确认该终端已可用。

MSVC 构建需要 PATH、INCLUDE 和 LIB 等配套环境。[微软说明](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#path-and-environment-variables-for-command-line-builds) 按用户的持久配置偏好，准备了 [一次性配置脚本](../tools/configure-msvc-system.ps1)：它运行官方开发环境初始化脚本获取本机实际值，然后仅保存以下四个系统变量。

| 系统变量 | 内容 |
| --- | --- |
| Path | 前置编译器、Windows SDK 工具和 Ninja 三个目录，保留其他系统 Path 条目；不复制当前进程或用户的完整 Path |
| INCLUDE | MSVC、C++ 标准库和 Windows SDK 头文件搜索目录 |
| LIB | x64 MSVC 与 Windows SDK 链接库搜索目录 |
| LIBPATH | MSVC 开发环境提供的元数据搜索目录 |

这会将普通终端的默认工具链固定为当前 x64 版本。升级 MSVC 或 Windows SDK 后重新运行脚本以更新实际目录；切换其他目标架构时再使用对应的开发环境。项目源码与 CMake 中仍不硬编码本机路径。

### 一次性安装

本次已安装成功，无需重复执行。以后需要更新工具链目录时，从开始菜单以“管理员身份运行”打开 PowerShell，在项目根目录执行：

```powershell
powershell.exe -NoProfile -File .\tools\configure-msvc-system.ps1 -Apply
```

`-Apply` 将配置写入 Windows 的 Machine 系统环境变量，对新启动的两种终端均生效。脚本会先将旧值备份至项目的 `build/environment-backups/`，写入后读回核对；遇到写入错误会尝试恢复旧值。它不会修改 Bash 或 PowerShell 的启动配置。

运行成功后，完全退出 PowerShell、Git Bash，以及承载终端的 Windows Terminal 或 VS Code，再从开始菜单重新打开。已运行的父进程可能继续保存旧环境；如仍继承旧值，注销 Windows 后重新登录。[持久环境变量及变更通知](https://learn.microsoft.com/en-us/dotnet/api/system.environment.setenvironmentvariable?view=netframework-4.8.1#remarks)

只查看配置时，省略 `-Apply`；预览无需管理员权限，也不写持久环境：

```powershell
powershell.exe -NoProfile -File .\tools\configure-msvc-system.ps1
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

已通过脚本语法检查、预览、持久配置写入与读回复核，以及助手的 PowerShell/Git Bash 参考工程构建；备份和本机工具版本见上文。2026-11-15 用户亲自用 Git Bash 完成练习编译、链接、运行和名称变体，两次退出码均为 0。更早的权限限制及 ABI 探测记录保留在进度文档历史部分，不代表本次仍有同样问题。

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

clangd 可通过 VS Code、Vim/Neovim 的 LSP 客户端提供语言服务。2026-11-21 已只读核验当前 Windows 配置：LazyVim 使用 `clangd-msvc.cmd` 启动 MSYS2 clangd 23.1.5；主工程的编译数据库位于 `build/vulkan-windows-debug/compile_commands.json`。L00 的 `build/lesson00-windows-debug` 只对应 `practice/`，不能代替主工程数据库。[clangd 使用说明](https://clangd.llvm.org/installation)、[编译数据库配置](https://clangd.llvm.org/config#compilationdatabase)

当次使用相同启动入口执行 `clangd --check`：默认未找到主工程数据库，使用 fallback 参数，`samples/sandbox/main.cpp` 出现 2 个错误；显式指定 `--compile-commands-dir=D:/Code-dx12/build/vulkan-windows-debug` 后，读到 MSVC 编译命令、引擎及 Vulkan include 路径，检查为 0 个错误。这是语言服务检查，不是项目 Vulkan 运行或编辑器界面补全验收。

用户已在根目录创建以下 `.clangd`，让当前主工程源码读取这个数据库（路径相对于 `.clangd`）；限定目录以免把历史 `practice/` 误指向主工程：

```yaml
If:
  PathMatch: (engine|samples)/.*
CompileFlags:
  CompilationDatabase: build/vulkan-windows-debug
```

用户已将 `engine/src/core/IceVulkan.cpp` 加入 `vle_core` 的源文件列表，并添加 `target_link_libraries(vle_core PUBLIC Vulkan::Vulkan)`；公开头文件 `IceVulkan.hpp` 已包含 Vulkan 头文件。仅给 Sandbox 声明依赖不会将 include 路径反向传给 `vle_core`。

用户应用上述修改后仍遇到头文件找不到：当时数据库仍是修改前的旧版本。助手执行 `cmake -S . -B build/vulkan-windows-debug` 成功更新数据库，再对 `IceVulkan.hpp` 运行 clangd 检查，结果从找不到 `<vulkan/vulkan.h>` 变为 0 个错误。修改 CMake 后需要重新配置生成数据库；若 Neovim 仍保留旧诊断，在对应文件中执行 `:lsp restart clangd`。实际界面补全效果待用户反馈。

本机已核验为 Neovim 0.12.6，内置小写 `:lsp` 命令；可用 `:help :lsp-restart` 查看帮助。当前 `nvim-lspconfig` 检测到内置 `:lsp` 后，会跳过旧的 `:LspRestart` 等插件命令注册。因此本机应使用上述内置命令；此前给出的 `:LspRestart` 不适用于当前配置。

此前日志中的 `--function-arg-placeholders` 参数值无效问题，用户已在 `lua/plugins/languages.lua` 中改为 `--function-arg-placeholders=true`。助手未改写项目构建、源码或用户编辑器配置，仅执行上述配置生成与语言服务检查。

编译器切换和数据库位置由用户实际构建决定。编辑器中的语法标记与命令行构建结果分别检查；不靠 IDE 自动补全隐式增加 include 路径或链接库。
