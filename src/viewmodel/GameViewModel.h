#pragma once
#include "common/frame.h"
#include "model/GameModel.h"
#include "model/SaveInfo.h"

class GameViewModel : public PropertyTrigger {
public:
    GameViewModel();
    GameViewModel(const GameViewModel&) = delete;
    ~GameViewModel() noexcept;

    GameViewModel& operator=(const GameViewModel&) = delete;

    const GameModel* get_model() const noexcept { return m_model; }

    std::function<void()> get_new_game_command();
    std::function<void(const QString&)> get_launch_command();
    std::function<void(const QString&)> get_update_cost_preview_command();
    std::function<void()> get_next_turn_command();
    std::function<void()> get_pause_command();
    std::function<void()> get_resume_command();
    std::function<void(const GameConfig&)> get_set_config_command();
    std::function<void(int)> get_save_slot_command();
    std::function<void(int)> get_load_slot_command();
    std::function<void(int)> get_delete_slot_command();

    int costPreview() const noexcept { return m_costPreview; }

private:
    void forwardModelSignals();

    GameModel *m_model = nullptr;
    int m_costPreview = 0;
};
