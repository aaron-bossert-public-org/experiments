
#pragma once

#include <igpu/buffer/compute_buffer.h>
#include <gl/buffer/gl_buffer.h>

namespace igpu
{
	class gl_compute_buffer : public gl_buffer_t < compute_buffer >
	{
	public:

		static std::unique_ptr<gl_compute_buffer> make(const config&);

	protected:

		gl_compute_buffer(const config&);
	};
}