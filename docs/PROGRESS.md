# 学习与实现进度

更新日期：2026-11-15。

## 暂停记录与下次续学入口

**用户于 2026-11-15 明确暂停学习，要求保存教学和问题进度。当前停在 L00.2 任务 A：CMake 预设已讲解，待用户实现与运行验收。** 本次保存仅维护教学文档，没有修改课程源码、构建脚本或执行构建运行。

- 已完成：L00.1 的返回值修正、正确名称输出和退出码 0；L00.2 任务 B 的名称变体与增量构建观察。用户已正确解释 main 不重编译、程序需要重新链接的原因，无需重复安排这些练习。
- 已讲解：`CMakeLists.txt`、configure preset、build preset 的职责；预设格式 `version: 6`；`configurePresets`/`buildPresets` 数组；`generator`、`binaryDir`、`cacheVariables` 与命令行参数的对应；`${sourceDir}` 的展开；`configurePreset` 对配置预设的引用。已给出空数组结构和局部字段提示，由用户组织完整 JSON。
- 待实现：在 `practice/` 中创建 `CMakePresets.json`，配置与构建预设都命名为 `lesson00-debug`，使用 Ninja、Debug、编译数据库，新构建目录为 `${sourceDir}/../build/lesson00-windows-debug`。保存时文件列表未发现该预设文件；尚无本任务的用户运行反馈，不记为已完成。
- 待回答的问题 1：`configurePreset` 字段起什么作用？
- 待回答的问题 2：如果要给 `vle_core` 新增一个 `.cpp`，应该修改哪个文件？
- 待验收：从 `practice/` 列出全部预设、配置、构建并运行新目录中的程序；预期输出 `IceShellEngine`、退出码 0，并生成 `compile_commands.json`。具体命令与字段见 [课文任务 A](lessons/00-project-foundation.md#任务-a把命令整理成预设)。
- 后续待办：任务 A 通过后再做任务 C（移除整个函数定义，观察链接错误并恢复），再核对 PUBLIC/PRIVATE 与 string_view 生命周期；L00 整课尚未完成，L01 尚未开始。
- 当前没有新报告的编译或运行故障；旧的缺少 return、空输出问题已解决。待办是实现、运行和理解反馈，不应重新当作环境故障排查。

下次用户说“继续”时，先读 `practice/` 最新文件和用户新增反馈；若预设已写好，直接审查并验收，不要求重写；若仍未创建，从任务 A 的实现提示衔接。当前实际平台为 Windows Git Bash + MSVC + Ninja，沿用已完成的持久环境配置。用户暂停期间不继续推进课程。

## 最近已验证的学习与环境记录

最新学习反馈：用户提供了两次 Windows Git Bash 中的练习构建与运行日志，名称分别为 `IceShell` 和 `IceShellEngine`，退出码均为 0。助手只读核对当前源码，确认 `engine_name()` 已返回 `"IceShellEngine"`；本次未重新构建、运行或修改课程代码。L00.1 的实现与运行验收通过，L00.2 的名称变体与增量构建观察已完成；其余学习证据见下文。

最新环境操作：用户要求为 Windows 的 Git Bash 和 PowerShell 配置系统路径。已在 `D:\dx12_code\IceShell` 所在机器执行 `tools/configure-msvc-system.ps1 -Apply`，备份并写入 Machine Path、INCLUDE、LIB、LIBPATH，读回确认成功。备份：`build/environment-backups/msvc-system-20261114-235517-086.json`。本机选择 MSVC x64 `19.43.34810.0`（工具集 `14.43.34808`）、Windows SDK `10.0.26100.0`、Ninja `1.12.1`。

2026-11-14 的环境准备中，仅使用读回的持久环境，在 PowerShell 与 Git Bash 登录 shell 中分别完成根目录参考工程的全新 CMake + Ninja / Debug 配置、编译、链接、运行，均退出 0，输出未见 warning/error。构建目录为 `build/windows-env-powershell-20261114` 与 `build/windows-env-gitbash-20261114`，均生成编译数据库。当时未复现历史 ABI 探测停滞；不据此宣称已定位其历史原因。2026-11-15 用户已提供 Git Bash 中练习工程的成功构建与运行证据，无需重复环境配置；用户亲自运行 PowerShell 构建的证据尚未提供。

此前环境准备仅配置工具和维护文档，未修改课程源码、shader 或构建脚本；当时未审查用户已修改的 `practice/src/engine_info.cpp`。本次已完成只读审查。下文 Linux 和早期 Windows 记录均为历史证据；续学时先读取最新代码并核对平台。

下次衔接顺序：L00.2 任务 A（练习预设，采用独立的 Windows 构建目录）→ 任务 C（移除整个定义，观察链接错误并恢复）→ PUBLIC/PRIVATE 与 string_view 生命周期自检 → 验收后进入 L01。任务 B（名称变体与增量构建）已完成，无需重复安排。

## 当前学习入口

**状态：按用户要求暂停。L00.1 实现与运行验收通过；L00.2 任务 B 已完成，任务 A 已讲解待实现，任务 C 和其余自检待完成。** 尚未判定 L00 整课掌握，L01 尚未开始。

- 当前环境：Windows x64 / Git Bash，MSVC 工具集 `14.43.34808` + Ninja / Debug。日志中 `MINGW64` 是 shell 提示符；实际编译器是 `cl.exe`，不是 GCC。
- 实际练习目录仍为 `practice/`。第一份 `CMakeLists.txt` 是用户明确请求助手补全的参考，已有中文注释；用户完成 C++ 实现和本次名称变体。
- 只读审查：声明、定义和调用匹配；`engine_name()` 返回指向字符串字面量 `"IceShellEngine"` 的视图，生命周期有效；main 输出并返回 0。
- 用户首次日志包含 4 步：编译实现、归档静态库、编译 main、链接程序；输出 `IceShell`，退出码 0。变体日志包含 3 步：仅编译实现、归档库、重新链接；输出 `IceShellEngine`，退出码 0。两份日志均未见 warning/error。
- 用户正确解释了 main 的源文件和所含头文件未变，因此其目标文件可复用；引擎实现变更导致静态库更新，最终程序需要重新链接。补充：Ninja 也会因实际编译命令变化或输出缺失等原因重建，触发条件不只有源码内容变化。
- 当前使用的 `build/lesson00-linux-debug` 名称与平台不符；已读缓存确认 source 为本机 `practice/`、编译器为 MSVC、generator 为 Ninja、配置为 Debug。这次是 Windows 验证，不是新的 Linux 验证。完成本次审查时保留该目录；后续预设由用户配置新目录 `build/lesson00-windows-debug`，不要直接搬动缓存。
- 本次助手仅检查代码、缓存、日志并维护教学文档，未重复构建运行；后续仍由用户实现预设并亲自执行故障实验。

## 当前结论

**工程状态：参考骨架的 Windows/Linux 验证保留为历史记录；用户练习已通过 Windows x64 / MSVC / Ninja / Debug 编译、链接与运行，缺少返回值的问题已修正。学习状态：亲自实现运行、名称变体及增量构建解释已有证据；预设、链接故障恢复、PUBLIC/PRIVATE 与 string_view 生命周期解释仍待验收。**

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

代码目前没有窗口、Vulkan 初始化或其他引擎子系统。第三方库均未接入项目。当前目录已是 Git 仓库，检查时 HEAD 为 `38f4418 first commit`；助手本次没有创建提交或修改远程配置。

## 本次用户实践与学习证据（2026-11-15）

| 学习证据 | 当前状态 |
| --- | --- |
| 用户亲自实现并运行 | 已补返回值，两次名称输出与退出码均符合预期；当前源码与变体日志一致 |
| 用户解释关键数据流与取舍 | 已正确解释为什么 main 不重编译而程序要重新链接；PUBLIC/PRIVATE、string_view 生命周期、声明与定义区别仍待自检 |
| 用户完成小变体或故障诊断 | 已完成名称变体并观察 4 步变 3 步；链接故障与恢复尚未执行 |
| 参考示例与独立实现的区分 | 第一份练习 CMake 是助手按请求提供的参考；C++ 修正、运行和名称变体由用户完成；用户预设尚未创建 |

## Linux 历史验证与学习快照（2026-11-14）

检查日期：2026-11-14。以下是当时源码与环境的记录，缺少 return 和空输出已在本次 Windows 实践中修正。以下构建由助手执行，未修改课程源代码或构建脚本。

| 项目 | 结果 |
| --- | --- |
| 工具检查 | CMake 4.4.2、Ninja 1.11.1、GCC 13.3.0；Clang/clangd 18.1.3、GDB 15.1；发现 VS Code/Neovim 入口 |
| `cmake --preset linux-debug` | 成功；GNU 13.3.0，编译器 ABI 探测完成 |
| `cmake --build --preset linux-debug` | 成功；静态库与程序生成，输出未见 warning/error |
| `./build/linux-debug/bin/vle_sandbox` | 三行参考输出，退出码 0；仅控制台程序 |
| 练习配置 | `-S practice -B build/lesson00-linux-debug -G Ninja`，Debug、编译数据库启用；成功 |
| 练习初次构建 | 当时因 include 路径错误失败；该问题现已由用户修正 |
| 练习 review 构建 | 新目录 `build/lesson00-linux-review`，GCC / Ninja / Debug；配置、编译、链接成功，1 条 `no return statement in function returning non-void [-Wreturn-type]` 警告 |
| 练习运行 | 用户反馈输出“空”；完整执行命令与退出码未提供。助手此次未执行缺少返回值的版本；名称输出验收未通过 |
| 生成文件 | 两个目录均已生成 `build.ninja` 和 `compile_commands.json`；练习数据库可见 `-std=c++20` 和传递到两个源文件的 include 根目录 |
| Vulkan 环境查询 | `vulkaninfo --summary` 成功；Instance Version 1.4.313，系统头文件 1.4.313，validation layer 可枚举；glslc 可执行 |
| Vulkan 设备 | Intel Iris Xe（TGL GT2），API 1.4.318，Mesa 25.2.8；另有 CPU 软件设备 llvmpipe。本项目尚无 Vulkan 初始化或绘制验证 |
| 其他配置 | 未构建 Linux Release、Clang 或 macOS；未检查编辑器 LSP 配置 |

| 学习证据 | 当时状态 |
| --- | --- |
| 用户亲自实现并运行 | 已修正 include、编写函数框架与 main；反馈已编译运行且输出为空。函数仍缺少 return，待修正后验证名称输出 |
| 用户解释关键数据流与取舍 | 待反馈：声明/定义/链接、PUBLIC/PRIVATE、string_view 生命周期 |
| 用户完成小变体或故障诊断 | include 错误已在提示后修正；缺少 return 由助手审查定位。增量构建与链接故障实验尚未执行 |

当时已在 [Lesson 00](lessons/00-project-foundation.md) 安排 L00.2：用户创建练习预设、修改字面量观察增量构建、临时移除整个函数定义观察链接错误并恢复。本次已完成返回值修正与名称变体，后续以上方当前学习入口为准。

## Windows 历史验证证据

以下为切换到 Linux 之前的记录，版本与路径不代表当前机器。

| 项目 | 结果 |
| --- | --- |
| `cmake --list-presets` | 在 Windows 上正确显示 `windows-vs2022` |
| `cmake --preset windows-vs2022` | 成功；检测到 MSVC 19.44.35222.0 和 Windows SDK 10.0.26100.0 |
| `cmake --build --preset windows-debug` | 成功；生成静态库与可执行程序；本次输出未见编译警告 |
| `build/windows-vs2022/bin/Debug/vle_sandbox.exe` | 输出第 0 课三行信息；退出码 0 |
| Windows Release | 提供构建预设，尚未执行 |
| Linux / macOS（当时） | 当时仅有预设；Linux 后续验证见上节，macOS 仍未执行 |
| Vulkan GPU 运行 | 仅执行环境查询，尚未运行本项目的图形路径 |

环境检查检测到 CMake 4.2.3、Visual Studio 2022 C++ 工具集和 Vulkan SDK 1.4.335.0。`vulkaninfo --summary` 正常返回，列出 Khronos validation layer、Intel Arc 140T GPU 和 NVIDIA RTX PRO 2000 Blackwell Generation Laptop GPU。上述检测只证明工具和枚举可用，尚未逐项验证 mesh shader 或光追能力。

初始终端 PATH 未找到 Ninja；之后在现有工具安装中找到 Ninja 1.12.1。通过 Developer PowerShell 初始化后，CMake 4.2.3 能识别 MSVC 19.44.35222.0；对 `build/dev-debug` 的 Ninja 配置停在编译器 ABI 探测处，没有得到成功完成结果，已停止该次验证。其原因尚未定位，不能判定为项目源码错误或 Ninja 通用不兼容。用户本机终端的检查结果待反馈。

已检测到 VS Code 与 Vim；未在 PATH 和已检查的常用位置发现 Clang/clangd。没有安装新编译器或编辑器插件，没有修改持久环境变量。未为固定字符串输出添加单元测试；CPU 算法出现后再加入针对真实行为的测试。

## Windows 阶段实践历史

以下按此前反馈保留；当前目录、构建命令和待办以上方“当前学习入口”为准。

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
- L00 参考骨架的编译运行由助手执行；用户自己的练习已输出正确名称且退出码为 0，并完成名称变体及增量构建解释。预设、链接故障恢复与其余自检仍待完成。
- 学习方式已确认：用户写代码，助手教学与审查；用户明确要求时才提供具体参考实现。
- 核心代码提示已授权，助手应主动提供局部片段；整课实现仍由用户组织。
- 资产获取分工已确认，当前无外部资产下载需求。
- 编辑器偏好已确认：VS Code 或 Vim，避免依赖 Visual Studio IDE，命令行构建优先。
- 当前 shell 已由最新日志确认：Windows Git Bash；使用 Bash 语法和 Windows 程序的 `.exe` 后缀，构建工具链为 MSVC + Ninja。
- 全路线初估 524–829 小时；加入约 20% 的额外复习与返工余量后按 630–1,000 小时排期。这是规划估计，不是用户已投入时间或掌握承诺。
- 每周可投入时间尚待反馈；L06 后依据真实学习记录调整估计。
- 当前使用 Windows 本机开发；此前 Linux 验证独立保留。Windows/Linux 均为优先桌面目标，macOS 仍待评估兼容性。
- 引擎名称暂为 Vulkan Learning Engine，可随时更名。

以后每课继续补充真实验证证据与学习反馈，保持“已实现”“已验证”“待学习”三者分开。
