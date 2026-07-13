#include "GameViewModel.h"
#include "model/SaveManager.h"

GameViewModel::GameViewModel(QObject *parent) : QObject(parent) {
    m_model = new GameModel(this);
    forwardModelSignals();
    registerGetters();
    connectCommands();
}

GameViewModel::~GameViewModel() {
    // EventBus 是单例，不需要手动断开
}

void GameViewModel::forwardModelSignals() {
    connect(m_model, &GameModel::turnChanged, [](int p) {
        EventBus::instance().evtTurnChanged(p);
    });
    connect(m_model, &GameModel::roundChanged, [](int r) {
        EventBus::instance().evtRoundChanged(r);
    });
    connect(m_model, &GameModel::pointsChanged, [](int p) {
        EventBus::instance().evtPointsChanged(p);
    });
    connect(m_model, &GameModel::phaseChanged, [](GamePhase ph) {
        EventBus::instance().evtPhaseChanged(ph);
    });
    connect(m_model, &GameModel::messageChanged, [](const QString &msg) {
        EventBus::instance().evtMessageChanged(msg);
    });
    connect(m_model, &GameModel::squareHit, [](int p, int idx) {
        EventBus::instance().evtSquareHit(p, idx);
    });
    connect(m_model, &GameModel::trajectoryUpdated, []() {
        EventBus::instance().evtTrajectoryUpdated();
    });
    connect(m_model, &GameModel::gameOver, [](const QString &info) {
        EventBus::instance().evtGameOver(info);
    });
    connect(m_model, &GameModel::pausedChanged, [](bool p) {
        EventBus::instance().evtPausedChanged(p);
    });
}

void GameViewModel::registerGetters() {
    auto &bus = EventBus::instance();
    bus.registerStateGetters(
        [this]() { return m_model->currentPlayer(); },
        [this]() { return m_model->roundNumber(); },
        [this]() { return m_model->availablePoints(); },
        [this]() { return m_costPreview; },
        [this](int p) { return m_model->aliveCount(p); },
        [this]() { return m_model->selectedSquareIndex(); },
        [this]() { return m_model->paused(); },
        [this]() { return m_model->config().showGridLines; },
        [this]() { return m_model->config().showCoordinates; },
        [this]() { return m_model->isGameOver(); },
        [this]() { return m_model->isWaitingInput(); },
        [this]() { return m_model->isAnimating(); },
        [this]() { return m_model->message(); },
        [this](int p) { return m_model->playerColor(p); },
        [this](int p) { return m_model->playerSquares(p); },
        [this]() { return m_model->obstacles(); },
        [this]() { return m_model->trajectory(); },
        [this]() {
            auto h = m_model->history();
            return h.isEmpty() ? QVector<QPointF>() : h.last();
        },
        []() { return SaveManager::kSlotCount; },
        []() {
            QVector<SaveInfo> result;
            for (int slot = 0; slot < SaveManager::kSlotCount; ++slot)
                result.append(SaveManager::slotInfo(slot));
            return result;
        },
        [](int slot) { return SaveManager::slotPath(slot); },
        [this]() -> const GameConfig& { return m_model->config(); }
    );
}

void GameViewModel::connectCommands() {
    auto &bus = EventBus::instance();
    connect(&bus, &EventBus::cmdNewGame, this, &GameViewModel::onCmdNewGame);
    connect(&bus, &EventBus::cmdLaunch, this, &GameViewModel::onCmdLaunch);
    connect(&bus, &EventBus::cmdUpdateCostPreview, this, &GameViewModel::onCmdUpdateCostPreview);
    connect(&bus, &EventBus::cmdNextTurn, this, &GameViewModel::onCmdNextTurn);
    connect(&bus, &EventBus::cmdPause, this, &GameViewModel::onCmdPause);
    connect(&bus, &EventBus::cmdResume, this, &GameViewModel::onCmdResume);
    connect(&bus, &EventBus::cmdTogglePause, this, &GameViewModel::onCmdTogglePause);
    connect(&bus, &EventBus::cmdSetConfig, this, &GameViewModel::onCmdSetConfig);
    connect(&bus, &EventBus::cmdSaveToSlot, this, &GameViewModel::onCmdSaveToSlot);
    connect(&bus, &EventBus::cmdLoadFromSlot, this, &GameViewModel::onCmdLoadFromSlot);
    connect(&bus, &EventBus::cmdDeleteSlot, this, &GameViewModel::onCmdDeleteSlot);
}

void GameViewModel::onCmdNewGame() {
    m_costPreview = 0;
    m_model->newGame(m_model->config());
    emit EventBus::instance().evtCostPreviewChanged(0);
}

void GameViewModel::onCmdLaunch(const QString &expr) {
    bool ok = m_model->launch(expr);
    if (ok) {
        m_costPreview = 0;
        emit EventBus::instance().evtCostPreviewChanged(0);
    }
}

void GameViewModel::onCmdUpdateCostPreview(const QString &expr) {
    if (expr.isEmpty()) {
        m_costPreview = 0;
        emit EventBus::instance().evtCostPreviewChanged(0);
        return;
    }
    m_costPreview = m_model->calculateCost(expr);
    emit EventBus::instance().evtCostPreviewChanged(m_costPreview);
}

void GameViewModel::onCmdNextTurn() {
    m_model->nextTurn();
}

void GameViewModel::onCmdPause() {
    m_model->pause();
}

void GameViewModel::onCmdResume() {
    m_model->resume();
}

void GameViewModel::onCmdTogglePause() {
    m_model->togglePause();
}

void GameViewModel::onCmdSetConfig(const GameConfig &cfg) {
    m_model->setConfig(cfg);
}

void GameViewModel::onCmdSaveToSlot(int slot) {
    QString json = m_model->toJson();
    bool ok = SaveManager::writeSlot(slot, json);
    emit EventBus::instance().evtSaveResult(slot, ok, ok ? "Saved" : "Save failed");
}

void GameViewModel::onCmdLoadFromSlot(int slot) {
    bool ok = false;
    QString text = SaveManager::readSlot(slot, &ok);
    if (!ok) {
        emit EventBus::instance().evtSaveResult(slot, false, "Load failed: file not found");
        return;
    }
    if (!m_model->fromJson(text)) {
        emit EventBus::instance().evtSaveResult(slot, false, "Load failed: bad data");
        return;
    }
    m_costPreview = 0;
    auto &bus = EventBus::instance();
    emit bus.evtTurnChanged(m_model->currentPlayer());
    emit bus.evtRoundChanged(m_model->roundNumber());
    emit bus.evtPointsChanged(m_model->availablePoints());
    emit bus.evtPhaseChanged(m_model->phase());
    emit bus.evtMessageChanged(m_model->message());
    emit bus.evtTrajectoryUpdated();
    emit bus.evtCostPreviewChanged(0);
    emit bus.evtSaveResult(slot, true, "Loaded");
}

void GameViewModel::onCmdDeleteSlot(int slot) {
    bool ok = SaveManager::deleteSlot(slot);
    emit EventBus::instance().evtSaveResult(slot, ok, ok ? "Deleted" : "Delete failed");
}
