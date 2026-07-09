#include "GameCanvas.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>

GameCanvas::GameCanvas(GameViewModel *vm, QWidget *parent)
    : QWidget(parent) {
    setViewModel(vm);
    setMinimumSize(600, 400);
    setMouseTracking(true);
}

void GameCanvas::setViewModel(GameViewModel *vm) {
    m_vm = vm;
    if (m_vm) {
        connect(m_vm, &GameViewModel::trajectoryUpdated, this, &GameCanvas::refresh);
        connect(m_vm, &GameViewModel::phaseChanged, this, &GameCanvas::refresh);
        connect(m_vm, &GameViewModel::turnChanged, this, &GameCanvas::refresh);
    }
}

void GameCanvas::refresh() {
    update();
}

QPointF GameCanvas::worldToScreen(double wx, double wy) const {
    double sx = m_ox + wx * m_scale;
    double sy = m_oy - wy * m_scale;
    return {sx, sy};
}

QPointF GameCanvas::screenToWorld(int sx, int sy) const {
    double wx = (sx - m_ox) / m_scale;
    double wy = -(sy - m_oy) / m_scale;
    return {wx, wy};
}

void GameCanvas::paintEvent(QPaintEvent *) {
    if (!m_vm) return;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width(), h = height();
    m_ox = w / 2.0;
    m_oy = h / 2.0;
    m_scale = std::min(w, h) / 40.0;

    // Background
    p.fillRect(rect(), QColor(20, 20, 30));

    // Axes
    p.setPen(QPen(QColor(180, 180, 200), 2));
    auto ox = worldToScreen(0, 0);
    p.drawLine(QPointF(0, ox.y()), QPointF(w, ox.y()));
    p.drawLine(QPointF(ox.x(), 0), QPointF(ox.x(), h));

    const auto &model = m_vm->model();

    // Draw squares
    for (int pl = 0; pl < 2; ++pl) {
        const auto &player = model.players[pl];
        for (int i = 0; i < player.squares.size(); ++i) {
            const auto &sq = player.squares[i];
            auto tl = worldToScreen(sq.rect.cx - sq.rect.w / 2, sq.rect.cy + sq.rect.h / 2);
            auto br = worldToScreen(sq.rect.cx + sq.rect.w / 2, sq.rect.cy - sq.rect.h / 2);
            QRectF r(tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y());

            QColor c = player.color;
            if (sq.destroyed) {
                c = QColor(80, 80, 80, 120);
            } else {
                c.setAlpha(160);
            }
            p.fillRect(r, c);
            p.setPen(QPen(player.color.darker(130), 1.5));
            p.drawRect(r);

            // Highlight selected square
            if (pl == model.currentPlayer && i == model.selectedSquare && !sq.destroyed) {
                p.setPen(QPen(Qt::white, 2, Qt::DashLine));
                p.drawRect(r.adjusted(-2, -2, 2, 2));
            }
        }
    }

    // Draw history trajectories (dimmed)
    p.setPen(QPen(QColor(150, 150, 170, 60), 1.5));
    for (const auto &traj : model.history) {
        if (traj.size() < 2) continue;
        QPainterPath path;
        auto sp = worldToScreen(traj[0].x(), traj[0].y());
        path.moveTo(sp);
        for (int i = 1; i < traj.size(); ++i) {
            sp = worldToScreen(traj[i].x(), traj[i].y());
            path.lineTo(sp);
        }
        p.drawPath(path);
    }

    // Draw current trajectory
    if (!model.trajectory.isEmpty()) {
        QPainterPath path;
        auto sp = worldToScreen(model.trajectory[0].x(), model.trajectory[0].y());
        path.moveTo(sp);
        for (int i = 1; i < model.trajectory.size(); ++i) {
            sp = worldToScreen(model.trajectory[i].x(), model.trajectory[i].y());
            path.lineTo(sp);
        }
        QColor trajColor = (model.currentPlayer == 0) ? QColor(100, 180, 255) : QColor(255, 120, 100);
        p.setPen(QPen(trajColor, 2.5));
        p.drawPath(path);

        // Bullet head
        if (!model.trajectory.isEmpty()) {
            auto head = model.trajectory.back();
            auto sh = worldToScreen(head.x(), head.y());
            p.setBrush(trajColor);
            p.setPen(Qt::NoPen);
            p.drawEllipse(sh, 6, 6);
        }
    }

    // Turn & phase info
    p.setPen(Qt::white);
    QFont font = p.font();
    font.setPointSize(12);
    p.setFont(font);
    QString info = QString("P1: %1  |  Round %2  |  P2: %3")
        .arg(model.players[0].aliveCount())
        .arg(model.roundNumber)
        .arg(model.players[1].aliveCount());
    p.drawText(QRect(10, 10, w - 20, 30), Qt::AlignLeft, info);
}

void GameCanvas::mousePressEvent(QMouseEvent *event) {
    if (!m_vm) return;
    auto wpos = screenToWorld(event->pos().x(), event->pos().y());
    const auto &model = m_vm->model();
    const auto &player = model.currentPlayerRef();
    for (int i = 0; i < player.squares.size(); ++i) {
        if (!player.squares[i].destroyed &&
            player.squares[i].rect.contains(wpos.x(), wpos.y())) {
            m_vm->selectSquare(i);
            refresh();
            return;
        }
    }
}
