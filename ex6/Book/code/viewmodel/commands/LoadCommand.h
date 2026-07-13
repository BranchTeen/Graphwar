
#ifndef __LOAD_COMMAND_H__
#define __LOAD_COMMAND_H__

#include "../../common/command_base.h"

class BookViewModel;

class LoadCommand : public ICommandBase
{
public:
	LoadCommand(BookViewModel *p) noexcept : m_pvm(p)
	{
	}
	LoadCommand(const LoadCommand&) = delete;
	~LoadCommand() noexcept
	{
	}

	LoadCommand& operator=(const LoadCommand&) = delete;

//overriders
	virtual int exec(ICommandParameter *p);

private:
	BookViewModel *m_pvm;
};

#endif
