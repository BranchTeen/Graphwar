# Graphwar 游戏开发计划

> **规则：**
> 1. 所有代码层面的更新（新增/修改文件、逻辑变更、配置变更、UI 变更等）必须同步更新 PLAN.md 中对应章节，确保 PLAN.md 始终反映项目最新状态。
> 2. 程序的结构必须遵循 MVVM 设计模式。

## 游戏规则

### 基本概念
- 双方各拥有 **N 个方格**（默认 5，可在配置页调整 1-10 个，矩形区域，每个 0.8×0.8 单位），在地图两侧**各自随机分布**
- 方格同时作为 **己方发射点** 和 **对方攻击目标**
- **玩家不可以自行选择发射方块**：每回合由**系统随机挑选**一个未被摧毁的己方方块作为本次发射点
- 被选中的方块会以**白色虚线边框 + 旁边显示 (cx, cy) 坐标标签**进行高亮显示
- 玩家输入数学函数 y = f(x)，系统自动添加常数 C，使图像经过发射点
- 函数图像从发射点开始向对方方向延伸
- 若图像进入对方任意一个方格，则命中，该方格被摧毁
- 函数由**消耗点数系统**约束：每个运算符、常量、数学函数、变量都消耗一定点数
- 点数通过 `pointsLevel` 递增（与 `roundNumber` 不同步）：P0 操作结束、切换到 P1 时 `pointsLevel` 增长，P1 操作结束、切回 P0 时 `roundNumber` 增长
- 当前可用点数公式：`3 + pointsLevel * 2`（起始 pointsLevel=1，即第 1 轮 P0 操作时有 5 点，P1 操作时 7 点，第 2 轮 P0 操作时 7 点，P1 操作时 9 点……）
- **无时间限制**，**无输入遮挡**（双方可见）

### 障碍物系统
- 地图随机分布 **N 个障碍物**（默认 10 个，可在配置页调整 0-30 个，默认边长 1.8，可在配置页调整 0.5-5.0）
- 障碍物随机分布在 x ∈ [-20, 20]，y ∈ [-20, 20]
- 障碍物之间最小中心距 = `size + 0.7`（非硬编码，随障碍物大小动态调整）
- 障碍物与玩家方格最小中心距 = `size * 0.5 + 0.3`（随障碍物大小动态调整）
- **玩家攻击过程中，若在到达对方方格或触碰边界之前碰到障碍物：**
  - 攻击**立即中止**
  - 该障碍物被破坏（视觉上变为虚线轮廓）
  - 回合结束，轮到对方
- **已被破坏的障碍物不阻挡攻击**（子弹会直接穿过）
- 障碍物在游戏过程中保持已破坏状态，不会恢复

### 胜负条件
- 先摧毁对方全部方格的一方获胜
- 游戏结束后弹出 "PLAY AGAIN" 与 "BACK TO START PAGE" 两个选项

### 开始界面
- 游戏启动时显示开始界面，包含标题、简介和 "NEW GAME"、"Load / Manage Saves" 两个按钮（**已移除独立的 Config 按钮**）
- **NEW GAME 流程**：点击 NEW GAME → 进入 Config 页（而非直接进入游戏）→ 调整配置后点击 **START GAME** → 进入游戏
- **Load / Manage Saves**：进入存档管理页面，读取已保存的游戏状态

### 方格布局（随机生成）
- 每局开始前，双方各自的 **N 个方格**（默认 5，可在配置页调整 1-10 个）在己方半区**随机生成**
- **玩家 1（左侧）**：x ∈ [-20, -1]，y ∈ [-10, 10]，方块之间最小间距 3.0 单位
- **玩家 2（右侧）**：x ∈ [1, 20]，y ∈ [-10, 10]，方块之间最小间距 3.0 单位
- 每个方块大小 **0.8×0.8** 单位
- 方块以半透明色块绘制，被摧毁后变灰

### 资源点数系统

**当前可用点数（与 roundNumber 不同步，跟随 pointsLevel）：**
- 公式：`availablePoints = 3 + pointsLevel × 2`
- `pointsLevel` 在 **P0 操作结束、切换到 P1 时**增长
- `roundNumber` 在 **P1 操作结束、切换回 P0 时**增长
- 起始 pointsLevel = 1，即：
  - 第 1 轮 P0 操作：5 点
  - 第 1 轮 P1 操作：7 点
  - 第 2 轮 P0 操作：7 点
  - 第 2 轮 P1 操作：9 点
  - ……

