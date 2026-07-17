# Graphwar 项目中期报告

> **项目名称**：Graphwar — 基于数学函数图像的双人对战游戏  
> 
#### 小组成员：杨瑞潼、李之彦
---

## 一、智能体的使用

### 1.1 智能体配置与工具链

| 配置项 | 说明 |
|--------|------|
| **AI 助手** | Opencode(DeepSeek) & Trae |
| **上下文管理** | 通过 `PLAN.md` 保持任务/架构的一致性 |
| **代码读写** | 支持文件级 Write（覆盖/新建）和行级 Edit（精确替换） |
| **构建能力** | 通过 Shell 工具在本地运行编译命令并收集错误日志 |
| **文件搜索** | 支持 glob 搜索、grep 内容搜索、目录浏览 |
| **任务分解** | 通过 TodoList 工具进行多步任务的规划与进度追踪 |

### 1.2 MCP Server 设置

当前项目未使用 MCP Server。

### 1.3 规则文档与技能文档的编写

`PLAN.md` 是项目的唯一规则文档，包含：

- **游戏规则**：基本概念、障碍物系统、胜负条件、开始界面、方格布局、资源点数系统、历史轨迹、暂停系统、存档系统
- **技术栈与项目结构**：MVVM + PropertyTrigger + Command 架构模式、每层职责说明
- **核心模块详细设计**：数学表达式解析器、游戏引擎状态机、画布渲染、碰撞检测
- **构建与运行**：跨平台构建说明、常见问题

**编写策略**：每次架构调整后同步更新 PLAN.md 的结构树和通信模式说明，确保文档始终反映最新代码。

### 1.4 每轮对话的记录

典型对话流：

1. **需求描述** → 用户提出具体问题或需求
2. **代码分析与修复** → AI 搜索相关代码，定位问题原因，提出修改方案
3. **实现修改** → AI 使用 Edit/Write 工具修改代码
4. **验证** → 用户本地编译测试，反馈错误日志
5. **迭代** → 根据编译错误或行为不符继续修正

关键架构决策（如 MVVM 分层、PropertyTrigger 通信模式）记录在 `PLAN.md` 中。

#### 对话记录截图

对话较长，选取部分截图

![](image/c7eb127e1f71f867f4821a86f8a9ff64.jpg)

![](image/f14d9195d9b5a4ad290dc9138497e7c4.jpg)

![](image/67105f66e4c654a8f8ba7100ccb9e8fc.png)

![](image/e6821d60ae441433a852700a1fbbef2b.png)

![](image/1a40376a72fd383d0b46aea53f99b174.png)

![](image/de6f5ad14d5f07f3301e2b3cbb364338.png)

![](image/c8101001a25b4b841fd96e967d85c971.png)

### 1.5 实际效果与现存问题

**实际效果**：

1. **代码修改精度高**：行级 Edit 工具能精确定位并替换代码片段，避免整文件重写
2. **跨文件重构能力**：将架构从 EventBus 迁移到 PropertyTrigger + Command 模式，涉及 20+ 个文件的逐文件修改，AI 能保持一致性
3. **多文件上下文理解**：AI 能同时理解 common/frame、viewmodel/commands、view/widgets 等不同层的代码，进行跨层修改

**现存问题**：

1. **include 路径管理**：AI 在记住文件新位置（如从 model/ 移到 common/ 又移回 model/）方面偶尔会出现路径错误
2. **Qt 特有机制**：`QObject::connect` 在非 QObject 类中的调用需要显式加命名空间限定，AI 初期容易遗漏
3. **编译错误迭代**：AI 编写程序有小概率出现编译错误

---

## 二、团队协作情况

### 2.1 团队成员分工

团队为人机协作模式——二人负责与 AI 助手的全部交互，承担架构设计、编码实现、测试验证的全流程工作。

### 2.2 人与智能体协同工作的具体实践

**模式一："需求 → 分析 → 修改 → 验证"闭环**

1. 人类提出需求（如"加载存档和删除存档时不应弹出保存对话框"）
2. AI 分析相关代码（MainWindow、SaveManagerPage、PauseMenuPage 的 `onSaveResult` 方法）
3. AI 实施修改并说明原因
4. 人类本地编译测试，反馈结果
5. 如有问题，AI 根据错误信息迭代修复

**模式二："架构重构"**

从 EventBus 全局单例模式迁移到 PropertyTrigger + Command 模式：

1. 人类提出目标（"修改项目结构"）
2. AI 规划 TodoList：目录结构 → precomp.h → CMakeLists → GameViewModel → commands/ → view 文件 → app 绑定
3. 逐文件实现，每完成一组文件后通知人类运行验证

**模式三："分层依赖解耦"**

在处理 View 层对 Model/ViewModel 的依赖问题时：

