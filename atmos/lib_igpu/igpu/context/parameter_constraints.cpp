
#include "igpu/context/parameter_constraints.h"

#include "framework/logging/log.h"

using namespace igpu;

parameter_constraints::parameter_constraints(const config& cfg)
	: _cfg (cfg)
{
}

const parameter_constraints::config& parameter_constraints::cfg() const
{
	return _cfg;
}

size_t parameter_constraints::index_of(const std::string_view& name) const
{
	auto find = _lookup.find(name);
	if (find != _lookup.end())
	{
		return find->second;
	}

	return (size_t)-1;
}