**各语法单元消耗点数（根据 AST 遍历计费）：**

| 语法单元 | 消耗 | 示例 |
|---------|------|------|
| 变量 `x` | 1 | `x` |
| 整数常量 | 1 | `2`, `-5` |
| 小数常量 | 每数字 1 | `3.14` → 4 |
| `+`, `-`（二元） | 1 | `x+2` |
| `-`（一元负号） | 0（不额外消耗） | `-x`, `-(x+1)` |
| `*` | 2 | `x*3` |
| `/` | 2 | `x/2` |
| `^`（乘方） | 3 | `x^2` |
| `sin`, `cos`, `tan` | 3 | `sin(x)` |
| `sqrt`, `abs` | 2 | `sqrt(x)` |
| `log`, `ln`, `exp` | 3 | `log(x)` |
| `asin`, `acos`, `atan` | 4 | `asin(x)` |

**计费规则：**
- 解析用户输入的函数表达式 → 遍历 AST 节点 → 累加各节点消耗
- 若总消耗 > 当前轮可用点数，拒绝发射并提示
- 若总消耗 ≤ 可用点数，执行发射（每次发射独立计费）

**策略意义：**
- 早期轮次点数少，倒逼玩家使用简单的线性/二次函数，考验对基础函数轨迹的预判
- 后期轮次点数多，允许构造复杂的复合函数绕过障碍，增加翻盘可能性
- 玩家需在函数复杂度与命中率之间做权衡

### 历史轨迹
- **只保留上一条**攻击轨迹（淡灰色线条）
- 新的攻击开始后，上一条轨迹会被替换
- 方便玩家参考上一次发射效果调整

### 暂停系统
- 游戏过程中随时可以**暂停**
- 两种触发方式：
  1. **点击游戏页面的 PAUSE 按钮**
  2. **按下键盘 ESC 键**
- 暂停后进入暂停菜单页，可执行：
  - **继续游戏**（继续当前回合，不重置任何状态）
  - **存档到三个槽位**（覆盖已有存档会提示确认）
  - **返回标题页**
- 再次按下 ESC 或点击继续按钮即回到游戏
- 暂停期间游戏状态、轨迹、方块、轮数、点数全部保留

### 存档系统
- **三个存档槽位**，玩家可自由保存/读取/删除
- **存档存储在可执行文件所在目录**的 `saves/` 子目录（跨平台通用：Windows/Linux/macOS 统一行为）
- 存档内容（JSON）：
  - `version`: 存档版本号
  - `currentPlayer`: 当前玩家索引（0 或 1）
  - `roundNumber`: 当前轮数
  - `pointsLevel`: 点数等级（与 roundNumber 不同步）
  - `selectedSquare`: 已选中的发射方格索引
  - `phase`: 游戏阶段（WaitingInput / Animating / RoundEnd / GameOver）
  - `message`: 当前提示文字
  - `currentExpr`: 当前表达式（如有）
  - `config`: GameConfig（含方块数、障碍物数/大小、玩家颜色、坐标标签开关、网格线开关）
  - `player0` / `player1`: 每个玩家的所有方块（坐标、大小、是否摧毁）
  - `obstacles`: 所有障碍物（坐标、大小、是否摧毁）
  - `history`: 上一条历史轨迹的点集
- **跨游戏会话持久化**：关闭程序后重新打开，存档依然可读取
- **读档后恢复配置**：存档中的 GameConfig（玩家颜色、坐标开关、网格开关等）会被读回并应用
- 存档管理页在标题页的 "Load / Manage Saves" 按钮进入，每个槽位显示：
  - 轮数
  - 当前轮到谁
  - 保存时间戳
  - LOAD / DELETE 操作按钮

---

## 技术栈

