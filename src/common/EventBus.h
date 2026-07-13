#pragma once
#include <QObject>
#include <QString>
#include <QColor>
#include <QVector>
#include <QPointF>
#include "../model/GamePhase.h"
#include "../model/Square.h"
#include "../model/GameConfig.h"
#include "../model/SaveInfo.h"

class EventBus : public QObject {
    Q_OBJECT
public:
    static EventBus &instance() {
        static EventBus bus;
        return bus;
    }

    static void reset() {
        // 清理所有连接，重新初始化
        auto &bus = instance();
        bus.disconnect();
    }

private:
    EventBus() = default;
    EventBus(const EventBus &) = delete;
    EventBus &operator=(const EventBus &) = delete;

signals:
    // ==================== View → ViewModel 命令（View 发射，ViewModel 订阅）====================
    void cmdNewGame();
    void cmdLaunch(const QString &expr);
    void cmdUpdateCostPreview(const QString &expr);
    void cmdNextTurn();
    void cmdPause();
    void cmdResume();
    void cmdTogglePause();
    void cmdSetConfig(const GameConfig &cfg);
    void cmdSaveToSlot(int slot);
    void cmdLoadFromSlot(int slot);
    void cmdDeleteSlot(int slot);

    // ==================== ViewModel → View 状态变更事件（ViewModel 发射，View 订阅）====================
    void evtTurnChanged(int player);
    void evtRoundChanged(int round);
    void evtPointsChanged(int points);
    void evtCostPreviewChanged(int cost);
    void evtPhaseChanged(GamePhase phase);
    void evtMessageChanged(const QString &msg);
    void evtPausedChanged(bool paused);
    void evtGameOver(const QString &winnerInfo);
    void evtTrajectoryUpdated();
    void evtSquareHit(int playerId, int squareIndex);
    void evtSaveResult(int slot, bool ok, const QString &info);
    void evtAnimationFinished();

public:
    // ==================== 状态查询接口（ViewModel 注册回调，View 调用）====================
    // 使用 std::function 而不是直接调用 ViewModel，避免依赖
    using IntGetter = std::function<int()>;
    using IntArgGetter = std::function<int(int)>;
    using BoolGetter = std::function<bool()>;
    using StringGetter = std::function<QString()>;
    using StringArgGetter = std::function<QString(int)>;
    using ColorGetter = std::function<QColor(int)>;
    using VectorGetter = std::function<QVector<Square>(int)>;
    using ObstaclesGetter = std::function<QVector<Square>()>;
    using TrajectoryGetter = std::function<QVector<QPointF>()>;
    using SaveInfoGetter = std::function<QVector<SaveInfo>()>;
    using ConfigGetter = std::function<const GameConfig&()>;

    void registerStateGetters(
        IntGetter currentPlayer,
        IntGetter roundNumber,
        IntGetter availablePoints,
        IntGetter costPreview,
        IntArgGetter aliveCount,
        IntGetter selectedSquareIndex,
        BoolGetter paused,
        BoolGetter showGridLines,
        BoolGetter showCoordinates,
        BoolGetter isGameOver,
        BoolGetter isWaitingInput,
        BoolGetter isAnimating,
        StringGetter message,
        ColorGetter playerColor,
        VectorGetter playerSquares,
        ObstaclesGetter obstacles,
        TrajectoryGetter trajectory,
        TrajectoryGetter historyTrajectory,
        IntGetter slotCount,
        SaveInfoGetter slotInfos,
        StringArgGetter slotPath,
        ConfigGetter config
    ) {
        m_currentPlayer = std::move(currentPlayer);
        m_roundNumber = std::move(roundNumber);
        m_availablePoints = std::move(availablePoints);
        m_costPreview = std::move(costPreview);
        m_aliveCount = std::move(aliveCount);
        m_selectedSquareIndex = std::move(selectedSquareIndex);
        m_paused = std::move(paused);
        m_showGridLines = std::move(showGridLines);
        m_showCoordinates = std::move(showCoordinates);
        m_isGameOver = std::move(isGameOver);
        m_isWaitingInput = std::move(isWaitingInput);
        m_isAnimating = std::move(isAnimating);
        m_message = std::move(message);
        m_playerColor = std::move(playerColor);
        m_playerSquares = std::move(playerSquares);
        m_obstacles = std::move(obstacles);
        m_trajectory = std::move(trajectory);
        m_historyTrajectory = std::move(historyTrajectory);
        m_slotCount = std::move(slotCount);
        m_slotInfos = std::move(slotInfos);
        m_slotPath = std::move(slotPath);
        m_config = std::move(config);
    }

    // ==================== View 调用这些方法获取状态 ====================
    int currentPlayer() const { return m_currentPlayer ? m_currentPlayer() : 0; }
    int roundNumber() const { return m_roundNumber ? m_roundNumber() : 0; }
    int availablePoints() const { return m_availablePoints ? m_availablePoints() : 0; }
    int costPreview() const { return m_costPreview ? m_costPreview() : 0; }
    int aliveCount(int player) const { return m_aliveCount ? m_aliveCount(player) : 0; }
    int selectedSquareIndex() const { return m_selectedSquareIndex ? m_selectedSquareIndex() : -1; }
    bool paused() const { return m_paused ? m_paused() : false; }
    bool showGridLines() const { return m_showGridLines ? m_showGridLines() : false; }
    bool showCoordinates() const { return m_showCoordinates ? m_showCoordinates() : false; }
    bool isGameOver() const { return m_isGameOver ? m_isGameOver() : false; }
    bool isWaitingInput() const { return m_isWaitingInput ? m_isWaitingInput() : false; }
    bool isAnimating() const { return m_isAnimating ? m_isAnimating() : false; }
    QString message() const { return m_message ? m_message() : QString(); }
    QColor playerColor(int player) const { return m_playerColor ? m_playerColor(player) : QColor(); }
    QVector<Square> playerSquares(int player) const { return m_playerSquares ? m_playerSquares(player) : QVector<Square>(); }
    QVector<Square> obstacles() const { return m_obstacles ? m_obstacles() : QVector<Square>(); }
    QVector<QPointF> trajectory() const { return m_trajectory ? m_trajectory() : QVector<QPointF>(); }
    QVector<QPointF> historyTrajectory() const { return m_historyTrajectory ? m_historyTrajectory() : QVector<QPointF>(); }
    int slotCount() const { return m_slotCount ? m_slotCount() : 0; }
    QVector<SaveInfo> slotInfos() const { return m_slotInfos ? m_slotInfos() : QVector<SaveInfo>(); }
    QString slotPath(int slot) const { return m_slotPath ? m_slotPath(slot) : QString(); }
    const GameConfig &config() const {
        static GameConfig empty;
        return m_config ? m_config() : empty;
    }

private:
    IntGetter m_currentPlayer;
    IntGetter m_roundNumber;
    IntGetter m_availablePoints;
    IntGetter m_costPreview;
    IntArgGetter m_aliveCount;
    IntGetter m_selectedSquareIndex;
    BoolGetter m_paused;
    BoolGetter m_showGridLines;
    BoolGetter m_showCoordinates;
    BoolGetter m_isGameOver;
    BoolGetter m_isWaitingInput;
    BoolGetter m_isAnimating;
    StringGetter m_message;
    ColorGetter m_playerColor;
    VectorGetter m_playerSquares;
    ObstaclesGetter m_obstacles;
    TrajectoryGetter m_trajectory;
    TrajectoryGetter m_historyTrajectory;
    IntGetter m_slotCount;
    SaveInfoGetter m_slotInfos;
    StringArgGetter m_slotPath;
    ConfigGetter m_config;
};
