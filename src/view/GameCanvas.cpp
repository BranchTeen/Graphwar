#include "view/GameCanvas.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

GameCanvas::GameCanvas(GameViewModel *vm, QWidget *parent)
    : QWidget(parent), m_vm(vm)
{
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
    if (!m_vm) return;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width(), h = height();
    m_ox = w / 2.0;
    m_oy = h / 2.0;
    m_scale = std::min(w, h) / 40.0;

    // 背景
    p.fillRect(rect(), QColor(20, 20, 30));

    // 网格线
    if (m_vm->showGridLines()) {
        p.setPen(QPen(QColor(35, 35, 50), 1));
        for (int gx = -20; gx <= 20; gx += 5)
            p.drawLine(worldToScreen(gx, -30), worldToScreen(gx, 30));
        for (int gy = -20; gy <= 20; gy += 5)
            p.drawLine(worldToScreen(-30, gy), worldToScreen(30, gy));
    }

    // 坐标轴
    p.setPen(QPen(QColor(180, 180, 200), 2));
    auto ox = worldToScreen(0, 0);
    p.drawLine(QPointF(0, ox.y()), QPointF(w, ox.y()));
    p.drawLine(QPointF(ox.x(), 0), QPointF(ox.x(), h));

    // 玩家方块 —— 通过 ViewModel 接口获取副本
    int selIdx = m_vm->selectedSquareIndex();
    int curPlayer = m_vm->currentPlayer();

    for (int pl = 0; pl < 2; ++pl) {
        QColor playerColor = m_vm->playerColor(pl);
        QVector<Square> squares = m_vm->playerSquares(pl);
        for (int i = 0; i < squares.size(); ++i) {
            const Square &sq = squares[i];
            auto tl = worldToScreen(sq.rect.cx - sq.rect.w / 2, sq.rect.cy + sq.rect.h / 2);
            auto br = worldToScreen(sq.rect.cx + sq.rect.w / 2, sq.rect.cy - sq.rect.h / 2);
            QRectF r(tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y());

            QColor c = playerColor;
            if (sq.destroyed) {
                c = QColor(80, 80, 80, 120);
            } else {
                c.setAlpha(160);
            }
            p.fillRect(r, c);
            p.setPen(QPen(playerColor.darker(130), 1.5));
            p.drawRect(r);

            // 选中方块高亮 + 坐标标签
            if (pl == curPlayer && i == selIdx && !sq.destroyed) {
                p.save();
                p.setPen(QPen(Qt::white, 2, Qt::DashLine));
                p.drawRect(r.adjusted(-2, -2, 2, 2));

                if (m_vm->showCoordinates()) {
                    QString label = QString("(%1, %2)")
                        .arg(sq.rect.cx, 0, 'f', 2)
                        .arg(sq.rect.cy, 0, 'f', 2);
                    QFont f = p.font();
                    f.setPointSize(10); f.setBold(true);
                    p.setFont(f);

                    QFontMetricsF fm(f);
                    qreal textW = fm.horizontalAdvance(label);
                    qreal textH = fm.height();
                    QPointF labelPos(r.right() + 8, r.top() - textH / 2);

                    if (labelPos.x() + textW + 6 > width())
                        labelPos.setX(r.left() - textW - 14);
                    if (labelPos.y() < 4) labelPos.setY(4);
                    if (labelPos.y() + textH + 4 > height()) labelPos.setY(height() - textH - 4);

                    QRectF bg(labelPos.x() - 4, labelPos.y() - 2, textW + 8, textH + 4);
                    p.setPen(Qt::NoPen);
                    p.setBrush(QColor(0, 0, 0, 180));
                    p.drawRoundedRect(bg, 4, 4);
                    p.setPen(Qt::white);
                    p.drawText(QRectF(labelPos.x(), labelPos.y(), textW, textH),
                               Qt::AlignLeft | Qt::AlignVCenter, label);
                }
                p.restore();
            }
        }
    }

    // 障碍物 —— 通过 ViewModel 接口获取
    QVector<Square> obstacles = m_vm->obstacles();
    for (const auto &ob : obstacles) {
        auto tl = worldToScreen(ob.rect.cx - ob.rect.w / 2, ob.rect.cy + ob.rect.h / 2);
        auto br = worldToScreen(ob.rect.cx + ob.rect.w / 2, ob.rect.cy - ob.rect.h / 2);
        QRectF r(tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y());

        if (ob.destroyed) {
            p.save();
            p.setPen(QPen(QColor(100, 100, 100), 1.5, Qt::DashLine));
            p.setBrush(Qt::NoBrush);
            p.drawRect(r);
            p.restore();
        } else {
            p.save();
            p.setBrush(QColor(120, 120, 120, 200));
            p.setPen(QPen(QColor(60, 60, 60), 2));
            p.drawRect(r);
            p.restore();
        }
    }

    // 历史轨迹（上一轮）
    QVector<QPointF> history = m_vm->historyTrajectory();
    if (history.size() >= 2) {
        p.setPen(QPen(QColor(150, 150, 170, 60), 1.5));
        QPainterPath path;
        path.moveTo(worldToScreen(history[0].x(), history[0].y()));
        for (int i = 1; i < history.size(); ++i)
            path.lineTo(worldToScreen(history[i].x(), history[i].y()));
        p.drawPath(path);
    }

    // 当前轨迹
    QVector<QPointF> traj = m_vm->trajectory();
    if (!traj.isEmpty()) {
        QPainterPath path;
        path.moveTo(worldToScreen(traj[0].x(), traj[0].y()));
        for (int i = 1; i < traj.size(); ++i)
            path.lineTo(worldToScreen(traj[i].x(), traj[i].y()));
        QColor trajColor = (curPlayer == 0) ? QColor(100, 180, 255) : QColor(255, 120, 100);
        p.setPen(QPen(trajColor, 2.5));
        p.drawPath(path);

        // 子弹头
        auto head = traj.back();
        auto sh = worldToScreen(head.x(), head.y());
        p.setBrush(trajColor);
        p.setPen(Qt::NoPen);
        p.drawEllipse(sh, 6, 6);
    }

    // 回合与玩家信息显示
    p.setPen(Qt::white);
    QFont font = p.font();
    font.setPointSize(12);
    p.setFont(font);
    QString info = QString("P1: %1  |  Round %2  |  P2: %3")
        .arg(m_vm->aliveCount(0))
        .arg(m_vm->roundNumber())
        .arg(m_vm->aliveCount(1));
    p.drawText(QRect(10, 10, w - 20, 30), Qt::AlignLeft, info);

    // 游戏结束遮罩
    if (m_vm->isGameOver()) {
        p.save();
        p.setPen(QPen(QColor(74, 170, 255), 3));
        QFont big = p.font();
        big.setPointSize(48); big.setBold(true);
        p.setFont(big);
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER");
        p.restore();
    }
}
