
#pragma once

namespace igpu
{
	class gl_depth_target
	{
	public:
		virtual void attach() const = 0;

		virtual ~gl_depth_target() = default;

	protected:
		gl_depth_target() = default;
		gl_depth_target( const gl_depth_target& ) = delete;
		gl_depth_target& operator=( const gl_depth_target& ) = delete;
	};
}
