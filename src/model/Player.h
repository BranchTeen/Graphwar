#pragma once
#include "common/Square.h"
#include <QVector>
#include <QColor>

struct Player {
    int id = 0;
    QColor color;
    QVector<Square> squares;

    int aliveCount() const {
        int n = 0;
        for (auto &s : squares)
            if (!s.destroyed) ++n;
        return n;
    }

    bool allDestroyed() const {
        return aliveCount() == 0;
    }
};
