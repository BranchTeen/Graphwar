#include "GameModel.h"
#include "AudioManager.h"
#include "SaveManager.h"
#include "model/parser/Tokenizer.h"
#include "model/parser/Evaluator.h"
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <cmath>
#include <algorithm>

GameModel::GameModel(QObject *parent) : QObject(parent) {
    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(16);  // ~60 FPS
    connect(m_animTimer, &QTimer::timeout, this, &GameModel::stepAnimation);

    m_particleTimer = new QTimer(this);
    m_particleTimer->setInterval(16);
    connect(m_particleTimer, &QTimer::timeout, this, &GameModel::onParticleTimer);
}

// ======================== 新游戏 ========================
void GameModel::newGame(const GameConfig &cfg) {
    m_config = cfg;
    // 重置所有状态
    for (int i = 0; i < 2; ++i) {
        m_players[i].id = i;
        m_players[i].squares.clear();
    }
    m_obstacles.clear();
    m_currentPlayer = 0;
    m_roundNumber = 1;
    m_pointsLevel = 1;
    m_phase = GamePhase::WaitingInput;
    m_selectedSquare = -1;
    m_trajectory.clear();
    m_history.clear();
    m_paused = false;
    m_currentExpr.clear();
    m_animX = 0;
    m_hasHit = false;
    m_stats.reset();

    generateSquares(m_config.squaresPerPlayer, m_config.player1Color, m_config.player2Color);
    generateObstacles(m_config.obstacleCount, m_config.obstacleSize);
    pickRandomSquare();
    m_message = "Player 1's turn - enter a function";

    // 发射状态变化信号
    emit turnChanged(m_currentPlayer);
    emit roundChanged(m_roundNumber);
    emit pointsChanged(availablePoints());
    emit phaseChanged(m_phase);
    emit messageChanged(m_message);
    emit trajectoryUpdated();
    emit pausedChanged(false);
}

void GameModel::generateSquares(int count, const QColor &p1Color, const QColor &p2Color) {
    double w = 0.8, h = 0.8;
    double minDist = 3.0;
    int maxAttempts = 300;

    for (int p = 0; p < 2; ++p) {
        double xMin = (p == 0) ? -20.0 : 1.0;
        double xMax = (p == 0) ? -1.0 : 20.0;
        double yMin = -10.0;
        double yMax = 10.0;
        auto &squares = m_players[p].squares;
        squares.clear();

        for (int i = 0; i < count; ++i) {
            bool placed = false;
            for (int attempt = 0; attempt < maxAttempts; ++attempt) {
                double cx = QRandomGenerator::global()->generateDouble() * (xMax - xMin) + xMin;
                double cy = QRandomGenerator::global()->generateDouble() * (yMax - yMin) + yMin;
                bool ok = true;
                for (const auto &sq : squares) {
                    double dx = cx - sq.rect.cx;
                    double dy = cy - sq.rect.cy;
                    if (dx * dx + dy * dy < minDist * minDist) {
                        ok = false; break;
                    }
                }
                if (ok) {
                    squares.push_back(Square(cx, cy, w, h));
                    placed = true; break;
                }
            }
            if (!placed) squares.push_back(Square((xMin + xMax) / 2, 0, w, h));
        }
    }

    m_players[0].color = p1Color;
    m_players[1].color = p2Color;
}

