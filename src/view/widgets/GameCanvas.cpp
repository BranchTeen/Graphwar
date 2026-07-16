#include "GameCanvas.h"
#include "common/GameState.h"
#include "common/Particle.h"

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

    if (!m_state) return;
    const auto &s = *m_state;

    if (s.config.showGridLines) {
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

    QVector<Square> obstacles = s.obstacles;
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

    int curPlayer = s.currentPlayer;

    for (int pl = 0; pl < 2; ++pl) {
        QColor playerColor = s.playerColors[pl];
        const auto &squares = s.playerSquares[pl];
        for (int i = 0; i < squares.size(); ++i) {
            const auto &sq = squares[i];
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

            if (!sq.destroyed && pl == curPlayer && i == s.selectedSquareIndex) {
                p.setPen(QPen(QColor(255, 255, 255), 2, Qt::DashLine));
                p.drawRect(r.adjusted(-3, -3, 3, 3));
            }
        }
    }

    QVector<QVector<QPointF>> fullHistory = s.history;
    QVector<QPointF> history = fullHistory.isEmpty() ? QVector<QPointF>() : fullHistory.last();
    if (!history.isEmpty()) {
        p.setPen(QPen(QColor(100, 100, 100), 1));
        for (int i = 1; i < history.size(); ++i)
            p.drawLine(worldToScreen(history[i-1].x(), history[i-1].y()),
                       worldToScreen(history[i].x(), history[i].y()));
    }

    const auto &traj = s.trajectory;
    if (!traj.isEmpty()) {
        QColor playerColor = s.playerColors[curPlayer];
        p.setPen(QPen(playerColor, 2));
        for (int i = 1; i < traj.size(); ++i)
            p.drawLine(worldToScreen(traj[i-1].x(), traj[i-1].y()),
                       worldToScreen(traj[i].x(), traj[i].y()));
    }

    if (!traj.isEmpty()) {
        QPointF bullet = worldToScreen(traj.last().x(), traj.last().y());
        p.setBrush(QBrush(s.playerColors[curPlayer]));
        p.setPen(QPen(QColor(255, 255, 255), 1));
        p.drawEllipse(bullet, 5, 5);
    }

    for (const auto &particle : s.particles) {
        QPointF pt = worldToScreen(particle.pos.x(), particle.pos.y());
        double alpha = particle.life / particle.maxLife;
        QColor color = particle.color;
        color.setAlphaF(alpha);
        p.setBrush(QBrush(color));
        p.setPen(Qt::NoPen);
        double size = particle.size * m_scale * alpha;
        p.drawEllipse(pt, size, size);
    }

    if (s.gameOver) {
        p.fillRect(rect(), QColor(0, 0, 0, 180));
        p.setPen(QPen(QColor(255, 255, 255), 3));
        p.setFont(QFont("Arial", 24, QFont::Bold));
        QString winner = QString("PLAYER %1 WINS!\nAlive: %2 vs %3\nRound %4")
            .arg(curPlayer + 1)
            .arg(s.aliveCount[0])
            .arg(s.aliveCount[1])
            .arg(s.roundNumber);
        p.drawText(rect(), Qt::AlignCenter, winner);
    }
}
