
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/shader/program_parsing.h"

namespace igpu
{
	class vulkan_parameter : public parameter
	{
	public:
		struct config : parameter::config
		{
			struct vulkan
			{
				VkDescriptorType											//
					descriptor_type = VK_DESCRIPTOR_TYPE_MAX_ENUM;			//
				VkImageLayout												//
					image_layout = VK_IMAGE_LAYOUT_MAX_ENUM;				//
				VkShaderStageFlagBits										//
					shader_stages = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;		//
				VkPipelineStageFlagBits										//
					pipeline_stages = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM; //
			};

			vulkan vk;
		};

		const config& cfg() const override;

		vulkan_parameter( const parameter::config& );

	private:
		const config _cfg;
	};

	shader_stages from_vulkan_shader_stage_flags( VkShaderStageFlagBits );

	vulkan_parameter::config::vulkan to_vulkan(
		const parameter::config& config );
}
