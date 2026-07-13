
#ifndef __IMAGE_MODEL_H__
#define __IMAGE_MODEL_H__

#include "../common/frame.h"

#include "base/image_data.h"

class ImageModel
{
public:
	ImageModel() noexcept
	{
	}
	ImageModel(const ImageModel&) = delete;
	~ImageModel() noexcept
	{
	}

	ImageModel& operator=(const ImageModel&) = delete;

	const ImageDataT<ColorPixel> *get_data() const noexcept
	{
		return &m_data;
	}

	bool load(const char *file);
	void gray();

	PropertyTrigger& get_trigger() noexcept
	{
		return m_trigger;
	}

private:
	//data
	ImageDataT<ColorPixel> m_data;

	PropertyTrigger m_trigger;
};

#endif
