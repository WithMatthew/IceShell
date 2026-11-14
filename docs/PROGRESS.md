# 学习与实现进度

更新日期：2026-11-14。

## 暂停记录与下次入口

**状态：已按用户要求暂停。** 用户于 2026-11-14 要求保留记录，等待用户明确说“继续”后再恢复课程、构建或问题排查。

- 当前停在 L00.1，实际练习目录为 `practice/`。
- `practice/CMakeLists.txt` 已按用户明确请求由助手补全，包含静态库、可执行程序、C++20 和依赖传递，并有中文注释。
- 暂停时只读核对：`practice/include/vle/engine_info.hpp` 和 `practice/src/engine_info.cpp` 已有内容，尚未审查；`practice/src/main.cpp` 仍为空。不要沿用之前“三个 C++ 文件均为空”的旧状态，也不要覆盖用户的新内容。
- Windows 系统级工具环境已确认写入，Git Bash 与 PowerShell 共用。无需重复安排环境安装或 shell 启动配置。
- 未解决项：助手执行的 Ninja 配置停在 MSVC 编译器 ABI 探测，相关验证进程已在上次操作中终止；`build/lesson00-debug` 尚无 `build.ninja` 和 `compile_commands.json`。未完成练习程序的构建与运行验收。
- 下次先读取本记录、课文及用户最新的练习文件，继续讲解和只读审查；由用户完成函数实现及 `main.cpp`。构建时源码路径使用 `-S practice`，构建目录使用 `-B build/lesson00-debug`，保留错误输出再定位 ABI 探测问题。
- 学习边界保持：代码由用户亲自实现，助手提供关键提示；第一份 CMakeLists 的代写仅是用户明确授权的局部例外。L00 尚未完成，不自动推进 L00.2 或 L01。

## 当前结论

**工程状态：助手生成的 P00 / L00 参考骨架曾通过 Windows x64 Debug 的 Visual Studio generator 构建与运行。用户已在 `practice/` 创建练习文件，第一份 CMakeLists 已按明确请求由助手补全；头文件与实现文件已有待审查内容，main.cpp 仍为空。当前 Ninja 配置停在编译器 ABI 探测处，尚未验证完整构建。学习状态：L00.1 已按用户要求暂停，待恢复后继续实现、运行并解释，不将 L00 标为学习完成。**

用户已确认熟悉 C++，刚开始学习 Vulkan。讲解按这一基础安排，重点放在图形概念、显式资源管理、同步和系统设计。

用户进一步明确：**代码需要由用户亲自实现。** 助手负责讲解、任务拆解、提示、代码审查和调试指导，默认不修改源代码、shader、构建脚本或测试实现。教学文档和进度由助手维护。

新增备忘：核心环节主动提供关键代码提示；外部资产由助手按需获取并记录来源/许可；尽量不用 Visual Studio IDE，优先 VS Code 或 Vim，命令行 CMake + Ninja。资产准备不替代用户实现引擎导入与资源管线。

## 已准备的参考工程与资料

- [x] `AGENTS.md`：逐课推进方式、技术方向、验证与文档约定。
- [x] 路线图：P00–P14、L00–L45，含依赖关系与验收标准。
- [x] 架构方向：跨平台、渲染能力分级、场景与子系统边界。
- [x] C++20 静态库 `vle_core` 与示例程序 `vle_sandbox`。
- [x] Windows、Linux、macOS 的 CMake 预设。
- [x] 第 0 课讲解、运行命令、可选练习和自检问题。
- [x] Windows 上从新构建目录配置、编译、链接、执行。
- [x] 依用户要求切换为“用户亲自实现、助手教学审查”，补充 46 课逐课时间和阶段累计时间。
- [x] 补充开发环境、资产获取约定；重写 L00 的命令行教学和局部代码提示。此次只修改教学文档，未修改课程代码或构建脚本。

代码目前没有窗口、Vulkan 初始化或其他引擎子系统。第三方库均未下载或链接。起始目录为空且不是 Git 仓库；当前没有创建 Git 提交或远程仓库。

## 本次验证证据

| 项目 | 结果 |
| --- | --- |
| `cmake --list-presets` | 在 Windows 上正确显示 `windows-vs2022` |
| `cmake --preset windows-vs2022` | 成功；检测到 MSVC 19.44.35222.0 和 Windows SDK 10.0.26100.0 |
| `cmake --build --preset windows-debug` | 成功；生成静态库与可执行程序；本次输出未见编译警告 |
| `build/windows-vs2022/bin/Debug/vle_sandbox.exe` | 输出第 0 课三行信息；退出码 0 |
| Windows Release | 提供构建预设，尚未执行 |
| Linux / macOS | 提供构建预设，尚未实机执行 |
| Vulkan GPU 运行 | 仅执行环境查询，尚未运行本项目的图形路径 |

