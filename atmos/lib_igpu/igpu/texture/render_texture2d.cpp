
#include <igpu/texture/render_texture2d.h>

#include <framework/logging/log.h>

using namespace igpu;

const render_texture2d::config& render_texture2d::cfg() const
{
    return _cfg;
}

const glm::ivec2& render_texture2d::resolution() const
{
    return _cfg.res;
}

render_texture2d::render_texture2d(const config& cfg)
: _cfg(cfg)
{
}
