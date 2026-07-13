
#ifndef __IMAGE_DATA_H__
#define __IMAGE_DATA_H__

#pragma pack(push, 1)

struct GrayPixel
{
	uint8_t g;
};
struct Gray2Pixel
{
	uint8_t g;
	uint8_t a;
};
struct ColorPixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};
struct Color2Pixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

#pragma pack(pop)

template <class T>
class ImageDataT
{
public:
	ImageDataT() noexcept : m_w(0), m_h(0)
	{
	}
	ImageDataT(const ImageDataT& src) : m_w(0), m_h(0)
	{
		copy_from(src);
	}
	ImageDataT(ImageDataT&& src) noexcept : m_buffer(std::forward(src.m_buffer)), m_w(src.m_w), m_h(src.m_h)
	{
		src.m_w = src.m_h = 0;
	}		
	~ImageDataT() noexcept
	{
	}

	ImageDataT& operator=(const ImageDataT& src)
	{
		copy_from(src);
		return *this;
	}
	ImageDataT& operator=(ImageDataT&& src) noexcept
	{
		//optimize
		assert( this != &src );
		m_buffer = std::forward(src.m_buffer);
		m_w = src.m_w;
		m_h = src.m_h;
		src.m_w = src.m_h = 0;
		return *this;
	}

	void clear() noexcept
	{
		m_buffer.clear();
		m_w = m_h = 0;
	}

	bool is_null() const noexcept
	{
		return m_buffer.empty();
	}

	const T *get_address() const noexcept
	{
		return m_buffer.data();
	}
	T *get_address() noexcept
	{
		return m_buffer.data();
	}

	size_t get_width() const noexcept
	{
		return m_w;
	}
	size_t get_height() const noexcept
	{
		return m_h;
	}

	void set_size(size_t w, size_t h)
	{
		assert( w > 0 && h > 0 );
		if ((std::numeric_limits<size_t>::max)() / w < h)
			throw std::overflow_error("Overflow!");
		std::vector<T> vec(w * h);
		//optimize
		m_buffer.swap(vec);
		m_w = w;
		m_h = h;
	}

	//copy
	void copy_from(const ImageDataT<T>& src)
	{
		//optimize
		assert( this != &src );
		if (src.is_null()) {
			clear();
			return ;
		}
		set_size(src.get_width(), src.get_height());
		::memcpy(get_address(), src.get_address(), m_w * m_h * sizeof(T));
	}

private:
	std::vector<T> m_buffer;  //gray array
	size_t m_w, m_h;
};

#endif
