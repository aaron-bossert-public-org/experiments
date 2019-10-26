
#pragma once

#include <vulkan/defines/vulkan_includes.h>

#include <igpu/buffer/buffer_raw.h>

namespace igpu
{   
    class vulkan_shader
    {
    public:

		struct config
		{
			buffer_usage usage;
			VkDevice device;
		};

		vulkan_shader(const config&);
		
		~vulkan_shader();

		const config& cfg() const;

		VkShaderModule shader_module() const;

		void map(size_t byte_size, buffer_view_base*);

		void unmap();

		size_t byte_size() const;

	private:

		const config _cfg;
		VkShaderModule _shader_module = nullptr;
		buffer_raw _buffer;
    };

	template<typename T>
	class vulkan_shader_t : public T
	{
	public:

		struct config : T::config
		{
			VkDevice device = nullptr;
			VkShaderStageFlagBits shader_stage = {};
		};

		const config& cfg() const override
		{
			return _cfg;
		}

		VkPipelineShaderStageCreateInfo shader_stage_info() override
		{
			VkPipelineShaderStageCreateInfo shader_stage_info = {};
			shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shader_stage_info.stage = _cfg.shader_stage;
			shader_stage_info.module = _shader.shader_module();
			shader_stage_info.pName = "main";
			return shader_stage_info;
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
		
		size_t byte_size() const override
		{
			return _shader.byte_size();
		}

		static std::unique_ptr<vulkan_shader_t> make(
			const typename T::config& cfg,
			VkDevice device,
			VkShaderStageFlagBits shader_stage)
		{
			config cfg_t;
			static_cast<typename T::config&>(cfg_t) = cfg;
			cfg_t.device = device;
			cfg_t.shader_stage = shader_stage;

			switch (shader_stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			case VK_SHADER_STAGE_GEOMETRY_BIT:
			case VK_SHADER_STAGE_FRAGMENT_BIT:
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return std::unique_ptr<vulkan_shader_t>(
					new vulkan_shader_t(cfg_t));
			}

			LOG_CRITICAL("unhandled vulkan shader type: %d ", cfg_t.shader_stage);

			return nullptr;
		}

	private:

		vulkan_shader_t(const config& cfg)
			: _cfg(cfg)
			, _shader({ cfg.device })
		{ }

	private:

		const config _cfg;
		vulkan_shader _shader;
	};
}
