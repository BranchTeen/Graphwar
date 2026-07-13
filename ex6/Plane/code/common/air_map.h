
#ifndef __AIR_MAP_H__
#define __AIR_MAP_H__

enum {
	ACTOR_PLANE_RED = 1,
	ACTOR_PLANE_BLUE,
	ACTOR_BULLET
};

struct Actor
{
	int type;
	int x, y;
};

class AirMap
{
public:
	AirMap() noexcept
	{
	}
	AirMap(const AirMap&) = delete;
	~AirMap() noexcept
	{
	}

	AirMap& operator=(const AirMap&) = delete;

	void clear() noexcept
	{
		m_map.clear();
	}

	size_t get_size() const noexcept
	{
		return m_map.size();
	}

	const Actor& get_at(size_t idx) const noexcept
	{
		return m_map[idx];
	}
	Actor& get_at(size_t idx) noexcept
	{
		return m_map[idx];
	}

	void set_size(size_t size)
	{
		m_map.resize(size);
	}

	void append(const Actor& v)
	{
		m_map.push_back(v);
	}
	void insert(size_t idx, const Actor& v)
	{
		m_map.insert(m_map.begin() + idx, v);
	}
	void remove(size_t idx) noexcept
	{
		m_map.erase(m_map.begin() + idx);
	}

private:
	std::vector<Actor> m_map;
};

#endif
