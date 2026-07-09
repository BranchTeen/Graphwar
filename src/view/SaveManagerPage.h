#pragma once
#include <QWidget>
#include "viewmodel/GameViewModel.h"

// 存档管理页：三列槽位，每槽位 Load / Delete，空槽位显示 Empty
class SaveManagerPage : public QWidget {
    Q_OBJECT
public:
    explicit SaveManagerPage(GameViewModel *vm, QWidget *parent = nullptr);

    // 每次进入页面时刷新显示
    void refresh();

signals:
    // 点击返回标题
    void backToStart();
    // 某个槽位成功读取存档后，应当进入游戏页
    void gameLoaded();

private:
    void rebuild();
    void loadSlot(int slot);
    void deleteSlot(int slot);

    GameViewModel *m_vm = nullptr;
    QWidget *m_slotsContainer = nullptr;
};
