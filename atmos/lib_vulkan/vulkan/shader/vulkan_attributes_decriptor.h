
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
		bool reset( const vulkan_vertex_parameters&, const vulkan_geometry& );

		const attribute_indexer& indexer() const;

		uint32_t binding_description_count() const;

		const VkVertexInputBindingDescription* binding_descriptions() const;

		const VkVertexInputAttributeDescription* attribute_descriptions() const;

		const vulkan_vertex_parameters* vertex_parameters() const;

		const vulkan_geometry* geometry() const;

	private:
		attribute_indexer _indexer;

		const vulkan_vertex_parameters* _vertex_parameters;
		const vulkan_geometry* _geometry;

		uint32_t _binding_description_count;

		std::array<
			VkVertexInputBindingDescription,
			attribute_indexer::MAX_PARAMETERS >
			_binding_descriptions;

		std::array<
			VkVertexInputAttributeDescription,
			attribute_indexer::MAX_PARAMETERS >
			_attribute_descriptions;
	};
}
