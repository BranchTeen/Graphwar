#include "view/GameCanvas.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

GameCanvas::GameCanvas(QWidget *parent) : QWidget(parent) {
    setMinimumSize(600, 400);
}

void GameCanvas::refresh() {
    update();
}

QPointF GameCanvas::worldToScreen(double wx, double wy) const {
    double sx = m_ox + wx * m_scale;
    double sy = m_oy - wy * m_scale;
    return {sx, sy};
}

void GameCanvas::paintEvent(QPaintEvent *) {
    auto &bus = EventBus::instance();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width(), h = height();
    m_ox = w / 2.0;
    m_oy = h / 2.0;
    m_scale = std::min(w, h) / 40.0;

    p.fillRect(rect(), QColor(20, 20, 30));

    if (bus.showGridLines()) {
        p.setPen(QPen(QColor(35, 35, 50), 1));
        for (int gx = -20; gx <= 20; gx += 5)
            p.drawLine(worldToScreen(gx, -30), worldToScreen(gx, 30));
        for (int gy = -20; gy <= 20; gy += 5)
            p.drawLine(worldToScreen(-30, gy), worldToScreen(30, gy));
    }

    p.setPen(QPen(QColor(180, 180, 200), 2));
    auto ox = worldToScreen(0, 0);
    p.drawLine(QPointF(0, ox.y()), QPointF(w, ox.y()));
    p.drawLine(QPointF(ox.x(), 0), QPointF(ox.x(), h));

    int selIdx = bus.selectedSquareIndex();
    int curPlayer = bus.currentPlayer();

    for (int pl = 0; pl < 2; ++pl) {
        QColor playerColor = bus.playerColor(pl);
        QVector<Square> squares = bus.playerSquares(pl);
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

            if (!sq.destroyed && pl == curPlayer && i == selIdx) {
                p.setPen(QPen(QColor(255, 255, 255), 2, Qt::DashLine));
                p.drawRect(r.adjusted(-3, -3, 3, 3));

                if (bus.showCoordinates()) {
                    p.setPen(QColor(200, 200, 200));
                    p.setFont(QFont("Arial", 9));
                    QString coord = QString("(%1,%2)").arg(sq.rect.cx, 0, 'f', 1).arg(sq.rect.cy, 0, 'f', 1);
                    p.drawText(r.bottomLeft() + QPointF(3, 15), coord);
                }
            }
        }
    }

    QVector<Square> obstacles = bus.obstacles();
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

    QVector<QPointF> history = bus.historyTrajectory();
    if (!history.isEmpty()) {
        p.setPen(QPen(QColor(100, 100, 100), 1));
        for (int i = 1; i < history.size(); ++i)
            p.drawLine(worldToScreen(history[i-1].x(), history[i-1].y()),
                       worldToScreen(history[i].x(), history[i].y()));
    }

    QVector<QPointF> traj = bus.trajectory();
    if (!traj.isEmpty()) {
        QColor playerColor = bus.playerColor(curPlayer);
        p.setPen(QPen(playerColor, 2));
        for (int i = 1; i < traj.size(); ++i)
            p.drawLine(worldToScreen(traj[i-1].x(), traj[i-1].y()),
                       worldToScreen(traj[i].x(), traj[i].y()));
    }

    if (!traj.isEmpty()) {
        QPointF bullet = worldToScreen(traj.last().x(), traj.last().y());
        p.setBrush(QBrush(bus.playerColor(curPlayer)));
        p.setPen(QPen(QColor(255, 255, 255), 1));
        p.drawEllipse(bullet, 5, 5);
    }

    if (bus.isGameOver()) {
        p.fillRect(rect(), QColor(0, 0, 0, 180));
        p.setPen(QPen(QColor(255, 255, 255), 3));
        p.setFont(QFont("Arial", 24, QFont::Bold));
        QString winner = QString("PLAYER %1 WINS!\nAlive: %2 vs %3\nRound %4")
            .arg(curPlayer + 1)
            .arg(bus.aliveCount(0))
            .arg(bus.aliveCount(1))
            .arg(bus.roundNumber());
        p.drawText(rect(), Qt::AlignCenter, winner);
    }
}
