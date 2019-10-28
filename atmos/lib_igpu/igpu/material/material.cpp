
#include <igpu/material/material.h>

#include <framework/logging/log.h>

#include <algorithm>

using namespace igpu;

const material::config& material::cfg() const
{
	return _cfg;
}

const std::vector<primitive>& material::primitives() const
{
	return _primitives;
}

material::material(config cfg, std::vector<primitive> primitives)
	: _cfg(std::move(cfg))
	, _primitives(std::move(primitives))
{}