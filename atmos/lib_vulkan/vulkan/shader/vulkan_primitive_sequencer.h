
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/primitive_indexer.h"

#include <array>

namespace igpu
{
	class vulkan_geometry;
	class vulkan_vertex_parameters;

	class vulkan_primitive_sequencer
	{
	public:
		using image_descriptors_t = std::
			array< VkDescriptorBufferInfo, primitive_indexer::MAX_PARAMETERS >;
		using buffer_descriptors_t = std::
			array< VkDescriptorBufferInfo, primitive_indexer::MAX_PARAMETERS >;

		bool reset( const vulkan_parameters&, const vulkan_primitives& );

		const primitive_indexer& indexer() const;

		uint32_t _image_count() const;

		const std::
			array< VkDescriptorImageInfo, primitive_indexer::MAX_PARAMETERS >
				_image_descriptions;
		&binding_descriptions() const;

		const VkVertexInputAttributeDescription* attribute_descriptions() const;

	private:
		primitive_indexer _indexer;

		size_t _image_count;
		size_t _buffer_count;

		std::array< VkDescriptorImageInfo, primitive_indexer::MAX_PARAMETERS >
			_image_descriptions;

		std::array< VkDescriptorBufferInfo, primitive_indexer::MAX_PARAMETERS >
			_buffer_descriptions;
	};
}