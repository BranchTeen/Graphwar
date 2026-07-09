#include "GameCanvas.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

GameCanvas::GameCanvas(GameViewModel *vm, QWidget *parent)
    : QWidget(parent) {
    setViewModel(vm);
    setMinimumSize(600, 400);
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

                p.save();
                QString label = QString("(%1, %2)")
                    .arg(sq.rect.cx, 0, 'f', 2)
                    .arg(sq.rect.cy, 0, 'f', 2);
                QFont f = p.font();
                f.setPointSize(10);
                f.setBold(true);
                p.setFont(f);

                QFontMetricsF fm(f);
                qreal textW = fm.horizontalAdvance(label);
                qreal textH = fm.height();
                QPointF labelPos(r.right() + 8, r.top() - textH / 2);

                if (labelPos.x() + textW + 6 > width()) {
                    labelPos.setX(r.left() - textW - 14);
                }
                if (labelPos.y() < 4) labelPos.setY(4);
                if (labelPos.y() + textH + 4 > height()) labelPos.setY(height() - textH - 4);

                QRectF bg(labelPos.x() - 4, labelPos.y() - 2, textW + 8, textH + 4);
                p.setPen(Qt::NoPen);
                p.setBrush(QColor(0, 0, 0, 180));
                p.drawRoundedRect(bg, 4, 4);

                p.setPen(Qt::white);
                p.drawText(QRectF(labelPos.x(), labelPos.y(), textW, textH),
                           Qt::AlignLeft | Qt::AlignVCenter, label);
                p.restore();
            }
        }
    }

    // Draw obstacles：灰色方块，被破坏的显示为虚线轮廓
    for (const auto &ob : model.obstacles) {
        auto tl = worldToScreen(ob.rect.cx - ob.rect.w / 2, ob.rect.cy + ob.rect.h / 2);
        auto br = worldToScreen(ob.rect.cx + ob.rect.w / 2, ob.rect.cy - ob.rect.h / 2);
        QRectF r(tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y());

        if (ob.destroyed) {
            // 被破坏：只画虚线轮廓，表示已经破坏的障碍物
            p.save();
            p.setPen(QPen(QColor(100, 100, 100), 1.5, Qt::DashLine));
            p.setBrush(Qt::NoBrush);
            p.drawRect(r);
            p.restore();
        } else {
            // 完好：实心填充 + 深色边框
            p.save();
            p.setBrush(QColor(120, 120, 120, 200));
            p.setPen(QPen(QColor(60, 60, 60), 2));
            p.drawRect(r);

            // 中间加一个小圆点，突出"障碍物"视觉感
            QPointF c = worldToScreen(ob.rect.cx, ob.rect.cy);
            p.setBrush(QColor(80, 80, 80, 220));
            p.setPen(Qt::NoPen);
            p.drawEllipse(c, 5, 5);
            p.restore();
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
