
#pragma once

#include <vulkan/defines/vulkan_includes.h>

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
			struct vulkan
			{
				VkDevice device = nullptr;
				VkShaderStageFlagBits shader_stage = {};
			};

			vulkan vk;
		};

		vulkan_shader_impl(const config&);
		
		~vulkan_shader_impl();

		const config& cfg() const override;

		void map(size_t byte_size, buffer_view_base*) override;

		void unmap() override;

		size_t byte_capacity() const override;

		size_t resource_count() const;
		
		const spirv_resource& resource(size_t) const;

		size_t attribute_count() const;

		const spirv_attribute& attribute(size_t) const;
		
		VkShaderModule shader_module() const;

	private:

		const config _cfg;
		VkShaderModule _shader_module = nullptr;
		std::vector<uint32_t> _memory;
		std::vector<spirv_resource> _resources;
		std::vector<spirv_attribute> _attributes;

		perf::metric _cpu_mem_metric;
		static constexpr size_t _element_size = sizeof(_memory[0]);
    };

	template<typename T>
	class vulkan_shader_impl_t : public T
	{
	public:

		struct config : T::config
		{
			vulkan_shader_impl::config::vulkan vk;
		};

		const config& cfg() const override
		{
			return _cfg;
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

		VkPipelineShaderStageCreateInfo shader_stage_info() override
		{
			VkPipelineShaderStageCreateInfo shader_stage_info = {};
			shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shader_stage_info.stage = _cfg.vk.shader_stage;
			shader_stage_info.module = _shader.shader_module();
			shader_stage_info.pName = "main";
			return shader_stage_info;
		}

		size_t resource_count() const override
		{
			return _shader.resource_count();
		}

		const spirv_resource& resource(size_t i) const
		{
			return _shader.resource(i);
		}

		size_t attribute_count() const
		{
			return _shader.attribute_count();
		}

		const spirv_attribute& attribute(size_t i) const
		{
			return _shader.attribute(i);
		}

		static std::unique_ptr<vulkan_shader_impl_t> make(
			const typename T::config& cfg,
			VkDevice device,
			VkShaderStageFlagBits shader_stage)
		{
			config cfg_t;
			COPY_TO_DERRIVED_CONFIG(cfg, &cfg_t);
			cfg_t.vk.device = device;
			cfg_t.vk.shader_stage = shader_stage;

			switch (shader_stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			case VK_SHADER_STAGE_GEOMETRY_BIT:
			case VK_SHADER_STAGE_FRAGMENT_BIT:
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return std::unique_ptr<vulkan_shader_impl_t>(
					new vulkan_shader_impl_t(cfg_t));
			}

			LOG_CRITICAL("unhandled vulkan shader type: %d ", cfg_t.vk.shader_stage);

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
