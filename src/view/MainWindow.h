#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include "common/EventBus.h"
#include "GameCanvas.h"
#include "FunctionInput.h"
#include "SaveManagerPage.h"
#include "PauseMenuPage.h"
#include "ConfigPage.h"
#include "model/GameConfig.h"

class GameViewModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(GameViewModel *vm, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum PageIndex {
        PageStart = 0,
        PageGame,
        PageSaveMgr,
        PagePause,
        PageConfig
    };

    void showPage(PageIndex p);
    void startNewGame();
    void goToConfig();
    void goToSaveManager();
    void goToPause();
    void resumeFromPause();
    void backToStart();
    void onGameLoaded();
    void onGameOver(const QString &winnerInfo);

    void updateTopBarColors();

    QStackedWidget *m_stack;
    GameViewModel *m_vm;
    GameCanvas *m_canvas;
    FunctionInput *m_input;
    QLabel *m_p1Label;
    QLabel *m_p2Label;
    SaveManagerPage *m_savePage;
    PauseMenuPage *m_pausePage;
    ConfigPage *m_configPage;
};
