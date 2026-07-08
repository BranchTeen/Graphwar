#pragma once
#include <QWidget>
#include "viewmodel/GameViewModel.h"

class GameCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GameCanvas(GameViewModel *vm, QWidget *parent = nullptr);

    void setViewModel(GameViewModel *vm);

public slots:
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPointF worldToScreen(double wx, double wy) const;
    QPointF screenToWorld(int sx, int sy) const;

    GameViewModel *m_vm = nullptr;
    double m_scale = 40.0; // pixels per world unit
    double m_ox = 0, m_oy = 0; // screen offset
};
