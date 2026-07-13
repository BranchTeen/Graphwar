
#include "BookViewModel.h"

#include "../common/property_ids.h"

BookViewModel::BookViewModel() noexcept : m_change_price_command(this),
					m_load_command(this)
{
}

void BookViewModel::notification_cb(uint32_t id, void *p)
{
	BookViewModel* pThis = (BookViewModel*)p;

	switch (id) {
	case PROP_ID_BOOK_SERIAL:
	case PROP_ID_BOOK_NAME:
	case PROP_ID_BOOK_SUMMARY:
		break;
	case PROP_ID_BOOK_PRICE:
		pThis->m_price = std::to_string(pThis->m_sp_info_model->get_info()->price);
		break;
	default:
		break;
	}

	pThis->m_trigger.fire(id);
}