环境检查检测到 CMake 4.2.3、Visual Studio 2022 C++ 工具集和 Vulkan SDK 1.4.335.0。`vulkaninfo --summary` 正常返回，列出 Khronos validation layer、Intel Arc 140T GPU 和 NVIDIA RTX PRO 2000 Blackwell Generation Laptop GPU。上述检测只证明工具和枚举可用，尚未逐项验证 mesh shader 或光追能力。

初始终端 PATH 未找到 Ninja；之后在现有工具安装中找到 Ninja 1.12.1。通过 Developer PowerShell 初始化后，CMake 4.2.3 能识别 MSVC 19.44.35222.0；对 `build/dev-debug` 的 Ninja 配置停在编译器 ABI 探测处，没有得到成功完成结果，已停止该次验证。其原因尚未定位，不能判定为项目源码错误或 Ninja 通用不兼容。用户本机终端的检查结果待反馈。

已检测到 VS Code 与 Vim；未在 PATH 和已检查的常用位置发现 Clang/clangd。没有安装新编译器或编辑器插件，没有修改持久环境变量。未为固定字符串输出添加单元测试；CPU 算法出现后再加入针对真实行为的测试。

## 当前实践任务

用户已反馈 Windows Git Bash 中 `ninja --version` 报 `command not found`，`Get-Command cl` 也报命令不存在。已明确：前者是当前 PATH 未找到本机已安装工具，后者是混用了 PowerShell 内建命令。课文已增加 Git Bash 入口、`command -v` 检查，以及带 MSVC 环境的 Bash 子 shell 启动步骤。

已重新确认 Ninja、Launch-VsDevShell.ps1、Git Bash 与 CMake 的本机安装路径。助手尝试通过工具启动 Bash 时遭遇受限环境的 `CreateFileMapping ... Win32 error 5`，因此 Bash 嵌套启动未在助手会话中验证成功；未把该工具限制归因为用户的环境问题。等待用户在自己的终端报告工具路径和版本；尚无用户工程编译结果。

用户随后反馈 `cd workspace/` 后出现 `__git_ps1: command not found`。已检查本机 Git Bash 提示符脚本：提示符可被继承，但旧步骤跳过启动配置后未加载其依赖的 Git 分支函数。已修正文档，在启动子 shell 前设置不依赖函数的 `PS1`，并提供当前 Bash 的临时修复命令。目录切换结果由 `pwd` 确认；提示符修复和 Ninja 构建仍待用户反馈。

用户反馈重新打开 PowerShell 或 Git Bash 后仍无法识别 Ninja。再次用完整路径运行确认 Ninja 版本为 `1.12.1`，并读取用户、系统和当前进程的 Path，均未包含 Ninja 目录。已补充一次性用户 Path 配置与完整重启终端宿主的步骤，明确区分持久定位 Ninja 和每次初始化 MSVC。此次仅检查环境并更新教学文档，未修改用户持久环境；新终端验证待用户反馈。

用户继续询问 `cl` 的位置和配置。已通过 `vswhere` 和默认工具版本文件核实 `VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\cl.exe`，报告编译器版本 `19.44.35222`。在助手的 PowerShell 进程中运行 `Launch-VsDevShell.ps1 -Arch amd64 -HostArch amd64 -SkipAutomaticLocation` 后，`Get-Command cl` 成功解析该路径，INCLUDE 和 LIB 均已设置，主机/目标架构为 x64，Windows SDK 为 `10.0.26100.0`。本次只验证环境初始化，未重新编译项目，也不会修改用户已打开终端的环境。

L00.1：用户先按开发环境文档检查命令行工具，再自行创建 `practice/lesson00` 的四文件小工程，使用 CMake + Ninja 构建并运行。课文提供接口和三条 target 关系提示，完整源文件与 CMakeLists 由用户完成。尚未收到用户代码或运行结果。

