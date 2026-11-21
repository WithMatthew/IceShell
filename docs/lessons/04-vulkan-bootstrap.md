# L04.1：Vulkan Instance、验证层与设备枚举

这是 2026-11-21 调整后的当前课程入口。默认你具备 C++ 经验；本课聚焦 Vulkan 对象和调用顺序。课文与局部提示已准备，项目内的 Vulkan 实现仍由你完成，尚未通过本课运行验收。

## 本课解决什么问题

让你自己的程序接入 Vulkan，建立诊断通道，并查询本机可访问的设备。成果是一个无窗口的 Vulkan 探针：打印 loader 版本、启用的 layer/extension、设备名称与 API 版本，最后正确释放对象。Surface、逻辑设备和 GPU 提交分别在后续小节引入。

先建立这个对象模型：

| 概念 | 直觉与职责 | 本课的所有权 |
| --- | --- | --- |
| Vulkan headers / SDK | 开发时使用的接口声明与工具 | SDK 版本不代表运行时设备能力 |
| loader | 应用调用 Vulkan 的入口，连接驱动与 layers | 通过系统 loader 使用，不自行编写加载器 |
| `VkInstance` | 应用的一次 Vulkan 实例，确定 instance layers/extensions，并提供设备枚举入口 | 应用创建并销毁 |
| `VkDebugUtilsMessengerEXT` | 将诊断消息交给你的回调 | 应用创建；早于所属 Instance 销毁 |
| `VkPhysicalDevice` | 供查询的设备句柄 | 从 Instance 枚举获得，不自行销毁；使用期间 Instance 必须有效 |
| `VkDevice` | 按所选 features 和队列配置创建的逻辑设备 | L04.3 才创建；本课只做查询 |

