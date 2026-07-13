#pragma once
#include <QString>

// 存档元信息（纯数据结构）—— 供存档管理页面显示使用
// 属于 MVVM 的 Model 层：只保存数据，不含业务逻辑
struct SaveInfo {
    int slot = -1;
    bool exists = false;
    int roundNumber = 0;
    int currentPlayer = 0;
    qint64 savedAt = 0;  // 秒级 unix timestamp

    QString displayTime() const;
};
