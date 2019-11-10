
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/primitive_indexer.h"

#include <array>

namespace igpu
{
	class vulkan_buffer;
	class vulkan_image;

	class vulkan_parameter;
	class vulkan_parameters;
	class vulkan_primitive;
	class vulkan_primitives;

	class vulkan_primitives_descriptor
	{
	public:
		using image_descriptors_t = std::
			array< VkDescriptorImageInfo, primitive_indexer::MAX_PARAMETERS >;
		using buffer_descriptors_t = std::
			array< VkDescriptorBufferInfo, primitive_indexer::MAX_PARAMETERS >;
		using write_descriptors_t = std::
			array< VkWriteDescriptorSet, primitive_indexer::MAX_PARAMETERS >;

		bool reset(
			VkDescriptorSet,
			const vulkan_parameters&,
			const vulkan_primitives& );

		const primitive_indexer& indexer() const;

		uint32_t image_count() const;

		uint32_t buffer_count() const;

		const image_descriptors_t& image_descriptors() const;

		const buffer_descriptors_t& buffer_descriptors() const;

		const write_descriptors_t& write_descriptors() const;

		const vulkan_parameters* parameters() const;

		const vulkan_primitives* primitives() const;

		VkWriteDescriptorSet* next_write_descriptor();

		VkDescriptorImageInfo* append(
			const vulkan_parameter&,
			const vulkan_image& );

		VkDescriptorBufferInfo* append(
			const vulkan_parameter&,
			const vulkan_buffer& );

	private:
		primitive_indexer _indexer;

		VkDescriptorSet _descriptor_set = nullptr;

		const vulkan_parameters* _parameters = nullptr;
		const vulkan_primitives* _primitives = nullptr;

		uint32_t _image_count = 0;
		uint32_t _buffer_count = 0;

		image_descriptors_t _image_descriptors;
		buffer_descriptors_t _buffer_descriptors;
		write_descriptors_t _write_descriptors;
	};
}