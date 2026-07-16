#pragma once
#include <QColor>

struct GameConfig {
    int squaresPerPlayer = 5;
    int obstacleCount = 10;
    double obstacleSize = 1.8;
    QColor player1Color{60, 120, 220, 200};
    QColor player2Color{220, 60, 60, 200};
    bool showCoordinates = true;
    bool showGridLines = false;
};
