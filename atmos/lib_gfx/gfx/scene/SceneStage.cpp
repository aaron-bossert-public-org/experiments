
#include <gfx/scene/scene_stage.h>

#include <zna/source/common/utility/stack_timer.h>

namespace
{
    enum active_flag
    {
        NOT_EMPTY,
        PREFIX_COMMANDS_NOT_EMPTY,
        POSTFIX_COMMANDS_NOT_EMPTY,
    };
}

using namespace gfx

scene_stage::scene_stage(const pb::scene_stage& msg)
: _msg(msg)
{
    this->empty(true);
}

scene_stage::binding_t scene_stage::make_binding(
    const scene_stage_binding::msg&,
    const std::shared_ptr<scene>&)
{
    LOG_CONTEXT( CRITICAL, "child class must override this function if it is used for drawing" );
    
    return scene_stage::binding_t();
}

const pb::scene_stage& scene_stage::msg() const
{
    return _msg;
}

void scene_stage::render(scene_parameters_manager& scene_parameters_manager, const utility::viewport& viewport)
{
	STACK_TIMER_PAIR("scene_stage::render", _msg.name())

		on_render();
}

void scene_stage::empty(bool empty)
{
    _empty = empty;
    
    update_is_active_flags(active_flag::NOT_EMPTY, !_empty);
}
