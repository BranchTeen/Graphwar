#include "../GameViewModel.h"

std::function<void(const GameConfig&)> GameViewModel::get_set_config_command() {
    return [this](const GameConfig& cfg) {
        m_model->setConfig(cfg);
    };
}
