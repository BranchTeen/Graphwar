
#ifndef __FRAME_H__
#define __FRAME_H__

class IPropertyNotification
{
public:
	virtual void on_property_changed(uint32_t id) = 0;
};

class PropertyTrigger
{
public:
	PropertyTrigger() noexcept
	{
	}
	PropertyTrigger(const PropertyTrigger&) = delete;
	~PropertyTrigger() noexcept
	{
	}

	PropertyTrigger& operator=(const PropertyTrigger&) = delete;

	void clear() noexcept
	{
		m_vec_nf.clear();
	}

	uintptr_t add(IPropertyNotification *pn);
	void remove(uintptr_t cookie) noexcept
	{
		assert( cookie > 0 && cookie <= m_vec_nf.size() );
		m_vec_nf[cookie - 1] = nullptr;
	}

	void fire(uint32_t id);

private:
	std::vector<IPropertyNotification *> m_vec_nf;
};

#endif