| 项目 | 选择 |
|------|------|
| 语言 | C++17 |
| GUI 框架 | Qt6 (Widgets) |
| 构建工具 | CMake + Ninja |
| 依赖管理 | vcpkg（需 VCPKG_ROOT 环境变量） |
| 预设配置 | CMakePresets.json（Windows/Linux/macOS Release/Debug preset） |
| 数学解析 | 递归下降解析器（Recursive Descent Parser） + AST 求值 |
| 图形渲染 | QWidget + QPainter 自定义绘制 |
| 存档格式 | JSON 文件 |
| 架构模式 | MVVM (Model-View-ViewModel) |

---

## 项目结构 (MVVM)

```
Graphwar/
├── CMakeLists.txt                          # CMake 主配置（Qt6、跨平台部署、RPATH、Icon）
├── CMakePresets.json                       # 跨平台构建预设（release-windows/debug-windows/release-linux/release-macos）
├── PLAN.md
├── README.md
├── resources/
│   ├── app.ico                             # 应用图标（Windows exe 嵌入 + Qt 运行时）
│   ├── resources.qrc                       # Qt 资源文件（AUTORCC 编译）
│   └── app.rc                              # Windows 资源脚本（exe 图标）
└── src/
    ├── main.cpp                            # 程序入口，设置窗口图标
    ├── model/                              # Model 层：纯数据容器，不含业务逻辑
    │   ├── GameModel.h                     # 游戏数据模型（玩家、方块、障碍物、当前玩家、轮数、点数等级、阶段、轨迹、历史）
    │   ├── GameConfig.h                    # 配置数据（方块数、障碍物数/大小、玩家颜色、坐标标签开关、网格线开关）
    │   ├── GamePhase.h                     # 游戏阶段枚举（WaitingInput / Animating / RoundEnd / GameOver）
    │   ├── Player.h                        # 玩家数据（id、颜色、方块列表）
    │   ├── Square.h                        # 方块数据（坐标、大小、存活状态）
    │   ├── SaveInfo.h/cpp                  # 存档元信息（slot、exists、roundNumber、currentPlayer、savedAt、displayTime）
    │   └── parser/                         # 数学表达式解析器（仅被 Model 层调用）
    │       ├── Token.h                     # Token 类型定义（Number / Variable / 运算符 / 数学函数 / Paren / End）
    │       ├── Tokenizer.h/cpp             # 词法分析器（字符串 → Token 列表）
    │       ├── Expression.h/cpp            # AST 节点定义（NumberExpr / VariableExpr / BinaryExpr / UnaryExpr）及 eval / cost 实现
    │       ├── Parser.h/cpp                # 递归下降解析器（Token 列表 → AST）
    │       └── Evaluator.h/cpp             # 对外接口：evaluate(expr, x) 求值、calculateCost(expr) 计费
    ├── viewmodel/                          # ViewModel 层：业务逻辑 + 数据适配
    │   ├── GameViewModel.h/cpp             # 核心 ViewModel：持有 Model+GameConfig，暴露只读接口（currentPlayer/roundNumber/availablePoints/playerColor/playerSquares/obstacles/historyTrajectory/config 等），通过 signals/slots 与 View 通信
    │   └── SaveManager.h/cpp               # 存档文件读写（三槽位 JSON 读写，位于可执行文件同目录 saves/）
    ├── view/                               # View 层：被动渲染 + 用户输入
    │   ├── MainWindow.h/cpp                # 主窗口（QStackedWidget 切换五页：开始/游戏/存档管理/暂停/配置），信号绑定 ViewModel
    │   ├── GameCanvas.h/cpp                # 画布：坐标系、方块、障碍物、函数曲线、动画轨迹、网格线、坐标标签、游戏结束遮罩
    │   ├── FunctionInput.h/cpp             # 函数输入面板（回车触发发射、默认按钮、点数实时预览、消息提示）
    │   ├── ConfigPage.h/cpp                # 配置页（方块数 SpinBox、障碍物数 SpinBox、障碍物大小 SpinBox、8 色预设色块、坐标标签开关、网格线开关、START GAME / BACK 按钮）
    │   ├── SaveManagerPage.h/cpp           # 存档管理页（三槽位 Load/Delete，通过 ViewModel 接口操作）
    │   └── PauseMenuPage.h/cpp             # 暂停菜单（继续/保存到三槽位/返回标题，通过 ViewModel 接口操作）
    └── utils/                              # 工具层：通用算法与结构
        └── Geometry.h                      # Rect（矩形数据结构 + 矩形包含判定）
```

