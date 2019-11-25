
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_shader.h"

#include "igpu/shader/fragment_shader.h"

#include <memory>

namespace igpu
{
	class vulkan_fragment_shader
		: public fragment_shader
		, public vulkan_shader
	{
	public:
		static std::unique_ptr< vulkan_fragment_shader > make(
			const vulkan& vk,
			std::vector< uint32_t >&& );
	};
}
