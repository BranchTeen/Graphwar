
#ifndef __IMAGE_CONVERT_H__
#define __IMAGE_CONVERT_H__

class ImageConvertHelper
{
public:
	static void fill_image_data(const Fl_RGB_Image &img, void *pdata) noexcept;
	static Fl_RGB_Image *to_fl_image(const void *pdata, size_t w, size_t h, size_t depth);
};

#endif
