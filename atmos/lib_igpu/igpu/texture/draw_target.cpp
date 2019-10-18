
#include <igpu/texture/draw_target.h>

#include <igpu/texture/color_target.h>
#include <igpu/texture/depth_target.h>

#include <glm/common.hpp>

using namespace igpu;

const std::shared_ptr<color_target>& draw_target::color() const
{
    return _color;
}

const std::shared_ptr<depth_target>& draw_target::depth() const
{
    return _depth;
}

glm::ivec2 draw_target::res() const
{
    if(_color)
    {
        if(_depth)
        {
            return glm::min(_color->cfg().res, _depth->cfg().res);
        }
        else
        {
            return _color->cfg().res;
        }
    }
    else if(_depth)
    {
        return _depth->cfg().res;
    }
    
    return glm::ivec2(0);
}

draw_target::draw_target(const std::shared_ptr<color_target>& color,
                       const std::shared_ptr<depth_target>& depth)
: _color(color)
, _depth(depth)
{
}