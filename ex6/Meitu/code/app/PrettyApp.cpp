
#include "PrettyApp.h"

PrettyApp::PrettyApp() : m_main_wnd(800, 700, "Mei Tu")
{
	//binding
	m_image_viewmodel.set_image_model(&m_image_model);
	//properties
	m_main_wnd.set_image(m_image_viewmodel.get_image());
	//commands
	m_main_wnd.set_load_command(m_image_viewmodel.get_load_command());
	m_main_wnd.set_gray_command(m_image_viewmodel.get_gray_command());
	//notification
	m_image_model.get_trigger().add(m_image_viewmodel.get_notification());
	m_image_viewmodel.get_trigger().add(m_main_wnd.get_notification());
}
