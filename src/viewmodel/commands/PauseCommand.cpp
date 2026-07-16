#include "../GameViewModel.h"

std::function<void()> GameViewModel::get_pause_command() {
    return [this]() {
        m_model->pause();
    };
}
