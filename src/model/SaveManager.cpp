#include "SaveManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDateTime>

QString SaveManager::savesDir() {
    QString base = QCoreApplication::applicationDirPath();
    QDir d(base);
    if (!d.exists()) d.mkpath(".");
    QString saves = base + "/saves";
    QDir s(saves);
    if (!s.exists()) s.mkpath(".");
    return saves;
}

QString SaveManager::slotPath(int slot) {
    return savesDir() + QString("/slot_%1.json").arg(slot);
}

bool SaveManager::writeSlot(int slot, const QString &jsonText) {
    QFile f(slotPath(slot));
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;
    f.write(jsonText.toUtf8());
    return true;
}

QString SaveManager::readSlot(int slot, bool *ok) {
    QFile f(slotPath(slot));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (ok) *ok = false;
        return QString();
    }
    if (ok) *ok = true;
    return QString::fromUtf8(f.readAll());
}

SaveInfo SaveManager::slotInfo(int slot) {
    SaveInfo info;
    info.slot = slot;
    QFile f(slotPath(slot));
    if (!f.exists()) return info;
    info.exists = true;
    info.savedAt = f.fileTime(QFileDevice::FileModificationTime).toSecsSinceEpoch();

    bool ok = false;
    QString txt = readSlot(slot, &ok);
    if (!ok) return info;
    QJsonDocument doc = QJsonDocument::fromJson(txt.toUtf8());
    if (!doc.isObject()) return info;
    QJsonObject root = doc.object();
    info.roundNumber = root.value("roundNumber").toInt(0);
    info.currentPlayer = root.value("currentPlayer").toInt(0);
    return info;
}

bool SaveManager::deleteSlot(int slot) {
    QFile f(slotPath(slot));
    if (!f.exists()) return true;
    return f.remove();
}
