
#pragma once

#include "igpu/buffer/compute_buffer.h"

namespace igpu
{
	class gl_compute_buffer : public compute_buffer
	{
	public:
		virtual unsigned gl_handle() const = 0;

		static std::unique_ptr< gl_compute_buffer > make( const config& );

	protected:
		gl_compute_buffer() = default;
	};
}