**MVVM 职责划分：**

| 层 | 职责 | 文件 |
|----|------|------|
| **Model** | 纯数据容器，无业务逻辑，可被序列化/反序列化 | `GameModel`, `GameConfig`, `GamePhase`, `Player`, `Square`, `SaveInfo` |
| **ViewModel** | 持有 Model + GameConfig，封装全部游戏/配置/存档逻辑，通过只读访问器和 signal 暴露状态，通过 slot 接收用户操作 | `GameViewModel`, `SaveManager` |
| **View** | 被动渲染，通过信号绑定 ViewModel 的属性变化，用户操作调用 ViewModel 的槽函数，不直接读写 Model | `MainWindow`, `GameCanvas`, `FunctionInput`, `ConfigPage`, `SaveManagerPage`, `PauseMenuPage` |
| **Utils** | 通用工具（与业务逻辑无关） | `Geometry.h` |

**ViewModel 与 View 之间的关键接口（不完全清单）：**

- 状态读取：`currentPlayer()`, `roundNumber()`, `availablePoints()`, `phase()`, `playerColor(int)`, `aliveCount(int)`, `playerSquares(int)`, `obstacles()`, `trajectory()`, `historyTrajectory()`, `selectedSquareIndex()`, `isWaitingInput()`, `isAnimating()`, `isGameOver()`, `showGridLines()`, `showCoordinates()`, `config()`
- 玩家操作槽：`newGame()`, `launch(QString)`, `updateCostPreview(QString)`, `pause()`, `resume()`, `togglePause()`, `setConfig(GameConfig)`
- 存档操作槽：`saveToSlot(int)`, `loadFromSlot(int)`, `deleteSlot(int)`, `slotCount()`, `slotInfos()`, `slotPath(int)`
- 信号：`turnChanged`, `roundChanged`, `pointsChanged`, `costPreviewChanged`, `phaseChanged`, `messageChanged`, `trajectoryUpdated`, `animationFinished`, `pausedChanged`, `saveResult`, `gameOver`

---

## 核心模块详细设计

### 1. 数学表达式解析器

**支持的语法：**
- 运算符：`+`, `-`（二元和一元）, `*`, `/`, `^`（乘方）
- 数学函数：`sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sqrt`, `abs`, `log`, `ln`, `exp`
- 注意：`log` / `ln` / `sqrt` 的真数自动取绝对值，`asin` / `acos` 的参数自动夹紧到 [-1, 1]，避免无定义
- 常数：支持整数和小数（如 `2`, `-3.14`）
- 变量：`x`（大小写不敏感）
- 输入示例：`2*x^3 + sin(x) + 5`
- **输入框回车直接触发发射**（与点击 FIRE 按钮等效）

**流程：**
```
输入字符串 → 词法分析(Tokenizer) → 递归下降解析(Parser) → AST → Evaluator 求值 / 计费
```

**一元负号解析：**
- 当 `-` 出现在表达式开头、紧跟 `(` 或 `^` 之后、紧跟其他运算符之后时，解析为 `NegateExpr`（一元负号）
- `NegateExpr` 不额外消耗点数，仅从其唯一子节点继承 cost

**自动常数调整：**
- 由于函数必须经过发射方格中心 (cx, cy)，系统自动计算常数项
- 用户输入函数 f(x) → 系统计算 C = cy - f(cx) → 实际绘制函数为 f(x) + C
- 此时 f(cx) + C = cy，保证图像必过发射点
- 用户无需手动调整常数，直接在输入框输入表达式即可
- 常数 C 不消耗点数（不计入函数 cost）

### 2. 游戏引擎 (GameViewModel)

**状态机：**
```
WAITING_INPUT → ANIMATING → ROUND_END → WAITING_INPUT → ...
                                        → GAME_OVER (当一方所有方块被摧毁)
```

