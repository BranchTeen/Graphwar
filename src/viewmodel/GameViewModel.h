#pragma once
#include <QObject>
#include <QTimer>
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

public:
    explicit GameViewModel(QObject *parent = nullptr);

    int currentPlayer() const { return m_model.currentPlayer; }
    int roundNumber() const { return m_model.roundNumber; }
    int availablePoints() const { return m_model.availablePoints(); }
    int costPreview() const { return m_costPreview; }
    GamePhase phase() const { return m_model.phase; }
    QString message() const { return m_message; }

    const GameModel &model() const { return m_model; }

public slots:
    void newGame();
    void selectSquare(int index);
    void updateCostPreview(const QString &expr);
    void launch(const QString &expr);
    void nextTurn();
    void advanceAnimation();

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

private:
    void generateSquares();
    void pickRandomSquare();
    bool checkHit(const QPointF &pt) const;

    GameModel m_model;
    QTimer m_animTimer;
    QString m_currentExpr;
    double m_animX = 0;
    double m_animStep = 0.15;
    int m_costPreview = 0;
    QString m_message;
    double m_constAdjust = 0;
    bool m_hasHit = false;
};
