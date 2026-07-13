
#include "app/AirApp.h"

int main(int argc, char **argv)
{
	std::unique_ptr<AirApp> app(std::make_unique<AirApp>());

	if (!app->initialize()) {
		fl_alert("Initialization failed!");
		return 1;
	}

	app->show_main_window();

	return Fl::run();
}
