#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "viewmodel/GameViewModel.h"

class FunctionInput : public QWidget {
    Q_OBJECT
public:
    explicit FunctionInput(GameViewModel *vm, QWidget *parent = nullptr);
    void setViewModel(GameViewModel *vm);

private slots:
    void onTextChanged(const QString &text);
    void onLaunch();
    void onPhaseChanged(GamePhase phase);

private:
    GameViewModel *m_vm = nullptr;
    QLineEdit *m_input;
    QPushButton *m_launchBtn;
    QLabel *m_costLabel;
    QLabel *m_msgLabel;
};
