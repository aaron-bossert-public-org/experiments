
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_shader.h"

#include "igpu/shader/program_parsing.h"
#include "igpu/shader/vertex_shader.h"
#include "igpu/buffer/vector_buffer.h"

#include <memory>

namespace igpu
{
	class vulkan_vertex_shader : public vertex_shader, public vulkan_shader
	{
	public:

		virtual size_t vertex_parameter_count() const = 0;

		virtual const spirv::vertex_parameter& vertex_parameter(size_t) const = 0;

		static std::unique_ptr<vulkan_vertex_shader> make(
			const vulkan& vk,
			vector_buffer<uint32_t>&& buffer);
	};
}
