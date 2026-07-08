#pragma once
#include "utils/Geometry.h"

struct Square {
    Rect rect;
    bool destroyed = false;

    Square() = default;
    Square(double cx, double cy, double w, double h)
        : rect{cx, cy, w, h} {}
};
