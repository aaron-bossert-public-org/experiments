
 #pragma once

 #include <igpu/buffer/vertex_buffer.h>
 #include <igpu/buffer/vertex_format.h>
 
#include <vulkan/buffer/vulkan_staged_buffer.h>
 
 namespace igpu
 {
 	class vulkan_vertex_buffer : public vulkan_staged_buffer_t < vertex_buffer >
 	{
 	public:

		static std::unique_ptr<vulkan_vertex_buffer> make(
			const config&,
			const std::weak_ptr < vulkan_buffer_mediator >&);

 	private:

 		vulkan_vertex_buffer(
			const config&,
			const vulkan_staged_buffer::config&);
 	};
 }
