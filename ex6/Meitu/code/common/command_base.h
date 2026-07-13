
#ifndef __COMMAND_BASE_H__
#define __COMMAND_BASE_H__

class ICommandBase
{
public:
	virtual int exec(const std::any& v) = 0;
};

#endif
