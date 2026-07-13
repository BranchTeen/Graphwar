#pragma once
#include <QWidget>
#include <QPointF>
#include "common/EventBus.h"

class GameCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GameCanvas(QWidget *parent = nullptr);

public slots:
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF worldToScreen(double wx, double wy) const;

    double m_scale = 40.0;
    double m_ox = 0, m_oy = 0;
};
