
#pragma once

#include "igpu/buffer/buffer.h"

#include "framework/meta_programming/va_macro.h"

#include <string>

namespace igpu
{
	ENUM_FLAGS_SERIALIZABLE(

		compute_buff_flag,
		DEFAULT( NOTHING ),

		( NOTHING, 0 ),
		( INDIRECT_DRAW, 1 << 0 ) );

	class compute_buffer : public buffer
	{
	public:
		struct config
		{
			compute_buff_flag flags = compute_buff_flag::NOTHING;
			memory_type memory = memory_type::WRITE_COMBINED;
		};

		virtual const config& cfg() const = 0;

		virtual void reset_gpu_only( size_t byte_size ) = 0;

		virtual ~compute_buffer() = default;
	};
}
