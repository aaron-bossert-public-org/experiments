
#pragma once

#include "igpu/buffer/buffer.h"
#include "igpu/texture/sampler.h"
#include "igpu/texture/texture_format.h"

#include "glm/vec2.hpp"

namespace igpu
{
	class texture2d : public buffer
	{
	public:
		struct config
		{
			std::string name;
			sampler sampler;
			bool can_auto_generate_mips = false;
			memory_type memory = memory_type::WRITE_COMBINED;
		};

		struct state
		{
			glm::ivec2 res = {};
			texture_format format = texture_format::UNDEFINED;
			size_t mipmap_count = 1;
		};

		virtual ~texture2d() = default;

		virtual void unmap( const state& ) = 0;

		virtual const config& cfg() const = 0;

		virtual const state& current_state() const = 0;

	protected:
		texture2d() = default;
		texture2d( const texture2d& ) = delete;
		texture2d& operator=( const texture2d& ) = delete;
	};
}
