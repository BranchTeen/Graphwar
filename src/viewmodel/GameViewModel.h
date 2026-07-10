#pragma once
#include <QObject>
#include <QTimer>
#include <QString>
#include "model/GameModel.h"
#include "model/SaveInfo.h"

// GameViewModel：MVVM 的 ViewModel 层（薄协调者）
// - 持有一个 GameModel 实例（真正的业务逻辑所在）
// - 向 View 暴露细粒度的只读访问接口（通过转发到 m_model）
// - 提供给 View 调用的 slots（内部转发到 m_model）
// - 桥接 m_model 的 signals → ViewModel 的 signals → View 订阅
// - 管理存档槽读写（调用 model/SaveManager + model 的 toJson/fromJson）
// View 层不应直接 #include "model/GameModel.h"，而通过 GameViewModel 间接访问它
class GameViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentPlayer READ currentPlayer NOTIFY turnChanged)
    Q_PROPERTY(int roundNumber READ roundNumber NOTIFY roundChanged)
    Q_PROPERTY(int availablePoints READ availablePoints NOTIFY pointsChanged)
    Q_PROPERTY(int costPreview READ costPreview NOTIFY costPreviewChanged)
    Q_PROPERTY(GamePhase phase READ phase NOTIFY phaseChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)

public:
    explicit GameViewModel(QObject *parent = nullptr);

    // ===== 基础状态访问器（供 View 只读访问，转发到 Model）=====
    int currentPlayer() const;
    int roundNumber() const;
    int availablePoints() const;
    int costPreview() const;
    GamePhase phase() const;
    QString message() const;
    bool paused() const;

    // ===== 玩家 / 方块信息（细粒度，从 Model 查询）=====
    QColor playerColor(int player) const;
    int aliveCount(int player) const;
    int totalSquaresPerPlayer() const;
    int selectedSquareIndex() const;

    // 返回快照副本 —— 保证 View 无法修改 Model 内部状态
    QVector<Square> playerSquares(int player) const;
    QVector<Square> obstacles() const;

    // ===== 轨迹信息（从 Model 查询）=====
    QVector<QPointF> trajectory() const;
    QVector<QPointF> historyTrajectory() const;

    // ===== 游戏状态语义方法 =====
    bool isGameOver() const;
    bool isWaitingInput() const;
    bool isAnimating() const;

    // ===== 配置 =====
    GameConfig configSnapshot() const;
    const GameConfig &config() const;
    bool showGridLines() const;
    bool showCoordinates() const;

    // ===== 存档管理（统一入口）=====
    int slotCount() const;
    QVector<SaveInfo> slotInfos() const;
    QString slotPath(int slot) const;
    bool saveToSlot(int slot);
    bool loadFromSlot(int slot);
    bool deleteSlot(int slot);

public slots:
    void newGame();
    void updateCostPreview(const QString &expr);
    void launch(const QString &expr);
    void nextTurn();

    // 暂停/恢复（转发到 Model）
    void pause();
    void resume();
    void togglePause();

    // 应用新的配置（新一局生效）
    void setConfig(const GameConfig &cfg);

signals:
    // === 与 GameModel 的 signals 同名，用于桥接给 View ===
    void turnChanged(int player);
    void roundChanged(int round);
    void pointsChanged(int points);
    void costPreviewChanged(int cost);
    void phaseChanged(GamePhase phase);
    void messageChanged(const QString &msg);
    void squareHit(int playerId, int squareIndex);
    void trajectoryUpdated();
    void animationFinished();
    void pausedChanged(bool paused);
    void saveResult(int slot, bool ok, const QString &info);
    void gameOver(const QString &winnerInfo);

private:
    GameModel *m_model = nullptr;   // 业务逻辑 + 状态所在
    int m_costPreview = 0;           // UI 输入预览消耗点数（属于 ViewModel 状态）
};
