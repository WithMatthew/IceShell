# 教学路线

学习起点：熟悉 C++，刚开始学习 Vulkan。主线采用“讲解原理 → 你亲自实现 → 运行与调试 → 审查和复盘 → 变体练习”的节奏。阶段是里程碑，一阶段通常包含多课；每次聚焦一个可验证的小目标。代码由用户编写，助手负责教学与审查；明确请求参考实现时再提供相应代码。

以下是课程规划，实际实现情况以 [进度](PROGRESS.md) 为准。课号用于导航，可根据学习反馈拆分，不预设固定完工日期。

## 路线调整记录

- **2026-11-27 按用户诉求重组主线。** 主线覆盖：基础 RHI、光栅化、光追、物理、材质系统、场景管理、UI。核心代码由用户亲自实现，以熟悉 Vulkan API 为首要目标。
  - 新增 **基础 RHI 阶段（R1–R5）**：只做 Vulkan 后端，**RHI 公开接口不暴露任何 `Vk*` 类型或 `vulkan.h`**，为以后增加其他后端保留空间，但当前不实现第二个后端。
  - 新增 **UI 阶段（U1–U3）**：自写 Dear ImGui 的 Vulkan 渲染后端与调试面板；游戏内 UI（SDF 文字）为可选。原 L32“调试面板”并入 U1–U2。
  - 物理采用用户确认的方案：固定步长、碰撞检测与简单冲量求解由用户自写；复杂刚体堆叠由 Jolt 承担；新增 GPU compute 物理单元（G1，XPBD 布料/粒子）。
  - **动画（L26–L28）、GPU 驱动绘制（L33–L35）、Mesh clustering（L36–L39）移入扩展清单**；**音频（L29–L30）暂缓**，不在当前计划内。工程收束（L44–L45）一并移入扩展清单。
  - 历史课号保留，新增单元使用 R/U/G 前缀。
- 2026-11-21：以用户已有的 C++ 经验为起点，直接进入 Vulkan。L00 作为可选工程参考，未做的基础练习按要求跳过，不记为已验证掌握。原 L01–L03（窗口、输入时间、日志所有权）并入实际使用它们的课程。

## “自己实现”的边界

| 类别 | 内容 |
| --- | --- |
| 用户必须亲自实现 | 所有 Vulkan 调用与对象生命周期；同步与 barrier；RHI 层；render graph；材质系统；场景与组件存储；**Dear ImGui 的 Vulkan 渲染后端**；加速结构构建与 SBT；compute 物理；自写的碰撞检测与冲量求解 |
| 允许使用的库（不是 Vulkan 学习重点） | SDL3（窗口与输入）、GLM（数学）、stb_image 与 cgltf（文件解析）、Dear ImGui 核心（只用布局逻辑）、Jolt（复杂刚体）、glslc |
| 先手写、后换库 | 内存分配：L07 先用 `vkAllocateMemory` 写最简分配器，理解 memory type/heap/对齐后再接入 VMA |
| 不使用 | vk-bootstrap、Vulkan-Hpp RAII、`imgui_impl_vulkan`、nvpro 的 `nvvk` 辅助库、照搬 Sascha Willems 示例的 base 框架。它们会替你完成正好要学的部分；自己完成后可对照阅读 |

## 主线阶段

| 阶段 | 课程 | 重点 Vulkan 内容 | 可观察成果与验收 | 粗估小时 |
| --- | --- | --- | --- | --- |
| A Vulkan 最小闭环（**当前**） | L04.1–L06 | instance、debug utils、device/queue、swapchain、sync2、dynamic rendering、pipeline | 看见三角形；缩放、最小化与恢复可用；正常路径及销毁无 validation error | 36–56 |
| B 资源与 3D | L07–L10 | 内存类型、自写分配器→VMA、staging、image layout、descriptor、深度 | 自由相机观察多个带纹理物体；深度正确；上传与销毁通过同步验证 | 42–66 |
| C 基础 RHI | R1–R5 | 帧资源、延迟销毁、代际句柄、状态跟踪 barrier、bindless、上传管理 | Sandbox 改用 RHI 后功能不变；公开头文件不含 `vulkan.h` | 30–50 |
| D 调试 UI | U1–U2 | 每帧动态顶点/索引 buffer、字体 atlas 上传、scissor、push constant | 运行时面板显示帧时间、相机参数与渲染开关 | 10–18 |
| E 场景管理 | L11–L14 | 从场景提取渲染数据、instance buffer | 加载 glTF；父子变换正确；视锥剔除；保存/加载往返；失效句柄可检测 | 44–70 |
| F 材质与光栅 | L15–L22 | bindless 材质表、compute 预滤波 IBL、多 pass/render graph、阴影、HDR、timestamp query | PBR + 阴影 + 后处理；材质可在 ImGui 中实时调整；有可复现的 CPU/GPU 基线 | 88–140 |
| G 物理 | L23–L25、G1 | compute pipeline、compute → vertex 的 buffer barrier、调试线框 pipeline | 方块落地堆叠、可推动；固定步长稳定；GPU 布料或粒子运行 | 44–70 |
| H 光线追踪 | L40–L43 | BLAS/TLAS 构建与压缩、ray query、RT pipeline、SBT、时间累积 | 光栅/混合路径可切换；参考路径追踪器；无光追能力时仍可光栅运行 | 64–102 |
| 主线收尾 | L31 | — | 一个小型交互场景：走动、PBR 材质、推动刚体、UI 调参、切换光追 | 12–20 |
| 可选：游戏 UI | U3 | SDF 字体、批量 quad、UI pass | HUD 与菜单文字 | 10–18 |

