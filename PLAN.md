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
- 玩家攻击过程中，若在到达对方方格或触碰边界之前碰到障碍物：
  - 攻击**立即中止**
  - 该障碍物被破坏（视觉上变为虚线轮廓）
  - 回合结束，轮到对方
- **已被破坏的障碍物不阻挡攻击**（子弹会直接穿过）
- 障碍物在游戏过程中保持已破坏状态，不会恢复

### 游戏统计功能
- 游戏结束时弹出统计弹窗，分别显示双方在本局中的统计数据
- 统计内容包括：
  - **发射次数**：玩家总共发射的次数
  - **命中次数**：击中对方方块的次数
  - **障碍物破坏次数**：击中障碍物的次数
  - **命中率**：命中次数 / 发射次数 × 100%
  - **消耗点数**：总共消耗的点数
- 统计数据在每局游戏开始时重置，游戏结束时汇总显示

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

### 粒子特效系统
- 攻击命中、障碍物破坏等事件触发粒子效果
- 粒子包含位置、速度、颜色、大小、生命周期属性
- 粒子使用 QPainter 渲染，随时间逐渐消散
- 粒子系统是纯代码实现，无需额外资源文件

### 音频系统
- **背景音乐 (BGM)**：游戏开始时自动播放 `resources/AIZO-8bit.m4a`，支持音量调节（0-100）和静音切换
- **音效 (SFX)**：使用程序合成的 WAV 音效，支持以下类型：
  - `Launch` - 发射
  - `Hit` - 命中
  - `Obstacle` - 障碍物碰撞
  - `Miss` - 未命中
  - `TurnEnd` - 回合结束
  - `GameOver` - 游戏结束
  - `Button` - 按钮点击
- 音效支持音量调节（0-100）和静音切换
- 音频管理器采用单例模式（`AudioManager::instance()`）

### 回合过渡动画
- 回合结束后，进入过渡动画阶段（约 0.5 秒）
- 底部显示进度条，从左向右填充下一个玩家的颜色
- 上方显示 "PLAYER X" 文字，渐入效果
- 过渡动画期间禁止输入，动画结束后自动切换到下一个玩家
- 使用缓动函数使动画更加平滑自然

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
| 构建工具 | CMake + Ninja / MinGW |
| 依赖管理 | vcpkg（需 VCPKG_ROOT 环境变量） |
| 预设配置 | CMakePresets.json（Windows/Linux/macOS Release/Debug preset） |
| 数学解析 | ShuntingYard 调车场算法 + Token 流求值 |
| 图形渲染 | QWidget + QPainter 自定义绘制 |
| 粒子系统 | QPointF + QPainter 自定义实现 |
| 音频播放 | Qt6 Multimedia (QMediaPlayer + QAudioOutput for BGM, QAudioSink for SFX) |
| 存档格式 | JSON 文件 |
| 架构模式 | MVVM + PropertyTrigger + Command (Model-View-ViewModel) |
| 预编译头 | precomp.h（标准库 + Qt6 + 项目公共头文件） |

---

## 项目结构 (MVVM + PropertyTrigger + Command)