**详细流程：**
1. 轮到当前玩家 → 系统**随机选择**一个未摧毁方格作为发射点（玩家不可自己选择）；更新可用点数显示
2. 玩家输入函数 → 实时点数预览（updateCostPreview）
3. 回车或点击 FIRE 触发发射
4. **点数校验**：解析 AST 计算函数总消耗，若超过当前可用点数则拒绝并提示
5. 系统自动计算常数 C = cy - f(cx)，调整函数图像使其经过发射方格中心
6. 进入动画阶段（QTimer @ 16ms）：从发射点向**对方方向**逐帧延伸轨迹
7. **障碍物优先碰撞**：先检查是否碰到未被破坏的障碍物，碰到则**立即中止攻击**，破坏该障碍物，结束该回合
8. 若未被障碍物阻挡，则检查是否命中对方方块；命中后标记被击中的方格（继续延伸直到边界，允许一次攻击命中多个方格）
9. 检查胜负：若一方所有方格全被摧毁，游戏结束，弹 "PLAY AGAIN / BACK TO START PAGE" 对话框
10. 否则：轨迹超出边界（|x| > 30 或 |y| > 25）后结束当前回合
11. `nextTurn()`：
    - 若当前是 P1：`roundNumber++`（轮数增长）
    - 若当前是 P0：`pointsLevel++`（点数增长）
    - 切换当前玩家 → 系统重新随机挑选发射点 → WAITING_INPUT
12. 显示 "Hit!" 或 "Miss!"

### 3. 画布渲染 (GameCanvas / View)

**与 ViewModel 的绑定方式：**
- GameCanvas 构造时接收 `GameViewModel* vm`，通过 vm 的只读接口获取数据
- 信号触发重绘（通过外部 timer 或 MainWindow 驱动）
- Canvas 不直接修改任何游戏数据，仅作绘制

**坐标系：**
- 世界坐标：x ∈ [-20, 20], y ∈ [-17, 17] 实际使用范围；动画轨迹扩展到 x ∈ [-30, 30], y ∈ [-25, 25]
- 自适应窗口大小：画布宽高与 `min(w, h)/40` 为缩放系数
- 原点 (0,0) 在画布中心

**绘制内容（按 z-order，由先到后）：**
1. 背景（深色 #0f0f1a）
2. **网格线**（可选，默认关闭；每 5 单位一条，横竖对称分布，范围覆盖 [-30, 30]）
3. 坐标轴（灰色实线，横轴穿过 y=0，纵轴穿过 x=0）
4. **障碍物**（灰色填充矩形 + 深色边框；被破坏后显示为虚线轮廓，无填充）
5. 双方的方格（P1 蓝调、P2 红调，半透明矩形，被摧毁的统一标记为灰色）
6. **选中方格**：白色虚线边框 + **可选的 (cx, cy) 坐标标签**（默认开启，可在配置页关闭；标签带半透明黑底，避免重叠画布边缘时自动翻转到另一侧）
7. **上一条历史轨迹**（淡灰色，仅保留最近一次）
8. **当前动画轨迹**（玩家 1 蓝调 / 玩家 2 红调，亮色，逐渐延伸）
9. **轨迹上的"弹头"**（高亮圆点，沿曲线运动）
10. 顶部状态栏：P1 存活数、Round N、点数、P2 存活数（P1/P2 颜色随配置动态变化）
11. 游戏结束遮罩：大字 "GAME OVER" + 对话框（由 MainWindow 的 Dialog 处理）

### 4. 碰撞检测

- 每帧对动画新绘制的轨迹点进行检测
- **优先检查障碍物**（从玩家发射方向，先检测障碍，再检测对方）
- 对每个未被破坏的障碍物：检测点是否在矩形区域内
- 对对方每个未摧毁方格：检测点是否在矩形区域内
- 区域判定：`cx - w/2 ≤ px ≤ cx + w/2 && cy - h/2 ≤ py ≤ cy + h/2`（与方块可视大小一致）
- 碰到障碍物后立即停止动画，标记破坏，结束该回合（不检查后续方块）
- 命中对方方块后继续延伸直到出边界，累计所有命中数

**障碍物生成时的间距（非碰撞检测阶段，而是生成阶段）：**
- 障碍物之间最小中心距 = `size + 0.7`，避免生成时互相重叠
- 障碍物与玩家方格最小中心距 = `size * 0.5 + 0.3`（障碍物半宽 + 方格半宽 + 缓冲）
- 两个间距均随 size 自动缩放

### 5. 存档系统

