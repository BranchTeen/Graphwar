#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_delete_slot_command() {
    return [this](int slot) {
        m_model->deleteSlot(slot);
        m_state.slotInfos = m_model->slotInfos();
        m_state.slotCount = m_model->slotCount();
        fire(PROP_ID_SAVE_RESULT);
    };
}