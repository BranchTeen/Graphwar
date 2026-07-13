#include "GameViewModel.h"
#include <QObject>
#include "model/SaveManager.h"
#include "common/property_ids.h"

GameViewModel::GameViewModel() {
    m_model = new GameModel();
    forwardModelSignals();
}

GameViewModel::~GameViewModel() noexcept {
    delete m_model;
}

void GameViewModel::forwardModelSignals() {
    QObject::connect(m_model, &GameModel::turnChanged, [this](int) {
        fire(PROP_ID_TURN);
    });
    QObject::connect(m_model, &GameModel::roundChanged, [this](int) {
        fire(PROP_ID_ROUND);
    });
    QObject::connect(m_model, &GameModel::pointsChanged, [this](int) {
        fire(PROP_ID_POINTS);
    });
    QObject::connect(m_model, &GameModel::phaseChanged, [this](GamePhase) {
        fire(PROP_ID_PHASE);
    });
    QObject::connect(m_model, &GameModel::messageChanged, [this](const QString&) {
        fire(PROP_ID_MESSAGE);
    });
    QObject::connect(m_model, &GameModel::squareHit, [this](int, int) {
        fire(PROP_ID_SQUARE_HIT);
    });
    QObject::connect(m_model, &GameModel::trajectoryUpdated, [this]() {
        fire(PROP_ID_TRAJECTORY);
    });
    QObject::connect(m_model, &GameModel::gameOver, [this](const QString&) {
        fire(PROP_ID_GAME_OVER);
    });
    QObject::connect(m_model, &GameModel::pausedChanged, [this](bool) {
        fire(PROP_ID_PAUSED);
    });
}
