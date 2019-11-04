
#pragma once

#include "igpu/shader/render_states.h"
#include "vulkan/defines/vulkan_includes.h"

namespace igpu
{
	VkCullModeFlagBits to_vulkan_cull_mode(const cull_mode& mode);

	VkBlendFactor to_vulkan_blend(blend);
	
	VkCompareOp to_vulkan_compare(compare);
	
	VkStencilOp to_vulkan_stencil(stencil);

	VkPipelineColorBlendAttachmentState
		to_vulkan_color_blend
		(const blend_mode&, uint8_t color_write_mask);

	VkPipelineDepthStencilStateCreateInfo
		to_vulkan_depth_stencil
		(const depth_mode&, const stencil_mode&);

	class vulkan_render_states : public render_states
	{
	public:

		struct config : render_states::config
		{
			struct vulkan
			{
				VkCullModeFlagBits cull = {};
				VkPipelineColorBlendAttachmentState color_blend = {};
				VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
			};

			vulkan vk;
		};

		vulkan_render_states(const config&);

		const config& cfg() const override;

	private:

		const config _cfg;
	};

	vulkan_render_states::config::vulkan
		to_vulkan_render_states
		(const render_states::config&);
}
