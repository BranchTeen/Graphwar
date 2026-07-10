#include "GameViewModel.h"
#include "model/SaveManager.h"

GameViewModel::GameViewModel(QObject *parent) : QObject(parent) {
    m_model = new GameModel(this);

    // === 把 Model 的信号桥接到 ViewModel → View ===
    // View 层始终只连接 ViewModel 的 signals，保持单向依赖
    connect(m_model, &GameModel::turnChanged,         this, &GameViewModel::turnChanged);
    connect(m_model, &GameModel::roundChanged,        this, &GameViewModel::roundChanged);
    connect(m_model, &GameModel::pointsChanged,       this, &GameViewModel::pointsChanged);
    connect(m_model, &GameModel::phaseChanged,        this, &GameViewModel::phaseChanged);
    connect(m_model, &GameModel::messageChanged,      this, &GameViewModel::messageChanged);
    connect(m_model, &GameModel::squareHit,           this, &GameViewModel::squareHit);
    connect(m_model, &GameModel::trajectoryUpdated,   this, &GameViewModel::trajectoryUpdated);
    connect(m_model, &GameModel::gameOver,            this, &GameViewModel::gameOver);
    connect(m_model, &GameModel::pausedChanged,       this, &GameViewModel::pausedChanged);
}

// ===================== 状态查询（全部转发到 Model） =====================
int GameViewModel::currentPlayer() const   { return m_model->currentPlayer(); }
int GameViewModel::roundNumber() const     { return m_model->roundNumber(); }
int GameViewModel::availablePoints() const { return m_model->availablePoints(); }
int GameViewModel::costPreview() const     { return m_costPreview; }
GamePhase GameViewModel::phase() const     { return m_model->phase(); }
QString GameViewModel::message() const     { return m_model->message(); }
bool GameViewModel::paused() const         { return m_model->paused(); }

QColor GameViewModel::playerColor(int player) const { return m_model->playerColor(player); }
int GameViewModel::aliveCount(int player) const      { return m_model->aliveCount(player); }

int GameViewModel::totalSquaresPerPlayer() const {
    return static_cast<int>(m_model->playerSquares(m_model->currentPlayer()).size());
}

int GameViewModel::selectedSquareIndex() const { return m_model->selectedSquareIndex(); }

QVector<Square> GameViewModel::playerSquares(int player) const {
    return m_model->playerSquares(player);
}

QVector<Square> GameViewModel::obstacles() const {
    return m_model->obstacles();
}

QVector<QPointF> GameViewModel::trajectory() const {
    return m_model->trajectory();
}

QVector<QPointF> GameViewModel::historyTrajectory() const {
    auto h = m_model->history();
    if (h.isEmpty()) return {};
    return h.last();
}

bool GameViewModel::isGameOver() const     { return m_model->isGameOver(); }
bool GameViewModel::isWaitingInput() const { return m_model->isWaitingInput(); }
bool GameViewModel::isAnimating() const    { return m_model->isAnimating(); }

// ===================== 配置 =====================
GameConfig GameViewModel::configSnapshot() const { return m_model->config(); }
const GameConfig &GameViewModel::config() const { return m_model->config(); }
bool GameViewModel::showGridLines() const { return m_model->config().showGridLines; }
bool GameViewModel::showCoordinates() const { return m_model->config().showCoordinates; }

// ===================== 存档管理 =====================
int GameViewModel::slotCount() const { return SaveManager::kSlotCount; }

QVector<SaveInfo> GameViewModel::slotInfos() const {
    QVector<SaveInfo> result;
    for (int slot = 0; slot < SaveManager::kSlotCount; ++slot)
        result.append(SaveManager::slotInfo(slot));
    return result;
}

QString GameViewModel::slotPath(int slot) const {
    return SaveManager::slotPath(slot);
}

bool GameViewModel::saveToSlot(int slot) {
    QString json = m_model->toJson();
    bool ok = SaveManager::writeSlot(slot, json);
    emit saveResult(slot, ok, ok ? "Saved" : "Save failed");
    return ok;
}

bool GameViewModel::loadFromSlot(int slot) {
    bool ok = false;
    QString text = SaveManager::readSlot(slot, &ok);
    if (!ok) {
        emit saveResult(slot, false, "Load failed: file not found");
        return false;
    }
    if (!m_model->fromJson(text)) {
        emit saveResult(slot, false, "Load failed: bad data");
        return false;
    }
    m_costPreview = 0;
    // 加载后需要通知所有订阅者刷新 UI —— 通过手动发出几个关键信号
    emit turnChanged(m_model->currentPlayer());
    emit roundChanged(m_model->roundNumber());
    emit pointsChanged(m_model->availablePoints());
    emit phaseChanged(m_model->phase());
    emit messageChanged(m_model->message());
    emit trajectoryUpdated();
    emit costPreviewChanged(0);
    emit saveResult(slot, true, "Loaded");
    return true;
}

bool GameViewModel::deleteSlot(int slot) {
    bool ok = SaveManager::deleteSlot(slot);
    emit saveResult(slot, ok, ok ? "Deleted" : "Delete failed");
    return ok;
}

// ===================== 业务操作（转发到 Model） =====================
void GameViewModel::newGame() {
    m_costPreview = 0;
    m_model->newGame(m_model->config());
    emit costPreviewChanged(0);
}

void GameViewModel::updateCostPreview(const QString &expr) {
    if (expr.isEmpty()) {
        m_costPreview = 0;
        emit costPreviewChanged(0);
        return;
    }
    m_costPreview = m_model->calculateCost(expr);
    emit costPreviewChanged(m_costPreview);
}

void GameViewModel::launch(const QString &expr) {
    bool ok = m_model->launch(expr);
    if (ok) {
        // 成功发射 → 重置点数预览（输入框应该清空）
        m_costPreview = 0;
        emit costPreviewChanged(0);
    }
}

void GameViewModel::nextTurn() {
    m_model->nextTurn();
}

void GameViewModel::pause()       { m_model->pause(); }
void GameViewModel::resume()      { m_model->resume(); }
void GameViewModel::togglePause() { m_model->togglePause(); }

void GameViewModel::setConfig(const GameConfig &cfg) {
    m_model->setConfig(cfg);
}
