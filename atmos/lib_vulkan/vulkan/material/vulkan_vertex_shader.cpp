
#include <vulkan/material/vulkan_vertex_shader.h>
#include <vulkan/material/vulkan_shader_impl.h>

using namespace igpu;


std::unique_ptr<vulkan_vertex_shader> vulkan_vertex_shader::make(
	const config& cfg)
{
	return vulkan_shader_impl_t<vulkan_vertex_shader>::make(
		cfg);
}
