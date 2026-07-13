
#ifndef __BOOK_VIEW_MODEL_H__
#define __BOOK_VIEW_MODEL_H__

#include "../model/InfoModel.h"

#include "commands/ChangePriceCommand.h"
#include "commands/LoadCommand.h"

class BookViewModel
{
public:
	BookViewModel() noexcept;
	BookViewModel(const BookViewModel&) = delete;
	~BookViewModel() noexcept
	{
	}

	BookViewModel& operator=(const BookViewModel&) = delete;

//properties
	const std::string* get_serial() const noexcept
	{
		return &(m_sp_info_model->get_info()->serial);
	}
	const std::string* get_name() const noexcept
	{
		return &(m_sp_info_model->get_info()->name);
	}
	const std::string* get_summary() const noexcept
	{
		return &(m_sp_info_model->get_info()->summary);
	}
	const std::string* get_price() const noexcept
	{
		return &m_price;
	}

//commands
	ICommandBase* get_change_price_command() noexcept
	{
		return &m_change_price_command;
	}
	ICommandBase* get_load_command() noexcept
	{
		return &m_load_command;
	}

//trigger
	PropertyTrigger& get_trigger() noexcept
	{
		return m_trigger;
	}

//methods
	void set_info_model(const std::shared_ptr<InfoModel>& sp)
	{
		m_sp_info_model = sp;
		m_sp_info_model->get_trigger().add(&notification_cb, this);
	}

	std::shared_ptr<InfoModel> get_info_model() const noexcept
	{
		return m_sp_info_model;
	}

private:
//notification
	static void notification_cb(uint32_t id, void *p);

private:
	//model
	std::shared_ptr<InfoModel> m_sp_info_model;

	//properties
	std::string m_price;

	//commands
	ChangePriceCommand m_change_price_command;
	LoadCommand m_load_command;

	//trigger
	PropertyTrigger m_trigger;
};

#endif
