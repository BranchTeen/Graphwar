#include "../GameViewModel.h"
#include "common/SaveManager.h"
#include "common/property_ids.h"

std::function<void(int)> GameViewModel::get_load_slot_command() {
    return [this](int slot) {
        bool ok = false;
        QString text = SaveManager::readSlot(slot, &ok);
        if (!ok) return;
        if (!m_model->fromJson(text)) return;
        syncState();
        m_costPreview = 0;
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
