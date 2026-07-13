
#include "ChangePriceCommand.h"

#include "../BookViewModel.h"

#include "../../common/command_parameters.h"

int ChangePriceCommand::exec(ICommandParameter *p)
{
	PriceParameter *param = dynamic_cast<PriceParameter *>(p);
	assert( param != NULL );
	m_pvm->get_info_model()->change_price(param->v);
	return 0;
}
