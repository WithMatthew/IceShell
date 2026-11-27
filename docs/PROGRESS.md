# 学习与实现进度

更新日期：2026-11-27。

## 续学记录与当前入口

2026-11-27 路线重组：用户要求覆盖基础 RHI、光栅化、光追、物理、材质系统、场景管理、UI，并尽量保证核心代码由自己实现。用户确认三点。第一，RHI 只做 Vulkan 后端，公开接口不暴露 Vk 类型。第二，物理基础部分自写，加 GPU compute 布料/粒子，复杂刚体用 Jolt。第三，音频暂缓；动画、meshlet 与 GPU 驱动绘制列入扩展清单。已更新 [路线图](ROADMAP.md)、`AGENTS.md`、`README.md` 与 [架构](ARCHITECTURE.md)。新增 R1–R5、U1–U3、G1，主线粗估 370–592 小时（含余量约 445–710）。本次只修改文档，未改源码或构建脚本；当前入口不变，仍是 L04.1 实例扩展练习。

**L04.1 暂停在实例扩展练习开始前。** 查询并启用 `VK_EXT_debug_utils` 已讲解，用户确认尚未开始实现。上一项验证层预检查已实现，正常启动与缺层退出均有运行记录。本机 `vulkaninfo --summary` 已列出该扩展（revision 2），项目当前的 `enabledExtensionCount` 为 0。[本步课文](lessons/04-vulkan-bootstrap.md#查询并启用实例扩展)

2026-11-22 用户确认上一练习的两次运行均符合预期。缺失验证层时，创建返回 `VK_ERROR_LAYER_NOT_PRESENT`（`-6`），跳过销毁并退出 `1`。正常启动时创建和销毁成功，退出 `0`。运行结果按用户反馈记录。助手已从源码核实失败分支输出实际的 `ret`。

此前已完成 Vulkan 构建依赖接入和编译数据库更新。Sandbox 与 `vle_core` 均有 Vulkan include 路径，clangd 对 `IceVulkan.hpp` 的检查为 0 个错误。Neovim 已核实使用 `:lsp restart clangd`，编辑器界面的最终补全效果待用户反馈。

- 默认教学方式：聚焦 Vulkan 对象、数据流、同步、GPU 资源生命周期与引擎设计。通用 C++、CMake 和编译链接知识只在用户单独提出时讲解；当前 Vulkan 功能需要构建修改时，单独说明具体文件/target、目的与最小片段，由用户实现。
- 按用户要求跳过：L00.2 任务 C 的链接故障与恢复实验；任务 A 的 `configurePreset`、新增源文件归属问答；PUBLIC/PRIVATE、string_view 等基础自检。它们不再是待办或 Vulkan 前置门槛，状态是“跳过”，不记为已验证掌握。
- 保留的已完成证据：用户实现运行 L00 程序，完成名称变体与增量构建解释；亲自创建预设、修正路径并反馈输出 `IceShellEngine`、退出码 0。助手核对两个预设可列出，新构建目录有程序和编译数据库。
- Sandbox 与 `vle_core` 已接入 Vulkan。用户实现的 `IceVulkan` 请求验证层，仅在创建成功后接管局部句柄，并通过 `IsValid()` 向 main 和析构函数提供状态。创建失败时日志包含调用名和实际返回码。
- 用户已用 `vkEnumerateInstanceLayerProperties` 查询数量并填充数组，再用 `std::find_if` 逐个查找所需层名。`enabledLayerCount` 已从同一个请求数组 `IceVulkanLayers` 取得。当前实现按首次分配的数组长度搜索，填充成功时可用返回的 `count` 限定实际条目范围。
- 新顺序：L04.1 无窗口初始化与设备枚举 → L04.2 最小 SDL3 窗口与 Surface → L04.3 Device、队列与 feature 启用 → L05 清屏与呈现同步 → L06 三角形。原 L01–L03 的配套内容按需并入，输入和时间管理放在相机/物理课程。
- 实现分工保持不变：用户亲自写源码、shader 和构建配置；助手提供关键提示、审查和调试指导。2026-11-21 的 CMake 配置与 clangd 检查结果保留；2026-11-22 续学由用户完成源码修改，助手只读审查、核对官方规则、构建运行并维护教学文档，没有代写课程实现。
- 当前工作目录为 `D:\dx12_code\IceShell`，本地已生成 `build/vulkan-windows-debug` 和编译数据库。先前在 `D:\Code-dx12` 下完成的构建另有记录。`practice/` 是 L00 练习目录。

下次续学从 `IceVulkan::InitVulkan()` 中已有的验证层检查之后接上，由用户首次调用 `vkEnumerateInstanceExtensionProperties` 查询实例扩展数量，再查找并请求 `VK_EXT_debug_utils`。

## 最近已验证的学习与环境记录

2026-11-22 已核对用户改为 `static_cast<uint32_t>(std::size(IceVulkanLayers))`，Windows Debug 增量构建成功，正常和缺层路径分别退出 `0` 和 `1`。针对用户关于层数变化的提问，助手在独立进程中直接调用本机 Vulkan loader，只在查询数量和填充之间切换该进程的 `VK_LAYER_PATH`。可见层数由 6 增至 15 时，容量为 6 的填充调用返回 `VK_INCOMPLETE` 并写回 6，随后重新查询得到 15。反向由 15 减至 6 时，填充返回 `VK_SUCCESS` 并写回 6。该实验验证了 loader 的返回语义。日常启动时遇到层数变化的发生率未知。

2026-11-22 用户完成层名查找后请求复查。助手在 Windows / MSVC / Ninja / Debug 增量构建成功，随后用 Git Bash 实际运行。正常启动打印创建和销毁日志并退出 `0`。单次设置 `VK_LAYER_PATH=./samples/sandbox` 后只打印 `Missing required vulkan layer VK_LAYER_KHRONOS_validation` 并退出 `1`。源码确认缺层时在 `vkCreateInstance` 之前返回。两次枚举调用之间层数变化的行为依据源码与 Vulkan 接口规则审查。

2026-11-22 加入错误码日志之前，助手已在当前检出目录构建并用 Git Bash 对照运行。正常启动打印创建和销毁日志，退出 `0`。以 `VK_LAYER_PATH=./samples/sandbox` 启动，只打印当时的固定失败文本，退出 `1`。相同路径设置下，`vulkaninfo --summary` 的层列表中也没有所需验证层。

2026-11-22 补充修改复查与运行：用户反馈“ok,已经补充修改”。只读确认创建输出已改为局部 `instance`，失败返回时不修改成员，成功后才赋值；`IsValid()` 已加 const，析构在条件销毁后将成员置空。关键失败状态与所有权修正通过本次审查，错误日志仍只有固定文本、尚未包含 `ret`。助手最初在受限会话中执行增量构建无输出，已中止；经工具权限流程在沙箱外重试同一构建命令，4 步编译链接成功、退出码 0，输出未见编译警告。随后在当前工具会话实际运行 `build/vulkan-windows-debug/bin/vle_sandbox.exe`，输出 `Create VK Instance Success`、`Destroy VK Instance Sucess`，退出码 0；本次终端未见额外 warning/error，但 messenger 尚未建立。没有修改源码或运行缺失 layer 变体，未将正常路径验证标记为整课掌握。

2026-11-22 修改后审查：用户反馈“输出没有问题，你直接看代码修改吧”。助手只读检查三份源码，确认 `enabledLayerCount = 1` 与数组接线、成员空初始化、`IsValid()` 查询、main 失败返回 1、析构条件清理和删除复制操作均已加入。主要遗留问题在 `vkCreateInstance(..., &_vk_instance)`：调用失败后输出内容不保证有效，初始空值也不能保证保留，因此后续状态判断及清理可能使用无效值；需用局部输出句柄并在成功后赋给成员。失败日志仍未输出实际返回码，销毁后尚未复位成员。`IceVulkan(IceVulkan&) = delete` 已能阻止复制，不将缺少 const 误报为仍可复制；改成 const 引用及将 `IsValid()` 标为 const 属于接口整理建议。此次只维护进度，没有修改源码或重复构建运行；用户反馈与助手审查证据分别记录。

2026-11-22 恢复学习：用户要求“继续”。只读确认当前代码仍请求 0 个 layer，构造时调用的初始化结果未反映到 main 的退出码，Instance 成员未显式初始化且销毁无状态保护，所有者类仍可默认复制。继续提供局部提示：成员以 `VK_NULL_HANDLE` 表示空状态，创建先写入局部句柄，仅在 `VK_SUCCESS` 后交给成员；以 `is_valid()` 暴露当前 Instance 状态供 main 决定退出码；只清理有效成员并复位，禁止复制；将现有 layer 数组及数量接入创建信息。计划由用户修正后运行正常路径，再临时请求不存在的 layer 验证错误码、安全清理与非零退出码，并恢复正确名称。已核对 Khronos 的创建配置、错误输出与销毁规则；本次未构建、运行或修改源码，修正实现、运行与理解证据仍待用户反馈。

2026-11-21 保存并暂停：用户明确要求“进度保存，今天先到这里”。已保存首次 Instance 正常路径运行成功的证据、三项待修正内容及下次入口。暂停前没有收到修正后的代码或运行反馈，不将审查建议记为已实现或已掌握。本次仅更新进度文档，未修改源码、构建脚本，也未继续构建或运行；等待用户下次恢复学习。

2026-11-21 首次 Instance 运行反馈：用户在 Git Bash 运行 `./build/vulkan-windows-debug/bin/vle_sandbox.exe`，随后 `echo $?`，输出为 `Create VK Instance Success`、`Destroy VK Instance Sucess`、`0`。助手只读核对当前源码，确认 `vkCreateInstance` 的成功分支与日志对应，析构调用 `vkDestroyInstance`；用户正常路径已运行。发现 `enabledLayerCount = 0`、`ppEnabledLayerNames = nullptr`，已定义的 validation 名称数组未使用；构造函数忽略 `InitVulkan()` 返回值，main 固定返回 0；句柄成员未显式初始化，失败后仍进入无条件销毁；类的默认复制可能造成重复销毁（当前入口没有复制操作，尚未触发）。已提供局部修正方向，用户负责实现。Vulkan 创建失败后的输出句柄不可据此视为有效对象，已核对官方返回码与句柄规则。此次助手未构建、运行或修改课程源码，只维护教学记录；尚无用户的失败路径运行和理解反馈。

2026-11-21 恢复学习：用户要求继续。助手只读确认当前 Vulkan 实现尚未开始、已有构建接入保留，恢复 Instance 创建与销毁的小步教学，并核对 Khronos 的结构体、创建与销毁说明。本次只维护教学文档，未修改源码或构建脚本，也未构建或运行用户的 Vulkan 程序；初始化实现和学习验收仍待用户完成。

2026-11-21 重启命令纠正：用户指出 `:LspRestart` 不存在。助手核对本机 Neovim 0.12.6 的帮助和插件源码，并在不加载用户配置的 headless 实例中确认内置 `:lsp` 存在。`nvim-lspconfig` 检测到该内置命令时会跳过旧命令注册，因此此前给出的 `:LspRestart` 不适用，已纠正为 `:lsp restart clangd`，并同步开发文档。尚未操作用户正在使用的编辑器会话，界面补全效果待反馈。

2026-11-21 Neovim 后续排查：用户反馈仍报错。只读确认用户已创建正确的 `.clangd`、为 `vle_core` 添加源文件及 PUBLIC Vulkan 依赖，并修正语言服务参数；但编译数据库仍为 19:23 的旧版本。助手对实际包含 Vulkan 的 `IceVulkan.hpp` 运行 clangd 检查，复现 `vulkan/vulkan.h file not found`。随后仅执行 `cmake -S . -B build/vulkan-windows-debug`，配置生成成功；新数据库含 `IceVulkan.cpp`，三个编译条目均含 Vulkan include 路径。再次检查该头文件，clangd 自动读取项目 `.clangd` 和新数据库，结果为 0 个错误。已提示用户在 Neovim 中执行 `:LspRestart`；编辑器界面的诊断消失及补全效果仍待用户反馈。助手没有修改源码、构建脚本或编辑器配置，也没有执行 Vulkan 初始化程序。

2026-11-21 Neovim 补全排查：用户反馈 Vulkan include/符号提示缺失。助手只读确认 Vulkan SDK 头文件存在，主工程数据库已包含 Sandbox 的 Vulkan include 路径；LazyVim 通过 `clangd-msvc.cmd` 使用 clangd 23.1.5，但没有指定嵌套的主工程数据库目录。使用该入口对现有 `samples/sandbox/main.cpp` 执行 `--check`：默认找不到数据库并产生 2 个错误；显式指定 `build/vulkan-windows-debug` 后加载正确的 MSVC/include 参数，检查为 0 个错误。另发现 `vle_core` 尚无 Vulkan 依赖、新建 `IceVulkan.cpp` 不在 target 中，以及 Neovim 日志里的 `--function-arg-placeholders` 无效值告警。已提供项目 `.clangd`、按实际源码归属接入 target、修正参数的提示，见 [开发环境](DEVELOPMENT.md#编辑器读取构建信息)。助手未修改源码、构建或编辑器配置；用户应用修正后的实际补全结果待反馈，不据此标记 Vulkan 初始化完成。

2026-11-21 课程调整时，助手在当前 Windows 会话运行 `vulkaninfo --summary`，退出码 0：Instance Version 1.4.335，`VK_LAYER_KHRONOS_validation` 与 `VK_EXT_debug_utils` 可枚举；输出含 Intel Arc 140T（API 1.4.335）和 NVIDIA RTX PRO 2000 Blackwell Generation Laptop GPU（API 1.4.329）。Intel 有重复枚举条目，原因未调查，不将条目数解释为物理显卡数量。另确认 `glslc.exe` 位于 SDK 1.4.335.0 目录。上述只是环境查询，不是项目 Vulkan 初始化、渲染或 feature 验证。

2026-11-21：用户修正练习预设后反馈配置、构建、运行输出均符合预期。只读检查新构建目录确认 CMake 4.2.3、MSVC `19.44.35222.0`（工具集 `14.44.35207`）、x64、Ninja、Debug；这与下方另一历史记录中的工具集版本不同，以当前缓存为准。本轮助手只执行了预设列表查询，未重新编译或运行程序，也未修改环境变量。

此前的实现与运行反馈（2026-11-15）：用户提供了两次 Windows Git Bash 中的练习构建与运行日志，名称分别为 `IceShell` 和 `IceShellEngine`，退出码均为 0。助手只读核对源码，确认 `engine_name()` 已返回 `"IceShellEngine"`；当次未重新构建、运行或修改课程代码。L00.1 的实现与运行验收通过，L00.2 的名称变体与增量构建观察已完成；其余学习证据见下文。

最新环境操作：用户要求为 Windows 的 Git Bash 和 PowerShell 配置系统路径。已在 `D:\dx12_code\IceShell` 所在机器执行 `tools/configure-msvc-system.ps1 -Apply`，备份并写入 Machine Path、INCLUDE、LIB、LIBPATH，读回确认成功。备份：`build/environment-backups/msvc-system-20261114-235517-086.json`。本机选择 MSVC x64 `19.43.34810.0`（工具集 `14.43.34808`）、Windows SDK `10.0.26100.0`、Ninja `1.12.1`。

2026-11-14 的环境准备中，仅使用读回的持久环境，在 PowerShell 与 Git Bash 登录 shell 中分别完成根目录参考工程的全新 CMake + Ninja / Debug 配置、编译、链接、运行，均退出 0，输出未见 warning/error。构建目录为 `build/windows-env-powershell-20261114` 与 `build/windows-env-gitbash-20261114`，均生成编译数据库。当时未复现历史 ABI 探测停滞；不据此宣称已定位其历史原因。2026-11-15 用户已提供 Git Bash 中练习工程的成功构建与运行证据，无需重复环境配置；用户亲自运行 PowerShell 构建的证据尚未提供。

此前环境准备仅配置工具和维护文档，未修改课程源码、shader 或构建脚本；当时未审查用户已修改的 `practice/src/engine_info.cpp`。本次已完成只读审查。下文 Linux 和早期 Windows 记录均为历史证据；续学时先读取最新代码并核对平台。

旧的“任务 C → 基础自检 → L01”衔接顺序已按用户要求取消。当前顺序以本文开头和 [路线图](ROADMAP.md) 的 Vulkan 主线为准；下文早期实践记录仅保留当时证据。

## 当前学习入口

**L04.1 的实例扩展练习已讲解，用户在动手前暂停。** Instance 创建、销毁与缺层诊断已有用户运行反馈。L00 已按用户要求结束，保留已有实践证据。

- 当前检出目录使用 Windows x64、MSVC `19.43.34810.0`、Ninja 和 Debug 配置。CMake 找到 Vulkan SDK 头文件版本 `1.3.296`，`vulkaninfo --summary` 报告实例版本 `1.4.341`。2026-11-21 在另一工作目录核对的 MSVC 版本为 `19.44.35222.0`。
- L00 练习目录为 `practice/`，保留原样；第一份 `CMakeLists.txt` 是助手按明确请求提供的参考，C++ 与预设由用户实现。Vulkan 从主工程 Sandbox 开始，必要接入修改见 L04.1。
- 只读审查：声明、定义和调用匹配；`engine_name()` 返回指向字符串字面量 `"IceShellEngine"` 的视图，生命周期有效；main 输出并返回 0。
- 用户首次日志包含 4 步：编译实现、归档静态库、编译 main、链接程序；输出 `IceShell`，退出码 0。变体日志包含 3 步：仅编译实现、归档库、重新链接；输出 `IceShellEngine`，退出码 0。两份日志均未见 warning/error。
- 用户正确解释了 main 的源文件和所含头文件未变，因此其目标文件可复用；引擎实现变更导致静态库更新，最终程序需要重新链接。补充：Ninja 也会因实际编译命令变化或输出缺失等原因重建，触发条件不只有源码内容变化。
- 当前预设已使用新目录 `build/lesson00-windows-debug`，缓存指向本机 `practice/`。此前名称不符的 `build/lesson00-linux-debug` 仅作为历史记录，不搬动旧缓存。
- 本轮课程调整只修改文档、读取现有代码并查询 Vulkan 环境；没有创建课程源码、shader 或构建实现。后续由用户亲自实现 Vulkan API 路径。

## 当前结论

用户已实现 Instance 创建与销毁、验证层请求、唯一所有权、实际错误码日志和验证层预检查。实例扩展查询留待下一步实现。Vulkan 生命周期与配置取舍的理解反馈单独记录。

用户已确认熟悉 C++，刚开始学习 Vulkan。讲解按这一基础安排，重点放在图形概念、显式资源管理、同步和系统设计。

用户进一步明确：**代码需要由用户亲自实现。** 助手负责讲解、任务拆解、提示、代码审查和调试指导，默认不修改源代码、shader、构建脚本或测试实现。教学文档和进度由助手维护。

新增备忘：核心环节主动提供关键代码提示；外部资产由助手按需获取并记录来源/许可；尽量不用 Visual Studio IDE，优先 VS Code 或 Vim，命令行 CMake + Ninja。资产准备不替代用户实现引擎导入与资源管线。

## 已准备的参考工程与资料

- [x] `AGENTS.md`：逐课推进方式、技术方向、验证与文档约定。
- [x] 路线图：2026-11-27 重组为主线 A–H（含 RHI R1–R5、UI U1–U3、GPU 物理 G1）、扩展清单与暂缓项，含依赖关系与验收标准。
- [x] 架构方向：跨平台、渲染能力分级、场景与子系统边界。
- [x] C++20 静态库 `vle_core` 与示例程序 `vle_sandbox`。
- [x] Windows、Linux、macOS 的 CMake 预设。
- [x] 第 0 课讲解、运行命令、可选练习和自检问题。
- [x] Windows 上从新构建目录配置、编译、链接、执行。
- [x] 依用户要求切换为“用户亲自实现、助手教学审查”，补充 46 课逐课时间和阶段累计时间。
- [x] 补充开发环境、资产获取约定；重写 L00 的命令行教学和局部代码提示。此次只修改教学文档，未修改课程代码或构建脚本。

初始参考工程仅包含控制台骨架；用户现已增加 Instance 创建与销毁初版，状态以上方审查记录为准。窗口、其他引擎子系统及 SDL3 等配套第三方库尚未接入。当前目录已是 Git 仓库，早期检查时 HEAD 为 `38f4418 first commit`；助手未创建提交或修改远程配置。

## 本次用户实践与学习证据（2026-11-21 至 2026-11-22）

| 学习证据 | 当前状态 |
| --- | --- |
| 用户亲自实现并运行 | 预设已由用户创建并修正；用户反馈配置、构建、程序名称与退出码均符合预期；助手确认新目录中已有构建文件、编译数据库与程序 |
| 用户解释关键数据流与取舍 | 此前增量构建解释已通过；任务 A 两个基础问题及 PUBLIC/PRIVATE、string_view 自检按要求跳过，不再待答 |
| 用户完成小变体或故障诊断 | 在提示后修正 `binaryDir` 的 `$` 与 `/`；任务 C 链接实验未执行，已按用户要求跳过 |
| 参考示例与独立实现的区别 | 预设由用户组织与修正；助手提供字段提示和只读审查，没有代写本次预设 |
| Vulkan 用户实现与运行 | 用户实现了 `IceVulkan` 并补齐错误码日志，助手已核对源码。用户确认正常创建和销毁、缺层时安全退出均符合预期。 |
| Vulkan 理解与取舍 | 尚未收到应用信息与 Instance 生命周期、layer 配置及资源唯一所有权的解释，不标记已掌握 |
| Vulkan 变体与诊断 | 用户已运行并确认直接创建时的缺层变体。接入枚举后，助手另行验证了用户的新实现能点名缺失的层并在创建前退出。 |
| Vulkan 参考提示与独立实现 | 助手提供局部 API 提示；用户自行选择类封装并实现源码，助手本次只审查和维护文档 |

## 用户实践与学习证据历史（2026-11-15）

以下为当时状态。历史记录中的未做基础项已按 2026-11-21 的要求跳过，不作为当前待办。

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

当时计划在 L00.2 完成预设、故障实验和自检，再从 L01 演进主工程；这一前置顺序已于 2026-11-21 按用户要求取消，当前直接从主工程进入 L04.1。

## 下一课的明确范围

恢复后继续 L04.1，先做实例扩展查询，随后推进其余能力查询、messenger 与设备枚举。继续使用 Vulkan C API，暂定 1.3 基线。GLSL/shader 编译在 L06 引入。

1. 理解 SDK/header、loader、Instance、PhysicalDevice 与未来 Device 的职责区别。
2. Vulkan 构建依赖接入已由用户完成，编译数据库也已更新；沿用现有配置，不重复安排。
3. 查询实例版本和可用 layers/extensions，启用 validation 与 debug_utils，创建 Instance 和 messenger。
4. 枚举设备、打印名称/类型/API 版本，处理实际 `VkResult` 和对象销毁顺序。
5. 通过应用主动提交的 INFO 消息确认回调接线，观察 INFO 过滤变体；保留真实 warning/error。

验收来自用户自己的程序、运行日志和 Vulkan 对象解释。完成后再在 L04.2 引入 SDL3 窗口和 Surface，L04.3 创建逻辑设备与队列；不以窗口框架或通用日志系统的完成作为 L04.1 前提。

## 学习反馈与待确认事项

- 学习基础与最新偏好：具备 C++ 经验，直接推进 Vulkan；不再单独提编译链接基础问题，除非用户询问，或当前功能需要具体修改。
- L00 已有实践与运行证据保留；未做基础问答和链接故障实验已明确跳过。不能因它们未验收而退回 L00，也不能将跳过记为已验证掌握。
- 学习方式已确认：用户写代码，助手教学与审查；用户明确要求时才提供具体参考实现。
- 核心代码提示已授权，助手应主动提供局部片段；整课实现仍由用户组织。
- 资产获取分工已确认，当前无外部资产下载需求。
- 编辑器偏好已确认：VS Code 或 Vim，避免依赖 Visual Studio IDE，命令行构建优先。
- 当前 shell 已由最新日志确认：Windows Git Bash；使用 Bash 语法和 Windows 程序的 `.exe` 后缀，构建工具链为 MSVC + Ninja。
- 2026-11-27 重组后主线粗估 370–592 小时，约 20% 余量后为 445–710 小时；此前的 508–804 / 610–965 小时不再用于当前排期。估计不代表已投入时间或掌握承诺，L06 后依据实际学习反馈重估。
- 每周可投入时间尚待反馈；L06 后依据真实学习记录调整估计。
- 当前使用 Windows 本机开发；此前 Linux 验证独立保留。Windows/Linux 均为优先桌面目标，macOS 仍待评估兼容性。
- 引擎名称暂为 Vulkan Learning Engine，可随时更名。

以后每课继续补充真实验证证据与学习反馈，保持“已实现”“已验证”“待学习”三者分开。
