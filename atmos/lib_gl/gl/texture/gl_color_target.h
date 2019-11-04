
#pragma once

namespace igpu
{
	class gl_render_target
	{
	public:
		virtual void attach() const = 0;

		virtual ~gl_render_target() = default;

	protected:
		gl_render_target() = default;
		gl_render_target( const gl_render_target& ) = delete;
		gl_render_target& operator=( const gl_render_target& ) = delete;
	};
}
