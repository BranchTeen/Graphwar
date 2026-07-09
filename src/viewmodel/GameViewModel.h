#pragma once
#include <QObject>
#include <QTimer>
#include <QString>
#include "model/GameModel.h"
#include "utils/Geometry.h"

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

    int currentPlayer() const { return m_model.currentPlayer; }
    int roundNumber() const { return m_model.roundNumber; }
    int availablePoints() const { return m_model.availablePoints(); }
    int costPreview() const { return m_costPreview; }
    GamePhase phase() const { return m_model.phase; }
    QString message() const { return m_message; }
    bool paused() const { return m_paused; }

    const GameModel &model() const { return m_model; }

public slots:
    void newGame();
    void updateCostPreview(const QString &expr);
    void launch(const QString &expr);
    void nextTurn();
    void advanceAnimation();

    // 暂停/恢复
    void pause();
    void resume();
    void togglePause();

    // 存档/读档/删除；返回是否成功
    bool saveToSlot(int slot);
    bool loadFromSlot(int slot);
    bool deleteSlot(int slot);

signals:
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
    void saveResult(int slot, bool ok, const QString &info); // 存档操作完成
    void gameOver(const QString &winnerInfo);                // 游戏结束：UI 弹出"重新开始"按钮

private:
    void generateSquares();
    void generateObstacles();
    void pickRandomSquare();
    bool checkHit(const QPointF &pt) const;

    // JSON 序列化/反序列化整个模型 + 运行状态
    QString toJson() const;
    bool fromJson(const QString &text);
    // 加载后发出 UI 刷新信号
    void emitAllState();

    GameModel m_model;
    QTimer m_animTimer;
    QString m_currentExpr;
    double m_animX = 0;
    double m_animStep = 0.15;
    int m_costPreview = 0;
    QString m_message;
    double m_constAdjust = 0;
    bool m_hasHit = false;
    bool m_paused = false;
};