1. 人类指出"View 不应包含 viewmodel 头文件"
2. AI 分析现有依赖关系，移除 `#include "viewmodel/GameViewModel.h"`
3. 人类继续指出"也不应包含 model 头文件"
4. AI 引入 GameState 作为 View 访问数据的唯一入口
5. 人类继续提出"common 层文件是否太多"——AI 将 GameConfig、GamePhase、SaveInfo、SaveManager 等跨层共享数据留在 common/

### 2.3 协同效果

| 指标 | 数据 |
|------|------|
| 代码行数 | ≈ 3,000 行 |
| 文件数量 | ≈ 50 个（含头文件、实现文件、配置文件） |
| 架构重构次数 | 2 次重大重构（EventBus → PropertyTrigger；model 依赖解耦） |
| AI 辅助代码占比 | ≈ 80%（AI 写初稿 + 人类调整） |
| 编译迭代轮次 | ≈ 15 轮（平均每 3-5 次修改触发一次编译验证） |

---

## 三、阶段性成果展示

### 3.1 功能模块完成度

| 模块 | 状态 | 说明 |
|------|------|------|
| **启动页** | ✅ 已完成 | 标题"GRAPH WAR"、游戏简介、NEW GAME / LOAD GAME 按钮 |
| **配置页** | ✅ 已完成 | 方格数、障碍物数/大小、P1/P2 颜色（8 色预设）、坐标标签/网格线开关 |
| **游戏画布** | ✅ 已完成 | QPainter 绘制；坐标系、双方方块、障碍物、弹道轨迹、网格线、坐标标签 |
| **函数输入面板** | ✅ 已完成 | 输入框、回车发射、点数预览、消息提示、FIRE 按钮、PAUSE 按钮 |
| **表达式解析器** | ✅ 已完成 | Tokenizer + ShuntingYard + Token 流求值/计费 |
| **发射与动画** | ✅ 已完成 | 沿曲线逐帧推进；命中/碰障碍物/出界三种终止条件 |
| **点数系统** | ✅ 已完成 | 表达式总点数设有上限，点数上限随回合递增 |
| **暂停系统** | ✅ 已完成 | PAUSE 按钮/ESC 触发；暂停菜单含继续/存档/返回标题 |
| **三槽位存档系统** | ✅ 已完成 | JSON 存档；Load/Delete；读档恢复配置 |
| **障碍物系统** | ✅ 已完成 | 随机生成、最小间距、被破坏变为虚线轮廓 |
| **胜负判定** | ✅ 已完成 | 弹出 PLAY AGAIN / BACK TO START PAGE 对话框 |
| **坐标标签** | ✅ 已完成 | 顶栏左右两侧显示，带黑色描边 |

### 3.2 核心架构

```
Graphwar/
├── CMakeLists.txt                  # 根 CMake：project() + add_subdirectory(src)
├── vcpkg.json                      # vcpkg 依赖声明（qtbase）
├── CMakePresets.json               # 构建预设（release-windows 等）
└── src/
    ├── CMakeLists.txt              # 可执行文件定义 + 预编译头 + Qt 链接
    ├── precomp.h                   # 预编译头
    ├── main.cpp
    ├── app/
    │   └── GraphwarApp.h/cpp       # Composition Root（绑定 ViewModel ↔ View）
    ├── common/                     # 基础设施 + 跨层共享数据
    │   ├── frame.h/cpp             # PropertyTrigger 通知系统
    │   ├── property_ids.h          # 属性 ID 枚举
    │   ├── GameConfig.h            # 配置数据
    │   ├── GamePhase.h             # 阶段枚举
    │   ├── GameState.h             # 游戏状态快照（含 slotInfos/slotCount）
    │   ├── SaveInfo.h/cpp          # 存档元信息
    │   └── Square.h                # 方块/障碍物数据（含 Rect）
    ├── model/                      # 业务逻辑 + 工具类
    │   ├── GameModel.h/cpp         # 核心业务逻辑
    │   ├── Player.h
    │   ├── SaveManager.h/cpp       # 存档文件 IO
    │   └── parser/                 # 数学表达式解析器
    ├── viewmodel/                  # 桥接层
    │   ├── GameViewModel.h/cpp     # 继承 PropertyTrigger
    │   └── commands/               # 命令实现（8 个文件）
    └── view/                       # 仅包含 common/ 头文件
        ├── MainWindow.h/cpp        # QStackedWidget 五页切换
        └── widgets/                # 自定义控件（5 个文件）
```

**通信模式**：

```
GraphwarApp (binding):
  m_view_model.add_notification(m_main_wnd.get_notification());
  m_main_wnd.set_pause_command(m_view_model.get_pause_command());

用户操作 → View 调用 std::function 命令 → GameViewModel → GameModel 变更
GameModel 变更 → Qt signal → GameViewModel → PropertyTrigger::fire(id) → View 通知回调 → UI 更新
```

