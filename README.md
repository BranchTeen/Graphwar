# Graphwar

双人对战，一条曲线。用数学函数向对手开火！

一个 C++17 / Qt6 双人热座游戏，玩家输入数学表达式，发射函数图像攻击对方方格。

## 游戏规则

- 每方拥有 **N 个方格**，随机分布在地图两侧
- 每回合系统随机选一个方格作为发射点
- 输入函数 `y = f(x)`，系统自动调整常数使曲线经过发射点
- 曲线碰到对方方格则摧毁之，碰到障碍物则立即停止并破坏障碍物
- 每发射击消耗**点数**（运算符、函数各有定价），每轮可用点数有限
- 先摧毁对方全部方格的一方获胜

## 技术栈

| | |
|---|---|
| 语言 | C++17 |
| GUI | Qt6 Widgets |
| 音频 | Qt Multimedia（QMediaPlayer + QAudioOutput / QAudioSink） |
| 构建 | CMake + Ninja / MinGW |
| 数学解析 | 调车场算法 + 递归下降解析器 |
| 架构 | MVVM + PropertyTrigger + Command |
| 存档 | JSON 文件 |

## 构建

### 前置条件

- C++17 编译器
- CMake ≥ 3.20
- Qt6（Widgets + Multimedia）

### 使用 vcpkg（推荐）

```bash
cmake --preset release-windows   # 或 release-linux / release-macos
cmake --build build
```

### 直接使用 Qt（无 vcpkg）

```bash
cmake -B build -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="D:/Qt/6.11.1/mingw_64"
mingw32-make -C build
```

## 操作

| 按键 | 功能 |
|------|------|
| Enter | 发射！ |
| ESC | 暂停 / 继续 / 返回 |

## 功能

- 双人热座对战
- 可自定义方格数、障碍物数/大小、玩家颜色
- 数学表达式解析器（sin、cos、log、sqrt 等）
- 表达式点数预算系统
- 击中/摧毁粒子特效
- 回合过渡动画
- 存档/读档（3 个槽位）
- 回放系统 — 当局结束后可回看完整过程
- 背景音乐 + 音效

## 项目结构

```
src/
├── app/          # 组合根（GraphwarApp）
├── common/       # 共享数据结构（GameState、GameConfig、Square 等）
├── model/        # 业务逻辑（GameModel、AudioManager、解析器）
├── viewmodel/    # 中介层（GameViewModel、命令）
└── view/         # UI 层（MainWindow、GameCanvas、控件）
```
