#pragma once
#include "GameConfig.h"
#include "Square.h"
#include <QString>
#include <QVector>
#include <QPointF>

struct ReplayShot {
    int playerIndex = 0;
    double launchX = 0, launchY = 0;
    QString expression;
};

struct ReplayData {
    GameConfig config;
    QVector<Square> playerSquares[2];
    QVector<Square> obstacles;
    QVector<ReplayShot> shots;

    bool isValid() const { return !playerSquares[0].isEmpty() || !playerSquares[1].isEmpty(); }
};
