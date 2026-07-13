
#include "../common/property_ids.h"

#include "InfoModel.h"

//methods

void InfoModel::change_price(double price) noexcept
{
	if (price != m_info.price) {
		m_info.price = price;
		m_trigger.fire(PROP_ID_BOOK_PRICE);
	}
}

bool InfoModel::load(const char* file)
{
	std::ifstream stm;
	stm.open(file, std::ios_base::in);
	if (!stm.is_open())
		return false;
	stm >> m_info.serial >> m_info.name >> m_info.summary >> m_info.price;
	m_trigger.fire(PROP_ID_BOOK_SERIAL);
	m_trigger.fire(PROP_ID_BOOK_NAME);
	m_trigger.fire(PROP_ID_BOOK_SUMMARY);
	m_trigger.fire(PROP_ID_BOOK_PRICE);
	return true;
}
