
#include <igpu/material/material.h>

#include <framework/logging/log.h>

#include <algorithm>

using namespace igpu;

const material::config& material::cfg() const
{
	return _cfg;
}

material::material(config cfg, std::vector<primitive> primitives)
	: _cfg(std::move(cfg))
{}