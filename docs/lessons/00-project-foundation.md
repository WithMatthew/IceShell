# 第 0 课：用命令行建立 C++ 工程（历史与按需参考）

**2026-11-21 课程调整：本课不再是必修前置。** 用户已有 C++ 经验，明确要求直接学习 Vulkan。已完成的实现、预设和运行证据保留；未做的链接故障实验、预设问答、PUBLIC/PRIVATE 与 string_view 自检按要求跳过。下文保留旧教学材料供用户主动查阅，不是当前待办。当前入口为 [L04.1：Vulkan Instance、验证层与设备枚举](04-vulkan-bootstrap.md)。

本课目标：你亲自编写一个静态库和调用它的程序，通过 CMake + Ninja 构建，在 VS Code 或 Vim 中编辑。先学会表达源码和依赖的关系，后续再把窗口、Vulkan 和其他系统加入同一套构建流程。

预计整课 3–5 小时。L00.1 是环境检查与最小工程；L00.2 是预设整理、依赖故障实验和自检。时间用于规划，不作为掌握标准。

初始化时助手生成的根目录骨架仅作参考。以下练习由你亲自创建，助手提供关键提示并审查，不替你写入课程代码。

前次验收（2026-11-15）：日志确认环境为 **Windows x64 / Git Bash / MSVC / Ninja / Debug**。用户已补齐返回值，两次运行分别输出 `IceShell` 和 `IceShellEngine`，退出码均为 0；名称变体只重编译实现、归档库并重新链接，用户已正确解释原因。**L00.1 实现与运行验收通过，L00.2 任务 B 已完成。** 下文运行命令按 Git Bash 环境给出，Linux 验证记录见进度文档。

本课结束记录（2026-11-21）：用户已创建练习预设、修正路径并反馈运行输出正确；助手确认两个预设可列出，新目录已有程序与编译数据库。任务 A 的实现运行与任务 B 已有证据，其余基础练习按用户要求跳过；没有将跳过记为已掌握。完整记录见 [学习进度](../PROGRESS.md)。

## L00.1：先理解工具关系

```text
你在 VS Code / Vim 中编辑 C++ 与 CMakeLists.txt
                 ↓
CMake 读取 target 和依赖，生成 build.ninja
                 ↓
Ninja 按依赖顺序调用编译器与链接器
                 ↓
MSVC / GCC / Clang 工具链生成本平台程序
```

编辑器负责修改文件，CMake 描述并生成构建关系，Ninja 执行构建，编译器与链接器生成程序。切换编辑器不会改变工程的编译规则；切换平台则要重新选择工具链、配置并构建。

Windows 当前可以直接使用已有 MSVC 命令行工具链，无需打开 Visual Studio IDE。Linux 使用 GCC 或 Clang，公共源代码与 CMake 描述继续复用。

## 第一步：确认工具

本课只需要 C++20 编译器、CMake 3.25+ 和 Ninja。当前 Windows 系统级持久配置已完成，用户已成功构建运行，无需重复安装。需要排查工具路径时可在 Git Bash 复查：

```bash
cmake --version
ninja --version
command -v cmake ninja cl
```

2026-11-21 当前练习缓存与编译数据库确认使用 MSVC `19.44.35222.0`、工具集 `14.44.35207`、x64、Ninja 和 Debug。`MINGW64` 是 Git Bash 的提示符，不代表本次使用 GCC。此前 Linux 的 GCC/clangd 验证独立保留；clangd 是语言服务，不决定构建编译器。

本课没有 Vulkan 调用。详细工具版本、平台验证与持久配置见 [开发环境说明](../DEVELOPMENT.md)。

跨平台只复用源码与构建描述；CMake 缓存、目标文件和程序必须在本平台重新生成。此前用户使用的目录虽名为 `build/lesson00-linux-debug`，当时缓存实际为 Windows/MSVC/Ninja/Debug，名称不决定平台。下文第三步保留该历史命令；任务 A 后来采用 `build/lesson00-windows-debug`，未搬动旧缓存。任务 C 已按用户要求跳过。

