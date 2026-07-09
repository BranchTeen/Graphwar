#pragma once
#include "Player.h"
#include "GamePhase.h"
#include <QVector>
#include <QPointF>

struct GameModel {
    Player players[2];
    QVector<Square> obstacles;
    int currentPlayer = 0;
    int selectedSquare = -1;
    int roundNumber = 0;
    int pointsLevel = 0;            // 与 roundNumber 解耦，控制可用点数增长
    GamePhase phase = GamePhase::WaitingInput;

    QVector<QPointF> trajectory;       // 当前动画轨迹点
    QVector<QVector<QPointF>> history; // 历史轨迹
    double trajectoryMaxX = 0;

    int availablePoints() const {
        // 点数随 pointsLevel 递增（注意：与 roundNumber 不同步）
        // pointsLevel 在 P0 操作结束后增长，所以一轮内点数会从 3 + 2 × N 涨到 3 + 2 × (N+1)
        // 第 1 轮（P0 先手）：5 点；P1 操作时：7 点；第 2 轮 P0 操作：7 点；P1 操作：9 点；……
        return 3 + pointsLevel * 2;
    }

    Player &currentPlayerRef() { return players[currentPlayer]; }
    const Player &currentPlayerRef() const { return players[currentPlayer]; }
    Player &opponentRef() { return players[1 - currentPlayer]; }
    const Player &opponentRef() const { return players[1 - currentPlayer]; }
};
