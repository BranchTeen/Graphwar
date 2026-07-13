
#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "../common/command_base.h"
#include "../common/frame.h"

class MainWindow : public Fl_Window
{
public:
	MainWindow(int w, int h, const char *title = NULL);
	MainWindow(const MainWindow&) = delete;
	~MainWindow() noexcept
	{
		m_editor_summary.buffer(NULL);
	}

	MainWindow& operator=(const MainWindow&) = delete;

//properties
	void set_serial(const std::string *p) noexcept
	{
		m_string_serial = p;
	}
	void set_name(const std::string *p) noexcept
	{
		m_string_name = p;
	}
	void set_summary(const std::string *p) noexcept
	{
		m_string_summary = p;
	}
	void set_price(const std::string *p) noexcept
	{
		m_string_price = p;
	}

//commands
	void set_change_price_command(ICommandBase *p) noexcept
	{
		m_change_price_command = p;
	}
	void set_load_command(ICommandBase *p) noexcept
	{
		m_load_command = p;
	}

//notification
	PropertyNotification get_notification() const noexcept
	{
		return &notification_cb;
	}

//methods
	void update_ui();

private:
//callbacks
	static void btn_change_cb(Fl_Widget *, void *);
	static void btn_load_cb(Fl_Widget *, void *);

//notification
	static void notification_cb(uint32_t id, void *p);

private:
	Fl_Button m_btn_change;
	Fl_Button m_btn_load;
	Fl_Box m_box_serial;
	Fl_Input m_input_serial;
	Fl_Box m_box_name;
	Fl_Input m_input_name;
	Fl_Box m_box_summary;
	Fl_Text_Editor m_editor_summary;
	Fl_Box m_box_price;
	Fl_Input m_input_price;

	//for Fl_Text_Editor
	Fl_Text_Buffer m_buffer_summary;

	//properties
	const std::string* m_string_serial;
	const std::string* m_string_name;
	const std::string* m_string_summary;
	const std::string* m_string_price;

	//commands
	ICommandBase* m_change_price_command;
	ICommandBase* m_load_command;
};

#endif
