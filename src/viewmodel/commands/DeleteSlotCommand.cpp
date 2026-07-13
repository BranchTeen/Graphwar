#include "../GameViewModel.h"
#include "model/SaveManager.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_delete_slot_command() {
    return [this](int slot) {
        SaveManager::deleteSlot(slot);
        fire(PROP_ID_SAVE_RESULT);
    };
}
