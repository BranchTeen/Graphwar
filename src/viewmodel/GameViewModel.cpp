#include "GameViewModel.h"
#include "SaveManager.h"
#include "parser/Tokenizer.h"
#include "parser/Evaluator.h"
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <cmath>

GameViewModel::GameViewModel(QObject *parent) : QObject(parent) {
    m_animTimer.setInterval(16);
    connect(&m_animTimer, &QTimer::timeout, this, &GameViewModel::advanceAnimation);
}

void GameViewModel::newGame() {
    m_model = GameModel{};
    generateSquares();
    m_model.currentPlayer = 0;
    m_model.roundNumber = 1;
    m_model.phase = GamePhase::WaitingInput;
    m_model.history.clear();
    m_costPreview = 0;
    m_model.selectedSquare = -1;
    m_currentExpr.clear();
    m_animX = 0;
    m_hasHit = false;
    m_constAdjust = 0;
    m_paused = false;
    pickRandomSquare();
    m_message = "Player 1's turn - enter a function";
    emit pausedChanged(false);
    emitAllState();
}

void GameViewModel::generateSquares() {
    double w = 0.8, h = 0.8;
    double minDist = 3.0;
    int maxAttempts = 300;

    for (int p = 0; p < 2; ++p) {
        double xMin = (p == 0) ? -20.0 : 10.0;
        double xMax = (p == 0) ? -10.0 : 20.0;
        auto &squares = m_model.players[p].squares;
        squares.clear();

        for (int i = 0; i < 5; ++i) {
            bool placed = false;
            for (int attempt = 0; attempt < maxAttempts; ++attempt) {
                double cx = QRandomGenerator::global()->generateDouble() * (xMax - xMin) + xMin;
                double cy = QRandomGenerator::global()->generateDouble() * 20.0 - 10.0;
                bool ok = true;
                for (const auto &sq : squares) {
                    double dx = cx - sq.rect.cx;
                    double dy = cy - sq.rect.cy;
                    if (dx * dx + dy * dy < minDist * minDist) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    squares.push_back(Square(cx, cy, w, h));
                    placed = true;
                    break;
                }
            }
            if (!placed) {
                squares.push_back(Square((xMin + xMax) / 2, 0, w, h));
            }
        }
    }

    m_model.players[0].id = 0;
    m_model.players[0].color = QColor(60, 120, 220, 200);
    m_model.players[1].id = 1;
    m_model.players[1].color = QColor(220, 60, 60, 200);
}

void GameViewModel::selectSquare(int index) {
    if (m_model.phase != GamePhase::WaitingInput) return;
    auto &player = m_model.currentPlayerRef();
    if (index >= 0 && index < player.squares.size() && !player.squares[index].destroyed) {
        m_model.selectedSquare = index;
    }
}

void GameViewModel::updateCostPreview(const QString &expr) {
    if (expr.isEmpty()) {
        m_costPreview = 0;
        emit costPreviewChanged(0);
        return;
    }
    m_costPreview = calculateCost(expr);
    emit costPreviewChanged(m_costPreview);
}

void GameViewModel::launch(const QString &expr) {
    if (m_paused) return;
    if (m_model.phase != GamePhase::WaitingInput) return;
    if (m_model.selectedSquare < 0) {
        pickRandomSquare();
        if (m_model.selectedSquare < 0) return;
    }

    auto tokens = tokenize(expr);
    if (tokens.size() <= 1) {
        m_message = "Invalid expression!";
        emit messageChanged(m_message);
        return;
    }

    int cost = calculateCost(expr);
    int avail = m_model.availablePoints();
    if (cost > avail) {
        m_message = QString("Not enough points! Need %1, available %2").arg(cost).arg(avail);
        emit messageChanged(m_message);
        return;
    }

    auto &sq = m_model.currentPlayerRef().squares[m_model.selectedSquare];
    double cx = sq.rect.cx, cy = sq.rect.cy;

    double fcx = evaluate(expr, cx);
    m_constAdjust = cy - fcx;

    m_currentExpr = expr;
    m_model.trajectory.clear();
    m_animX = 0;
    m_hasHit = false;

    double y0 = evaluate(expr, cx) + m_constAdjust;
    m_model.trajectory.push_back({cx, y0});

    m_model.phase = GamePhase::Animating;
    emit phaseChanged(GamePhase::Animating);
    m_message = "Firing...";
    emit messageChanged(m_message);

    m_animTimer.start();
}

