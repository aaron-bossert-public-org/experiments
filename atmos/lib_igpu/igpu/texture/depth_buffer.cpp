
#include <igpu/texture/depth_buffer.h>

#include <framework/logging/log.h>

using namespace igpu;

const depth_buffer::config& depth_buffer::cfg() const
{
    return _cfg;
}

const glm::ivec2& depth_buffer::resolution() const
{
    return _cfg.res;
}

depth_buffer::depth_buffer(const config& cfg)
: _cfg(cfg)
{
}
