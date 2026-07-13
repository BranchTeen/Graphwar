#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_load_slot_command() {
    return [this](int slot) {
        if (!m_model->loadFromSlot(slot)) return;
        syncState();
        m_costPreview = 0;
        m_state.slotInfos = m_model->slotInfos();
        m_state.slotCount = m_model->slotCount();
        fire(PROP_ID_COST_PREVIEW);
        fire(PROP_ID_TURN);
        fire(PROP_ID_ROUND);
        fire(PROP_ID_POINTS);
        fire(PROP_ID_PHASE);
        fire(PROP_ID_MESSAGE);
        fire(PROP_ID_TRAJECTORY);
        fire(PROP_ID_SAVE_RESULT);
    };
}