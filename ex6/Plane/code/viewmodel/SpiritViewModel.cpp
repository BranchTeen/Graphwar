
#include "SpiritViewModel.h"

//methods

bool SpiritViewModel::initialize()
{
	char buffer[FL_PATH_MAX];
	fl_getcwd(buffer, FL_PATH_MAX);
	std::string str(buffer);
	if (str.back() != '/' && str.back() != '\\')
		str += '/';
	str += "assets/";
	m_up_plane_red = std::make_unique<Fl_PNG_Image>((str + "plane_red.png").c_str());
	if (m_up_plane_red->fail())
		return false;
	m_up_plane_blue = std::make_unique<Fl_PNG_Image>((str + "plane_blue.png").c_str());
	if (m_up_plane_blue->fail())
		return false;
	m_up_bullet = std::make_unique<Fl_PNG_Image>((str + "bullet.png").c_str());
	if (m_up_bullet->fail())
		return false;
	return true;
}
