
#include "app/PrettyApp.h"

int main(int argc, char **argv)
{
	PrettyApp app;
	app.show_main_window();
	return Fl::run();
}
