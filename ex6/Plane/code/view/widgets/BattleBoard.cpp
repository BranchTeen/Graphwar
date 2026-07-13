
#include "../../common/actions.h"

#include "BattleBoard.h"

#define BOX_PLANE_X  (x + 10)
#define BOX_PLANE_Y  (y + 10)
#define BOX_PLANE_W  (90)
#define BOX_PLANE_H  (90)

BattleBoard::BattleBoard(int x, int y, int w, int h, const char *l) : BaseClass(x, y, w, h, l),
					m_box_plane(BOX_PLANE_X, BOX_PLANE_Y, BOX_PLANE_W, BOX_PLANE_H)
{
	end();

	box(FL_NO_BOX);

	m_box_plane.box(FL_NO_BOX);

	//properties
	m_pmap = nullptr;
	m_img_plane_red = nullptr;
	m_img_plane_blue = nullptr;
	m_img_bullet = nullptr;
}

BattleBoard::~BattleBoard() noexcept
{
}

void BattleBoard::draw()
{
	fl_rectf(x(), y(), w(), h(), FL_CYAN);
	if (m_pmap != nullptr) {
		for (size_t i = 0; i < m_pmap->get_size(); i ++) {
			const auto& actor(m_pmap->get_at(i));
			switch (actor.type) {
			case ACTOR_PLANE_RED:
				m_box_plane.position(x() + actor.x, y() + actor.y);
				break;
			case ACTOR_PLANE_BLUE:
				(*m_img_plane_blue)->draw(x() + actor.x, y() + actor.y);
				break;
			case ACTOR_BULLET:
				(*m_img_bullet)->draw(x() + actor.x, y() + actor.y);
				break;
			default:
				break;
			}
		}
	}
	//base
	BaseClass::draw_children();
}

int BattleBoard::handle(int event)
{
	int ret = 1;

	switch (event) {
	case FL_FOCUS:
		break;
	case FL_KEYDOWN:
		{
			int key = Fl::event_key();
			int type = MOVE_NONE;
			switch (key) {
			case FL_Left:
				type = MOVE_LEFT;
				break;
			case FL_Right:
				type = MOVE_RIGHT;
				break;
			case FL_Up:
				type = MOVE_UP;
				break;
			case FL_Down:
				type = MOVE_DOWN;
				break;
			default:
				ret = 0;
				break;
			}
			if (type != MOVE_NONE && m_move_red_command != nullptr)
				m_move_red_command(type);
		} //end block
		break;
	default:
		ret = 0;
		break;
	}

	if (ret == 0)
		return Fl_Group::handle(event);
	return ret;
}
