# L04.1：Vulkan Instance、验证层与设备枚举

用户已实现 Instance 创建、销毁、失败日志和创建前的验证层检查。本步查询并启用实例扩展 `VK_EXT_debug_utils`，之后接入 messenger 与设备枚举。[进度记录](../PROGRESS.md)

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

本课沿用用户已建立的 `engine/include/vle/core/IceVulkan.hpp` 与 `engine/src/core/IceVulkan.cpp`，由主工程的 `samples/sandbox/main.cpp` 调用；`practice/` 保留为之前的练习记录。用户已选择用 `IceVulkan` 类负责 Instance 创建与销毁，继续沿用该封装，明确空状态、失败传播与唯一所有权，暂不预先设计渲染器框架。L04.3 形成持续使用的 Instance/Device 所有权后，再按实际需求调整封装。

2026-11-21 恢复学习时，当前小步仅为 Instance 创建与销毁：填写应用信息与创建配置，启用 validation layer，检查 `vkCreateInstance` 的结果，在成功路径销毁 Instance。先利用上方已有的本机 loader/layer 查询记录理解这条路径；用户程序自己的能力查询、debug messenger 与设备枚举在本课后续逐步补齐。这一小步成功不代表整课验收通过。

## 本课需要的构建调整

以下为已完成的接入记录：用户已为 Sandbox 和 `vle_core` 接入 `Vulkan::Vulkan`，并将 `IceVulkan.cpp` 加入 `vle_core`；编译数据库已更新。本次续学沿用现有配置，下面的最初接入说明无需重复执行。

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

