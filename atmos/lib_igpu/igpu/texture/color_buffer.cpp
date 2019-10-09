
#include <igpu/texture/color_buffer.h>

#include <framework/logging/log.h>

using namespace igpu;

const color_buffer::config& color_buffer::cfg() const
{
	return _cfg;
}

const glm::ivec2& color_buffer::resolution() const
{
	return _cfg.res;
}

color_buffer::color_buffer(const config& cfg)
	: _cfg(cfg)
{
}
