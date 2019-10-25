
#pragma once

#include <igpu/buffer/vertex_buffer.h>
#include <igpu/buffer/vertex_format.h>
#include <igpu/utility/scoped_ptr.h>

#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	class vertex_constraints;
	class vulkan_buffer_mediator;

	VkFormat to_vulkan_format(components);

	class vulkan_vertex_buffer : public vertex_buffer
	{
	public:

		static std::unique_ptr<vulkan_vertex_buffer> make(
			const config&,
			const scoped_ptr< vulkan_buffer_mediator >&,
			const vertex_constraints&);

		virtual VkBuffer get() = 0;

		const VkVertexInputBindingDescription& binding_description() const;

		const std::vector<VkVertexInputAttributeDescription>& attribute_descriptions() const;

	protected:

		vulkan_vertex_buffer(
			const config&,
			const std::vector<VkVertexInputAttributeDescription>&);

	private:

		const VkVertexInputBindingDescription _binding_description = {};
		const std::vector<VkVertexInputAttributeDescription> _attribute_descriptions;
	};
}
