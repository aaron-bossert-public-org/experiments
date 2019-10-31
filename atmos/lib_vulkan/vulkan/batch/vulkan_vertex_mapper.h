
#pragma once

#include <vulkan/defines/vulkan_includes.h>

#include <array>
#include <optional>

namespace igpu
{	
	class vulkan_geometry;
	class vulkan_program;

	class vulkan_vertex_mapper
	{
	public:

		vulkan_vertex_mapper(
			const vulkan_program&,
			const vulkan_geometry&);

		const VkPipelineVertexInputStateCreateInfo* results() const;

	private:

		std::optional<VkPipelineVertexInputStateCreateInfo> _results;

		// assuming max is 16 buffers, and combined max attributes is 16 (if each buffer passed one attribute in)
		// vertex buffers can contain more than that, but programs will only beable to referecce a subset for a draw call.
		std::array< VkVertexInputBindingDescription, 16 > _bindings;
		std::array< VkVertexInputAttributeDescription, 16 > _attributes;
		
		vulkan_vertex_mapper(const vulkan_vertex_mapper&) = delete;
		vulkan_vertex_mapper& operator= (const vulkan_vertex_mapper&) = delete;
	};
}
