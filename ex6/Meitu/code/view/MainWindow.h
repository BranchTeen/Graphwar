
#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "../common/command_base.h"
#include "../common/frame.h"

class MainWindow : public Fl_Window,
		public IPropertyNotification
{
public:
	MainWindow(int w, int h, const char *title = nullptr);
	MainWindow(const MainWindow&) = delete;
	~MainWindow() noexcept;

	MainWindow& operator=(const MainWindow&) = delete;

//properties
	void set_image(const Fl_RGB_Image*const* pp) noexcept
	{
		m_ppimage = pp;
	}

//commands
	void set_load_command(ICommandBase *p) noexcept
	{
		m_load_command = p;
	}
	void set_gray_command(ICommandBase *p) noexcept
	{
		m_gray_command = p;
	}

//notification
	IPropertyNotification* get_notification() noexcept
	{
		return static_cast<IPropertyNotification *>(this);
	}

protected:
//overriders
	void on_property_changed(uint32_t id) override;

private:
	//callbacks
	static void btn_load_cb(Fl_Widget *, void *);
	static void btn_gray_cb(Fl_Widget *, void *);

private:
	Fl_Button m_btn_load;
	Fl_Button m_btn_gray;
	Fl_Box m_box;

private:
//properties
	const Fl_RGB_Image*const* m_ppimage;

//commands
	ICommandBase *m_load_command;
	ICommandBase *m_gray_command;
};

#endif