```
Graphwar/
├── CMakeLists.txt                  # 根 CMake：project() + find_package(Qt6) + add_subdirectory(src) + 打包配置
├── CMakePresets.json               # 跨平台构建预设
├── PLAN.md
├── README.md
├── vcpkg.json
├── resources/
│   ├── app.ico                     # 应用图标
│   ├── app.rc                      # Windows 资源脚本
│   ├── resources.qrc               # Qt 资源文件
│   ├── start_background.png        # 开始页背景图
│   └── AIZO-8bit.m4a               # 背景音乐
└── src/
    ├── CMakeLists.txt              # 可执行文件定义：源文件列表 + 预编译头 + Qt 链接 + 平台插件部署
    ├── precomp.h                   # 预编译头
    ├── main.cpp                    # 程序入口：创建 GraphwarApp → show_main_window → QApplication::exec()
    ├── app/                        # 应用层（Composition Root）
    │   └── GraphwarApp.h/cpp       # 持有 ViewModel + MainWindow，执行绑定（wiring）
    ├── common/                     # 基础设施 + 跨层共享数据
    │   ├── frame.h/cpp             # PropertyTrigger 通知系统
    │   ├── property_ids.h          # 属性 ID 枚举
    │   ├── GameConfig.h            # 配置数据
    │   ├── GamePhase.h             # 阶段枚举
    │   ├── GameState.h             # 游戏状态快照（含 slotInfos/slotCount/statistics，View 数据入口）
    │   ├── SaveInfo.h/cpp          # 存档元信息
    │   ├── Square.h                # 方块/障碍物数据（含 Rect）
    │   ├── Particle.h           # 粒子数据结构
    │   ├── AudioState.h         # 音效类型枚举
    │   └── GameStatistics.h     # 游戏统计数据结构（双方发射/命中/命中率等）
    ├── model/                      # Model 层：业务逻辑 + 工具类
    │   ├── GameModel.h/cpp         # 核心业务逻辑（状态管理、动画、回合切换、碰撞检测、统计数据收集、JSON 序列化）
    │   ├── Player.h                # 玩家数据
    │   ├── SaveManager.h/cpp       # 存档文件 IO（静态工具类）
    │   ├── AudioManager.h/cpp      # 音频管理器（单例模式，BGM + SFX）
    │   └── parser/                 # 数学表达式解析器
    │       ├── Token.h             # Token 类型
    │       ├── Tokenizer.h/cpp     # 词法分析
    │       ├── Expression.h/cpp    # AST 节点 + eval/cost
    │       ├── Parser.h/cpp        # 递归下降解析器（备用）
    │       ├── ShuntingYard.h/cpp  # 调车场算法（主要使用）
    │       └── Evaluator.h/cpp     # 对外接口
    ├── viewmodel/                  # ViewModel 层：继承 PropertyTrigger，暴露属性 + 命令
    │   ├── GameViewModel.h/cpp     # 持有 GameModel，转发 Model 信号 → PropertyTrigger::fire()，同步统计数据到 GameState
    │   └── commands/               # 命令实现（每个文件一个 std::function getter）
    │       ├── NewGameCommand.cpp
    │       ├── LaunchCommand.cpp
    │       ├── UpdateCostPreviewCommand.cpp
    │       ├── PauseCommand.cpp
    │       ├── ResumeCommand.cpp
    │       ├── NextTurnCommand.cpp
    │       ├── SetConfigCommand.cpp
    │       ├── SaveSlotCommand.cpp
    │       ├── LoadSlotCommand.cpp
    │       ├── DeleteSlotCommand.cpp
    │       ├── SetBgmVolumeCommand.cpp
    │       ├── SetBgmMutedCommand.cpp
    │       ├── SetSfxVolumeCommand.cpp
    │       └── SetSfxMutedCommand.cpp
    ├── view/                       # View 层：QWidget，存储命令指针 + 注册通知回调
    │   ├── MainWindow.h/cpp        # 主窗口（QStackedWidget 五页切换），持有所需命令 + 通知入口
    │   └── widgets/                # 自定义 Widget
    │       ├── GameCanvas.h/cpp    # 画布渲染（含粒子渲染）
    │       ├── FunctionInput.h/cpp # 输入面板
    │       ├── ConfigPage.h/cpp    # 配置页
    │       ├── SaveManagerPage.h/cpp  # 存档管理页
    │       ├── PauseMenuPage.h/cpp    # 暂停菜单页
    │       └── GuidePage.h/cpp        # 游戏引导页（游戏规则说明）
```

**MVVM + PropertyTrigger + Command 职责划分：**

