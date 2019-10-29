
#include <igpu/batch/batch_utility.h>

#include <igpu/buffer/geometry.h>
#include <igpu/material/material.h>
#include <igpu/material/render_states.h>

using namespace igpu;

void batch_utility::get_token(const batch_binding::config& cfg, const program** program)
{
	*program = cfg.program.get();
}

void batch_utility::get_token(const batch_binding::config& cfg, const render_states** render_states)
{
	*render_states = cfg.render_states.get();
}

void batch_utility::get_token(const batch_binding::config& cfg, const material** material)
{
	*material = cfg.material.get();
}

void batch_utility::get_token(const batch_binding::config& cfg, const geometry** geometry)
{
	*geometry = cfg.geometry.get();
}
