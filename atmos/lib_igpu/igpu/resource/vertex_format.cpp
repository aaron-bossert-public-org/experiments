
#pragma once

#include <igpu/resource/vertex_format.h>

using namespace igpu;

const vertex_format::config& vertex_format::cfg() const
{
	return _cfg;
}

vertex_format::vertex_format(const config& cfg)
	: _cfg(cfg)
{
}

