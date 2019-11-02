
#include <igpu/texture/draw_target.h>

#include <igpu/texture/render_buffer.h>
#include <igpu/texture/depth_buffer.h>

#include <glm/common.hpp>

using namespace igpu;


glm::ivec2 draw_target::res() const
{
	render_buffer* color = this->cfg().color.get();
	depth_buffer* depth = this->cfg().depth.get();

	if (!color)
	{
		return depth->cfg().res;
	}
	else if (!depth)
	{
		return color->cfg().res;
	}

	return glm::min(color->cfg().res, depth->cfg().res);
}
