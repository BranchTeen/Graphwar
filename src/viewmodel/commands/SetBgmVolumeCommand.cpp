#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_set_bgm_volume_command() {
    return [this](int v) {
        m_model->setBgmVolume(v);
    };
}