
#ifndef __GRAY_COMMAND_H__
#define __GRAY_COMMAND_H__

#include "../../common/command_base.h"

template <class T>
class GrayCommand : public ICommandBase
{
public:
	GrayCommand() noexcept
	{
	}
	GrayCommand(const GrayCommand&) = delete;
	~GrayCommand() noexcept
	{
	}

	GrayCommand& operator=(const GrayCommand&) = delete;

//overriders
	int exec(const std::any& v) override
	{
		T* pT = static_cast<T*>(this);
		pT->get_image_model()->gray();
		return 0;
	}
};

#endif
