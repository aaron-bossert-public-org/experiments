
#include <igpu/context/vertex_constraints.h>

#include <framework/logging/log.h>

using namespace igpu;

vertex_constraints::vertex_constraints(const config& cfg)
	: _cfg (cfg)
{
	for (size_t i = 0; i < _cfg.vertex_parameters.size(); ++i)
	{
		_locations[_cfg.vertex_parameters[i].name] = i;
	}
}

const vertex_constraints::config& vertex_constraints::cfg() const
{
	return _cfg;
}

const vertex_parameter& vertex_constraints::find_parameter(const std::string& name, size_t fallback) const
{
	auto location = find_location(name, fallback);
	if (-1 < location)
	{
		return _cfg.vertex_parameters[location];
	}

	static const vertex_parameter notFound;
	return notFound;
}

size_t vertex_constraints::find_location(const std::string& name, size_t fallback) const
{
	auto find = _locations.find(name);
	if (find != _locations.end())
	{
		return find->second;
	}
	return fallback;
}
