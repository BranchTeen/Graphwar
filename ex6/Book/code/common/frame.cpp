
#include "frame.h"

uintptr_t PropertyTrigger::add(PropertyNotification pn, void *pv)
{
	uintptr_t index = 0;
	for (size_t i = 0; i < m_vec_nf.size(); i ++) {
		_notification& nf = m_vec_nf[i];
		if (nf.pn == nullptr) {
			nf.pn = pn;
			nf.pv = pv;
			return index + 1;
		}
		index ++;
	}
	m_vec_nf.push_back(_notification{pn, pv});
	return index + 1;
}

void PropertyTrigger::fire(uint32_t id)
{
	for (size_t i = 0; i < m_vec_nf.size(); i ++) {
		_notification& nf = m_vec_nf[i];
		if (nf.pn != nullptr)
			nf.pn(id, nf.pv);
	}
}
