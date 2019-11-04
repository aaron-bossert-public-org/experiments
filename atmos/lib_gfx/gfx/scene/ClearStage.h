
#pragma once

#include "gfx/scene/scene_stage.h"

namespace gfx
{
	class clear_stage : public scene_stage
	{
		pb::draw_target_clearing _config;

		void on_render() override;

		clear_stage( const pb::draw_target_clearing& );

	public:
		void config( const pb::draw_target_clearing& );

		const pb::draw_target_clearing& config();

		static std::shared_ptr< clear_stage > make(
			const pb::draw_target_clearing& );
	};
}