最新环境偏好：用户明确要求配置 Windows 系统环境变量，让普通 Git Bash 和 PowerShell 共同使用，不再分别配置启动脚本或每次手动初始化。已准备 `tools/configure-msvc-system.ps1`，预览默认不写持久配置；`-Apply` 在管理员权限下保存 Path、INCLUDE、LIB、LIBPATH，并备份旧值。已通过语法、预览和临时进程验证：只应用计划中的四项配置后，可找到 cl/link/rc/mt/ninja，Ninja 为 1.12.1，现有参考程序用 cl 编译、链接、运行成功。系统变量保持原值，助手进程已确认不是管理员，未执行系统写入；待用户运行一次安装命令并反馈新终端结果。本次没有创建或修改课程 C++、CMake 或 shader 实现，Ninja 项目构建与用户学习验收状态不变。

用户要求继续第 0 课后，再次读取系统环境确认配置现已写入：Machine Path 包含 MSVC x64、Windows SDK x64 工具和 Ninja 三个目录，Machine INCLUDE、LIB、LIBPATH 均非空，并出现备份 `build/environment-backups/msvc-system-20261114-150401-229.json`。这证明持久配置已落地，尚不等于已验证用户新终端的完整构建。`practice/lesson00` 当前不存在，继续 L00.1 的四文件练习：用户实现声明、定义、调用及两个 CMake target，助手提供关键片段与构建验收说明；暂不进入 L00.2 或 L01。

用户随后明确请求代写第一份 CMakeLists 以回顾语法。实际练习位于 `practice/`，已有 CMake 内容引用不存在的 `engine` 和 `samples/sandbox` 子目录；已改为直接声明 `vle_core` 静态库、`vle_sandbox` 程序以及 C++20、公开头文件和链接关系，并保留项目名/版本和关闭编译器扩展的设置。未修改三个空的 C++ 文件。课文已同步实际目录，后续配置使用 `-S practice`。助手在进程中读取已持久保存的环境后尝试 Ninja 配置，识别到 MSVC 19.44.35222.0，但 25 秒后仍在 ABI 探测处；已用精确 PID 和启动时间确认并终止本次 CMake/Ninja 进程。尚未生成 build.ninja，未进行练习程序的构建或运行；该探测问题发生在用户源码编译之前，原因未定。

已有主工程保持为参考；练习审查通过后由用户将学到的结构应用到主工程，L01 起继续演进。L00.2 再处理 Ninja 预设、故障实验和自检。

## 下一课的明确范围

当前先由用户完成 L00 的独立实现和自检，或明确选择跳过已有内容；不得依据助手先前的构建结果自动判定已掌握。以下 L01 任务由用户实现，助手提供讲解和审查。

L01：创建第一个跨平台窗口。

1. 阅读 SDL3 当前官方集成说明，选择并固定版本/提交及来源信息。
2. 用 CMake 引入 SDL3，明确联网获取与离线预下载的入口。
3. 增加最小 Platform 模块，在 Sandbox 中创建可缩放窗口与事件循环。
4. 处理初始化失败、关闭事件和资源释放；如果使用动态库，处理运行时库路径或复制。
5. 实际观察窗口启动、缩放、最小化/恢复、关闭；更新第 1 课及本文。

本课完成标准是窗口与事件可用。Vulkan 设备和三角形属于 P02 的后续课程。

## 学习反馈与待确认事项

- 学习基础：已确认“熟悉 C++，刚开始学 Vulkan”。
- L00 参考骨架的编译运行由助手执行；用户亲自实现、运行、解释和诊断问题的学习证据尚无反馈。
- 学习方式已确认：用户写代码，助手教学与审查；用户明确要求时才提供具体参考实现。
- 核心代码提示已授权，助手应主动提供局部片段；整课实现仍由用户组织。
- 资产获取分工已确认，当前无外部资产下载需求。
- 编辑器偏好已确认：VS Code 或 Vim，避免依赖 Visual Studio IDE，命令行构建优先。
- 当前 shell 已确认：Windows Git Bash；命令示例需标注对应 shell，并优先提供 Bash 写法。
- 全路线初估 524–829 小时；加入约 20% 的额外复习与返工余量后按 630–1,000 小时排期。这是规划估计，不是用户已投入时间或掌握承诺。
- 每周可投入时间尚待反馈；L06 后依据真实学习记录调整估计。
- 暂按 Windows 本机开发、Linux 优先兼容、macOS 评估兼容推进；具体第二平台和硬件可在需要时调整。
- 引擎名称暂为 Vulkan Learning Engine，可随时更名。

以后每课继续补充真实验证证据与学习反馈，保持“已实现”“已验证”“待学习”三者分开。
