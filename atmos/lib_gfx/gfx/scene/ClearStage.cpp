
#include "igpu/utility/markers.h"

#include "gfx/scene/clear_stage.h"

using namespace gfx;

void clear_stage::on_render()
{
	ATMOS_MARK_FUNCTION;

	const auto& cm = _config.color();

	glm::vec4 color( cm.r(), cm.g(), cm.b(), cm.a() );
	float depth = _config.depth();
	uint8_t stencil = _config.stencil();

	_context.clear(
		_config.should_clear_color() ? &color : nullptr,
		_config.should_clear_depth() ? &depth : nullptr,
		_config.should_clear_stencil() ? &stencil : nullptr );
}

clear_stage::clear_stage( const pb::draw_target_clearing& config )
	: scene_stage( {} )
	, _config( config )
{}

void clear_stage::config( const pb::draw_target_clearing& config )
{
	_config = config;
}

const pb::draw_target_clearing& clear_stage::config()
{
	return _config;
}

std::shared_ptr< clear_stage > clear_stage::make(
	const pb::draw_target_clearing& config )
{
	return std::shared_ptr< clear_stage >( new clear_stage( config ) );
}
