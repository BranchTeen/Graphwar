#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include "viewmodel/GameViewModel.h"
#include "GameCanvas.h"
#include "FunctionInput.h"
#include "SaveManagerPage.h"
#include "PauseMenuPage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    // Esc 键：游戏页 / 暂停页 之间切换
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum PageIndex {
        PageStart = 0,
        PageGame,
        PageSaveMgr,
        PagePause
    };

    void showPage(PageIndex p);
    void startNewGame();
    void goToSaveManager();
    void goToPause();
    void resumeFromPause();
    void backToStart();
    void onGameLoaded();
    void onGameOver(const QString &winnerInfo);   // 游戏结束后弹出"重新开始"对话框

    QStackedWidget *m_stack;
    GameViewModel *m_vm;
    GameCanvas *m_canvas;
    FunctionInput *m_input;
    SaveManagerPage *m_savePage;
    PauseMenuPage *m_pausePage;
};
