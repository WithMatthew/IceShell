# Vulkan Learning Engine

一个通过逐课实践，从零搭建的 Vulkan 跨平台游戏引擎。学习目标覆盖场景、资源、光栅化、材质、物理、动画、音频、mesh clustering 与光线追踪。

当前实现为**第 0 课：C++20 + CMake 工程骨架**，包含独立静态库与 Sandbox 可执行程序。窗口和 Vulkan 渲染从后续课程逐步加入；当前运行结果是控制台输出。

**学习方式：代码由你亲自实现；助手负责原理讲解、任务拆解、核心代码提示、代码审查和调试指导。** 关键环节主动提供函数签名、局部调用或 TODO 片段；整课参考实现按你的明确请求提供。初始化生成的第 0 课骨架属于参考示例，其构建通过不代表你已经完成该课。

开发主线采用 **VS Code 或 Vim + 命令行 CMake/Ninja**。Windows 可使用现有 MSVC 编译器，无需打开 Visual Studio IDE；Linux 使用 GCC 或 Clang。助手按课程需要获取外部资产并记录许可与来源。

## 从这里开始

1. 阅读 [第 0 课：构建第一个程序](docs/lessons/00-project-foundation.md)，亲自完成实现任务并运行。
2. 浏览 [完整教学路线](docs/ROADMAP.md)，查看 46 课名称、估计学习时间和依赖关系。
3. 用 [学习进度](docs/PROGRESS.md) 衔接下一次学习；说“继续”时，助手继续讲解或布置任务，由你完成代码。

[开发环境](docs/DEVELOPMENT.md) 给出工具准备与编辑器配置方向；[资产记录](docs/ASSETS.md) 说明获取分工；[架构与技术选择](docs/ARCHITECTURE.md) 解释模块边界；[AGENTS.md](AGENTS.md) 规定后续协作方式。

## 构建与运行

第 0 课只需要 CMake 3.25+ 和支持 C++20 的编译器，无需下载第三方库，也不要求安装 Vulkan SDK。进入 Vulkan 课程时再检查 SDK、驱动和设备能力。

### 命令行主线

先按 [开发环境](docs/DEVELOPMENT.md) 在终端准备编译器与 Ninja。根目录参考工程使用以下跨平台构建命令：

```sh
cmake -S . -B build/dev-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/dev-debug
```

Windows 运行 `./build/dev-debug/bin/vle_sandbox.exe`；Linux/macOS 运行 `./build/dev-debug/bin/vle_sandbox`。切换生成器或工具链时另选构建目录。L00 用户练习的 source/build 路径不同，使用课文命令。

当前 Windows 上已确认 MSVC/Ninja 可发现，但本次 Ninja 配置在编译器 ABI 探测处停滞，已停止该次验证，尚无完整成功结果。它不是源代码错误的结论，也未验证 Linux/macOS 运行。实际状态见进度文档。

### 已有参考预设

以下预设保留初始化时的状态，由用户在 L00 中理解并按需要调整。Windows Visual Studio generator 预设曾通过命令行构建，不要求启动 IDE，但后续课程主线使用 Ninja：

```powershell
cmake --preset windows-vs2022
cmake --build --preset windows-debug
.\build\windows-vs2022\bin\Debug\vle_sandbox.exe
```

此旧预设由 CMake 查找 Visual Studio 工具集。Release 构建使用 `cmake --build --preset windows-release`，程序位于同目录的 `bin/Release/`。它与 Ninja 的编译器环境准备方式不同。

### Linux

需要支持 C++20 的 GCC 或 Clang，以及 CMake、Ninja，并将工具加入 PATH：

```sh
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/linux-debug/bin/vle_sandbox
```

### macOS

需要 Xcode Command Line Tools、CMake 与 Ninja：

```sh
cmake --preset macos-debug
cmake --build --preset macos-debug
./build/macos-debug/bin/vle_sandbox
```

以上 Linux/macOS 命令是待实机验证的入口。当前阶段只验证平台无关的 C++ 骨架；未来 Vulkan-on-Metal 的选择见架构文档。

预期输出：

```text
Vulkan Learning Engine
Lesson 00: build and link succeeded.
Next: Lesson 01 - window and event loop.
```

## 目录

```text
AGENTS.md                 教学与开发协作约定
CMakeLists.txt            根构建入口
CMakePresets.json         各平台构建预设
engine/
  include/vle/core/       引擎公共接口
  src/core/               引擎实现
samples/sandbox/          持续演进的教学程序
docs/
  ROADMAP.md              阶段、课程与验收标准
  ARCHITECTURE.md         模块职责、能力分级与技术选择
  DEVELOPMENT.md          命令行工具链与编辑器无关的工作流
  ASSETS.md               资产获取约定、来源与下载记录
  PROGRESS.md             已验证内容、待办与下一课
  lessons/                逐课讲解与练习
```

随着课程推进再增加 `shaders/`、`assets/`、`tests/` 和 `tools/`，避免空模块掩盖实际实现进度。生成文件统一放在 `build/`；本机个性化配置放在不进入版本控制的 `CMakeUserPresets.json`。

跨平台构建配置采用 [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)。各平台实际验证情况以进度文档为准。
