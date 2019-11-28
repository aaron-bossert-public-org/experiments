
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_shader.h"

#include "igpu/compute/compute_shader.h"

#include <memory>

namespace igpu
{
	class vulkan_compute_shader
		: public compute_shader
		, public vulkan_shader
	{
	public:
		static std::unique_ptr< vulkan_compute_shader > make(
			const vulkan&,
			std::vector< uint32_t >&& );
	};
}
