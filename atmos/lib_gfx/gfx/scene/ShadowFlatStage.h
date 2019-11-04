
#pragma once

#include "gfx/scene/general_render_stage.h"

namespace gfx
{
	class shadow_flat_stage : public general_render_stage
	{
	public:
        using general_render_stage::general_render_stage;
        
        void on_render() override;
	};
}
