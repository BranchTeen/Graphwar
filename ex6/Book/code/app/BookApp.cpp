
#include "BookApp.h"

BookApp::BookApp() : m_main_wnd(640, 480, "Book House"),
		m_sp_book_viewmodel(std::make_shared<BookViewModel>()),
		m_sp_info_model(std::make_shared<InfoModel>())
{
}

bool BookApp::initialize()
{
	//binding
	m_sp_book_viewmodel->set_info_model(m_sp_info_model);
	//properties
	m_main_wnd.set_serial(m_sp_book_viewmodel->get_serial());
	m_main_wnd.set_name(m_sp_book_viewmodel->get_name());
	m_main_wnd.set_summary(m_sp_book_viewmodel->get_summary());
	m_main_wnd.set_price(m_sp_book_viewmodel->get_price());
	//command
	m_main_wnd.set_change_price_command(m_sp_book_viewmodel->get_change_price_command());
	m_main_wnd.set_load_command(m_sp_book_viewmodel->get_load_command());
	//notification
	m_sp_book_viewmodel->get_trigger().add(m_main_wnd.get_notification(), &m_main_wnd);

	//update ui
	m_main_wnd.update_ui();

	return true;
}
