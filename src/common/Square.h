#pragma once
#include <cmath>

struct Rect {
    double cx, cy, w, h;
    bool contains(double px, double py) const {
        return px >= cx - w/2 && px <= cx + w/2 &&
               py >= cy - h/2 && py <= cy + h/2;
    }
};

struct Square {
    Rect rect;
    bool destroyed = false;

    Square() = default;
    Square(double cx, double cy, double w, double h)
        : rect{cx, cy, w, h} {}
};
