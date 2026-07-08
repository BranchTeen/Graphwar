#include "GameViewModel.h"
#include "parser/Tokenizer.h"
#include "parser/Evaluator.h"
#include <QRandomGenerator>
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
    pickRandomSquare();
    m_message = "Player 1's turn - enter a function";
    emit turnChanged(0);
    emit roundChanged(1);
    emit pointsChanged(m_model.availablePoints());
    emit costPreviewChanged(0);
    emit phaseChanged(GamePhase::WaitingInput);
    emit messageChanged(m_message);
}

void GameViewModel::generateSquares() {
    double w = 0.6, h = 0.6;
    double minDist = 1.5;
    int maxAttempts = 200;

    for (int p = 0; p < 2; ++p) {
        double xMin = (p == 0) ? -12.0 : 3.0;
        double xMax = (p == 0) ? -3.0 : 12.0;
        auto &squares = m_model.players[p].squares;
        squares.clear();

        for (int i = 0; i < 5; ++i) {
            bool placed = false;
            for (int attempt = 0; attempt < maxAttempts; ++attempt) {
                double cx = QRandomGenerator::global()->generateDouble() * (xMax - xMin) + xMin;
                double cy = QRandomGenerator::global()->generateDouble() * 14.0 - 7.0;
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

    // Calculate constant adjustment: C = cy - f(cx)
    double fcx = evaluate(expr, cx);
    m_constAdjust = cy - fcx;

    m_currentExpr = expr;
    m_model.trajectory.clear();
    m_animX = 0;
    m_hasHit = false;

    // Seed trajectory from the launch point
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

    if (std::fabs(x) > 22 || std::fabs(y) > 14) {
        m_animTimer.stop();
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
    pickRandomSquare();

    emit turnChanged(m_model.currentPlayer);
    emit pointsChanged(m_model.availablePoints());
    emit costPreviewChanged(0);
    emit phaseChanged(GamePhase::WaitingInput);
    m_message = QString("Player %1's turn - enter a function").arg(m_model.currentPlayer + 1);
    emit messageChanged(m_message);
}
