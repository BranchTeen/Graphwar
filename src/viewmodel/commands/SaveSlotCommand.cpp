#include "../GameViewModel.h"
#include "common/SaveManager.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_save_slot_command() {
    return [this](int slot) {
        QString json = m_model->toJson();
        bool ok = SaveManager::writeSlot(slot, json);
        fire(PROP_ID_SAVE_RESULT);
    };
}
