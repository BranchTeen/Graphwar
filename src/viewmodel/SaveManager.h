#pragma once
#include <QString>
#include <QStringList>
#include <QDateTime>

// 存档元信息（用于存档管理页面显示）
struct SaveInfo {
    int slot = -1;
    bool exists = false;
    int roundNumber = 0;
    int currentPlayer = 0;
    qint64 savedAt = 0;  // 秒级 unix timestamp

    QString displayTime() const;
};

// 外部文件保存管理器：3 个存档槽
// 文件位置：%APPDATA%/Graphwar/saves/slot_N.json
class SaveManager {
public:
    static constexpr int kSlotCount = 3;

    // 存档目录（确保存在），失败则回退到 exe 目录
    static QString savesDir();
    // 某个槽的存档路径
    static QString slotPath(int slot);

    // 原始 JSON 读写
    static bool writeSlot(int slot, const QString &jsonText);
    static QString readSlot(int slot, bool *ok = nullptr);

    // 元信息
    static SaveInfo slotInfo(int slot);

    // 删除
    static bool deleteSlot(int slot);
};