枚举通常先查数量，再提供数组接收结果。填充阶段返回 `VK_INCOMPLETE` 表示只写入了部分条目。应用若要取得完整列表，可以重新查数量并重试。成功后按实际写入数量使用结果，设备数量为 0 时输出清楚的诊断。[设备枚举规则](https://docs.vulkan.org/refpages/latest/refpages/source/vkEnumeratePhysicalDevices.html)

## 创建 Instance 前检查验证层

在 `engine/src/core/IceVulkan.cpp` 的 `InitVulkan()` 开头查找 `IceVulkanLayers[0]`。此时可以直接调用 `vkEnumerateInstanceLayerProperties`，它向 loader 查询当前可用的实例层，无需传入 Instance 句柄。数组里的名称是应用想启用的层，返回的 `VkLayerProperties::layerName` 是当前可用的层名。[实例层枚举](https://docs.vulkan.org/refpages/latest/refpages/source/vkEnumerateInstanceLayerProperties.html) · [VkLayerProperties](https://docs.vulkan.org/refpages/latest/refpages/source/VkLayerProperties.html)

```cpp
uint32_t layer_count = 0;
VkResult layer_result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
// TODO 检查查询结果。查询成功且数量为零时，直接报告缺少请求的层。

std::vector<VkLayerProperties> layers(layer_count);
layer_result = vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
// TODO 处理返回码，再用实际写入的条目查找层名。
```

1. 第一次调用写回可用层数。第二次调用前，`layer_count` 是数组可容纳的条目数，返回后它是实际写入数。本阶段的验证层检查在 `VK_INCOMPLETE` 时打印返回码并结束初始化。若以后需要自动恢复，可以重新查数量、分配并填充。任一调用返回错误时，也应保留实际 `VkResult`。
2. 填充成功后调用 `layers.resize(layer_count)`，按字符串内容比较 `layer.layerName` 和 `IceVulkanLayers[0]`。可以用 `std::string_view{layer.layerName} == IceVulkanLayers[0]`。若使用示例中的类型，引入 `<vector>` 和 `<string_view>`。这些属性只需存在于本次查询期间。
3. 找到验证层就继续用现有的 `ppEnabledLayerNames` 请求启用它，并检查 `vkCreateInstance` 自己的结果。完整列表中缺少所需层时，输出具体层名并从 `InitVulkan()` 返回 `false`。枚举成功表示取得了可用列表，列表可以恰好不含目标层。

验收时沿用下方的两组 Git Bash 命令。正常启动应走到创建、销毁并退出 `0`。为单次启动设置 `VK_LAYER_PATH=./samples/sandbox` 后，应在调用 `vkCreateInstance` 之前报告缺少 `VK_LAYER_KHRONOS_validation`，退出 `1`。

## 查询并启用实例扩展

验证层检查 Vulkan 调用的用法。`VK_EXT_debug_utils` 提供调试接口，下一步会用它创建 messenger，把诊断消息交给程序的回调。本步的目标是让创建成功的 Instance 启用这个扩展。本机 `vulkaninfo --summary` 已在实例扩展列表中列出 `VK_EXT_debug_utils`，revision 为 2。[调试扩展说明](https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_debug_utils.html)

在 `IceVulkan::InitVulkan()` 中，把扩展查询放在验证层检查通过之后、填写 `VkInstanceCreateInfo` 之前。在函数作用域定义请求的名称，供查找和创建共用。

```cpp
constexpr const char* required_extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
uint32_t available_count = 0;
VkResult ext_ret = vkEnumerateInstanceExtensionProperties(nullptr, &available_count, nullptr);
// TODO 检查返回码。查询成功且数量为零时，报告缺少的扩展并返回 false。

std::vector<VkExtensionProperties> available_extensions(available_count);
ext_ret = vkEnumerateInstanceExtensionProperties(nullptr, &available_count,
                                                 available_extensions.data());
// TODO 检查返回码，按实际写入数限定查找范围。
```

第一个参数选择从哪里查。传 `nullptr` 时，返回 Vulkan 实现及隐式启用层提供的实例扩展。要查某个显式层单独提供的扩展，可以传入它的层名。最后一个参数选择只取数量还是填充数组。两次调用都沿用现有的结果检查，非 `VK_SUCCESS` 时报告调用名和实际返回码并退出。[实例扩展枚举](https://docs.vulkan.org/refpages/latest/refpages/source/vkEnumerateInstanceExtensionProperties.html)

复用已有的 `std::find_if` 写法。对 `required_extensions` 中的每个名字，用 `VkExtensionProperties::extensionName` 做字符串内容比较。若在本次全局列表中找不到，打印缺少的名称并从初始化返回 `false`。[扩展属性](https://docs.vulkan.org/refpages/latest/refpages/source/VkExtensionProperties.html)

把 `vk_create_info.enabledExtensionCount` 设为 `static_cast<uint32_t>(std::size(required_extensions))`，把 `vk_create_info.ppEnabledExtensionNames` 指向 `required_extensions`。名称数组须在 `vkCreateInstance` 调用期间有效。成功后按这份数组打印启用的扩展名。这次请求 1 个扩展，系统可枚举的扩展数量由 `available_count` 给出。[Instance 创建配置](https://docs.vulkan.org/refpages/latest/refpages/source/VkInstanceCreateInfo.html)

从仓库根目录运行下方的 Git Bash 构建与运行命令。预期能看到创建成功、已启用 `VK_EXT_debug_utils`、销毁成功并退出 `0`。可选自检是将请求数组里的名字暂时换成一个不存在的名字，重新构建后应在创建前点名缺少的扩展并退出 `1`，然后恢复原名。

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

从主工程根目录在 Windows Git Bash 中运行。当前工作目录是 `/d/dx12_code/IceShell`，这里已完成首次配置。修改源码后从 `cmake --build` 开始即可。

```bash
cmake -S . -B build/vulkan-windows-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/vulkan-windows-debug
./build/vulkan-windows-debug/bin/vle_sandbox.exe
echo $?
```

这是主工程的新构建目录；不使用 `practice/` 的 L00 预设。Linux 使用独立的新目录并去掉 `.exe`，当前没有本课 Linux 实测结果。macOS 的 portability 要求在具体接入时单独核验，不将 Windows 运行等同于跨平台通过。

### 验证层缺失与可见时的对照运行

在普通权限的 Git Bash 中，从项目根目录运行同一个程序两次。

```bash
VK_LAYER_PATH=./samples/sandbox ./build/vulkan-windows-debug/bin/vle_sandbox.exe
echo $?

./build/vulkan-windows-debug/bin/vle_sandbox.exe
echo $?
```

第一条命令让 loader 只到 `samples/sandbox` 查找显式 layer 清单。该目录只含示例源码和构建描述，所以这次启动找不到应用请求的验证层。命令前的变量赋值仅对这次启动生效。[Khronos Loader 的环境变量说明](https://github.com/KhronosGroup/Vulkan-Loader/blob/main/docs/LoaderInterfaceArchitecture.md#active-environment-variables)

上一步已验证直接请求缺失的层时，`vkCreateInstance` 返回 `-6`（`VK_ERROR_LAYER_NOT_PRESENT`）。加入本节预检查后，第一组应由你的程序直接点名缺失的层并提前退出。第二组应正常创建并销毁 Instance。[vkCreateInstance](https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateInstance.html) · [VkResult](https://docs.vulkan.org/refpages/latest/refpages/source/VkResult.html)

## 验收与回顾

- 用户自己的程序成功创建并销毁 Instance 和 messenger，列出设备及版本，退出码为 0。
- 输出可区分 loader 版本、请求的 API 基线和每个设备的 API 版本；validation layer 与 debug_utils 的启用可从配置和日志核对。
- 人工 INFO 消息通过回调到达终端，过滤变体符合预期；记录实际 validation warning/error，正常路径无 validation error。
- 能解释 Instance 与 PhysicalDevice 的职责，以及“扩展可用”和“扩展已启用”的区别；能指出 messenger 的销毁位置。

代码、用户运行反馈和 Vulkan 理解分别记录，不追加 CMake、链接和普通 C++ 自检。完成后进入 L04.2：为 Vulkan 呈现加入最小 SDL3 窗口与 Surface；SDL3 的版本、许可和接入片段届时提供。
