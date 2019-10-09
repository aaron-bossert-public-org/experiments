
#include <igpu/material/parameter.h>

using namespace igpu;

parameter::parameter(const config& cfg, size_t constraint_index)
: _cfg(cfg)
, _constraint_index(constraint_index)
{}

const parameter::config& parameter::cfg() const
{
    return _cfg;
}


size_t parameter::constraint_index() const
{
	return _constraint_index;
}
