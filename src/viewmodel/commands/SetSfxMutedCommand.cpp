#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(bool)> GameViewModel::get_set_sfx_muted_command() {
    return [this](bool m) {
        m_model->setSfxMuted(m);
    };
}