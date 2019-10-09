
#include <igpu/context/parameter_constraints.h>

#include <framework/logging/log.h>

#include <igpu/batch/batch_parameters.h>

using namespace igpu;

parameter_constraints::parameter_constraints(const config& cfg)
	: _cfg (cfg)
{
	for (size_t i = 0; i < _cfg.parameters.size(); ++i)
	{
		_lookup[_cfg.parameters[i].first] = i;
	}
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