- 三个槽位 `slot_0.json` / `slot_1.json` / `slot_2.json`
- 存储在**可执行文件所在目录**的 `saves/` 子目录（跨平台通用）
- 不使用 `%APPDATA%` 或 `~/.local/share/`，确保三个平台一致行为
- 通过 `SaveManager::savesDir()` 计算路径 = `QCoreApplication::applicationDirPath() + "/saves"`
- `SaveManager::slotInfo(int)` 读取 JSON 中的 `currentPlayer` 和 `roundNumber` 用于 UI 显示
- 存档写入/读取通过 `GameViewModel::saveToSlot(int)` / `loadFromSlot(int)` 间接调用 SaveManager（View 层不直接调用 SaveManager）
- 存档字段请参考上面"存档系统"小节

### 6. UI 布局

**流程：MainWindow 使用 QStackedWidget 切换五页**

**第 0 页 — 开始界面：**
```
┌──────────────────────────────────────────────────────┐
│                                                      │
│                   GRAPH WAR                         │
│         Two players, one curve.                      │
│      Fire functions at each other!                   │
│                                                      │
│               [ NEW GAME ]                          │
│          [ Load / Manage Saves ]                    │
│                                                      │
└──────────────────────────────────────────────────────┘
```
- **NEW GAME** → 进入 Config 页（不直接进入游戏）
- **Load / Manage Saves** → 进入存档管理页
- **无独立 Config 按钮**（已合并到 NEW GAME 流程）

**第 1 页 — 游戏界面：**
```
┌──────────────────────────────────────────────────────┐
│  P1:3   Round 3   Pts:8   P2:2        [PAUSE]      │  ← 顶栏（P1/P2 文字颜色随配置动态变化）
├──────────────────────────────────────────────────────┤
│                                                      │
│              Game Canvas                            │
│   ┌───┐      ┌─┐          ┌─┐        ┌───┐        │
│   │ P │      │Ob│          │Ob│        │ P │        │
│   │ 1 │(-12.3,5.1)         │Ob│        │ 2 │        │  ← 选中方块坐标标签
│   │ ■ │ ╲ ╱ ╲ ╱ ────────→ ╱ ╲ ╱       │ ■ │        │
│   └───┘                                            └───┘
│                                                      │
├──────────────────────────────────────────────────────┤
│  f(x) = [________________]  消耗:3/8  [ FIRE ]     │  ← 输入区，回车触发发射
│  [消息提示：Hit! / Miss! / Player 1 wins! / ...]    │
└──────────────────────────────────────────────────────┘
```

**第 2 页 — 存档管理页：**
```
┌──────────────────────────────────────────────────────┐
│                MANAGE SAVES                         │
├──────────────────────────────────────────────────────┤
│  Slot 1:   Round 3, P1's turn   [LOAD] [DELETE]   │
│            saved 2024-07-08 14:32                   │
├──────────────────────────────────────────────────────┤
│  Slot 2:   Round 7, P2's turn   [LOAD] [DELETE]   │
│            saved 2024-07-08 15:01                   │
├──────────────────────────────────────────────────────┤
│  Slot 3:   (empty)               [-----] [------]  │
├──────────────────────────────────────────────────────┤
│                        [ ← Back ]                   │
└──────────────────────────────────────────────────────┘
```
- 通过 ViewModel 的 `slotInfos()` 读取槽位信息
- Load / Delete 操作调用 ViewModel 的 `loadFromSlot()` / `deleteSlot()`

**第 4 页 — 配置页：**
```
┌──────────────────────────────────────────────────────┐
│                   Game Settings                      │
├──────────────────────────────────────────────────────┤
│  Squares per player (1-10):   [5] ±                 │
│  Obstacles (0-30):           [10] ±                 │
│  Obstacle size (0.5-5.0):   [1.8] ±                 │
│                                                      │
│  ☑ Show square coordinates                          │
│  ☐ Show grid lines                                  │
│                                                      │
│  P1 Color:  ■ ■ ■ ■ ■ ■ ■ ■  (8 preset swatches)   │
│  P2 Color:  ■ ■ ■ ■ ■ ■ ■ ■  (8 preset swatches)   │
├──────────────────────────────────────────────────────┤
│           [ ← Back ]     [ START GAME ]            │
└──────────────────────────────────────────────────────┘
```
- 颜色使用 8 种预设色块（蓝/红/绿/橙/紫/青/黄/粉），点击选中
- 两位玩家不能选择相同颜色，否则 START GAME 时提示
- SpinBox 按钮使用 ± 符号（`setButtonSymbols(PlusMinus)`）
- START GAME 后将配置写入 ViewModel，再调用 `newGame()` 初始化游戏并切换到游戏页
- Back / ESC 返回开始页，配置不保存

