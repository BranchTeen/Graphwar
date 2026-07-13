
#include "image_convert.h"

void ImageConvertHelper::fill_image_data(const Fl_RGB_Image &img, void *pdata) noexcept
{
	int w  = img.w();
	int h  = img.h();
	int d  = img.d();
	int ld = img.ld();
	if ( ld == 0 )
		ld = w * d;
	int wd = w * d;

	uint8_t *pd = (uint8_t *)pdata;
	const uint8_t *ps = img.array;
	for (int i = 0; i < h; i ++) {
		const uint8_t *psr = ps;
		uint8_t *pdr = pd;
		for ( int j = 0; j < w; j ++ ) {
			::memcpy(pdr, psr, d);
			psr += d;
			pdr += d;
		}
		ps += ld;
		pd += wd;
	}
}

Fl_RGB_Image *ImageConvertHelper::to_fl_image(const void *pdata, size_t w, size_t h, size_t depth)
{
	assert( w <= (size_t)((std::numeric_limits<int>::max)()) && h <= (size_t)((std::numeric_limits<int>::max)()) );
	assert( depth <= (size_t)((std::numeric_limits<int>::max)()) );
	return new Fl_RGB_Image((const uchar*)pdata, (int)w, (int)h, (int)depth, 0);
}
