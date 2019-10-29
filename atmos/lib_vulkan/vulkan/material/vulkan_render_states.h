
#pragma once

#include <igpu/material/render_states.h>
#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	struct vulkan_render_states_raw
	{
		VkCullModeFlagBits vk_cull_mode = {};
		VkPipelineColorBlendAttachmentState vk_color_blend = {};
		VkPipelineDepthStencilStateCreateInfo vk_depth_stencil = {};
	};

	VkCullModeFlagBits to_vulkan_cull_mode(const cull_mode& mode);

	VkBlendFactor to_vulkan_blend(blend);
	
	VkCompareOp to_vulkan_compare(compare);
	
	VkStencilOp to_vulkan_stencil(stencil);

	vulkan_render_states_raw
		to_vulkan_render_states
		(const render_states::config&);

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
			vulkan_render_states_raw vulkan;
		};

		vulkan_render_states(const render_states::config&);

		const config& cfg() const override;

		VkCullModeFlagBits _cull_mode;

	private:

		const config _cfg;
	};
}
