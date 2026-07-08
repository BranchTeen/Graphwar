#pragma once

struct Point {
    double x, y;
};

struct Rect {
    double cx, cy, w, h;
    bool contains(double px, double py) const {
        return px >= cx - w/2 && px <= cx + w/2 &&
               py >= cy - h/2 && py <= cy + h/2;
    }
};