void GameViewModel::advanceAnimation() {
    auto &sq = m_model.currentPlayerRef().squares[m_model.selectedSquare];
    double cx = sq.rect.cx;

    m_animX += m_animStep;

    double x = (m_model.currentPlayer == 0) ? cx + m_animX : cx - m_animX;
    double y = evaluate(m_currentExpr, x) + m_constAdjust;
    m_model.trajectory.push_back({x, y});

    auto &opponent = m_model.opponentRef();
    for (int i = 0; i < opponent.squares.size(); ++i) {
        if (!opponent.squares[i].destroyed && opponent.squares[i].rect.contains(x, y)) {
            opponent.squares[i].destroyed = true;
            emit squareHit(1 - m_model.currentPlayer, i);
            m_hasHit = true;
        }
    }

    if (opponent.allDestroyed()) {
        m_animTimer.stop();
        m_model.phase = GamePhase::GameOver;
        m_message = QString("Player %1 wins!").arg(m_model.currentPlayer + 1);
        emit messageChanged(m_message);
        emit phaseChanged(GamePhase::GameOver);
        emit trajectoryUpdated();
        return;
    }

    if (std::fabs(x) > 30 || std::fabs(y) > 25) {
        m_animTimer.stop();
        if (!m_model.trajectory.isEmpty())
            m_model.history.append(m_model.trajectory);
        m_model.phase = GamePhase::RoundEnd;
        m_message = m_hasHit ? "Hit!" : "Miss!";
        emit messageChanged(m_message);
        emit phaseChanged(GamePhase::RoundEnd);
        emit trajectoryUpdated();
        return;
    }

    if (m_hasHit) {
        m_message = "Hit!";
        emit messageChanged(m_message);
    }

    emit trajectoryUpdated();
}

bool GameViewModel::checkHit(const QPointF &pt) const {
    auto &opponent = m_model.opponentRef();
    for (const auto &sq : opponent.squares) {
        if (!sq.destroyed && sq.rect.contains(pt.x(), pt.y()))
            return true;
    }
    return false;
}

void GameViewModel::pickRandomSquare() {
    auto &player = m_model.currentPlayerRef();
    QVector<int> alive;
    for (int i = 0; i < player.squares.size(); ++i)
        if (!player.squares[i].destroyed)
            alive.push_back(i);
    if (!alive.isEmpty())
        m_model.selectedSquare = alive[QRandomGenerator::global()->bounded(alive.size())];
}

void GameViewModel::nextTurn() {
    if (m_model.phase == GamePhase::GameOver) return;

    if (m_model.currentPlayer == 1) {
        m_model.roundNumber++;
        emit roundChanged(m_model.roundNumber);
    }
    m_model.currentPlayer = 1 - m_model.currentPlayer;
    m_model.phase = GamePhase::WaitingInput;
    m_costPreview = 0;
    m_model.trajectory.clear();
    pickRandomSquare();

    emit turnChanged(m_model.currentPlayer);
    emit pointsChanged(m_model.availablePoints());
    emit costPreviewChanged(0);
    emit phaseChanged(GamePhase::WaitingInput);
    m_message = QString("Player %1's turn - enter a function").arg(m_model.currentPlayer + 1);
    emit messageChanged(m_message);
}

void GameViewModel::pause() {
    if (m_paused) return;
    m_paused = true;
    if (m_animTimer.isActive()) m_animTimer.stop();
    emit pausedChanged(true);
}

void GameViewModel::resume() {
    if (!m_paused) return;
    m_paused = false;
    if (m_model.phase == GamePhase::Animating && !m_currentExpr.isEmpty()) {
        m_animTimer.start();
    }
    emit pausedChanged(false);
}

