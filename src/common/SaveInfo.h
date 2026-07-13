#pragma once
#include <QString>

struct SaveInfo {
    int slot = -1;
    bool exists = false;
    int roundNumber = 0;
    int currentPlayer = 0;
    qint64 savedAt = 0;

    QString displayTime() const;
};