#include "GameCanvas.h"
#include "viewmodel/GameViewModel.h"
#include "model/GameModel.h"

#include <QPainter>
#include <QPainterPath>
#include <cmath>

GameCanvas::GameCanvas(QWidget *parent) : QWidget(parent) {
    setMinimumSize(600, 400);
}
GameCanvas::~GameCanvas() noexcept {}

QPointF GameCanvas::worldToScreen(double wx, double wy) const {
    return {m_ox + wx * m_scale, m_oy - wy * m_scale};
}

void GameCanvas::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width(), h = height();
    m_ox = w / 2.0;
    m_oy = h / 2.0;
    m_scale = std::min(w, h) / 40.0;

    p.fillRect(rect(), QColor(20, 20, 30));

    if (!m_vm) return;
    const auto *model = m_vm->get_model();
    const auto &cfg = model->config();

    if (cfg.showGridLines) {
        for (int gx = -20; gx <= 20; gx += 5) {
            bool edge = (gx == -20 || gx == 20);
            p.setPen(QPen(edge ? QColor(80, 80, 110) : QColor(35, 35, 50), 1));
            p.drawLine(worldToScreen(gx, -30), worldToScreen(gx, 30));
        }
        for (int gy = -20; gy <= 20; gy += 5) {
            bool edge = (gy == -20 || gy == 20);
            p.setPen(QPen(edge ? QColor(80, 80, 110) : QColor(35, 35, 50), 1));
            p.drawLine(worldToScreen(-30, gy), worldToScreen(30, gy));
        }
    }

    p.setPen(QPen(QColor(180, 180, 200), 2));
    auto ox = worldToScreen(0, 0);
    p.drawLine(QPointF(0, ox.y()), QPointF(w, ox.y()));
    p.drawLine(QPointF(ox.x(), 0), QPointF(ox.x(), h));

    QVector<Square> obstacles = model->obstacles();
    for (auto &obs : obstacles) {
        QPointF tl = worldToScreen(obs.rect.cx - obs.rect.w/2, obs.rect.cy + obs.rect.h/2);
        QPointF br = worldToScreen(obs.rect.cx + obs.rect.w/2, obs.rect.cy - obs.rect.h/2);
        QRectF r(tl, br);

        if (obs.destroyed) {
            p.setPen(QPen(QColor(80, 80, 80), 1, Qt::DashLine));
            p.drawRect(r);
        } else {
            p.fillRect(r, QColor(120, 120, 120));
            p.setPen(QPen(QColor(50, 50, 50), 1));
            p.drawRect(r);
        }
    }

    int curPlayer = model->currentPlayer();

    for (int pl = 0; pl < 2; ++pl) {
        QColor playerColor = model->playerColor(pl);
        QVector<Square> squares = model->playerSquares(pl);
        for (int i = 0; i < squares.size(); ++i) {
            auto &sq = squares[i];
            QPointF tl = worldToScreen(sq.rect.cx - sq.rect.w/2, sq.rect.cy + sq.rect.h/2);
            QPointF br = worldToScreen(sq.rect.cx + sq.rect.w/2, sq.rect.cy - sq.rect.h/2);
            QRectF r(tl, br);

            if (sq.destroyed) {
                p.fillRect(r, QColor(60, 60, 60));
                p.setPen(QPen(QColor(100, 100, 100), 1));
                p.drawRect(r);
            } else {
                p.fillRect(r, playerColor);
            }

            if (!sq.destroyed && pl == curPlayer && i == model->selectedSquareIndex()) {
                p.setPen(QPen(QColor(255, 255, 255), 2, Qt::DashLine));
                p.drawRect(r.adjusted(-3, -3, 3, 3));
            }
        }
    }

    auto fullHistory = model->history();
    QVector<QPointF> history = fullHistory.isEmpty() ? QVector<QPointF>() : fullHistory.last();
    if (!history.isEmpty()) {
        p.setPen(QPen(QColor(100, 100, 100), 1));
        for (int i = 1; i < history.size(); ++i)
            p.drawLine(worldToScreen(history[i-1].x(), history[i-1].y()),
                       worldToScreen(history[i].x(), history[i].y()));
    }

    QVector<QPointF> traj = model->trajectory();
    if (!traj.isEmpty()) {
        QColor playerColor = model->playerColor(curPlayer);
        p.setPen(QPen(playerColor, 2));
        for (int i = 1; i < traj.size(); ++i)
            p.drawLine(worldToScreen(traj[i-1].x(), traj[i-1].y()),
                       worldToScreen(traj[i].x(), traj[i].y()));
    }

    if (!traj.isEmpty()) {
        QPointF bullet = worldToScreen(traj.last().x(), traj.last().y());
        p.setBrush(QBrush(model->playerColor(curPlayer)));
        p.setPen(QPen(QColor(255, 255, 255), 1));
        p.drawEllipse(bullet, 5, 5);
    }

    if (model->isGameOver()) {
        p.fillRect(rect(), QColor(0, 0, 0, 180));
        p.setPen(QPen(QColor(255, 255, 255), 3));
        p.setFont(QFont("Arial", 24, QFont::Bold));
        QString winner = QString("PLAYER %1 WINS!\nAlive: %2 vs %3\nRound %4")
            .arg(curPlayer + 1)
            .arg(model->aliveCount(0))
            .arg(model->aliveCount(1))
            .arg(model->roundNumber());
        p.drawText(rect(), Qt::AlignCenter, winner);
    }

}