void GameModel::generateObstacles(int count, double size) {
    const double w = size, h = size;
    const double minDistBetween = size + 0.7;
    const double minDistToPlayer = size * 0.5 + 0.3;
    const int maxAttempts = 500;

    m_obstacles.clear();
    double xMin = -20.0, xMax = 20.0;
    double yMin = -20.0, yMax = 20.0;

    for (int k = 0; k < count; ++k) {
        bool placed = false;
        for (int attempt = 0; attempt < maxAttempts && !placed; ++attempt) {
            double cx = QRandomGenerator::global()->generateDouble() * (xMax - xMin) + xMin;
            double cy = QRandomGenerator::global()->generateDouble() * (yMax - yMin) + yMin;

            bool ok = true;
            for (int pl = 0; pl < 2 && ok; ++pl) {
                for (const auto &sq : m_players[pl].squares) {
                    double dx = cx - sq.rect.cx;
                    double dy = cy - sq.rect.cy;
                    if (std::sqrt(dx*dx + dy*dy) < minDistToPlayer + 0.4) {
                        ok = false; break;
                    }
                }
            }
            if (!ok) continue;

            for (const auto &ob : m_obstacles) {
                double dx = cx - ob.rect.cx;
                double dy = cy - ob.rect.cy;
                if (std::sqrt(dx*dx + dy*dy) < minDistBetween) {
                    ok = false; break;
                }
            }
            if (!ok) continue;

            m_obstacles.push_back(Square(cx, cy, w, h));
            placed = true;
        }
        if (!placed) break;
    }
}

void GameModel::pickRandomSquare() {
    auto &player = m_players[m_currentPlayer];
    QVector<int> alive;
    for (int i = 0; i < player.squares.size(); ++i)
        if (!player.squares[i].destroyed) alive.push_back(i);
    if (!alive.isEmpty())
        m_selectedSquare = alive[QRandomGenerator::global()->bounded(alive.size())];
}

// ======================== 点数消耗 ========================
int GameModel::calculateCost(const QString &expr) const {
    return ::calculateCost(expr);
}

// ======================== 发射 ========================
bool GameModel::launch(const QString &expr) {
    if (m_paused) return false;
    if (m_phase != GamePhase::WaitingInput) return false;
    if (m_selectedSquare < 0) {
        pickRandomSquare();
        if (m_selectedSquare < 0) return false;
    }

    // 表达式合法性检查
    auto tokens = tokenize(expr);
    if (tokens.size() <= 1) {
        m_message = "Invalid expression!";
        emit messageChanged(m_message);
        return false;
    }

    // 点数是否充足
    int cost = calculateCost(expr);
    int avail = availablePoints();
    if (cost > avail) {
        m_message = QString("Not enough points! Need %1, available %2").arg(cost).arg(avail);
        emit messageChanged(m_message);
        return false;
    }

    // 计算弹道垂直偏移（使曲线从选中方块位置出发）
    auto &sq = m_players[m_currentPlayer].squares[m_selectedSquare];
    double cx = sq.rect.cx, cy = sq.rect.cy;
    double fcx = evaluate(expr, cx);
    m_constAdjust = cy - fcx;

    m_currentExpr = expr;
    m_trajectory.clear();
    m_animX = 0;
    m_hasHit = false;

    m_stats.player[m_currentPlayer].launchCount++;
    m_stats.player[m_currentPlayer].totalPointsSpent += cost;
    m_stats.player[m_currentPlayer].usedFunctions.append(expr);

    double y0 = evaluate(expr, cx) + m_constAdjust;
    m_trajectory.push_back({cx, y0});

    m_phase = GamePhase::Animating;
    emit phaseChanged(m_phase);
    m_message = "Firing...";
    emit messageChanged(m_message);
    playSfx(SfxType::Launch);

    if (m_animTimer) m_animTimer->start();
    return true;
}

