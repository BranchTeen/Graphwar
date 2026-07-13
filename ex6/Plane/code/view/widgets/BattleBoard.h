
#ifndef __BATTLE_BOARD_H__
#define __BATTLE_BOARD_H__

#include "../../common/air_map.h"

class BattleBoard : public Fl_Group
{
private:
	typedef Fl_Group  BaseClass;

public:
	BattleBoard(int x, int y, int w, int h, const char *l = nullptr);
	BattleBoard(const BattleBoard&) = delete;
	~BattleBoard() noexcept;

	BattleBoard& operator=(const BattleBoard&) = delete;

//properties
	void set_map(const AirMap* pmap) noexcept
	{
		m_pmap = pmap;
	}

	void set_plane_red(const std::unique_ptr<Fl_PNG_Image>* p) noexcept
	{
		m_img_plane_red = p;
		m_box_plane.image(m_img_plane_red->get());
	}
	void set_plane_blue(const std::unique_ptr<Fl_PNG_Image>* p) noexcept
	{
		m_img_plane_blue = p;
	}
	void set_bullet(const std::unique_ptr<Fl_PNG_Image>* p) noexcept
	{
		m_img_bullet = p;
	}

//commands
	void set_move_red_command(std::function<void(int)>&& pn) noexcept
	{
		m_move_red_command = std::move(pn);
	}

protected:
	void draw() override;
	int handle(int event) override;

private:
	Fl_Box m_box_plane;

private:
	//properties
	const AirMap* m_pmap;

	const std::unique_ptr<Fl_PNG_Image>* m_img_plane_red;
	const std::unique_ptr<Fl_PNG_Image>* m_img_plane_blue;
	const std::unique_ptr<Fl_PNG_Image>* m_img_bullet;

	//commands
	std::function<void(int)> m_move_red_command;
};

#endif
