#include "frame.h"

uintptr_t PropertyTrigger::add_notification(PropertyNotification&& pn) {
    m_vec_nf.push_back(std::move(pn));
    return m_vec_nf.size();
}

void PropertyTrigger::fire(uint32_t id) {
    for (auto& nf : m_vec_nf) {
        if (nf) nf(id);
    }
}