// ======================== 动画推进（每帧调用一次） ========================
void GameModel::stepAnimation() {
    if (m_phase != GamePhase::Animating) return;
    if (m_paused) return;

    auto &sq = m_players[m_currentPlayer].squares[m_selectedSquare];
    double cx = sq.rect.cx;

    m_animX += m_animStep;

    // 根据玩家方向决定弹道走向：P0 向右，P1 向左
    double x = (m_currentPlayer == 0) ? cx + m_animX : cx - m_animX;
    double y = evaluate(m_currentExpr, x) + m_constAdjust;
    m_trajectory.push_back({x, y});

    // 障碍物碰撞检测（碰到障碍物 → 终止攻击 + 破坏障碍物）
    for (int i = 0; i < m_obstacles.size(); ++i) {
        auto &ob = m_obstacles[i];
        if (!ob.destroyed && ob.rect.contains(x, y)) {
            ob.destroyed = true;
            m_stats.player[m_currentPlayer].obstacleHitCount++;
            spawnParticles(QPointF(x, y), QColor(120, 120, 120), 12);
            if (m_animTimer) m_animTimer->stop();
            m_history.clear();
            if (!m_trajectory.isEmpty())
                m_history.append(m_trajectory);
            m_phase = GamePhase::RoundEnd;
            m_message = "Hit an obstacle!";
            emit messageChanged(m_message);
            emit phaseChanged(m_phase);
            emit trajectoryUpdated();
            playSfx(SfxType::Obstacle);
            return;
        }
    }

    // 对手方块命中检测
    int opponent = 1 - m_currentPlayer;
    for (int i = 0; i < m_players[opponent].squares.size(); ++i) {
        auto &sq2 = m_players[opponent].squares[i];
        if (!sq2.destroyed && sq2.rect.contains(x, y)) {
            sq2.destroyed = true;
            m_stats.player[m_currentPlayer].hitCount++;
            spawnParticles(QPointF(x, y), m_players[opponent].color, 15);
            emit squareHit(opponent, i);
            m_hasHit = true;
            playSfx(SfxType::Hit);
        }
    }

    // 胜利判断：对手没有存活方块
    if (m_players[opponent].allDestroyed()) {
        if (m_animTimer) m_animTimer->stop();
        m_history.clear();
        if (!m_trajectory.isEmpty())
            m_history.append(m_trajectory);
        m_phase = GamePhase::GameOver;
        m_stats.winner = m_currentPlayer;
        m_stats.totalRounds = m_roundNumber;
        m_message = QString("Player %1 wins!").arg(m_currentPlayer + 1);
        emit messageChanged(m_message);
        emit phaseChanged(m_phase);
        emit trajectoryUpdated();
        emit gameOver(m_message);
        playSfx(SfxType::GameOver);
        return;
    }

    // 出界检测
    if (std::fabs(x) > 30 || std::fabs(y) > 25) {
        if (m_animTimer) m_animTimer->stop();
        m_history.clear();
        if (!m_trajectory.isEmpty())
            m_history.append(m_trajectory);
        m_phase = GamePhase::RoundEnd;
        if (!m_hasHit) {
            m_message = "Miss!";
            playSfx(SfxType::Miss);
        } else {
            m_message = "Hit!";
        }
        emit messageChanged(m_message);
        emit phaseChanged(m_phase);
        emit trajectoryUpdated();
        return;
    }

    if (m_hasHit) {
        m_message = "Hit!";
        emit messageChanged(m_message);
    }

    emit trajectoryUpdated();
}

// ======================== 回合切换 ========================
void GameModel::nextTurn() {
    if (m_phase == GamePhase::GameOver) return;
    playSfx(SfxType::TurnEnd);

    // 点数与轮次不同步：
    // - roundNumber 在 P1 操作结束、切回 P0 时增长
    // - pointsLevel 在 P0 操作结束、切换到 P1 时增长
    if (m_currentPlayer == 1) {
        m_roundNumber++;
        emit roundChanged(m_roundNumber);
    }
    if (m_currentPlayer == 0) {
        m_pointsLevel++;
        emit pointsChanged(availablePoints());
    }

    m_currentPlayer = 1 - m_currentPlayer;
    m_phase = GamePhase::WaitingInput;
    m_trajectory.clear();
    pickRandomSquare();

    emit turnChanged(m_currentPlayer);
    if (m_currentPlayer == 0) {
        emit pointsChanged(availablePoints());
    }
    emit phaseChanged(m_phase);
    m_message = QString("Player %1's turn - enter a function").arg(m_currentPlayer + 1);
    emit messageChanged(m_message);
}

// ======================== 暂停 / 恢复 ========================
void GameModel::pause() {
    if (m_paused) return;
    m_paused = true;
    if (m_animTimer && m_animTimer->isActive()) m_animTimer->stop();
    emit pausedChanged(true);
}