void GameViewModel::togglePause() {
    if (m_paused) resume(); else pause();
}

bool GameViewModel::saveToSlot(int slot) {
    QString json = toJson();
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
    if (!fromJson(text)) {
        emit saveResult(slot, false, "Load failed: bad data");
        return false;
    }
    m_costPreview = 0;
    m_paused = false;
    emitAllState();
    emit saveResult(slot, true, "Loaded");
    return true;
}

bool GameViewModel::deleteSlot(int slot) {
    bool ok = SaveManager::deleteSlot(slot);
    emit saveResult(slot, ok, ok ? "Deleted" : "Delete failed");
    return ok;
}

QString GameViewModel::toJson() const {
    QJsonObject root;
    root["version"] = 1;
    root["currentPlayer"] = m_model.currentPlayer;
    root["roundNumber"] = m_model.roundNumber;
    root["selectedSquare"] = m_model.selectedSquare;

    int phaseInt = 0;
    switch (m_model.phase) {
        case GamePhase::WaitingInput: phaseInt = 0; break;
        case GamePhase::Animating:    phaseInt = 1; break;
        case GamePhase::RoundEnd:     phaseInt = 2; break;
        case GamePhase::GameOver:     phaseInt = 3; break;
    }
    if (phaseInt == 1) phaseInt = 0;
    root["phase"] = phaseInt;

    root["message"] = m_message;
    root["currentExpr"] = m_currentExpr;

    for (int p = 0; p < 2; ++p) {
        QJsonArray arr;
        for (const auto &sq : m_model.players[p].squares) {
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
    for (const auto &traj : m_model.history) {
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

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

bool GameViewModel::fromJson(const QString &text) {
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
    if (!doc.isObject()) return false;
    QJsonObject root = doc.object();
    if (root.value("version").toInt(-1) != 1) return false;

    m_model = GameModel{};
    m_model.currentPlayer = root.value("currentPlayer").toInt(0);
    m_model.roundNumber   = root.value("roundNumber").toInt(1);
    m_model.selectedSquare = root.value("selectedSquare").toInt(-1);
    int pi = root.value("phase").toInt(0);
    switch (pi) {
        case 0: m_model.phase = GamePhase::WaitingInput; break;
        case 1: m_model.phase = GamePhase::Animating; break;
        case 2: m_model.phase = GamePhase::RoundEnd; break;
        case 3: m_model.phase = GamePhase::GameOver; break;
        default: m_model.phase = GamePhase::WaitingInput; break;
    }
    if (m_model.phase == GamePhase::Animating) m_model.phase = GamePhase::WaitingInput;

    m_message = root.value("message").toString("Loaded game");
    m_currentExpr = root.value("currentExpr").toString();

    for (int p = 0; p < 2; ++p) {
        QJsonValue v = root.value(p == 0 ? "player0" : "player1");
        if (!v.isArray()) return false;
        QJsonArray arr = v.toArray();
        auto &squares = m_model.players[p].squares;
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

    m_model.players[0].id = 0;
    m_model.players[0].color = QColor(60, 120, 220, 200);
    m_model.players[1].id = 1;
    m_model.players[1].color = QColor(220, 60, 60, 200);

    QJsonArray historyArr = root.value("history").toArray();
    m_model.history.clear();
    for (const auto &traj : historyArr) {
        if (!traj.isArray()) continue;
        QVector<QPointF> pts;
        for (const auto &pt : traj.toArray()) {
            if (!pt.isObject()) continue;
            pts.push_back(QPointF(pt.toObject().value("x").toDouble(),
                                  pt.toObject().value("y").toDouble()));
        }
        m_model.history.append(pts);
    }

    return true;
}

void GameViewModel::emitAllState() {
    emit turnChanged(m_model.currentPlayer);
    emit roundChanged(m_model.roundNumber);
    emit pointsChanged(m_model.availablePoints());
    emit costPreviewChanged(m_costPreview);
    emit phaseChanged(m_model.phase);
    emit messageChanged(m_message);
    emit trajectoryUpdated();
}
