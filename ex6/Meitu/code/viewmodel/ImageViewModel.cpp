
#include "../common/property_ids.h"

#include "ImageViewModel.h"

#include "../model/base/image_convert.h"

ImageViewModel::ImageViewModel() noexcept : m_image_model(nullptr), m_pimage(nullptr)
{
}
ImageViewModel::~ImageViewModel() noexcept
{
	delete m_pimage;
}

//overriders

void ImageViewModel::on_property_changed(uint32_t id)
{
	switch (id) {
	case PROP_ID_IMAGE:
		{
			const ImageDataT<ColorPixel> *pd = m_image_model->get_data();
			/*
			//convert image data
			delete m_pimage;
			m_pimage = ImageConvertHelper::to_fl_image(pd->get_address(), pd->get_width(), pd->get_height(), sizeof(ColorPixel));
			*/
			convert_image_data(pd);
		}
		break;
	default:
		break;
	}

	m_trigger.fire(id);
}

//tools

void ImageViewModel::convert_image_data(const ImageDataT<ColorPixel> *pd)
{
	delete m_pimage;
	m_pimage = ImageConvertHelper::to_fl_image(pd->get_address(), pd->get_width(), pd->get_height(), sizeof(ColorPixel));
}
