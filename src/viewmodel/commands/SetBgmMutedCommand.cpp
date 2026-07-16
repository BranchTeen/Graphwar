#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(bool)> GameViewModel::get_set_bgm_muted_command() {
    return [this](bool m) {
        m_model->setBgmMuted(m);
    };
}