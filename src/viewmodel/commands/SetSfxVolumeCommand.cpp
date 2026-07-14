#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_set_sfx_volume_command() {
    return [this](int v) {
        m_model->setSfxVolume(v);
    };
}