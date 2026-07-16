#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(const QString&)> GameViewModel::get_update_cost_preview_command() {
    return [this](const QString& expr) {
        if (expr.isEmpty()) {
            m_costPreview = 0;
            fire(PROP_ID_COST_PREVIEW);
            return;
        }
        m_costPreview = m_model->calculateCost(expr);
        fire(PROP_ID_COST_PREVIEW);
    };
}
