#pragma once
#include "GameConfig.h"
#include "GamePhase.h"
#include "Square.h"
#include "SaveInfo.h"
#include "Particle.h"
#include "GameStatistics.h"
#include <QVector>
#include <QPointF>
#include <QString>
#include <QColor>

struct GameState {
    int currentPlayer = 0;
    int roundNumber = 0;
    int availablePoints = 0;
    GamePhase phase = GamePhase::WaitingInput;
    int selectedSquareIndex = -1;
    QColor playerColors[2];
    QVector<Square> playerSquares[2];
    QVector<Square> obstacles;
    QVector<QVector<QPointF>> history;
    QVector<QPointF> trajectory;
    GameConfig config;
    QString message;
    bool gameOver = false;
    bool paused = false;
    int aliveCount[2] = {0, 0};
    QVector<SaveInfo> slotInfos;
    int slotCount = 0;

    int bgmVolume = 60;
    bool bgmMuted = false;
    int sfxVolume = 80;
    bool sfxMuted = false;

    QVector<Particle> particles;

    GameStatistics statistics;

    // ===== 回合过渡动画 =====
    bool inTransition = false;
    double transitionProgress = 0.0;
};
