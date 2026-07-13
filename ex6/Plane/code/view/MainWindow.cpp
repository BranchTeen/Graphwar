
#include "../common/property_ids.h"

#include "MainWindow.h"

#define BOARD_X  (5)
#define BOARD_Y  (5)
#define BOARD_W  (w - 5 - 5)
#define BOARD_H  (h - 50 - BOARD_Y)

MainWindow::MainWindow(int w, int h, const char *title) : Fl_Double_Window(w, h, title),
					m_board(BOARD_X, BOARD_Y, BOARD_W, BOARD_H)
{
	end();

	Fl::add_timeout(0.2, &timeout_cb, this);
}

MainWindow::~MainWindow() noexcept
{
}

//notification

PropertyNotification MainWindow::get_notification()
{
	return [this](uint32_t id)->void
		{
			switch (id) {
			case PROP_ID_MAP:
				m_board.redraw();
				break;
			default:
				break;
			}
		};
}

//callbacks

void MainWindow::timeout_cb(void* pv)
{
	MainWindow* pThis = (MainWindow*)pv;
	static int l_turn = 0;
	if (pThis->m_next_step_command != nullptr) {
		pThis->m_next_step_command(l_turn);
		++ l_turn;
	}
	Fl::repeat_timeout(0.2, &timeout_cb, pThis);
}
