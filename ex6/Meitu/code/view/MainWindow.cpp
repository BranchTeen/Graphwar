
#include "../common/property_ids.h"

#include "MainWindow.h"

#define BTN_LOAD_X  (5)
#define BTN_LOAD_Y  (5)
#define BTN_LOAD_W  (80)
#define BTN_LOAD_H  (25)

#define BTN_GRAY_X  (BTN_LOAD_X + BTN_LOAD_W + 5)
#define BTN_GRAY_Y  (5)
#define BTN_GRAY_W  (80)
#define BTN_GRAY_H  (25)

#define BOX_X  (5)
#define BOX_Y  (BTN_LOAD_Y + BTN_LOAD_H + 5)
#define BOX_W  (w - 5 - 5)
#define BOX_H  (h - 10 - BOX_Y)

MainWindow::MainWindow(int w, int h, const char *title) : Fl_Window(w, h, title),
					m_btn_load(BTN_LOAD_X, BTN_LOAD_Y, BTN_LOAD_W, BTN_LOAD_H, "Load"),
					m_btn_gray(BTN_GRAY_X, BTN_GRAY_Y, BTN_GRAY_W, BTN_GRAY_H, "Gray"),
					m_box(BOX_X, BOX_Y, BOX_W, BOX_H)
{
	end();

	m_btn_load.callback(&btn_load_cb, this);
	m_btn_gray.callback(&btn_gray_cb, this);

	//properties
	m_ppimage = nullptr;
	//commands
	m_load_command = nullptr;
	m_gray_command = nullptr;
}
MainWindow::~MainWindow() noexcept
{
}

//overriders

void MainWindow::on_property_changed(uint32_t id)
{
	switch (id) {
	case PROP_ID_IMAGE:
		m_box.image(const_cast<Fl_RGB_Image*>(*m_ppimage));
		m_box.redraw();
		break;
	default:
		break;
	}
}

//callbacks

void MainWindow::btn_load_cb(Fl_Widget *, void *pv)
{
	MainWindow *pThis = (MainWindow *)pv;

	Fl_Native_File_Chooser dlg;
	dlg.title("Pick a png file");
	dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
	dlg.filter("PNG\t*.png");
	if (dlg.show() == 0) {
		if (pThis->m_load_command != nullptr) {
			if (pThis->m_load_command->exec(std::make_any<const char*>(dlg.filename())) == 0) {
				fl_alert("Cannot open file!");
				return ;
			}
		}
	}
}
void MainWindow::btn_gray_cb(Fl_Widget *, void *pv)
{
	MainWindow *pThis = (MainWindow *)pv;
	if (pThis->m_gray_command != nullptr)
		pThis->m_gray_command->exec(std::make_any<int>(0));
}
