
#include "AirApp.h"

AirApp::AirApp() : m_main_wnd(600, 700, "Aircraft Battle")
{
}

bool AirApp::initialize()
{
	if (!m_spirit_viewmodel.initialize())
		return false;

	//binding

	//properties
	m_main_wnd.get_board().set_map(m_game_viewmodel.get_map());
	m_main_wnd.get_board().set_plane_red(m_spirit_viewmodel.get_plane_red());
	m_main_wnd.get_board().set_plane_blue(m_spirit_viewmodel.get_plane_blue());
	m_main_wnd.get_board().set_bullet(m_spirit_viewmodel.get_bullet());

	//commands
	m_main_wnd.set_next_step_command(m_game_viewmodel.get_next_step_command());
	m_main_wnd.get_board().set_move_red_command(m_game_viewmodel.get_move_red_command());

	//notification
	m_game_viewmodel.add_notification(m_main_wnd.get_notification());

	return true;
}
