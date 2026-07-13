#include "GameViewModel.h"
#include <QObject>
#include "common/property_ids.h"

GameViewModel::GameViewModel() {
    m_model = new GameModel();
    syncState();
    forwardModelSignals();
}

GameViewModel::~GameViewModel() noexcept {
    delete m_model;
}

void GameViewModel::syncState() {
    m_state.currentPlayer = m_model->currentPlayer();
    m_state.roundNumber = m_model->roundNumber();
    m_state.availablePoints = m_model->availablePoints();
    m_state.phase = m_model->phase();
    m_state.selectedSquareIndex = m_model->selectedSquareIndex();
    m_state.playerColors[0] = m_model->playerColor(0);
    m_state.playerColors[1] = m_model->playerColor(1);
    m_state.playerSquares[0] = m_model->playerSquares(0);
    m_state.playerSquares[1] = m_model->playerSquares(1);
    m_state.obstacles = m_model->obstacles();
    m_state.trajectory = m_model->trajectory();
    {
        auto h = m_model->history();
        m_state.history = h;
    }
    m_state.config = m_model->config();
    m_state.message = m_model->message();
    m_state.gameOver = m_model->isGameOver();
    m_state.paused = m_model->paused();
    m_state.aliveCount[0] = m_model->aliveCount(0);
    m_state.aliveCount[1] = m_model->aliveCount(1);
    m_state.slotInfos = m_model->slotInfos();
    m_state.slotCount = m_model->slotCount();
}

void GameViewModel::forwardModelSignals() {
    QObject::connect(m_model, &GameModel::turnChanged, [this](int) {
        m_state.currentPlayer = m_model->currentPlayer();
        fire(PROP_ID_TURN);
    });
    QObject::connect(m_model, &GameModel::roundChanged, [this](int) {
        m_state.roundNumber = m_model->roundNumber();
        fire(PROP_ID_ROUND);
    });
    QObject::connect(m_model, &GameModel::pointsChanged, [this](int) {
        m_state.availablePoints = m_model->availablePoints();
        fire(PROP_ID_POINTS);
    });
    QObject::connect(m_model, &GameModel::phaseChanged, [this](GamePhase) {
        syncState();
        fire(PROP_ID_PHASE);
    });
    QObject::connect(m_model, &GameModel::messageChanged, [this](const QString&) {
        m_state.message = m_model->message();
        fire(PROP_ID_MESSAGE);
    });
    QObject::connect(m_model, &GameModel::squareHit, [this](int, int) {
        syncState();
        fire(PROP_ID_SQUARE_HIT);
    });
    QObject::connect(m_model, &GameModel::trajectoryUpdated, [this]() {
        m_state.trajectory = m_model->trajectory();
        fire(PROP_ID_TRAJECTORY);
    });
    QObject::connect(m_model, &GameModel::gameOver, [this](const QString&) {
        m_state.gameOver = true;
        m_state.message = m_model->message();
        fire(PROP_ID_GAME_OVER);
    });
    QObject::connect(m_model, &GameModel::pausedChanged, [this](bool) {
        m_state.paused = m_model->paused();
        fire(PROP_ID_PAUSED);
    });
}
