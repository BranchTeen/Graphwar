
#ifndef __COMMAND_BASE_H__
#define __COMMAND_BASE_H__

class ICommandParameter
{
public:
	virtual ~ICommandParameter() noexcept
	{
	}
};

class ICommandBase
{
public:
	virtual int exec(ICommandParameter *p) = 0;
};

#endif
