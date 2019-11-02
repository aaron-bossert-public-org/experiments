
#pragma once

#include <igpu/shader/program_parsing.h>
#include <igpu/shader/vertex_shader.h>

#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/shader/vulkan_shader.h>

#include <memory>

namespace igpu
{
	class vulkan_vertex_shader : public vertex_shader, public vulkan_shader
	{
	public:

		struct config : vertex_shader::config
		{
			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		virtual size_t vertex_parameter_count() const = 0;

		virtual const spirv::vertex_parameter& vertex_parameter(size_t) const = 0;

		static std::unique_ptr<vulkan_vertex_shader> make(
			const config& cfg);
	};
}
