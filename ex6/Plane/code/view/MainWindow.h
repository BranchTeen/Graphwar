
#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "../common/frame.h"

#include "widgets/BattleBoard.h"

class MainWindow : public Fl_Double_Window
{
public:
	MainWindow(int w, int h, const char *title = nullptr);
	MainWindow(const MainWindow&) = delete;
	~MainWindow() noexcept;

	MainWindow& operator=(const MainWindow&) = delete;

//commands
	void set_next_step_command(std::function<void(int)>&& pn) noexcept
	{
		m_next_step_command = std::move(pn);
	}

//notification
	PropertyNotification get_notification();

//methods
	BattleBoard& get_board() noexcept
	{
		return m_board;
	}

protected:
//callbacks
	static void timeout_cb(void*);

private:
	BattleBoard m_board;

private:
//commands
	std::function<void(int)> m_next_step_command;
};

#endif