void GameModel::resume() {
    if (!m_paused) return;
    m_paused = false;
    if (m_animTimer && m_phase == GamePhase::Animating && !m_currentExpr.isEmpty())
        m_animTimer->start();
    emit pausedChanged(false);
}

void GameModel::togglePause() {
    if (m_paused) resume(); else pause();
}

// ======================== JSON 序列化 ========================
QString GameModel::toJson() const {
    QJsonObject root;
    root["version"] = 1;
    root["currentPlayer"] = m_currentPlayer;
    root["roundNumber"] = m_roundNumber;
    root["pointsLevel"] = m_pointsLevel;
    root["selectedSquare"] = m_selectedSquare;

    // 阶段：不持久化 Animating（统一转为 WaitingInput）
    int phaseInt = 0;
    switch (m_phase) {
        case GamePhase::WaitingInput: phaseInt = 0; break;
        case GamePhase::Animating:    phaseInt = 1; break;
        case GamePhase::RoundEnd:     phaseInt = 2; break;
        case GamePhase::GameOver:     phaseInt = 3; break;
    }
    if (phaseInt == 1) phaseInt = 0;
    root["phase"] = phaseInt;

    root["message"] = m_message;
    root["currentExpr"] = m_currentExpr;

    // GameConfig
    {
        QJsonObject cfg;
        cfg["squaresPerPlayer"] = m_config.squaresPerPlayer;
        cfg["obstacleCount"] = m_config.obstacleCount;
        cfg["obstacleSize"] = m_config.obstacleSize;
        cfg["player1Color_r"] = m_config.player1Color.red();
        cfg["player1Color_g"] = m_config.player1Color.green();
        cfg["player1Color_b"] = m_config.player1Color.blue();
        cfg["player1Color_a"] = m_config.player1Color.alpha();
        cfg["player2Color_r"] = m_config.player2Color.red();
        cfg["player2Color_g"] = m_config.player2Color.green();
        cfg["player2Color_b"] = m_config.player2Color.blue();
        cfg["player2Color_a"] = m_config.player2Color.alpha();
        cfg["showCoordinates"] = m_config.showCoordinates;
        cfg["showGridLines"] = m_config.showGridLines;
        root["config"] = cfg;
    }

    for (int p = 0; p < 2; ++p) {
        QJsonArray arr;
        for (const auto &sq : m_players[p].squares) {
            QJsonObject s;
            s["cx"] = sq.rect.cx;
            s["cy"] = sq.rect.cy;
            s["w"]  = sq.rect.w;
            s["h"]  = sq.rect.h;
            s["destroyed"] = sq.destroyed;
            arr.append(s);
        }
        root[p == 0 ? "player0" : "player1"] = arr;
    }

    QJsonArray historyArr;
    for (const auto &traj : m_history) {
        QJsonArray ptArr;
        for (const auto &pt : traj) {
            QJsonObject po;
            po["x"] = pt.x();
            po["y"] = pt.y();
            ptArr.append(po);
        }
        historyArr.append(ptArr);
    }
    root["history"] = historyArr;

    QJsonArray obsArr;
    for (const auto &ob : m_obstacles) {
        QJsonObject o;
        o["cx"] = ob.rect.cx;
        o["cy"] = ob.rect.cy;
        o["w"]  = ob.rect.w;
        o["h"]  = ob.rect.h;
        o["destroyed"] = ob.destroyed;
        obsArr.append(o);
    }
    root["obstacles"] = obsArr;

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

bool GameModel::fromJson(const QString &text) {
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
    if (!doc.isObject()) return false;
    QJsonObject root = doc.object();
    if (root.value("version").toInt(-1) != 1) return false;

    // 清空现有状态
    for (int i = 0; i < 2; ++i) {
        m_players[i].id = i;
        m_players[i].squares.clear();
    }
    m_obstacles.clear();
    m_trajectory.clear();
    m_history.clear();

    m_currentPlayer = root.value("currentPlayer").toInt(0);
    m_roundNumber   = root.value("roundNumber").toInt(1);
    m_pointsLevel   = root.value("pointsLevel").toInt(1);
    m_selectedSquare = root.value("selectedSquare").toInt(-1);
    int pi = root.value("phase").toInt(0);
    switch (pi) {
        case 0: m_phase = GamePhase::WaitingInput; break;
        case 1: m_phase = GamePhase::Animating; break;
        case 2: m_phase = GamePhase::RoundEnd; break;
        case 3: m_phase = GamePhase::GameOver; break;
        default: m_phase = GamePhase::WaitingInput; break;
    }
    if (m_phase == GamePhase::Animating) m_phase = GamePhase::WaitingInput;

    m_message = root.value("message").toString("Loaded game");
    m_currentExpr = root.value("currentExpr").toString();

    // GameConfig
    {
        QJsonValue cv = root.value("config");
        if (cv.isObject()) {
            QJsonObject c = cv.toObject();
            if (c.contains("squaresPerPlayer"))
                m_config.squaresPerPlayer = c.value("squaresPerPlayer").toInt(m_config.squaresPerPlayer);
            if (c.contains("obstacleCount"))
                m_config.obstacleCount = c.value("obstacleCount").toInt(m_config.obstacleCount);
            if (c.contains("obstacleSize"))
                m_config.obstacleSize = c.value("obstacleSize").toDouble(m_config.obstacleSize);
            if (c.contains("showCoordinates"))
                m_config.showCoordinates = c.value("showCoordinates").toBool(m_config.showCoordinates);
            if (c.contains("showGridLines"))
                m_config.showGridLines = c.value("showGridLines").toBool(m_config.showGridLines);

            int r1 = c.value("player1Color_r").toInt(m_config.player1Color.red());
            int g1 = c.value("player1Color_g").toInt(m_config.player1Color.green());
            int b1 = c.value("player1Color_b").toInt(m_config.player1Color.blue());
            int a1 = c.value("player1Color_a").toInt(m_config.player1Color.alpha());
            m_config.player1Color.setRgb(r1, g1, b1, a1);

            int r2 = c.value("player2Color_r").toInt(m_config.player2Color.red());
            int g2 = c.value("player2Color_g").toInt(m_config.player2Color.green());
            int b2 = c.value("player2Color_b").toInt(m_config.player2Color.blue());
            int a2 = c.value("player2Color_a").toInt(m_config.player2Color.alpha());
            m_config.player2Color.setRgb(r2, g2, b2, a2);
        }
    }

    for (int p = 0; p < 2; ++p) {
        QJsonValue v = root.value(p == 0 ? "player0" : "player1");
        if (!v.isArray()) return false;
        QJsonArray arr = v.toArray();
        auto &squares = m_players[p].squares;
        squares.clear();
        for (const auto &el : arr) {
            if (!el.isObject()) return false;
            QJsonObject s = el.toObject();
            Square sq(s.value("cx").toDouble(), s.value("cy").toDouble(),
                      s.value("w").toDouble(0.6), s.value("h").toDouble(0.6));
            sq.destroyed = s.value("destroyed").toBool(false);
            squares.push_back(sq);
        }
    }

    m_players[0].color = m_config.player1Color;
    m_players[1].color = m_config.player2Color;

    QJsonArray historyArr = root.value("history").toArray();
    m_history.clear();
    for (const auto &traj : historyArr) {
        if (!traj.isArray()) continue;
        QVector<QPointF> pts;
        for (const auto &pt : traj.toArray()) {
            if (!pt.isObject()) continue;
            pts.push_back(QPointF(pt.toObject().value("x").toDouble(),
                                  pt.toObject().value("y").toDouble()));
        }
        m_history.append(pts);
    }

    QJsonArray obsArr = root.value("obstacles").toArray();
    m_obstacles.clear();
    for (const auto &el : obsArr) {
        if (!el.isObject()) continue;
        QJsonObject o = el.toObject();
        Square ob(o.value("cx").toDouble(), o.value("cy").toDouble(),
                  o.value("w").toDouble(1.8), o.value("h").toDouble(1.8));
        ob.destroyed = o.value("destroyed").toBool(false);
        m_obstacles.push_back(ob);
    }

    // 如果选中方块索引指向了一个已销毁的方块，重新随机选一个
    if (m_selectedSquare >= 0 && m_selectedSquare < m_players[m_currentPlayer].squares.size()) {
        if (m_players[m_currentPlayer].squares[m_selectedSquare].destroyed) {
            pickRandomSquare();
        }
    } else {
        pickRandomSquare();
    }

    return true;
}

bool GameModel::saveToSlot(int slot) {
    QString json = toJson();
    return SaveManager::writeSlot(slot, json);
}

bool GameModel::loadFromSlot(int slot) {
    bool ok = false;
    QString text = SaveManager::readSlot(slot, &ok);
    if (!ok) return false;
    return fromJson(text);
}

bool GameModel::deleteSlot(int slot) {
    return SaveManager::deleteSlot(slot);
}

int GameModel::slotCount() {
    return SaveManager::kSlotCount;
}

SaveInfo GameModel::slotInfo(int slot) {
    return SaveManager::slotInfo(slot);
}

QVector<SaveInfo> GameModel::slotInfos() {
    return SaveManager::slotInfos();
}

// ======================== 音频管理 ========================
int GameModel::bgmVolume() const {
    return AudioManager::instance().bgmVolume();
}

void GameModel::setBgmVolume(int v) {
    AudioManager::instance().setBgmVolume(v);
    emit bgmVolumeChanged(v);
}

bool GameModel::bgmMuted() const {
    return AudioManager::instance().bgmMuted();
}

void GameModel::setBgmMuted(bool m) {
    AudioManager::instance().setBgmMuted(m);
    emit bgmMutedChanged(m);
}

void GameModel::toggleBgmMuted() {
    AudioManager::instance().toggleBgmMuted();
    emit bgmMutedChanged(AudioManager::instance().bgmMuted());
}

int GameModel::sfxVolume() const {
    return AudioManager::instance().sfxVolume();
}

void GameModel::setSfxVolume(int v) {
    AudioManager::instance().setSfxVolume(v);
    emit sfxVolumeChanged(v);
}

bool GameModel::sfxMuted() const {
    return AudioManager::instance().sfxMuted();
}

void GameModel::setSfxMuted(bool m) {
    AudioManager::instance().setSfxMuted(m);
    emit sfxMutedChanged(m);
}

void GameModel::toggleSfxMuted() {
    AudioManager::instance().toggleSfxMuted();
    emit sfxMutedChanged(AudioManager::instance().sfxMuted());
}

void GameModel::playSfx(SfxType type) {
    AudioManager::instance().playSfx(type);
}

void GameModel::playBackgroundMusic(const QUrl &source) {
    AudioManager::instance().playBackgroundMusic(source);
}

void GameModel::stopBackgroundMusic() {
    AudioManager::instance().stopBackgroundMusic();
}

void GameModel::spawnParticles(const QPointF &pos, const QColor &color, int count) {
    for (int i = 0; i < count; ++i) {
        double angle = QRandomGenerator::global()->generateDouble() * M_PI * 2;
        double speed = QRandomGenerator::global()->generateDouble() * 3 + 1;
        QPointF vel(std::cos(angle) * speed, std::sin(angle) * speed);
        double size = QRandomGenerator::global()->generateDouble() * 0.3 + 0.1;
        double life = QRandomGenerator::global()->generateDouble() * 0.5 + 0.3;
        m_particles.push_back(Particle(pos, vel, color, size, life));
    }
    if (m_particleTimer && !m_particleTimer->isActive()) {
        m_particleTimer->start();
    }
}

void GameModel::updateParticles() {
    double dt = 0.016;
    for (auto &p : m_particles) {
        p.pos += p.vel * dt;
        p.life -= dt;
    }
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
        [](const Particle &p) { return p.life <= 0; }), m_particles.end());
}

void GameModel::onParticleTimer() {
    if (m_paused) return;
    updateParticles();
    if (m_particles.isEmpty()) {
        if (m_particleTimer) m_particleTimer->stop();
    }
    emit trajectoryUpdated();
}
