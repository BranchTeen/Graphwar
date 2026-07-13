#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "common/frame.h"
#include "model/SaveInfo.h"

class SaveManagerPage : public QWidget {
    Q_OBJECT
public:
    explicit SaveManagerPage(QWidget *parent = nullptr);
    SaveManagerPage(const SaveManagerPage&) = delete;
    ~SaveManagerPage() noexcept;
    SaveManagerPage& operator=(const SaveManagerPage&) = delete;

    void set_load_slot_command(std::function<void(int)>&& cmd) noexcept { m_loadSlotCmd = std::move(cmd); }
    void set_delete_slot_command(std::function<void(int)>&& cmd) noexcept { m_deleteSlotCmd = std::move(cmd); }

    void refreshSlots();

signals:
    void backRequested();

private slots:
    void onLoadClicked(int slot);
    void onDeleteClicked(int slot);
    void onBackClicked();

private:
    void buildSlots();
    QWidget *m_slotsContainer = nullptr;

    std::function<void(int)> m_loadSlotCmd;
    std::function<void(int)> m_deleteSlotCmd;
};
