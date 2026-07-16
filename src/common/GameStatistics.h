#pragma once
#include <QString>
#include <QVector>

struct PlayerStatistics {
    int launchCount = 0;
    int hitCount = 0;
    int obstacleHitCount = 0;
    int totalPointsSpent = 0;
    int roundsPlayed = 0;
    QVector<QString> usedFunctions;

    double hitRate() const {
        if (launchCount == 0) return 0.0;
        return static_cast<double>(hitCount) / launchCount * 100;
    }

    int avgPointsPerLaunch() const {
        if (launchCount == 0) return 0;
        return totalPointsSpent / launchCount;
    }
};

struct GameStatistics {
    PlayerStatistics player[2];
    int totalRounds = 0;
    int winner = -1;

    void reset() {
        player[0] = PlayerStatistics();
        player[1] = PlayerStatistics();
        totalRounds = 0;
        winner = -1;
    }
};