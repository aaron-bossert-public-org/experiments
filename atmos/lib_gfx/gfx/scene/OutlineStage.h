
#pragma once

#include <gfx/scene/scene_stage.h>
#include <gfx/scene/general_render_stage.h>

namespace gfx
{
	class outline_stage : public general_render_stage
	{
	public:
        using general_render_stage::general_render_stage;
        
        void on_render() override;
	};
}
