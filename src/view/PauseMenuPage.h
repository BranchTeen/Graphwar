#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "common/EventBus.h"

class PauseMenuPage : public QWidget {
    Q_OBJECT
public:
    explicit PauseMenuPage(QWidget *parent = nullptr);

signals:
    void backToTitle();

private slots:
    void onContinueClicked();
    void onBackClicked();
    void onSaveClicked(int slot);
    void onSaveResult(int slot, bool ok, const QString &info);

private:
    void buildSaveSlots();

    QPushButton *m_continueBtn;
    QPushButton *m_backBtn;
    QVector<QPushButton*> m_saveSlots;
    QWidget *m_slotsWidget = nullptr;
};
