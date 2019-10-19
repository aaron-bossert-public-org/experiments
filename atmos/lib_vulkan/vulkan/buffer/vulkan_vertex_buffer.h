
 #pragma once

 #include <igpu/buffer/vertex_buffer.h>
 #include <igpu/buffer/vertex_format.h>
 
#include <vulkan/buffer/vulkan_staged_buffer.h>
 
 namespace igpu
 {
	 class vertex_constraints;

	 VkFormat to_vulkan_format(components);

 	class vulkan_vertex_buffer : public vulkan_staged_buffer_t < vertex_buffer >
 	{
 	public:

		static std::unique_ptr<vulkan_vertex_buffer> make(
			const config&,
			const scoped_ptr< vulkan_buffer_mediator >&,
			const vertex_constraints&);

		const config& cfg() const override;

		const std::vector<VkVertexInputAttributeDescription>& attribute_descriptions() const;

 	private:

		vulkan_vertex_buffer(
			const config&,
			const vulkan_staged_buffer::config&,
			const std::vector<VkVertexInputAttributeDescription>&);

	private:

		const config _cfg;
		const std::vector<VkVertexInputAttributeDescription> _attribute_descriptions;
 	};
 }
