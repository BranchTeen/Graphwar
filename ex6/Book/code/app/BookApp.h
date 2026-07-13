
#ifndef __BOOK_APP_H__
#define __BOOK_APP_H__

#include "../model/InfoModel.h"
#include "../viewmodel/BookViewModel.h"
#include "../view/MainWindow.h"

class BookApp
{
public:
	BookApp();
	BookApp(const BookApp&) = delete;
	~BookApp() noexcept
	{
	}

	BookApp& operator=(const BookApp&) = delete;

	bool initialize();

	void show_main_window()
	{
		m_main_wnd.show();
	}

private:
	MainWindow m_main_wnd;
	std::shared_ptr<BookViewModel> m_sp_book_viewmodel;
	std::shared_ptr<InfoModel> m_sp_info_model;
};

#endif
