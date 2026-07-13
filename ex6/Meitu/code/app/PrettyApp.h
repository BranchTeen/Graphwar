
#ifndef __PRETTY_APP_H__
#define __PRETTY_APP_H__

#include "../model/ImageModel.h"
#include "../viewmodel/ImageViewModel.h"
#include "../view/MainWindow.h"

class PrettyApp
{
public:
	PrettyApp();
	PrettyApp(const PrettyApp&) = delete;
	~PrettyApp() noexcept
	{
	}

	PrettyApp& operator=(const PrettyApp&) = delete;

	void show_main_window()
	{
		m_main_wnd.show();
	}

private:
	MainWindow m_main_wnd;
	ImageViewModel m_image_viewmodel;
	ImageModel m_image_model;
};

#endif