## 第二步：完成已有的四文件练习

你的独立练习已经采用以下结构，继续编辑这些文件即可：

```text
practice/
  CMakeLists.txt
  include/vle/engine_info.hpp
  src/engine_info.cpp
  src/main.cpp
```

这是实际采用的 L00 练习目录，保留为历史记录。第一份 `practice/CMakeLists.txt` 由助手按明确请求补全并添加注释，C++ 实现与后来的预设由用户完成。调整后的课程直接在主工程开展 L04.1，不再布置迁移结构或构建语法练习。

你需要实现：

1. 头文件声明 `vle::engine_name()`，返回 `std::string_view`。
2. 在单独的 `.cpp` 中定义它，返回指向静态字符串字面量的视图。
3. `main.cpp` 通过头文件调用这个函数，将名称输出到终端，然后正常退出。
4. CMake 声明 C++ 项目，创建 `vle_core` 静态库与 `vle_sandbox` 可执行 target，使后者链接前者，并要求 C++20。

2026-11-15 只读审查与用户构建反馈：

| 文件 | 当前状态 | 你接下来要做的事 |
| --- | --- | --- |
| `practice/CMakeLists.txt` | 已有带注释的参考配置；用户 Windows 构建成功 | 自检两个 target 和三条依赖关系 |
| `practice/include/vle/engine_info.hpp` | 已包含 `<string_view>`，函数声明正确 | 保留接口，理解它只声明、不提供函数体 |
| `practice/src/engine_info.cpp` | include 正确；已返回 `"IceShellEngine"`，与变体输出一致 | 自检 string_view 的非拥有语义与生命周期 |
| `practice/src/main.cpp` | 已调用接口、输出并返回 0；两次运行符合预期 | 进入预设与链接故障实验 |

此前构建的首个错误是 `engine_info.hpp: No such file or directory`，发生在源码预处理阶段，现已修正。编译命令已有 `-I.../practice/include`，实际头文件在 `include/vle/`，可以包含：

```cpp
#include <vle/engine_info.hpp>
```

可以把查找关系理解为 `搜索根目录 practice/include` + `vle/engine_info.hpp`。仅把双引号换成尖括号不会补出缺少的 `vle/`。不要包含 `.cpp` 来绕过库的编译与链接关系。

此前 Linux review 在新目录 `build/lesson00-linux-review` 配置、编译和链接完成，但当时 GCC 报告：

```text
warning: no return statement in function returning non-void [-Wreturn-type]
```