**顺序：** A → B → C → D → E → F → H → L31。G 只依赖 E，可在 F 期间穿插进行。H 依赖 E（TLAS 实例来自场景）和 F（命中着色读取 bindless 材质、render graph 组织 pass）。

**为什么 RHI 在 L10 之后：** 先用原生 API 把 buffer、image、descriptor、pipeline 写过两三遍，再从重复代码中提炼抽象，才知道哪些值得封装。L04–L10 期间 Vulkan 类型出现在引擎头文件中是允许的，R1 起迁移到 RHI 的私有实现。

**为什么 UI 提前到 D：** 有了调试面板，后面的场景、材质、阴影和光追调参、排错都更高效。

## 课程目录与逐课用时

以下时间针对“熟悉 C++、初学 Vulkan、亲自实现代码”的粗估，包含阅读、编码、调试、验证和自检；L06 后按实际投入重新估算。

| 阶段 | 课程 | 名称 | 预计小时 |
| --- | --- | --- | --- |
| A | L04 | Instance、验证层、最小 SDL3 窗口与 Surface、设备与队列（拆为 .1–.3） | 10–16 |
|  | L05 | Swapchain、清屏、提交与呈现同步 | 16–24 |
|  | L06 | 着色器、Pipeline 与第一个三角形 | 10–16 |
| B | L07 | Buffer、Staging、内存类型；自写最简分配器后接入 VMA | 12–18 |
|  | L08 | Image、纹理、Sampler 与 Descriptor | 12–20 |
|  | L09 | 相机输入、帧时间、坐标变换与深度测试 | 10–16 |
|  | L10 | 索引网格与多物体绘制 | 8–12 |
| C | R1 | RHI 边界：Device/Queue/Swapchain 封装、frames in flight、延迟销毁队列；公开头文件去除 Vulkan 类型 | 6–10 |
|  | R2 | 资源描述与代际句柄：Buffer、Texture、Sampler 的创建、查找与失效检测 | 5–8 |
|  | R3 | CommandList 封装与资源状态跟踪，自动生成 sync2 barrier | 8–12 |
|  | R4 | Pipeline 描述、shader 模块与 pipeline cache；bindless descriptor（descriptor indexing）+ push constant | 7–12 |
|  | R5 | 上传管理器（staging ring buffer）；用 RHI 重写 Sandbox 并对比 | 4–8 |
| D | U1 | 自写 Dear ImGui Vulkan 后端：字体 atlas、动态 VB/IB、scissor、与 swapchain 同步 | 6–10 |
|  | U2 | 调试面板：帧时间、相机、渲染开关、资源统计 | 4–8 |
| E | L11 | Entity、Component 与代际句柄 | 10–16 |
|  | L12 | Transform 层级、包围体与视锥剔除 | 10–16 |
|  | L13 | glTF 导入、资源缓存与实例化 | 14–22 |
|  | L14 | 场景保存、加载与卸载；ImGui 场景层级面板 | 10–16 |
| F | L15 | 材质模板、材质实例、参数布局与 bindless 材质表 | 10–16 |
|  | L16 | 金属粗糙度 PBR 与直接光照 | 12–20 |
|  | L17 | 法线贴图与 IBL（compute 预滤波环境图与 BRDF LUT） | 16–24 |
|  | L18 | Alpha Mask、透明混合、材质调试视图与 ImGui 材质编辑器 | 10–16 |
|  | L19 | 多 Pass、资源状态与最小 Render Graph（建立在 R3 状态跟踪之上） | 16–24 |
|  | L20 | 阴影贴图（含级联阴影） | 10–16 |
|  | L21 | HDR、色调映射与后处理 | 8–14 |
|  | L22 | GPU 时间测量、调试标记、RenderDoc/Nsight 抓帧 | 6–10 |
| G | L23 | 固定步长、数值积分与碰撞检测基础（球、AABB、OBB，自写） | 8–12 |
|  | L24 | 自写简单冲量求解器；再集成 Jolt 承担复杂堆叠，完成场景同步 | 14–22 |
|  | L25 | 射线查询、角色交互、碰撞事件与碰撞体调试线框 | 10–16 |
|  | G1 | GPU compute 物理：XPBD 布料或粒子，compute → graphics 同步 | 12–20 |
| H | L40 | 光追能力查询、BLAS/TLAS 构建、压缩与构建同步 | 14–22 |
|  | L41 | Ray Query 阴影与 AO，嵌入现有光栅路径 | 10–16 |
|  | L42 | Ray Tracing Pipeline、SBT 与参考路径追踪器 | 20–32 |
|  | L43 | 采样、时间累积、去噪与历史失效 | 20–32 |
| 收尾 | L31 | 主线集成演示：场景、材质、物理、UI、光追切换 | 12–20 |
| 可选 | U3 | 游戏内 UI：SDF 字体（msdfgen 离线生成 atlas）、批量 quad、UI pass | 10–18 |

