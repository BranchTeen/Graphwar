#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "common/frame.h"
#include "common/GameState.h"

class PauseMenuPage : public QWidget {
    Q_OBJECT
public:
    explicit PauseMenuPage(QWidget *parent = nullptr);
    PauseMenuPage(const PauseMenuPage&) = delete;
    ~PauseMenuPage() noexcept;
    PauseMenuPage& operator=(const PauseMenuPage&) = delete;

    void set_state(const GameState *state) noexcept { m_state = state; }
    void set_resume_command(std::function<void()>&& cmd) noexcept { m_resumeCmd = std::move(cmd); }
    void set_save_slot_command(std::function<void(int)>&& cmd) noexcept { m_saveSlotCmd = std::move(cmd); }

    void refreshSlots();

signals:
    void backToTitle();

private slots:
    void onContinueClicked();
    void onBackClicked();
    void onSaveClicked(int slot);

private:
    void buildSaveSlots();

    const GameState *m_state = nullptr;
    QPushButton *m_continueBtn;
    QPushButton *m_backBtn;
    QVector<QPushButton*> m_saveSlots;
    QWidget *m_slotsWidget = nullptr;

    std::function<void()> m_resumeCmd;
    std::function<void(int)> m_saveSlotCmd;
};
