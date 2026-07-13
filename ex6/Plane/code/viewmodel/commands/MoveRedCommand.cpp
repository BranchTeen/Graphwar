
#include "../GameViewModel.h"

std::function<void(int)> GameViewModel::get_move_red_command()
{
	return [this](int type)->void
		{
			this->move_red(type);
		};
}
