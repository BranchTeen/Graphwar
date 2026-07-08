#pragma once
#include "Player.h"
#include "GamePhase.h"
#include <QVector>
#include <QPointF>

struct GameModel {
    Player players[2];
    int currentPlayer = 0;
    int selectedSquare = -1;
    int roundNumber = 0;
    GamePhase phase = GamePhase::WaitingInput;

    QVector<QPointF> trajectory;       // 当前动画轨迹点
    QVector<QVector<QPointF>> history; // 历史轨迹
    double trajectoryMaxX = 0;

    int availablePoints() const {
        return 3 + roundNumber;        // round 1 → 4, round 2 → 5, round 3 → 6, ...
    }

    Player &currentPlayerRef() { return players[currentPlayer]; }
    const Player &currentPlayerRef() const { return players[currentPlayer]; }
    Player &opponentRef() { return players[1 - currentPlayer]; }
    const Player &opponentRef() const { return players[1 - currentPlayer]; }
};
