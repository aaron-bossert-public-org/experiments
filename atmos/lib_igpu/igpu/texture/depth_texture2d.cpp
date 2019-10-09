
#include <igpu/texture/depth_texture2d.h>

#include <framework/logging/log.h>

using namespace igpu;

const depth_texture2d::config& depth_texture2d::cfg() const
{
    return _cfg;
}

const glm::ivec2& depth_texture2d::resolution() const
{
    return _cfg.res;
}

depth_texture2d::depth_texture2d(const config& cfg)
: _cfg(cfg)
{
}