这个问题现已修正。保留诊断原理：`engine_name()` 返回 `std::string_view`，执行到函数末尾却没有返回值会产生未定义行为，并不会自动构造一个空视图。当时的“空输出”不能用来推断该错误每次都会输出空白。编译链接通过、程序能退出和行为正确需要分别验证。[C++ return 规则](https://eel.is/c++draft/stmt.return#4)

用户已在函数体中返回名称字面量，先验证 `IceShell`，再改为 `IceShellEngine` 验证变体；两次日志均未见 warning/error，运行退出码均为 0。本次助手只读核对源码与日志，没有重复执行程序。

## 核心代码提示

接口可以从以下声明开始；头文件的必要 include 和命名空间周边由你组织：

```cpp
namespace vle {
    [[nodiscard]] std::string_view engine_name() noexcept;
}
```

实现中需要注意 `string_view` 不拥有字符串内存。字符串字面量的生命周期适合本例；不要返回指向局部 `std::string` 数据的视图。

定义的局部提示如下，由你放进实现文件的 `namespace vle`，并补上返回值：

```cpp
std::string_view engine_name() noexcept
{
    // TODO：返回指向你选定名称的字符串字面量的视图。
}
```

`engine_name()` 没有输入，输出的是非拥有的字符视图；字面量具有静态存储期，无需调用方释放。本例不需要动态分配、单例或复杂引擎类。TODO 未完成时不要运行这段代码。

`main.cpp` 的算法步骤：包含引擎公开头文件和 `<iostream>` → 进入 `main` → 调用接口 → 输出名称和换行 → 返回 0。核心调用提示：

```cpp
const auto name = vle::engine_name();
// TODO：将 name 和换行写入 std::cout。
```

在你用 `add_library` 和 `add_executable` 创建 target 后，三条关键关系如下：

```cmake
target_compile_features(vle_core PUBLIC cxx_std_20)
target_include_directories(vle_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(vle_sandbox PRIVATE vle_core)
```

第一条声明库的 C++ 标准要求并传给使用者；第二条让库自身及使用者都能找到公开头文件；第三条声明程序使用库，使所需使用条件传入程序，并让库参与链接。`PRIVATE` 表示这条依赖不作为程序的对外使用接口传播。

这里的 `target` 是 CMake 管理的构建目标：`vle_core` 产出静态库，`vle_sandbox` 产出程序。库的 `PUBLIC` 条件会被程序消费，即使程序用 `PRIVATE` 链接它；这个 `PRIVATE` 不会阻止程序接收库的公开条件。[头文件条件的作用域](https://cmake.org/cmake/help/latest/command/target_include_directories.html)、[链接与使用条件传递](https://cmake.org/cmake/help/latest/command/target_link_libraries.html)

第一份完整 CMake 参考见 [practice/CMakeLists.txt](../../practice/CMakeLists.txt)。阅读时依次确认 `cmake_minimum_required`、`project`、`add_library`、`add_executable` 和三条 target 关系各自负责什么。练习暂不设置自定义可执行输出目录，也不添加平台原生 API；保持问题规模足够小。

## 第三步：从项目根目录构建

此前已通过这组构建与运行验收。需要在当前检出目录复查时，在 Windows 的 Git Bash 中从项目根目录执行：

```sh
cd /d/Code-dx12
cmake -S practice -B build/lesson00-linux-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/lesson00-linux-debug
./build/lesson00-linux-debug/vle_sandbox.exe
echo $?
```

逐条执行，上一条成功后再执行下一条。这组路径对应当前 Windows 单目录练习；根目录参考工程另有构建目录和 `bin/` 输出子目录，两者不要混用。未来切回 Linux 时使用新的平台构建目录，程序没有 `.exe` 后缀。

`-S` 指向源码，`-B` 指向生成文件，`-G Ninja` 选择构建方式。这里的 Ninja 是单配置生成器，所以 Debug 在配置命令中选择。

预期结果：终端输出你选定的引擎名称，程序退出码为 0；构建目录内生成 `build.ninja` 和 `compile_commands.json`。名称不必与参考程序一致。

当前名称为 `IceShellEngine`，最后两条命令应分别输出 `IceShellEngine` 和 `0`。MSVC 静态库位于 `build/lesson00-linux-debug/vle_core.lib`。

`build.ninja` 和 `compile_commands.json` 是配置与生成的产物；它们本身不证明源码编译、链接或运行成功。本次另有用户构建与运行日志作为证据。编译数据库记录每个源文件的实际编译参数，供 clangd 等工具读取。[CMake 编译数据库说明](https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html)

需要定位错误或观察编译参数时，可用：

```sh
cmake --build build/lesson00-linux-debug --verbose
```

## 第四步：理解头文件与链接

```mermaid
flowchart LR
    Header[头文件提供声明] --> Main[编译 main.cpp]
    Implementation[编译 engine_info.cpp] --> Library[vle_core 静态库]
    Main --> Link[链接器]
    Library --> Link
    Link --> Program[vle_sandbox]
```

`#include` 让编译器看到声明，库提供函数的定义。能够找到头文件和能够链接到实现是两个独立条件。

在当前 MSVC 构建中，两个 `.cpp` 分别编译成 `.obj`；`engine_info.cpp.obj` 被 `lib.exe` 归档为 `vle_core.lib`，`link.exe` 用 `main.cpp.obj` 和库生成 `vle_sandbox.exe`。Linux 的对应文件通常是 `.o`、`libvle_core.a` 和无后缀程序。静态库归档并不要求整个程序的符号已经齐全，程序链接才会检查被调用函数是否有匹配定义。

| 阶段 | 关注什么 | 本课可观察的例子 |
| --- | --- | --- |
| CMake 配置与生成 | 编译器、target、依赖和生成规则 | 当前已成功；产出 Ninja 规则和编译数据库 |
| 预处理与编译 | 头文件可见、声明和语法正确；查看诊断 | include 和 return 已补齐；两份用户日志均未见 warning/error |
| 链接 | 被使用的符号有匹配定义 | 用户日志显示链接成功；移除整个定义的实验预期触发 MSVC 的未解析外部符号错误 |
| 运行 | 程序实际行为、退出码 | 用户分别得到 IceShell、IceShellEngine；两次退出码均为 0 |

完成后自检：

1. 你能说明 CMake、Ninja、编译器和编辑器各自做什么吗？
2. 为什么程序不必手动再配置一遍引擎的 include 路径？
3. 如果声明可见、库成功生成，但其中缺少 `engine_name` 的定义，错误会在哪个阶段出现？
4. 为什么返回字符串字面量的 `string_view` 安全，返回局部 `std::string` 的视图不安全？
5. 在 Windows/Linux 之间切换时，为什么需要重新生成构建目录，而不必重写这份公共 C++ 接口？

把你的实现、运行结果以及一个不确定点交给助手审查；你也可以直接说明练习所在路径，让助手只读检查。无需一次猜对所有配置细节，核心环节会提供代码提示。

## L00.2：完成首次运行后再做

本段是旧的预设与故障教学材料。用户已完成任务 A 的实现与运行，以及任务 B 的名称变体与解释；任务 C 与基础问答已跳过，不再安排后续补做。

### 任务 A：把命令整理成预设

由你创建 `practice/CMakePresets.json`。预设保存配置参数，target 与源文件关系仍由 `CMakeLists.txt` 表达。configure preset 用来生成构建系统，build preset 指向一个 configure preset 以选择要构建的目录。[CMake 3.25 预设说明](https://cmake.org/cmake/help/v3.25/manual/cmake-presets.7.html)

关键字段提示如下，由你组织成完整 JSON：

| 位置 | 字段 / 值 |
| --- | --- |
| 根对象 | `version: 6`，与项目 CMake 3.25 最低要求兼容 |
| `configurePresets` 中的一个对象 | `name: "lesson00-debug"`、`generator: "Ninja"` |
| 同一配置预设 | `binaryDir: "${sourceDir}/../build/lesson00-windows-debug"` |
| 同一配置预设的 `cacheVariables` | `CMAKE_BUILD_TYPE: "Debug"`、`CMAKE_EXPORT_COMPILE_COMMANDS: "ON"` |
| `buildPresets` 中的一个对象 | `name: "lesson00-debug"`、`configurePreset: "lesson00-debug"` |

`${sourceDir}` 在这里是 `practice/`。新 binaryDir 明确标记 Windows，并重新配置，不重命名旧缓存。不要写入本机绝对路径；JSON 属性名需带双引号，不支持普通注释。根目录的现有预设可作为语法参考，但对应的是参考工程。

创建后从练习目录运行：

```sh
cd /d/Code-dx12/practice
cmake --list-presets=all
cmake --preset lesson00-debug
cmake --build --preset lesson00-debug --verbose
../build/lesson00-windows-debug/vle_sandbox.exe
echo $?
```

验收：预设可列出、配置构建成功、输出与此前一致；可以解释 configure preset 和 build preset 的分工。

任务 A 的两项历史自检（已取消必答）：

1. `configurePreset` 字段起什么作用？
2. 如果要给 `vle_core` 新增一个 `.cpp`，应该修改哪个文件？

2026-11-21：用户已修正 `binaryDir` 缺少 `$` 与 `/` 的问题，并反馈配置、构建、运行名称与退出码均符合预期。助手只读确认预设正确、两个预设可列出、编译数据库与程序存在；运行结果由用户反馈，未重复构建运行。任务 A 的实现与运行验收通过，两个基础问答后来按用户要求跳过。

### 任务 B：观察增量构建

**已完成（2026-11-15）**：用户在原练习构建目录把名称从 IceShell 改为 IceShellEngine，日志由 4 步变为 3 步，跳过 main 编译；运行输出变更且退出码为 0。用户已正确解释原因。以下保留操作步骤供复习，不要求重复完成。

只修改 `engine_info.cpp` 返回的名称字面量，保留头文件和 main 不变。从 `practice/` 执行：

```sh
cmake --build --preset lesson00-debug --verbose
../build/lesson00-windows-debug/vle_sandbox.exe
```

预期重新编译 `engine_info.cpp`、重新归档库、重新链接程序；`main.cpp` 没变，通常不需要重新编译。记录实际输出，解释“程序要重新链接”与“main 要重新编译”的区别。文件不变时再构建一次，通常会看到 `ninja: no work to do.`。

补充：源码/头文件变化不是唯一重建原因。Ninja 根据依赖、文件时间戳及记录的命令判断产物是否需要更新；实际编译命令改变（例如编译选项改变）或目标文件缺失也会触发重建。这里 main 的依赖和编译命令未变且目标文件已存在，因此复用其 `.obj`。[Ninja 手册](https://ninja-build.org/manual.html)

### 任务 C：制造并恢复链接错误（已跳过，仅保留参考）

默认不执行以下实验；仅在用户单独要求回顾编译链接时使用。

先预测错误阶段，再在 `engine_info.cpp` 临时注释掉 **整个 `engine_name()` 定义（签名和函数体）**。保留 `.cpp` 文件、头文件声明、main 中的调用以及原有 CMake 关系。不要只删掉 return，否则会回到本次的未定义行为问题。

执行同一条构建命令，预期库可以归档、程序链接报未解析外部符号；当前 MSVC 通常是 `LNK2019`，随后 `LNK1120` 汇总，Linux 工具链常见措辞为 `undefined reference`。解释“声明仍在，为什么链接还失败”，然后恢复定义，重新构建运行，确认名称输出与退出码恢复正常。失败后先恢复并成功构建，再运行，避免误执行遗留程序。[MSVC LNK2019 说明](https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-error-lnk2019?view=msvc-170)

可选拓展：临时移除 `target_link_libraries` 的关系。注意这可能先导致传递的 include 路径消失；如要专门观察链接错误，实验时单独保留头文件搜索路径。每次只改变一个因素，结束后恢复原有配置。

课程调整后从主工程直接进入 L04.1；保留 `practice/` 作为历史练习，不再要求先完成本节实验或重做工程组织练习。

每次故障实验只改一个因素。L00 的验收是用户亲自完成构建、能解释依赖传递、能定位并恢复一次故障；助手先前的参考工程构建不计作这一学习证据。

历史学习记录：用户实现运行、预设、名称变体和路径修正已有证据，增量构建解释正确。其余基础自检与任务 C 按用户要求跳过，不自动判定为掌握，也不作为后续 Vulkan 学习的门槛。

## 下一课

直接进入 [L04.1：Vulkan Instance、验证层与设备枚举](04-vulkan-bootstrap.md)。最小 SDL3 窗口在 L04.2 接入 Surface 时引入。代码继续由用户实现；助手提供 Vulkan 核心 API 提示，不再布置通用编译链接测验。
