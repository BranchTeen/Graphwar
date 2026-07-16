#include "SaveInfo.h"
#include <QDateTime>

QString SaveInfo::displayTime() const {
    if (!exists) return QString();
    return QDateTime::fromSecsSinceEpoch(savedAt).toString("yyyy-MM-dd HH:mm:ss");
}