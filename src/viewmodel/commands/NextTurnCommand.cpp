#include "../GameViewModel.h"

std::function<void()> GameViewModel::get_next_turn_command() {
    return [this]() {
        m_model->nextTurn();
    };
}
