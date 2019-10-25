
#include <vulkan/material/vulkan_vertex_shader.h>
#include <vulkan/material/vulkan_shader.h>

using namespace igpu;


std::unique_ptr<vulkan_vertex_shader> vulkan_vertex_shader::make(VkDevice device)
{
	return vulkan_shader_t<vulkan_vertex_shader>::make({
		device,
		VK_SHADER_STAGE_VERTEX_BIT });
}
