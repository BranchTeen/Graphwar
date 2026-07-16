#pragma once
#include <QPointF>
#include <QColor>

struct Particle {
    QPointF pos;
    QPointF vel;
    QColor color;
    double size;
    double life;
    double maxLife;

    Particle() = default;
    Particle(const QPointF &p, const QPointF &v, const QColor &c, double s, double l)
        : pos(p), vel(v), color(c), size(s), life(l), maxLife(l) {}
};
