
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include <array>

namespace igpu
{	
	class vulkan_geometry;
	class vulkan_program;

	class vulkan_attribute_finder
	{
	public:

		bool find_all_attributes(
			const vulkan_program&,
			const vulkan_geometry&);

		uint32_t binding_description_count() const;
		
		const VkVertexInputBindingDescription* binding_descriptions() const;
		
		const VkVertexInputAttributeDescription* attribute_descriptions() const;

	private:

		uint32_t _binding_description_count = 0;
		std::array<VkVertexInputBindingDescription, 16> _binding_descriptions = {};
		std::array<VkVertexInputAttributeDescription, 16> _attribute_descriptions = {};
	};
}
