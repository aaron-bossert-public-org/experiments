
#pragma once

#include <vulkan/defines/vulkan_includes.h>

#include <vulkan/buffer/vulkan_vertex_parameter.h>
#include <vulkan/context/vulkan_context.h>
#include <vulkan/material/vulkan_shader.h>

#include <igpu/buffer/buffer.h>
#include <igpu/material/program_parsing.h>

#include <framework/perf/metrics.h>
namespace igpu
{   
    class vulkan_shader_impl : public buffer
    {
    public:

		struct config : buffer::config
		{
			vulkan_shader::vulkan vk;
		};

		vulkan_shader_impl(const config&);
		
		~vulkan_shader_impl();

		const config& cfg() const override;

		void map(size_t byte_size, buffer_view_base*) override;

		void unmap() override;

		size_t byte_capacity() const override;

		VkPipelineShaderStageCreateInfo stage_info() const;
		
		size_t parameter_count() const;
		
		const spirv::parameter& parameter(size_t) const;

		size_t vertex_parameter_count() const;

		const spirv::vertex_parameter& vertex_parameter(size_t) const;
		
		VkShaderModule shader_module() const;

	private:

		const config _cfg;
		VkShaderModule _shader_module = nullptr;
		std::vector<uint32_t> _memory;
		std::vector<spirv::parameter> _parameters;
		std::vector<spirv::vertex_parameter> _vertex_parameters;

		perf::metric _cpu_mem_metric;
		static constexpr size_t _element_size = sizeof(_memory[0]);
    };

	template<typename T>
	class vulkan_shader_impl_t : public T
	{
	public:

		using config = typename T::config;

		const config& cfg() const override
		{
			return _cfg;
		}

		const vulkan_shader::vulkan& vk() const override
		{
			return _cfg.vk;
		}

		void map(
			size_t byte_size,
			buffer_view_base* out_buffer_view)
		{
			_shader.map(byte_size, out_buffer_view);
		}

		void unmap()
		{
			_shader.unmap();
		}
		
		size_t byte_capacity() const override
		{
			return _shader.byte_capacity();
		}

		VkPipelineShaderStageCreateInfo stage_info() const override
		{
			return _shader.stage_info();
		}

		size_t parameter_count() const override
		{
			return _shader.parameter_count();
		}

		const spirv::parameter& parameter(size_t i) const override
		{
			return _shader.parameter(i);
		}

		size_t vertex_parameter_count() const
		{
			return _shader.vertex_parameter_count();
		}

		const spirv::vertex_parameter& vertex_parameter(size_t i) const
		{
			return _shader.vertex_parameter(i);
		}

		static std::unique_ptr<vulkan_shader_impl_t> make(
			const config& cfg)
		{
			switch (cfg.vk.stage_flags)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			case VK_SHADER_STAGE_GEOMETRY_BIT:
			case VK_SHADER_STAGE_FRAGMENT_BIT:
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return std::unique_ptr<vulkan_shader_impl_t>(
					new vulkan_shader_impl_t(cfg));
			}

			LOG_CRITICAL("unhandled vulkan shader type: %d ", cfg.vk.stage_flags);

			return nullptr;
		}

	private:

		vulkan_shader_impl_t(const config& cfg)
			: _cfg(cfg)
			, _shader({ cfg.usage, cfg.vk })
		{ }

	private:

		const config _cfg;
		vulkan_shader_impl _shader;
	};
}