**主线合计：370–592 小时**（不含可选 U3）。为复习、跨系统返工与兼容问题留约 20% 余量，排期约 **445–710 小时**。这是按课程范围的粗估，不是完成承诺。

| 阶段成果 | 完成位置 | 累计小时（未加余量） |
| --- | --- | --- |
| Vulkan 三角形与基本呈现闭环 | L06 / A | 36–56 |
| 自有 RHI 与调试 UI | U2 / D | 118–190 |
| 场景、PBR 材质、阴影与后处理 | L22 / F | 250–400 |
| 物理交互 | G1 / G | 294–470 |
| 光追与主线集成演示 | L31 | 370–592 |

| 每周有效学习时间 | 约需周数 | 约需月份 |
| --- | --- | --- |
| 8 小时 | 56–89 周 | 13–21 个月 |
| 10 小时 | 45–71 周 | 10–16 个月 |
| 15 小时 | 30–48 周 | 7–11 个月 |
| 20 小时 | 23–36 周 | 5–8 个月 |

## 扩展清单与暂缓内容

扩展清单在主线完成后，或在前置满足时按兴趣插入。建议的第一个扩展是 GPU 驱动绘制，它直接复用 bindless（R4）和 compute（G1、L17）。

| 类别 | 课程 | 名称 | 前置 | 预计小时 |
| --- | --- | --- | --- | --- |
| 扩展：GPU 驱动绘制 | L33 | Compute 与可见实例列表 | F | 10–16 |
|  | L34 | 间接绘制与实例、材质索引 | L33 | 12–20 |
|  | L35 | 视锥剔除与 Hi-Z 遮挡剔除 | L34 | 18–28 |
| 扩展：Mesh clustering | L36 | 离线 Meshlet 构建（meshoptimizer）与颜色可视化 | L35 | 10–16 |
|  | L37 | 包围体、Normal Cone 与 Cluster 剔除 | L36 | 12–20 |
|  | L38 | 层次 LOD 与误差度量 | L37 | 20–32 |
|  | L39 | Task/Mesh Shader 可选路径 | L37 | 14–22 |
| 扩展：动画 | L26 | 节点轨道、骨骼与关键帧采样 | E | 12–18 |
|  | L27 | CPU 姿态求值与 GPU 蒙皮 | L26、F | 12–20 |
|  | L28 | 动画混合、状态机与 Root Motion | L27、G | 14–22 |
| 扩展：工程收束 | L44 | 资源烘焙、异步任务与性能预算 | 主线 | 16–24 |
|  | L45 | 跨平台回归、构建自动化、发布与设计回顾 | 主线 | 16–24 |
| **暂缓** | L29–L30 | 音频（miniaudio、空间音频） | — | 不在当前计划 |

## 参考教程对照

参考资料用于理解和核对；课程代码由用户在自己的引擎结构中实现。**使用方式：读懂概念 → 关掉教程，在自己的结构里实现 → 运行通过且 validation 干净后，再对照参考实现找差异。**

