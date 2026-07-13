
#ifndef __SPIRIT_VIEW_MODEL_H__
#define __SPIRIT_VIEW_MODEL_H__

class SpiritViewModel
{
public:
	SpiritViewModel() noexcept
	{
	}
	SpiritViewModel(const SpiritViewModel&) = delete;
	~SpiritViewModel() noexcept
	{
	}

	SpiritViewModel& operator=(const SpiritViewModel&) = delete;

//properties
	const std::unique_ptr<Fl_PNG_Image>* get_plane_red() const noexcept
	{
		return &m_up_plane_red;
	}
	const std::unique_ptr<Fl_PNG_Image>* get_plane_blue() const noexcept
	{
		return &m_up_plane_blue;
	}
	const std::unique_ptr<Fl_PNG_Image>* get_bullet() const noexcept
	{
		return &m_up_bullet;
	}

//methods
	bool initialize();

private:
//properties
	std::unique_ptr<Fl_PNG_Image> m_up_plane_red;
	std::unique_ptr<Fl_PNG_Image> m_up_plane_blue;
	std::unique_ptr<Fl_PNG_Image> m_up_bullet;
};

#endif
