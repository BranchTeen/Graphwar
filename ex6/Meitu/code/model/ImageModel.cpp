
#include "../common/property_ids.h"

#include "base/image_convert.h"

#include "ImageModel.h"

bool ImageModel::load(const char *file)
{
	Fl_PNG_Image *pimg = new Fl_PNG_Image(file);
	if (pimg->fail()) {
		delete pimg;
		return false;
	}
	if (pimg->d() != 3) {
		delete pimg;
		return false;
	}

	m_data.set_size(pimg->w(), pimg->h());
	ImageConvertHelper::fill_image_data(*pimg, m_data.get_address());
	delete pimg;

	m_trigger.fire(PROP_ID_IMAGE);

	return true;
}

void ImageModel::gray()
{
	if (m_data.is_null())
		return ;
	size_t w = m_data.get_width();
	size_t h = m_data.get_height();
	ColorPixel* pc = m_data.get_address();
	for (size_t i = 0; i < h; ++ i) {
		for (size_t j = 0; j < w; ++ j) {
			uint8_t v = (uint8_t)((299 * (uint32_t)(pc->r) + 587 * (uint32_t)(pc->g) + 114 * (uint32_t)(pc->b)) / 1000);
			pc->r = pc->g = pc->b = v;
			++ pc;
		}
	}
	m_trigger.fire(PROP_ID_IMAGE);
}
