#pragma once
#include "common/frame.h"

struct GameState;
class GameCanvas;
class FunctionInput;
class ConfigPage;
class SaveManagerPage;
class PauseMenuPage;
class GuidePage;
struct GameConfig;

class QSlider;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    MainWindow(const MainWindow&) = delete;
    ~MainWindow() noexcept;
    MainWindow& operator=(const MainWindow&) = delete;

    void set_state(const GameState* state) noexcept { m_state = state; }
    void set_cost_preview_ptr(const int* ptr) noexcept { m_costPreviewPtr = ptr; }

    void set_new_game_command(std::function<void()>&& cmd) noexcept { m_newGameCmd = std::move(cmd); }
    void set_set_config_command(std::function<void(const GameConfig&)>&& cmd) noexcept { m_setConfigCmd = std::move(cmd); }
    void set_pause_command(std::function<void()>&& cmd) noexcept { m_pauseCmd = std::move(cmd); }
    void set_resume_command(std::function<void()>&& cmd) noexcept { m_resumeCmd = std::move(cmd); }
    void set_load_slot_command(std::function<void(int)>&& cmd) noexcept { m_loadSlotCmd = std::move(cmd); }
    void set_delete_slot_command(std::function<void(int)>&& cmd) noexcept { m_deleteSlotCmd = std::move(cmd); }
    void set_save_slot_command(std::function<void(int)>&& cmd) noexcept { m_saveSlotCmd = std::move(cmd); }
    void set_next_turn_command(std::function<void()>&& cmd) noexcept { m_nextTurnCmd = std::move(cmd); }
    void set_bgm_volume_command(std::function<void(int)>&& cmd) noexcept { m_setBgmVolumeCmd = std::move(cmd); }
    void set_sfx_volume_command(std::function<void(int)>&& cmd) noexcept { m_setSfxVolumeCmd = std::move(cmd); }
    void set_bgm_muted_command(std::function<void(bool)>&& cmd) noexcept { m_setBgmMutedCmd = std::move(cmd); }
    void set_sfx_muted_command(std::function<void(bool)>&& cmd) noexcept { m_setSfxMutedCmd = std::move(cmd); }

    PropertyNotification get_notification();

    GameCanvas* get_canvas() noexcept { return m_canvas; }
    FunctionInput* get_input() noexcept { return m_input; }
    ConfigPage* get_config_page() noexcept { return m_configPage; }
    SaveManagerPage* get_save_page() noexcept { return m_savePage; }
    PauseMenuPage* get_pause_page() noexcept { return m_pausePage; }

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum PageIndex {
        PageStart = 0, PageGame, PageSaveMgr, PagePause, PageConfig, PageGuide
    };

    void showPage(PageIndex p);
    void startNewGame();
    void goToConfig();
    void goToSaveManager();
    void goToGuide();
    void goToPause();
    void resumeFromPause();
    void backToStart();
    void onGameLoaded();
    void onGameOver(const QString &winnerInfo);
    void updateTopBarColors();
    void updateCoordLabels();

    void setupAudioControls();
    void onBgmVolumeChanged(int v);
    void onSfxVolumeChanged(int v);
    void onBgmMutedChanged(bool m);
    void onSfxMutedChanged(bool m);
    void onGlobalMutedClicked();

    QStackedWidget *m_stack;
    const GameState *m_state = nullptr;
    const int *m_costPreviewPtr = nullptr;
    GameCanvas *m_canvas;
    FunctionInput *m_input;
    QLabel *m_p1Label, *m_p2Label, *m_roundLabel, *m_pointsLabel, *m_coordLabel, *m_coordLabel2;
    SaveManagerPage *m_savePage;
    PauseMenuPage *m_pausePage;
    ConfigPage *m_configPage;
    GuidePage *m_guidePage;

    std::function<void()> m_newGameCmd;
    std::function<void(const GameConfig&)> m_setConfigCmd;
    std::function<void()> m_pauseCmd;
    std::function<void()> m_resumeCmd;
    std::function<void(int)> m_loadSlotCmd;
    std::function<void(int)> m_deleteSlotCmd;
    std::function<void(int)> m_saveSlotCmd;
    std::function<void()> m_nextTurnCmd;
    std::function<void(int)> m_setBgmVolumeCmd;
    std::function<void(int)> m_setSfxVolumeCmd;
    std::function<void(bool)> m_setBgmMutedCmd;
    std::function<void(bool)> m_setSfxMutedCmd;

    QWidget *m_audioBar = nullptr;
    QPushButton *m_muteBtn = nullptr;
    QSlider *m_bgmVolumeSlider = nullptr;
    QLabel *m_bgmVolumeLabel = nullptr;
    QSlider *m_sfxVolumeSlider = nullptr;
    QLabel *m_sfxVolumeLabel = nullptr;
};
