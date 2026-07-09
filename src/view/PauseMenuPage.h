#pragma once
#include <QWidget>
#include "viewmodel/GameViewModel.h"

// 暂停菜单页：继续 / 保存到 3 个槽位 / 返回标题
class PauseMenuPage : public QWidget {
    Q_OBJECT
public:
    explicit PauseMenuPage(GameViewModel *vm, QWidget *parent = nullptr);

    void refresh();

signals:
    void resumeGame();       // 继续游戏
    void backToStart();      // 返回标题

private:
    void rebuild();
    void saveTo(int slot);

    GameViewModel *m_vm = nullptr;
    QWidget *m_slotsContainer = nullptr;
};
