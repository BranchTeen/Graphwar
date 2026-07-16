#include "../GameViewModel.h"
#include "common/property_ids.h"

std::function<void(const QString&)> GameViewModel::get_launch_command() {
    return [this](const QString& expr) {
        bool ok = m_model->launch(expr);
        if (ok) {
            m_costPreview = 0;
            fire(PROP_ID_COST_PREVIEW);
        }
    };
}
