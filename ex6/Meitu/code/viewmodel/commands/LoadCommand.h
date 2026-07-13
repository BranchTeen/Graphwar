
#ifndef __LOAD_COMMAND_H__
#define __LOAD_COMMAND_H__

#include "../../common/command_base.h"

template <class T>
class LoadCommand : public ICommandBase
{
public:
	LoadCommand() noexcept
	{
	}
	LoadCommand(const LoadCommand&) = delete;
	~LoadCommand() noexcept
	{
	}

	LoadCommand& operator=(const LoadCommand&) = delete;

//overriders
	int exec(const std::any& v) override
	{
		T* pT = static_cast<T*>(this);
		if (!pT->get_image_model()->load(std::any_cast<const char*>(v)))
			return 0;
		return 1;
	}
};

#endif
