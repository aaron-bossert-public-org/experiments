
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/constant_parameter.h"

#include <vector>

namespace igpu
{
	class vulkan_specialization_constants
	{
	public:
		vulkan_specialization_constants(
			const std::vector< constant_parameter::config >& constants,
			shader_stages );

		const VkSpecializationInfo& info() const;

	private:
		std::vector< char > _data;
		std::vector< VkSpecializationMapEntry > _entries;
		VkSpecializationInfo _info = {};
	};
}
