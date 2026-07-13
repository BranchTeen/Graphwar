
#include "../common/property_ids.h"
#include "../common/command_parameters.h"

#include "MainWindow.h"

#define BTN_CHANGE_X  (5)
#define BTN_CHANGE_Y  (5)
#define BTN_CHANGE_W  (100)
#define BTN_CHANGE_H  (25)

#define BTN_LOAD_X  (BTN_CHANGE_X + BTN_CHANGE_W + 5)
#define BTN_LOAD_Y  (5)
#define BTN_LOAD_W  (100)
#define BTN_LOAD_H  (25)

#define BOX_SERIAL_X  (5)
#define BOX_SERIAL_Y  (BTN_CHANGE_Y + BTN_CHANGE_H + 5)
#define BOX_SERIAL_W  (100)
#define BOX_SERIAL_H  (25)

#define INPUT_SERIAL_X  (BOX_SERIAL_X + BOX_SERIAL_W + 5)
#define INPUT_SERIAL_Y  (BOX_SERIAL_Y)
#define INPUT_SERIAL_W  (w - 5 - INPUT_SERIAL_X)
#define INPUT_SERIAL_H  (BOX_SERIAL_H)

#define BOX_NAME_X  (5)
#define BOX_NAME_Y  (BOX_SERIAL_Y + BOX_SERIAL_H + 5)
#define BOX_NAME_W  (100)
#define BOX_NAME_H  (25)

#define INPUT_NAME_X  (BOX_NAME_X + BOX_NAME_W + 5)
#define INPUT_NAME_Y  (BOX_NAME_Y)
#define INPUT_NAME_W  (w - 5 - INPUT_NAME_X)
#define INPUT_NAME_H  (BOX_NAME_H)

#define BOX_SUMMARY_X  (5)
#define BOX_SUMMARY_Y  (BOX_NAME_Y + BOX_NAME_H + 5)
#define BOX_SUMMARY_W  (100)
#define BOX_SUMMARY_H  (25)

#define EDIT_SUMMARY_X  (BOX_SUMMARY_X + BOX_SUMMARY_W + 5)
#define EDIT_SUMMARY_Y  (BOX_SUMMARY_Y)
#define EDIT_SUMMARY_W  (w - 5 - EDIT_SUMMARY_X)
#define EDIT_SUMMARY_H  (200)

#define BOX_PRICE_H  (25)
#define BOX_PRICE_X  (5)
#define BOX_PRICE_Y  (h - 5 - BOX_PRICE_H)
#define BOX_PRICE_W  (100)

#define INPUT_PRICE_X  (BOX_PRICE_X + BOX_PRICE_W + 5)
#define INPUT_PRICE_Y  (BOX_PRICE_Y)
#define INPUT_PRICE_W  (w - 5 - INPUT_PRICE_X)
#define INPUT_PRICE_H  (BOX_PRICE_H)

MainWindow::MainWindow(int w, int h, const char *title) : Fl_Window(w, h, title),
					m_btn_change(BTN_CHANGE_X, BTN_CHANGE_Y, BTN_CHANGE_W, BTN_CHANGE_H, "Change"),
					m_btn_load(BTN_LOAD_X, BTN_LOAD_Y, BTN_LOAD_W, BTN_LOAD_H, "Load"),
					m_box_serial(BOX_SERIAL_X, BOX_SERIAL_Y, BOX_SERIAL_W, BOX_SERIAL_H, "serial:"),
					m_input_serial(INPUT_SERIAL_X, INPUT_SERIAL_Y, INPUT_SERIAL_W, INPUT_SERIAL_H),
					m_box_name(BOX_NAME_X, BOX_NAME_Y, BOX_NAME_W, BOX_NAME_H, "name:"),
					m_input_name(INPUT_NAME_X, INPUT_NAME_Y, INPUT_NAME_W, INPUT_NAME_H),
					m_box_summary(BOX_SUMMARY_X, BOX_SUMMARY_Y, BOX_SUMMARY_W, BOX_SUMMARY_H, "summary:"),
					m_editor_summary(EDIT_SUMMARY_X, EDIT_SUMMARY_Y, EDIT_SUMMARY_W, EDIT_SUMMARY_H),
					m_box_price(BOX_PRICE_X, BOX_PRICE_Y, BOX_PRICE_W, BOX_PRICE_H, "price:"),
					m_input_price(INPUT_PRICE_X, INPUT_PRICE_Y, INPUT_PRICE_W, INPUT_PRICE_H)
{
	end();

	m_btn_change.callback(&btn_change_cb, this);
	m_btn_load.callback(&btn_load_cb, this);

	m_editor_summary.buffer(m_buffer_summary);

	//properties
	m_string_serial = NULL;
	m_string_name = NULL;
	m_string_summary = NULL;
	m_string_price = NULL;

	//commands
	m_change_price_command = NULL;
	m_load_command = NULL;
}

//methods

void MainWindow::update_ui()
{
	assert( m_string_serial != NULL );
	assert( m_string_name != NULL );
	assert( m_string_summary != NULL );
	assert( m_string_price != NULL );
	m_input_serial.value(m_string_serial->c_str());
	m_input_name.value(m_string_name->c_str());
	m_buffer_summary.text(m_string_summary->c_str());
	m_input_price.value(m_string_price->c_str());
}

//callbacks

void MainWindow::btn_change_cb(Fl_Widget *, void *pv)
{
	MainWindow *pThis = (MainWindow *)pv;
	assert( pThis->m_change_price_command != NULL );
	PriceParameter param;
	param.v = 20.0;
	pThis->m_change_price_command->exec(&param);
}
void MainWindow::btn_load_cb(Fl_Widget *, void *pv)
{
	MainWindow *pThis = (MainWindow *)pv;
	assert( pThis->m_load_command != NULL );

	Fl_Native_File_Chooser dlg;
	dlg.title("Pick a txt file");
	dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
	dlg.filter("TXT\t*.txt");
	if (dlg.show() == 0) {
		LoadParameter param;
		param.v = dlg.filename();
		if (pThis->m_load_command->exec(&param) != 0) {
			fl_alert("Cannot open file!");
			return ;
		}
	}
}

//notification

void MainWindow::notification_cb(uint32_t id, void *p)
{
	MainWindow *pThis = (MainWindow *)p;
	switch (id) {
	case PROP_ID_BOOK_SERIAL:
		pThis->m_input_serial.value(pThis->m_string_serial->c_str());
		break;
	case PROP_ID_BOOK_NAME:
		pThis->m_input_name.value(pThis->m_string_name->c_str());
		break;
	case PROP_ID_BOOK_SUMMARY:
		pThis->m_buffer_summary.text(pThis->m_string_summary->c_str());
		break;
	case PROP_ID_BOOK_PRICE:
		pThis->m_input_price.value(pThis->m_string_price->c_str());
		break;
	default:
		break;
	}
}
