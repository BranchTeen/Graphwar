#include "../GameViewModel.h"

std::function<void()> GameViewModel::get_resume_command() {
    return [this]() {
        m_model->resume();
    };
}
