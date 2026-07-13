
#include "../common/property_ids.h"
#include "../common/actions.h"

#include "GameViewModel.h"

constexpr int c_board_width = 500;
constexpr int c_board_height = 500;

GameViewModel::GameViewModel()
{
	m_map.append(Actor{ACTOR_PLANE_RED, c_board_width / 2, c_board_height - 50});
	m_map.append(Actor{ACTOR_PLANE_BLUE, c_board_width / 2, 50});
	m_map.append(Actor{ACTOR_BULLET, c_board_width / 2, c_board_height - 50 - 50});
}

//methods

void GameViewModel::next_step(int turn)
{
	//blue
	Actor* pa = &(m_map.get_at(1));
	int y = pa->y + 10;
	if (y > c_board_height - 50)
		y = c_board_height - 50;
	pa->y = y;
	//bullet
	pa = &(m_map.get_at(2));
	y = pa->y - 5;
	if (y < 5)
		y = 5;
	pa->y = y;
	//fire
	fire(PROP_ID_MAP);
}

void GameViewModel::move_red(int type)
{
	Actor* pa = &(m_map.get_at(0));
	int x = pa->x;
	int y = pa->y;
	switch (type) {
	case MOVE_LEFT:
		x -= 5;
		if (x < 0)
			x = 0;
		break;
	case MOVE_RIGHT:
		x += 5;
		if (x >= c_board_width)
			x = c_board_width - 1;
		break;
	case MOVE_UP:
		y -= 5;
		if (y < 0)
			y = 0;
		break;
	case MOVE_DOWN:
		y += 5;
		if (y >= c_board_height)
			y = c_board_height - 1;
		break;
	default:
		break;
	}
	pa->x = x;
	pa->y = y;
	//fire
	fire(PROP_ID_MAP);
}
