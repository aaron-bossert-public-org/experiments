
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/attribute_indexer.h"

#include <array>

namespace igpu
{
	class vulkan_geometry;
	class vulkan_vertex_parameters;

	class vulkan_attributes_decriptor
	{
	public:
		using binding_descriptions_t = std::array<
			VkVertexInputBindingDescription,
			vertex_parameters::MAX_COUNT >;

		using attribute_descriptions_t = std::array<
			VkVertexInputAttributeDescription,
			vertex_parameters::MAX_COUNT >;

		[[nodiscard]] bool reset(
			const vulkan_vertex_parameters&,
			const vulkan_geometry& );

		const attribute_indexer& indexer() const;

		uint32_t binding_description_count() const;

		const VkPipelineVertexInputStateCreateInfo& vertex_input_info() const;

		const VkPipelineInputAssemblyStateCreateInfo& input_assembly() const;

		const binding_descriptions_t& binding_descriptions() const;

		const attribute_descriptions_t& attribute_descriptions() const;

		const vulkan_vertex_parameters* vertex_parameters() const;

		const vulkan_geometry* geometry() const;

	private:
		attribute_indexer _indexer;

		const vulkan_vertex_parameters* _vertex_parameters;
		const vulkan_geometry* _geometry;

		VkPipelineVertexInputStateCreateInfo _vertex_input_info;
		VkPipelineInputAssemblyStateCreateInfo _input_assembly;

		binding_descriptions_t _binding_descriptions;
		attribute_descriptions_t _attribute_descriptions;
	};
}