| 阶段 | 资料 | 注意事项 |
| --- | --- | --- |
| A/B | [vkguide.dev](https://vkguide.dev) | 与本项目基线最接近（1.3、dynamic rendering、sync2）；其初始化使用 vk-bootstrap，这部分自己写 |
| A/B | [vulkan-tutorial.com](https://vulkan-tutorial.com) | C API，但基于 `VkRenderPass`；只看概念，渲染部分换成 dynamic rendering |
| A/B | [Khronos Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html) | 使用 Vulkan 1.4、Vulkan-Hpp 与 Slang；只看概念，不拼接代码 |
| 全程 | [Khronos Vulkan Guide](https://docs.vulkan.org/guide/latest/index.html)、API 规范 | 查语义、版本与扩展依赖 |
| 同步 | [Synchronization Examples](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples)、Themaister *Yet another blog explaining Vulkan synchronization* | L05、R3、L19 重点阅读 |
| C RHI | zeux [Writing an efficient Vulkan renderer](https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/)、[NVRHI](https://github.com/NVIDIA-RTX/NVRHI)、[Granite](https://github.com/Themaister/Granite) | 只参考接口设计与取舍 |
| F Render graph | GDC 2017 *FrameGraph: Extensible Rendering Architecture in Frostbite* | L19 |
| D UI | [Dear ImGui](https://github.com/ocornut/imgui) 的 `docs/BACKENDS.md` | 自写后端完成后再对照 `imgui_impl_vulkan.cpp` |
| U3 | Valve *Improved Alpha-Tested Magnification*（Green 2007）、[msdfgen](https://github.com/Chlumsky/msdfgen) | 可选 |
| E 场景 | glTF 2.0 规范、[glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets)、skypjack *ECS back and forth* 系列 | — |
| F 材质 | [LearnOpenGL PBR](https://learnopengl.com/PBR/Theory)、[Filament 文档](https://google.github.io/filament/Filament.html)、Karis 2013 *Real Shading in Unreal Engine 4* | — |
| G 物理 | [Fix Your Timestep!](https://gafferongames.com/post/fix_your_timestep/)、[Ten Minute Physics](https://matthias-research.github.io/pages/tenMinutePhysics/)（XPBD）、Erin Catto 的 GDC 讲座、*Real-Time Collision Detection*、[Jolt 文档](https://jrouwe.github.io/JoltPhysics/) | — |
| H 光追 | [vk_raytracing_tutorial_KHR](https://github.com/nvpro-samples/vk_raytracing_tutorial_KHR) | 只看 API 流程，不使用其 `nvvk` 辅助代码 |
| H 光追 | [Ray Tracing in One Weekend](https://raytracing.github.io)、[PBRT 4th](https://pbr-book.org)、*Ray Tracing Gems* I/II（免费） | 采样与路径追踪理论 |
| 全程 | [Sascha Willems Vulkan](https://github.com/SaschaWillems/Vulkan)、[Khronos Vulkan Samples](https://docs.vulkan.org/samples/latest/index.html) | 按功能查对应示例核对 API 用法 |
| 工具 | RenderDoc、Nsight Graphics、validation layers 与 synchronization validation（vkconfig） | 从 L05 起常用 |

## 跨平台贯穿路线

- **A**：查询 loader、设备版本、features、extensions、limits、格式和呈现能力；在 macOS 上评估实际 Vulkan 实现与 portability 要求。
- **B/C/F**：检查 shader 目标环境、资源布局、格式支持和同步行为；RHI 能力查询结果以后端无关的结构向上暴露。
- **G/H**：高级功能有能力开关和回退；分别记录平台上实际走过哪条路径。
- 没有设备时标为“未验证”；软件驱动或仅构建通过不等于硬件渲染通过。

macOS 默认作为兼容目标评估；移动端、网络、第二图形后端、完整编辑器和大型开放世界流送不在当前计划内。

## 每一课怎样完成

1. 明确一个具体问题：例如“CPU 怎么知道上一帧仍在使用这个 buffer”。
2. 画出对象关系或数据流，区分 CPU 行为和 GPU 行为。
3. 助手给出职责、步骤、关键 API 片段和验收目标；用户亲自实现最小闭环，保持之前已完成的能力可用。
4. 用户运行、观察、排查并解释关键取舍；助手审查代码、协助定位，记录命令、环境和实际证据。助手的验证不替代用户的理解。
5. 用户完成一个与本课 Vulkan/引擎行为相关的小变体或诊断。分别更新参考工程、用户实现和掌握状态；用户选择跳过时如实记录。

C++ 基础不单独重讲；GPU 资源生命周期重点解释使用完成条件、所有权和销毁顺序。到相机时补矩阵和坐标，到光照时补线性空间与 BRDF，到物理时补积分与约束，到光追时补概率采样。
