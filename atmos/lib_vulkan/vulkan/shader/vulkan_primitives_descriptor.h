
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
		[[nodiscard]] bool reset(
			const vulkan_parameters&,
			const vulkan_primitives& );

		bool bind_descriptor_set( VkDescriptorSet ) const;

		const primitive_indexer& indexer() const;

		const vulkan_parameters* parameters() const;

		const vulkan_primitives* primitives() const;

	private:
		primitive_indexer _indexer;
		const vulkan_parameters* _parameters = nullptr;
		const vulkan_primitives* _primitives = nullptr;
	};
}