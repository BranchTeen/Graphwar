#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void()> GameViewModel::get_new_game_command() {
    return [this]() {
        m_costPreview = 0;
        m_model->newGame(m_model->config());
        fire(PROP_ID_COST_PREVIEW);
    };
}
