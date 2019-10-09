
#include <igpu/material/render_states.h>

#include <framework/logging/log.h>

using namespace igpu;

const render_states::config& render_states::cfg() const
{
	return _cfg;
}

render_states::render_states(const config& cfg)
: _cfg(cfg)
{
}
