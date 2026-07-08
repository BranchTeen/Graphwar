#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include "viewmodel/GameViewModel.h"
#include "GameCanvas.h"
#include "FunctionInput.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void startGame();

private:
    QStackedWidget *m_stack;
    GameViewModel *m_vm;
    GameCanvas *m_canvas;
    FunctionInput *m_input;
    QPushButton *m_playAgainBtn = nullptr;
};
