#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "common/EventBus.h"

class SaveManagerPage : public QWidget {
    Q_OBJECT
public:
    explicit SaveManagerPage(QWidget *parent = nullptr);

signals:
    void backRequested();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onLoadClicked(int slot);
    void onDeleteClicked(int slot);
    void onBackClicked();
    void onSaveResult(int slot, bool ok, const QString &info);

private:
    void buildSlots();
    QWidget *m_slotsContainer = nullptr;
};
