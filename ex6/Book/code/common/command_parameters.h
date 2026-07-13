
#ifndef __COMMAND_PARAMETERS_H__
#define __COMMAND_PARAMETERS_H__

#include "command_base.h"

template <typename T>
class TypeParameter : public ICommandParameter
{
public:
	T v;
};

typedef TypeParameter<double>  PriceParameter;
typedef TypeParameter<const char*>  LoadParameter;

#endif