| 层 | 职责 | 文件 |
|----|------|------|
| **Model** | 游戏业务逻辑 + 数据 + 序列化 + 音频管理 | `GameModel`, `SaveManager`, `AudioManager`, `parser/*` 等 |
| **ViewModel** | 继承 `PropertyTrigger`，转发 Model 信号 → `fire(id)`；暴露 `std::function` 命令 getter | `GameViewModel`, `commands/*` |
| **View** | 存储 `const GameViewModel*` + `std::function` 命令，注册 `PropertyNotification` 回调更新 UI | `MainWindow`, `widgets/*` |
| **Common** | PropertyTrigger 通知框架 + 共享数据结构 | `frame`, `property_ids`, `Particle`, `AudioState` |

**通信模式：**

```
GraphwarApp (构造时绑定):
  m_view_model.add_notification(m_main_wnd.get_notification());
  m_main_wnd.set_pause_command(m_view_model.get_pause_command());
  // ... 其他命令 & 属性绑定

用户操作 → View 调用存储的 std::function 命令 → GameViewModel → Model 状态变更
Model 状态变更 → Qt signal → GameViewModel → PropertyTrigger::fire(id) →  View 通知回调更新 UI
```

- View 不直接持有 ViewModel（通过指针访问状态），命令以 `std::function` 形式注入
- `GraphwarApp` 是 Composition Root，负责创建实例并执行绑定（wiring）
- Model 不依赖任何 View/ViewModel 代码

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
输入字符串 → 词法分析(Tokenizer) → ShuntingYard 调车场 → Token 流求值 / 计费
```
（递归下降解析器 `Parser` 保留作为备用实现）

**一元负号解析：**
- 当 `-` 出现在表达式开头、紧跟 `(` 或 `^` 之后、紧跟其他运算符之后时，解析为一元负号
- 一元负号不额外消耗点数，仅从其唯一子节点继承 cost

**自动常数调整：**
- 由于函数必须经过发射方格中心 (cx, cy)，系统自动计算常数项
- 用户输入函数 f(x) → 系统计算 C = cy - f(cx) → 实际绘制函数为 f(x) + C
- 此时 f(cx) + C = cy，保证图像必过发射点
- 用户无需手动调整常数，直接在输入框输入表达式即可
- 常数 C 不消耗点数（不计入函数 cost）

### 2. 游戏引擎 (GameModel)

**状态机：**
```
WAITING_INPUT → ANIMATING → ROUND_END → WAITING_INPUT → ...
                                        → GAME_OVER (当一方所有方块被摧毁)
```

**详细流程：**
1. 轮到当前玩家 → 系统**随机选择**一个未摧毁方格作为发射点（玩家不可自己选择）；更新可用点数显示
2. 玩家输入函数 → 实时点数预览（通过 `m_costPreviewCmd` 命令）
3. 回车或点击 FIRE 触发发射
4. **点数校验**：解析 Token 流计算函数总消耗，若超过当前可用点数则拒绝并提示
5. 系统自动计算常数 C = cy - f(cx)，调整函数图像使其经过发射方格中心
6. 进入动画阶段（QTimer @ 16ms）：从发射点向**对方方向**逐帧延伸轨迹
7. **障碍物优先碰撞**：先检查是否碰到未被破坏的障碍物，碰到则**立即中止攻击**，破坏该障碍物，结束该回合，触发粒子特效和音效
8. 若未被障碍物阻挡，则检查是否命中对方方块；命中后标记被击中的方格（继续延伸直到边界，允许一次攻击命中多个方格），触发粒子特效和音效
9. 检查胜负：若一方所有方格全被摧毁，游戏结束，弹 "PLAY AGAIN / BACK TO START PAGE" 对话框，触发 GameOver 音效
10. 否则：轨迹超出边界（|x| > 30 或 |y| > 25）后结束当前回合，触发 Miss 音效
11. `nextTurn()`：
    - 若当前是 P1：`roundNumber++`（轮数增长）
    - 若当前是 P0：`pointsLevel++`（点数增长）
    - 切换当前玩家 → 系统重新随机挑选发射点 → WAITING_INPUT，触发 TurnEnd 音效
12. 显示 "Hit!" 或 "Miss!"

### 3. 粒子系统

**粒子数据结构：**
```cpp
struct Particle {
    QPointF pos;      // 位置
    QPointF vel;      // 速度
    QColor color;     // 颜色
    double size;      // 大小
    double life;      // 当前生命周期
    double maxLife;   // 最大生命周期
};
```

**粒子更新：**
- 每帧更新粒子位置：`pos += vel`
- 每帧减少生命周期：`life -= deltaTime`
- 粒子大小随生命周期衰减
- 粒子透明度随生命周期衰减
- 生命周期结束后移除粒子

**粒子触发时机：**
- 攻击命中：发射点产生粒子
- 障碍物破坏：障碍物位置产生粒子
- 方块摧毁：方块位置产生粒子

### 4. 音频系统

**音频管理器（单例模式）：**
- `AudioManager::instance()` 获取全局唯一实例
- **BGM 播放**：使用 QMediaPlayer + QAudioOutput 播放 M4A 文件（自动循环）
- **SFX 播放**：程序合成 WAV 音效，通过 QAudioSink 播放，缓存避免重复合成

**支持的音效类型：**

| 音效类型 | 触发时机 |
|---------|---------|
| `Launch` | 玩家发射攻击 |
| `Hit` | 命中对方方块 |
| `Obstacle` | 碰撞障碍物 |
| `Miss` | 攻击未命中 |
| `TurnEnd` | 回合结束 |
| `GameOver` | 游戏结束 |
| `Button` | 按钮点击 |

**音量控制：**
- BGM 音量范围：0-100
- SFX 音量范围：0-100
- 支持独立静音/取消静音

### 5. 画布渲染 (GameCanvas / View)

**与 ViewModel 的绑定方式：**
- GameCanvas 通过 `m_vm->get_model()` 获取所有状态（`model->currentPlayer()`, `model->playerSquares(pl)`, `model->trajectory()` 等）
- 通过 MainWindow 通知回调中的 `PROP_ID_*` 分发触发 `update()` 重绘
- Canvas 不直接修改任何游戏数据，仅作绘制

**坐标系：**
- 世界坐标：x ∈ [-20, 20], y ∈ [-17, 17] 实际使用范围；动画轨迹扩展到 x ∈ [-30, 30], y ∈ [-25, 25]
- 自适应窗口大小：画布宽高与 `min(w, h)/40` 为缩放系数
- 原点 (0,0) 在画布中心

**绘制内容（按 z-order，由先到后）：**
1. 背景（深色 #14141e）
2. **网格线**（可选，默认关闭；每 5 单位一条，横竖对称分布；边界线 x=-20/x=20/y=-20/y=20 使用更亮颜色 `(80,80,110)` 以区别于内部网格 `(35,35,50)`）
3. 坐标轴（灰色实线，横轴穿过 y=0，纵轴穿过 x=0）
4. **障碍物**（灰色填充矩形 + 深色边框；被破坏后显示为虚线轮廓，无填充）
5. 双方的方格（P1 蓝调、P2 红调，半透明矩形，被摧毁的统一标记为灰色）
6. **选中方格**：白色虚线边框 + **可选的 (cx, cy) 坐标标签**（默认开启，可在配置页关闭）
7. **上一条历史轨迹**（淡灰色，仅保留最近一次）
8. **当前动画轨迹**（玩家 1 蓝调 / 玩家 2 红调，亮色，逐渐延伸）
9. **轨迹上的"弹头"**（高亮圆点，沿曲线运动）
10. **粒子特效**（命中/障碍物破坏时显示）
11. 顶部状态栏：P1 存活数、Round N、点数、P2 存活数（P1/P2 颜色随配置动态变化）
12. 游戏结束遮罩（由 MainWindow 的模态对话框处理，画布上不绘制大字）

### 6. 碰撞检测

- 每帧对动画新绘制的轨迹点进行检测
- **优先检查障碍物**（从玩家发射方向，先检测障碍，再检测对方）
- 对每个未被破坏的障碍物：检测点是否在矩形区域内
- 对对方每个未摧毁方格：检测点是否在矩形区域内
- 区域判定：`cx - w/2 ≤ px ≤ cx + w/2 && cy - h/2 ≤ py ≤ cy + h/2`（与方块可视大小一致）
- 碰到障碍物后立即停止动画，标记破坏，结束该回合（不检查后续方块），触发粒子和音效
- 命中对方方块后继续延伸直到出边界，累计所有命中数，触发粒子和音效

**障碍物生成时的间距（非碰撞检测阶段，而是生成阶段）：**
- 障碍物之间最小中心距 = `size + 0.7`，避免生成时互相重叠
- 障碍物与玩家方格最小中心距 = `size * 0.5 + 0.3`（障碍物半宽 + 方格半宽 + 缓冲）
- 两个间距均随 size 自动缩放

### 7. 存档系统

- 三个槽位 `slot_0.json` / `slot_1.json` / `slot_2.json`
- 存储在**可执行文件所在目录**的 `saves/` 子目录（跨平台通用）
- 不使用 `%APPDATA%` 或 `~/.local/share/`，确保三个平台一致行为
- 通过 `SaveManager::savesDir()` 计算路径 = `QCoreApplication::applicationDirPath() + "/saves"`
- `SaveManager` 是纯静态工具类（Model 层），提供 `writeSlot` / `readSlot` / `slotInfo` / `deleteSlot`
- 存档操作流程：View 调用 `m_saveSlotCmd(slot)` / `m_loadSlotCmd(slot)` / `m_deleteSlotCmd(slot)` → ViewModel 处理 → SaveManager 文件 IO → `fire(PROP_ID_SAVE_RESULT)` + `syncState()` 更新 `GameState::slotInfos` → 通知回调刷新 UI / 导航 / 弹窗
- 存档字段请参考上面"存档系统"小节

### 8. UI 布局

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
│            [ LOAD GAME ]                            │
│          [ HOW TO PLAY ]                            │
│                                                      │
└──────────────────────────────────────────────────────┘
```
- **NEW GAME** → 进入 Config 页（不直接进入游戏）
- **LOAD GAME** → 进入存档管理页
- **HOW TO PLAY** → 进入游戏引导页，展示游戏规则说明
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
│   │ 1 │(-12.3,5.1)         │Ob│        │ 2 │        │
│   │ ■ │ ╲ ╱ ╲ ╱ ────────→ ╱ ╲ ╱       │ ■ │        │
│   └───┘                                            └───┘
│                                                      │
├──────────────────────────────────────────────────────┤
│  [⏸ Pause]  f(x) = [________]  Cost: 0  [Fire!]   │  ← 输入区，回车触发发射
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
- 通过 `GameState::slotInfos` / `slotCount` 读取槽位信息（由 ViewModel 从 SaveManager 同步）
- Load / Delete 操作调用存储在 Widget 中的 `std::function` 命令
- 存/删操作后通过 `PROP_ID_SAVE_RESULT` 通知自动刷新槽位显示（`refreshSlots()`）

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

**第 4 页 — 配置页：**
```
┌──────────────────────────────────────────────────────┐
│                   Game Settings                      │
├──────────────────────────────────────────────────────┤
│  Squares per player (1-10):   [5] ±                 │
│  Obstacles (0-30):           [10] ±                 │
│  Obstacle size (0.5-5.0):   [1.8] ±                 │
├──────────────────────────────────────────────────────┤
│  ☑ Show square coordinates                          │
│  ☐ Show grid lines                                  │
├──────────────────────────────────────────────────────┤
│  BGM Volume:    [██████████]  [Mute]                │
│  SFX Volume:    [██████████]  [Mute]                │
├──────────────────────────────────────────────────────┤
│  P1 Color:  ■ ■ ■ ■ ■ ■ ■ ■  (8 preset swatches)   │
│  P2 Color:  ■ ■ ■ ■ ■ ■ ■ ■  (8 preset swatches)   │
├──────────────────────────────────────────────────────┤
│           [ ← Back ]     [ START GAME ]            │
└──────────────────────────────────────────────────────┘
```
- 颜色使用 8 种预设色块（蓝/红/绿/橙/紫/青/黄/粉），点击选中
- 两位玩家不能选择相同颜色，否则 START GAME 时提示
- SpinBox 按钮使用 ± 符号（`setButtonSymbols(PlusMinus)`）
- 音频音量滑块支持拖拽调节，范围 0-100，支持独立静音切换

**交互流程（MVVM + PropertyTrigger + Command 数据流）：**

初始绑定（GraphwarApp 构造函数）：
```
GraphwarApp::GraphwarApp()
  // 设置 ViewModel 指针
  m_main_wnd.set_view_model(&m_view_model);
  m_main_wnd.get_canvas()->set_view_model(&m_view_model);
  m_main_wnd.get_input()->set_view_model(&m_view_model);

  // 注入命令
  m_main_wnd.set_new_game_command(m_view_model.get_new_game_command());
  m_main_wnd.set_pause_command(m_view_model.get_pause_command());
  m_main_wnd.set_bgm_volume_command(m_view_model.get_set_bgm_volume_command());
  m_main_wnd.set_bgm_muted_command(m_view_model.get_set_bgm_muted_command());
  m_main_wnd.set_sfx_volume_command(m_view_model.get_set_sfx_volume_command());
  m_main_wnd.set_sfx_muted_command(m_view_model.get_set_sfx_muted_command());
  // ... 其他命令

  // 注册通知链
  m_view_model.add_notification(m_main_wnd.get_notification());
```

用户操作流程：
1. 开始界面点击 NEW GAME → 切换到配置页，通过 `m_vm->get_model()->config()` 回显
2. 配置页 START GAME → emit `configSaved(cfg)` → MainWindow 调用 `m_setConfigCmd(cfg)` + `startNewGame()` → 调用 `m_newGameCmd()` → GameModel::newGame() → 发射 Qt 信号 → ViewModel → `fire(PROP_ID_xxx)` → 通知回调更新 UI → 切到游戏页，播放 BGM
3. 配置页 Back / ESC → 返回开始页，配置不保存
4. 开始界面 Load / Manage Saves → 切到存档管理页
5. 暂停按钮 → `m_pauseCmd()` → GameModel::pause() → `fire(PROP_ID_PAUSED)` → MainWindow 通知回调 → `showPage(PagePause)`
6. ESC → 游戏页 → `goToPause()`；暂停页 → `resumeFromPause()`；其他页 → `backToStart()`
7. 暂停页 CONTINUE → `m_resumeCmd()` → GameModel::resume() → `fire(PROP_ID_PAUSED)` → 切回游戏页
8. 暂停页保存 → `m_saveSlotCmd(slot)` → SaveManager::writeSlot() → `fire(PROP_ID_SAVE_RESULT)` → 刷新槽位显示 + 弹窗
9. 存档管理页 LOAD → `m_loadSlotCmd(slot)` → SaveManager::readSlot() → GameModel::fromJson() → `fire(PROP_ID_*)` 全部 → 切到游戏页
10. 存档管理页 DELETE → 确认 → `m_deleteSlotCmd(slot)` → SaveManager::deleteSlot() → `fire(PROP_ID_SAVE_RESULT)` → 刷新槽位
11. 游戏过程：
    - 输入表达式 → 调用 `m_costPreviewCmd(text)` → ViewModel 计算消耗 → `fire(PROP_ID_COST_PREVIEW)` → 更新 Cost 标签
    - 点击 FIRE / 回车 → `m_launchCmd(text)` → GameModel::launch() → 动画开始，播放 Launch 音效
    - GameModel 动画 QTimer @ 16ms → `stepAnimation()` → 碰撞检测 → 出界/障碍物/胜利 → 触发粒子特效和对应音效 → `fire(PROP_ID_TRAJECTORY)` → 画布更新
12. 音频控制：
    - 调节 BGM 音量 → `m_setBgmVolumeCmd(v)` → AudioManager::setBgmVolume(v)
    - 切换 BGM 静音 → `m_setBgmMutedCmd(m)` → AudioManager::setBgmMuted(m)
    - 调节 SFX 音量 → `m_setSfxVolumeCmd(v)` → AudioManager::setSfxVolume(v)
    - 切换 SFX 静音 → `m_setSfxMutedCmd(m)` → AudioManager::setSfxMuted(m)

### 9. 应用图标

- `resources/app.ico` 同时服务两个用途：
  1. **Qt 运行时图标**：通过 `resources.qrc` 编译进二进制，`main.cpp` 调用 `app.setWindowIcon(QIcon(":/app.ico"))`，显示在窗口标题栏和任务栏
  2. **Windows exe 图标**：通过 `app.rc` 资源脚本嵌入，使 `.exe` 文件在资源管理器中显示图标
- macOS 需额外准备 `.icns` 文件并设置 `MACOSX_BUNDLE_ICON_FILE`
- Linux 使用 Qt 运行时图标即可，桌面图标依赖 `.desktop` 文件

---

## 方格随机生成算法

`GameModel::generateSquares()`：

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

障碍物调用 `GameModel::generateObstacles(count, size)`：
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
- Ninja 或 MinGW（make）
- vcpkg（环境变量 `VCPKG_ROOT` 指向 vcpkg 根目录）
- Qt6（通过 vcpkg 安装的 `qtbase:x64-windows` / `qtbase:x64-linux` / `qtbase:x64-osx`）

### Windows（使用 vcpkg + Ninja）

```bat
cd d:\Graphwar
cmake --preset release-windows
cd build
ninja
ninja graphwar_package
```

### Windows（使用 vcpkg + MinGW）

```bat
cd d:\Graphwar
cmake -B build -S . -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows
mingw32-make -C build
mingw32-make -C build graphwar_package
```

### Linux（使用 vcpkg）

```bash
cd ~/Graphwar
cmake --preset release-linux
cd build
ninja
ninja graphwar_package
```

### macOS（使用 vcpkg）

```bash
cd ~/Graphwar
cmake --preset release-macos
cd build
ninja
ninja graphwar_package
```

### 跨平台打包说明

- **打包目标**：`ninja graphwar_package`（Windows）或 `make graphwar_package`（其他平台）
- **生成结果**：`build/Graphwar.zip`
- **压缩包结构**：
  ```
  Graphwar.zip
  └── Graphwar/
      ├── Graphwar.exe (Windows) / Graphwar (Linux) / Graphwar.app (macOS)
      ├── *.dll (Windows) / *.so* (Linux)
      ├── resources/AIZO-8bit.m4a
      ├── platforms/qwindows.dll (Windows) / libqcocoa.dylib (macOS) / libqxcb.so (Linux)
      ├── audio/ (可选)
      └── mediaservice/ (可选)
  ```
- **Windows**：CMake 构建完成后自动通过 `POST_BUILD` 命令复制 `qwindows.dll` 到 `build/platforms/` 目录，复制所有 DLL 到 `build/` 目录
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
A: PLAN.md 中的规则描述需与 `GameModel.cpp` / `GameModel.h` / `GameConfig.h` 的实际实现一致；架构描述需与 `frame.h` / `GameViewModel.cpp` / `GraphwarApp.cpp` 一致；任何逻辑/配置变更后请同步更新 PLAN.md。

**Q: 音频播放没有声音？**
A: 检查系统音量设置；检查游戏配置页的 BGM/SFX 音量是否被静音；确保 Qt Multimedia 后端正确部署（Windows 需要 `mediaservice/` 目录包含 `windowsmediafoundation.dll`，Linux 需要 `gstreamer` 后端，macOS 使用 `AVFoundation`）。