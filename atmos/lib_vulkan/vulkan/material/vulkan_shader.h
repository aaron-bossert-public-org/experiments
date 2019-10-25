
#pragma once

#include <vulkan/defines/vulkan_includes.h>

#include <framework/utility/buffer_view.h>

namespace igpu
{   
    class vulkan_shader
    {
    public:

		struct config
		{
			VkDevice device;
			VkShaderStageFlagBits shader_stage;
		};

		vulkan_shader(const config&);
		
		~vulkan_shader();

		const config& cfg() const;

		VkShaderModule shader_module() const;

		void map(size_t byte_size, buffer_view_base*);

		void unmap();

	private:

		const config _cfg;
		VkShaderModule _shader_module = nullptr;
		
		struct
		{
			std::unique_ptr<char[]> raw;
			buffer_view_base view;
		} _mapped;
    };

	template<typename T>
	class vulkan_shader_t : public T
	{
	public:

		VkPipelineShaderStageCreateInfo shader_stage_info() override
		{
			VkPipelineShaderStageCreateInfo shader_stage_info = {};
			shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shader_stage_info.stage = _shader.cfg().shader_stage;
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

		static std::unique_ptr<vulkan_shader_t> make(const vulkan_shader::config& cfg)
		{
			switch (cfg.shader_stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			case VK_SHADER_STAGE_GEOMETRY_BIT:
			case VK_SHADER_STAGE_FRAGMENT_BIT:
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return std::unique_ptr<vulkan_shader_t>(
					new vulkan_shader_t(cfg));
			}

			LOG_CRITICAL("unhandled vulkan shader type: %d ", cfg.shader_stage);

			return nullptr;
		}

	private:

		vulkan_shader_t(const vulkan_shader::config& cfg)
			: _shader(cfg)
		{ }

	private:

		vulkan_shader _shader;
	};
}