### 3.3 界面说明

**① 开始页**：深色背景 `#0f0f1a`，顶部大标题、副标题、NEW GAME 和 LOAD GAME 两个按钮。

![开始页](image/start.png)

**② 配置页**：深色 `#141420` 背景，表单容器圆角卡片风格，SpinBox 带 ± 符号，8 色预设色块按钮，START GAME / BACK 按钮。

![配置页](image/settings.png)

**③ 游戏页**：
- 顶栏：P1 Label / 坐标标签 / Round / Points / 坐标标签 / P2 Label（坐标标签在 P1 回合显示在左侧、P2 回合在右侧）
- 画布：深色背景，可选网格线（边界线高亮），坐标轴，灰色障碍物，半透明方块，白色虚线方框高亮选中方块，当前弹道轨迹，历史轨迹，弹头圆点
- 底栏：PAUSE 按钮 / 输入框 / Cost 显示 / FIRE 按钮 / 消息提示

![游戏页](image/game.png)

**④ 暂停菜单页**：深色背景，Resume 按钮，三个存档槽位（每个显示 Slot N / Round / Player / 时间戳 / Save 按钮），Back to title 按钮。

![暂停菜单页](image/pause.png)

**⑤ 存档管理页**：三槽位展示（Round / Player / Saved at / LOAD / DELETE），Back 按钮。

![存档管理页](image/save.png)

### 3.4 技术指标

| 指标 | 当前值 |
|------|--------|
| 构建工具链 | CMake + vcpkg + Ninja |
| 警告数 | 0 |
| 动画帧率 | 60 FPS（QTimer 16ms） |

---

## 四、总体心得与个人感悟

### 4.1 总体心得

**1. 规则文档是 AI 协作的基石**

项目最有效的实践是维护一份与代码同步更新的 `PLAN.md`。当 AI 需要理解项目结构或规则时，直接读取 PLAN.md 比从代码中反向推导快得多。每次架构调整后同步更新文档的投入，在后续开发中能数倍收回。

**2. 分步验证减少返工**

每次只修改一个逻辑层（如只改 common/ 或只改 view/），然后编译验证，能有效定位问题。跨层修改（如从 EventBus 迁移到 PropertyTrigger）时，TodoList 工具帮助跟踪进度。

**3. 编译错误是高效的反馈信号**

在 Windows + MinGW + Qt 环境下，编译错误信息直接指明了问题（缺头文件、作用域错误、类型不匹配）。AI 读取错误日志后能快速定位并修复，平均每轮修复 1-2 个错误。

**4. 架构纯度需要持续维护**

View 层依赖 ViewModel 和 Model 的问题是在多次迭代中逐步发现并解决的。没有一次性的"完美架构"，而是在人类不断提出更高要求（"不应包含 viewmodel 头文件"、"也不应包含 model 头文件"）的过程中逐步完善的。

### 4.2 个人反思与成长收获

#### 杨瑞潼

> 这次项目让我深刻体会到"架构"在实际工程中的意义。从 EventBus 全局单例到 PropertyTrigger + Command 模式的重构，让每个 View 组件只知道自己需要的命令和数据，不再依赖全局通信总线。这种"减少隐式依赖"的思路对我理解软件设计有质的帮助。在与 AI 协作的过程中，我学会了如何将复杂的架构调整需求拆解为可执行的小步骤，通过 TodoList 工具逐步推进，确保每一步修改都能通过编译验证。这种"分而治之"的策略让我在面对大规模重构时不再感到无从下手。

#### 李之彦

> 本次项目收获是 AI 协作中的"渐进式需求"。人类不需要一开始就提出完美需求——可以先说"保存后页面没刷新"，再补充"删除存档不该跳转到游戏页"，再进一步"View 不应该包含 Model 头文件"。AI 能够在每一轮迭代中理解上下文并逐步逼近最终结果。这种渐进式协作方式比传统的"一次写完所有需求再让 AI 实现"要高效得多。我还认识到，编译错误并不可怕，反而是最直接的反馈信号。通过仔细阅读错误日志并与 AI 配合迭代修复，我对 C++ 的链接机制、Qt 的信号槽系统以及 CMake 的构建配置有了更深入的理解。

---

## 结语

Graphwar 项目已实现核心玩法全链路：函数输入 → 表达式解析 → 点数计费 → 弹道动画 → 碰撞检测 → 回合切换 → 胜负判定。架构上采用 MVVM + PropertyTrigger + Command 模式，View 层完全解耦于 Model 和 ViewModel，仅依赖 common/ 层的头文件。

AI 辅助下的最大收获不是"写代码更快"，而是让团队有精力在有限时间内持续优化架构纯度，产出更具工程质量的代码。