
#include <igpu/texture/texture2d.h>

using namespace igpu;

const texel_buffer2d::config& texture2d::cfg() const
{
    return _cfg;
}

size_t texture2d::size() const
{
    return _size;
}

texture2d::texture2d(
	const texel_buffer2d::config& cfg,
	const size_t size)
: _cfg(cfg)
, _size(size)
{
}

