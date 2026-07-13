
#ifndef __GAME_VIEW_MODEL_H__
#define __GAME_VIEW_MODEL_H__

#include "../common/frame.h"
#include "../common/air_map.h"

class GameViewModel : public PropertyTrigger
{
public:
	GameViewModel();
	GameViewModel(const GameViewModel&) = delete;
	~GameViewModel() noexcept
	{
	}

	GameViewModel& operator=(const GameViewModel&) = delete;

//properties
	const AirMap* get_map() const noexcept
	{
		return &m_map;
	}

//commands
	std::function<void(int)> get_next_step_command();
	std::function<void(int)> get_move_red_command();

//methods
	void next_step(int turn);

private:
	void move_red(int type);

private:
//properties
	AirMap m_map;
};

#endif
