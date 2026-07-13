
#ifndef __INFO_MODEL_H__
#define __INFO_MODEL_H__

#include "../common/frame.h"

#include "base/BookInfo.h"

class InfoModel
{
public:
	InfoModel() noexcept
	{
	}
	InfoModel(const InfoModel&) = delete;
	~InfoModel() noexcept
	{
	}

	InfoModel& operator=(const InfoModel&) = delete;

	const BookInfo *get_info() const noexcept
	{
		return &m_info;
	}

	PropertyTrigger& get_trigger() noexcept
	{
		return m_trigger;
	}

//methods

	void change_price(double price) noexcept;
	bool load(const char* file);

private:
	BookInfo m_info;

	PropertyTrigger m_trigger;
};

#endif
