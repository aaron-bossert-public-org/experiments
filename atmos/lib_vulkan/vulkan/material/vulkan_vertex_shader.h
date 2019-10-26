
#pragma once

#include <igpu/material/vertex_shader.h>
#include <vulkan/defines/vulkan_includes.h>
#include <memory>

namespace igpu
{
	class vulkan_vertex_shader : public vertex_shader
	{
	public:

		virtual VkPipelineShaderStageCreateInfo shader_stage_info() = 0;

		static std::unique_ptr<vulkan_vertex_shader> make(
			const config& cfg,
			VkDevice device);
	};
}
