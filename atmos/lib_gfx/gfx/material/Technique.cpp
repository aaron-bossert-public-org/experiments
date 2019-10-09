
#include <gfx/material/technique.h>

using namespace gfx;

const std::string& technique::default_stage_name( bool alpha )
{
    static const std::string s_default_stage_names[2] = {
        "opaque",
        "transparent",
    };
    
    return s_default_stage_names[ alpha ];
}

technique::technique(const std::shared_ptr<program_instance>& program_instance,
                     size_t scene_stage_index,
                     std::string scene_stage_name)
: _program_instance(program_instance)
, _scene_stage_index(scene_stage_index)
, _scene_stage_name(std::move(scene_stage_name))
{
}

const std::shared_ptr<program_instance>& technique::program_instance() const
{
    return _program_instance;
}

size_t technique::scene_stage_index() const
{
    return _scene_stage_index;
}

const std::string& technique::scene_stage_name() const
{
    return _scene_stage_name;
}
