#pragma once
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>
#include "SaveInfo.h"

// SaveManager：MVVM Model 层的存档管理组件
// - 提供文件 IO 能力（读写 JSON 到 saves/ 目录）
// - 提供存档元信息查询（供存档管理页面显示使用）
// - 是纯静态工具类，不持有任何可变状态
// - ViewModel 层通过它完成 saveToSlot / loadFromSlot / deleteSlot
//   View 层不应直接 #include 这个文件（MVVM 要求）
class SaveManager {
public:
    static constexpr int kSlotCount = 3;

    static QString savesDir();
    static QString slotPath(int slot);

    // 原始 JSON 读写（供 ViewModel 内部使用）
    static bool writeSlot(int slot, const QString &jsonText);
    static QString readSlot(int slot, bool *ok = nullptr);

    // 元信息（供 UI 刷新用）
    static SaveInfo slotInfo(int slot);
    static QVector<SaveInfo> slotInfos();

    // 删除
    static bool deleteSlot(int slot);
};
