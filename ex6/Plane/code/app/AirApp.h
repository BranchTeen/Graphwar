
#ifndef __AIR_APP_H__
#define __AIR_APP_H__

#include "../viewmodel/GameViewModel.h"
#include "../viewmodel/SpiritViewModel.h"
#include "../view/MainWindow.h"

class AirApp
{
public:
	AirApp();
	AirApp(const AirApp&) = delete;
	~AirApp() noexcept
	{
	}

	AirApp& operator=(const AirApp&) = delete;

	bool initialize();

	void show_main_window()
	{
		m_main_wnd.show();
	}

private:
	MainWindow m_main_wnd;
	GameViewModel m_game_viewmodel;
	SpiritViewModel m_spirit_viewmodel;
};

#endif
