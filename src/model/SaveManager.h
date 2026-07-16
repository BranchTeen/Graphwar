#pragma once
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>
#include "common/SaveInfo.h"

class SaveManager {
public:
    static constexpr int kSlotCount = 3;

    static QString savesDir();
    static QString slotPath(int slot);

    static bool writeSlot(int slot, const QString &jsonText);
    static QString readSlot(int slot, bool *ok = nullptr);

    static SaveInfo slotInfo(int slot);
    static QVector<SaveInfo> slotInfos();

    static bool deleteSlot(int slot);
};