项目暂定 Vulkan 1.3 基线。loader 的实例版本、单个设备的 API 版本和安装的头文件版本分别检查；较新的 SDK 不会替应用启用 features。[Khronos 版本说明](https://docs.vulkan.org/guide/latest/versions.html)

## 已知环境与本次边界

2026-11-21 助手执行 `vulkaninfo --summary`，返回 0：实例版本为 1.4.335，可枚举 `VK_LAYER_KHRONOS_validation` 与 `VK_EXT_debug_utils`；报告 Intel Arc 140T（API 1.4.335）及 NVIDIA RTX PRO 2000 Blackwell Generation Laptop GPU（API 1.4.329）。工具输出有重复的 Intel 条目，尚未调查其原因；不把条目数当作物理显卡数量。

这只是外部工具查询结果，不能代替你自己的初始化程序，也不能证明设备的 dynamic rendering、synchronization2、mesh shader 或光追 feature 已查询并启用。设备选择和 feature 检查在 L04.3 展开。

本课在主工程的 `samples/sandbox/main.cpp` 编写最小探针；`practice/` 保留为之前的练习记录。探针属于示例入口，暂不预先设计渲染器框架。L04.3 形成持续使用的 Instance/Device 所有权后，再把对应引擎实现移入 `engine/`。

## 本课需要的构建调整

原因只有一个：Sandbox 开始调用 Vulkan API，需要 SDK 头文件和 loader 链接库。以下是由你修改的两处局部配置，不是独立 CMake 练习：

- 根目录 `CMakeLists.txt`：在 `add_subdirectory(samples/sandbox)` 之前加入 `find_package(Vulkan REQUIRED)`。
- `samples/sandbox/CMakeLists.txt`：为已存在的 `vle_sandbox` 添加下面这一条，保留已有的 core 依赖。

```cmake
target_link_libraries(vle_sandbox PRIVATE Vulkan::Vulkan)
```

`Vulkan::Vulkan` 由 CMake 的 FindVulkan 模块提供，随 target 传入所需头文件路径与库。不要把本机 SDK 磁盘路径写进项目。[FindVulkan 官方说明](https://cmake.org/cmake/help/v3.25/module/FindVulkan.html)

本课不需要新增第三方源码依赖或 shader 编译步骤。若 SDK 查找或链接出现实际问题，再针对该问题沟通；沿用已可工作的 MSVC/Ninja 环境。

## 你要实现的调用顺序

```text
查询 loader 实例版本
  → 枚举可用的 instance layers/extensions
  → 核对并请求 validation layer 与 debug_utils extension
  → 填写应用信息、Instance 创建信息和诊断回调信息
  → 创建 Instance
  → 创建持续接收消息的 debug messenger
  → 发送一条应用自测消息，确认回调接线成功
  → 枚举 PhysicalDevice，查询并打印 properties
  → 销毁 messenger
  → 销毁 Instance
```

1. 通过 `vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion")` 查询版本函数入口；为空时按 Vulkan 1.0 loader 处理，本路线报告不满足 1.3 基线并退出。入口存在时检查版本查询的 `VkResult`，再判断版本。
2. 用 `vkEnumerateInstanceLayerProperties` 和 `vkEnumerateInstanceExtensionProperties` 查询可用项。正常全局扩展查询使用 `pLayerName = nullptr`；若扩展由特定 layer 提供，再查询该 layer。调试学习路径要求 validation 与 debug_utils 可用，缺失时报告具体名称。
3. 用 `VkInstanceCreateInfo` 明确启用 `VK_LAYER_KHRONOS_validation` 和 `VK_EXT_DEBUG_UTILS_EXTENSION_NAME`。发现支持和请求启用是两件事。
4. 创建 Instance 后，再创建 `VkDebugUtilsMessengerEXT`，接收 INFO、WARNING、ERROR，以及 GENERAL、VALIDATION、PERFORMANCE 类型的消息。保留实际 warning/error，不通过过滤错误来完成验收。
5. 用 `vkEnumeratePhysicalDevices` 获取设备列表；逐个调用 `vkGetPhysicalDeviceProperties`，至少打印 `deviceName`、`deviceType`、`apiVersion`。本课不按品牌选卡，也不选择第一个枚举条目就宣称满足渲染要求。
6. 覆盖创建中途失败的清理路径；只销毁成功创建的对象。本课没有 Device 或 GPU 工作，不需要加入 GPU 等待调用。

枚举通常先查数量，再提供数组接收结果。对返回 `VkResult` 的枚举检查状态；若填充阶段返回 `VK_INCOMPLETE`，重新查询数量并重试。最终按实际写入数量使用结果，设备数量为 0 时输出清楚的诊断。[设备枚举规则](https://docs.vulkan.org/refpages/latest/refpages/source/vkEnumeratePhysicalDevices.html)

## 核心 API 提示

先包含 `<vulkan/vulkan.h>`。创建信息从清零初始化开始，设置对应 `sType`；`pNext` 用来连接 API 规定的扩展结构，没有额外结构时为 `nullptr`。

```cpp
VkApplicationInfo app_info{};
app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
app_info.pApplicationName = "Vulkan Learning Engine";
app_info.apiVersion = VK_API_VERSION_1_3;

VkInstanceCreateInfo create_info{};
create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
create_info.pApplicationInfo = &app_info;
// TODO：填写已核对的 layer/extension 名称数组及数量。
// TODO：将已填好的 VkDebugUtilsMessengerCreateInfoEXT 接到 pNext。
```

这里的名称数组、`app_info` 和 `pNext` 指向的数据必须在创建调用期间有效。填完上述信息后，核心调用是：

```cpp
VkInstance instance = VK_NULL_HANDLE;
const VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
// TODO：检查 result；失败时输出调用名与结果，结束本条初始化路径。
```

`pCreateInfo` 是输入配置，最后一个参数接收输出句柄；第二个参数 `nullptr` 表示使用默认 host 分配方式。请求不存在的 layer 或 extension 会使创建失败，不能只检查句柄非空。[vkCreateInstance](https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateInstance.html)

诊断回调的签名提示：

```cpp
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);
```

由你实现回调：将严重级别、消息 ID 与 `callback_data->pMessage` 输出到终端，返回 `VK_FALSE`。回调中不要调用 Vulkan 命令，不向 C ABI 外传播 C++ 异常；若保存消息内容，需要复制数据。传入的 `pUserData` 若指向应用状态，其生命周期必须覆盖回调使用期。[回调约束](https://docs.vulkan.org/refpages/latest/refpages/source/PFN_vkDebugUtilsMessengerCallbackEXT.html)

为 `VkDebugUtilsMessengerCreateInfoEXT` 设置对应 `sType`、消息掩码和 `pfnUserCallback`。将其接入 Instance 创建信息的 `pNext` 可以接收 Instance 创建/销毁期间的诊断；这不会自动给你创建一个持续使用的 messenger 句柄，Instance 创建成功后仍需显式创建 messenger。[Instance 创建信息与诊断回调](https://docs.vulkan.org/refpages/latest/refpages/source/VkInstanceCreateInfo.html)

扩展函数从该 Instance 获取，局部提示：

```cpp
const auto create_messenger =
    reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
// TODO：检查函数指针，再用已填好的 debug create info 创建 messenger。
```

同样取得 `vkDestroyDebugUtilsMessengerEXT` 和 `vkSubmitDebugUtilsMessageEXT`。这些函数指针用于获得它们的 Instance；先核对与启用扩展，再取得入口并检查指针。[入口查询规则](https://docs.vulkan.org/refpages/latest/refpages/source/vkGetInstanceProcAddr.html)

销毁顺序是 messenger → Instance。`VkPhysicalDevice` 是枚举得到的非拥有句柄，没有对应的 `vkDestroyPhysicalDevice`。这一顺序与之后 Device、Swapchain 和 GPU 资源的生命周期管理相衔接。

## 诊断通道的小变体

用 `vkSubmitDebugUtilsMessageEXT` 主动发送一条标记为 `VLE_DIAGNOSTIC_TEST` 的 INFO / GENERAL 消息。填好 `VkDebugUtilsMessengerCallbackDataEXT` 的 `sType`、`pMessageIdName` 和 `pMessage`，确认你的回调收到了它。不要在回调内发送测试消息。

随后仅改变 INFO 过滤开关，观察该消息是否出现；保留 WARNING / ERROR 的接收。这个变体检查 Vulkan 消息筛选和回调接线，不制造编译链接错误。人工 INFO 消息与 validation 的 warning/error 分开记录；收到它只能证明消息通道可用，validation layer 是否启用仍要核对创建配置。[提交调试消息](https://docs.vulkan.org/refpages/latest/refpages/source/vkSubmitDebugUtilsMessageEXT.html)

## 构建与运行

完成你的实现后，在 Windows Git Bash 中逐条执行，上一条成功后再继续：

```bash
cd /d/Code-dx12
cmake -S . -B build/vulkan-windows-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/vulkan-windows-debug
./build/vulkan-windows-debug/bin/vle_sandbox.exe
echo $?
```

这是主工程的新构建目录；不使用 `practice/` 的 L00 预设。Linux 使用独立的新目录并去掉 `.exe`，当前没有本课 Linux 实测结果。macOS 的 portability 要求在具体接入时单独核验，不将 Windows 运行等同于跨平台通过。

## 验收与回顾

- 用户自己的程序成功创建并销毁 Instance 和 messenger，列出设备及版本，退出码为 0。
- 输出可区分 loader 版本、请求的 API 基线和每个设备的 API 版本；validation layer 与 debug_utils 的启用可从配置和日志核对。
- 人工 INFO 消息通过回调到达终端，过滤变体符合预期；记录实际 validation warning/error，正常路径无 validation error。
- 能解释 Instance 与 PhysicalDevice 的职责，以及“扩展可用”和“扩展已启用”的区别；能指出 messenger 的销毁位置。

代码、用户运行反馈和 Vulkan 理解分别记录，不追加 CMake、链接和普通 C++ 自检。完成后进入 L04.2：为 Vulkan 呈现加入最小 SDL3 窗口与 Surface；SDL3 的版本、许可和接入片段届时提供。
