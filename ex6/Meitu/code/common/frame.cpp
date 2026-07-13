
#include "frame.h"

uintptr_t PropertyTrigger::add(IPropertyNotification *pn)
{
	uintptr_t index = 0;
	for (size_t i = 0; i < m_vec_nf.size(); i ++) {
		IPropertyNotification *&p = m_vec_nf[i];
		if (p == nullptr) {
			p = pn;
			return index + 1;
		}
		index ++;
	}
	m_vec_nf.push_back(pn);
	return index + 1;
}

void PropertyTrigger::fire(uint32_t id)
{
	for (size_t i = 0; i < m_vec_nf.size(); i ++) {
		IPropertyNotification *p = m_vec_nf[i];
		if (p != nullptr)
			p->on_property_changed(id);
	}
}
