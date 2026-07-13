#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "common/EventBus.h"

class FunctionInput : public QWidget {
    Q_OBJECT
public:
    explicit FunctionInput(QWidget *parent = nullptr);

private slots:
    void onTextChanged(const QString &text);
    void onLaunch();
    void onPhaseChanged(GamePhase phase);
    void onMessageChanged(const QString &msg);
    void onCostPreviewChanged(int cost);

private:
    QLineEdit *m_input;
    QPushButton *m_fireBtn;
    QLabel *m_messageLabel;
    QLabel *m_costLabel;
};