**第 3 页 — 暂停菜单页：**
```
┌──────────────────────────────────────────────────────┐
│                                                      │
│                PAUSED                               │
│                                                      │
│                [ CONTINUE (ESC) ]                   │
│                                                      │
│  ┌─Slot 1─────────────────────────────────────────┐ │
│  │  Round 3, P1's turn | saved 2024-07-08 14:32  │ │
│  │                                   [ Save over ]│ │
│  └─────────────────────────────────────────────────┘ │
│  ┌─Slot 2─────────────────────────────────────────┐ │
│  │  Round 7, P2's turn | saved 2024-07-08 15:01  │ │
│  │                                   [ Save over ]│ │
│  └─────────────────────────────────────────────────┘ │
│  ┌─Slot 3─────────────────────────────────────────┐ │
│  │  (empty)                             [ Save ] │ │
│  └─────────────────────────────────────────────────┘ │
│                                                      │
│                [ BACK TO TITLE ]                    │
│                                                      │
└──────────────────────────────────────────────────────┘
```

**交互流程（MVVM 数据流）：**
1. 开始界面点击 NEW GAME → 切换到配置页，回显当前 GameConfig
2. 配置页点击 START GAME → 检查 P1/P2 颜色不冲突 → 调用 `vm.setConfig(cfg)` → 调用 `vm.newGame()` → 切换到游戏页，系统随机挑选发射点
3. 配置页点击 Back / ESC → 返回开始页，配置不保存
4. 开始界面点击 Load / Manage Saves → 切换到存档管理页
5. 游戏过程中点击 PAUSE 或按 ESC → 切换到暂停页（VM 进入 paused 状态）
6. 暂停页 CONTINUE 或 ESC → 回到游戏，状态完全恢复（VM 退出 paused 状态）
7. 暂停页点击某槽位的 Save → 通过 `vm.saveToSlot(slot)` 将当前游戏状态写入对应 JSON 文件，覆盖已有存档时提示确认
8. 存档管理页点击 LOAD → 通过 `vm.loadFromSlot(slot)` 读回游戏状态 + 配置 → 回到游戏页
9. 存档管理页点击 DELETE → 调用 `vm.deleteSlot(slot)` 删除对应 JSON 文件
10. 游戏过程中：
    - 输入区输入表达式 → 实时计算消耗点数（`vm.updateCostPreview`）
    - 回车或点击 FIRE → 调用 `GameViewModel::launch(QString expr)`
    - 玩家**无法自己选择发射点**，发射点完全由系统在回合开始时随机挑选
    - ViewModel 内部：解析表达式 → 计算消耗 → 校验点数 → 计算常数 C → 进入动画阶段
    - QTimer 驱动逐帧绘制轨迹；优先检查障碍物碰撞，然后检查对方方格；到达边界或障碍物后结束
    - 游戏结束后 emit `gameOver`，MainWindow 弹出 "PLAY AGAIN / BACK TO START PAGE" 模态对话框

### 7. 应用图标

- `resources/app.ico` 同时服务两个用途：
  1. **Qt 运行时图标**：通过 `resources.qrc` 编译进二进制，`main.cpp` 调用 `app.setWindowIcon(QIcon(":/app.ico"))`，显示在窗口标题栏和任务栏
  2. **Windows exe 图标**：通过 `app.rc` 资源脚本嵌入，使 `.exe` 文件在资源管理器中显示图标
- macOS 需额外准备 `.icns` 文件并设置 `MACOSX_BUNDLE_ICON_FILE`
- Linux 使用 Qt 运行时图标即可，桌面图标依赖 `.desktop` 文件

---

## 方格随机生成算法

每局初始化时调用 `GameViewModel::generateSquares()`：

