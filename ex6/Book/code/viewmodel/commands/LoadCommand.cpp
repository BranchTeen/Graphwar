
#include "LoadCommand.h"

#include "../BookViewModel.h"

#include "../../common/command_parameters.h"

int LoadCommand::exec(ICommandParameter *p)
{
	LoadParameter *param = dynamic_cast<LoadParameter *>(p);
	assert( param != NULL );
	return m_pvm->get_info_model()->load(param->v) ? 0 : -1;
}
