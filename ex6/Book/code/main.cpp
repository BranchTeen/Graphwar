
#include "app/BookApp.h"

int main(int argc, char **argv)
{
	BookApp app;
	if (!app.initialize()) {
		fl_alert("Initialization failed!");
		return 1;
	}
	app.show_main_window();
	return Fl::run();
}