```
输入：方块数量、P1 颜色、P2 颜色
输出：双方 N 个不重叠的方格

算法：
1. 定义半区范围：
   - P1（左侧）：x ∈ [-20, -1]，y ∈ [-10, 10]
   - P2（右侧）：x ∈ [1, 20]，y ∈ [-10, 10]
2. 对每个玩家重复生成候选位置，直到产生 count 个方格：
   a. 在范围内随机选取 (cx, cy)
   b. 检查与该玩家已生成方格的最小间距 ≥ 3.0 单位（中心距）
   c. 通过则加入列表
3. 若尝试次数超过上限（300 次），用固定位置（如 (xMin+xMax)/2, 0）兜底
```

方格世界坐标 **0.8×0.8**（正方形），碰撞判定区域与方格大小一致。

障碍物调用 `GameViewModel::generateObstacles(count, size)`：
- 分布在 x ∈ [-20, 20]，y ∈ [-20, 20]，共 count 个（默认 10）
- 障碍物之间最小中心距 `size + 0.7` 单位（避免互相重叠）
- 障碍物与玩家方格最小中心距 `size * 0.5 + 0.3` 单位（避免覆盖玩家方块）
- 最多尝试 500 次，超过则放弃生成后续障碍物

**对称性说明：**
- 两侧半区各自独立随机，不做镜像对称
- 每局重启时重新随机，增加 replayability

---

## 构建与运行

### 前提条件
- C++17 编译器（MSVC 2022 / GCC 9+ / Clang 10+）
- CMake ≥ 3.20
- Ninja
- vcpkg（环境变量 `VCPKG_ROOT` 指向 vcpkg 根目录）
- Qt6（通过 vcpkg 安装的 `qtbase:x64-windows` / `qtbase:x64-linux` / `qtbase:x64-osx`）

### Windows（使用 vcpkg）

```bat
cd d:\Graphwar
cmake --preset release-windows
cmake --build build
build\Graphwar.exe
```

### Linux（使用 vcpkg）

```bash
cd ~/Graphwar
cmake --preset release-linux
cmake --build build
./build/Graphwar
```

### macOS（使用 vcpkg）

```bash
cd ~/Graphwar
cmake --preset release-macos
cmake --build build
./build/Graphwar
```

### 跨平台部署说明

- **Windows**：CMake 构建完成后自动通过 `POST_BUILD` 命令复制 `qwindows.dll` 到 `build/platforms/` 目录
- **Linux / macOS**：`CMakeLists.txt` 配置了 `CMAKE_BUILD_WITH_INSTALL_RPATH` 和 `INSTALL_RPATH="$ORIGIN:$ORIGIN/lib"`，可执行文件运行时在自身所在目录及 `lib/` 子目录查找共享库
- **存档位置**：三平台统一为 `可执行文件所在目录/saves/slot_{0,1,2}.json`

### 存档位置

存档统一存放在**可执行文件所在目录**的 `saves/` 子目录下（跨平台通用）：

| 平台 | 路径 |
|------|------|
| 任意平台 | `<可执行文件所在目录>/saves/slot_{0,1,2}.json` |
| Windows 示例 | `D:\Graphwar\build\saves\slot_0.json` |
| Linux 示例 | `~/Graphwar/build/saves/slot_0.json` |
| macOS 示例 | `~/Graphwar/build/saves/slot_0.json` |

### 常见问题

**Q: 编译器报 `type_traits` / `cstddef` 找不到？**
A: 说明 MSVC 的环境变量（INCLUDE / LIBPATH / PATH）未激活。请在 VS BuildTools 的 "x64 Native Tools Command Prompt" 中执行构建，或在 PowerShell 里调用 `& "${env:VCPKG_ROOT}\vcpkg env" -- cmake --build build`。

**Q: 运行时 Qt 报 "no Qt platform plugin could be initialized"？**
A: 可执行文件旁的 `platforms/qwindows.dll`（Windows）或 `libqcocoa.dylib`（macOS）或 `libqxcb.so`（Linux）未部署。CMakePresets.json 已通过 POST_BUILD 自动部署，清理 build 目录后重新配置即可。

**Q: 修改了 PLAN.md 但游戏行为不一致？**
A: PLAN.md 中的规则描述需与 `GameViewModel.cpp` / `GameModel.h` / `GameConfig.h` 的实际实现一致；任何逻辑/配置变更后请同步更新 PLAN.md。
