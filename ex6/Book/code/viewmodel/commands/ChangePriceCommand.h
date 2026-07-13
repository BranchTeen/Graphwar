
#ifndef __CHANGE_PRICE_COMMAND_H__
#define __CHANGE_PRICE_COMMAND_H__

#include "../../common/command_base.h"

class BookViewModel;

class ChangePriceCommand : public ICommandBase
{
public:
	ChangePriceCommand(BookViewModel *p) noexcept : m_pvm(p)
	{
	}
	ChangePriceCommand(const ChangePriceCommand&) = delete;
	~ChangePriceCommand() noexcept
	{
	}

	ChangePriceCommand& operator=(const ChangePriceCommand&) = delete;

//overriders
	virtual int exec(ICommandParameter *p);

private:
	BookViewModel *m_pvm;
};

#endif
