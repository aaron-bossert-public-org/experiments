
#pragma once

#include <igpu/material/vertex_shader.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/material/vulkan_shader.h>
#include <memory>

namespace igpu
{
	class vulkan_vertex_shader : public vertex_shader, public vulkan_shader
	{
	public:

		virtual size_t attribute_count() const = 0;

		virtual const spirv_attribute& attribute(size_t) const = 0;

		static std::unique_ptr<vulkan_vertex_shader> make(
			const config& cfg,
			VkDevice device);
	};
}
