
#include <vulkan/material/vulkan_render_states.h>

#include <framework/logging/log.h>

using namespace igpu;

namespace
{
	vulkan_render_states::config to_vulkan(const render_states::config& base_cfg)
	{
		vulkan_render_states::config cfg = {};
		static_cast<render_states::config&>(cfg) = base_cfg;
		cfg.vulkan = to_vulkan_render_states(cfg);

		return cfg;
	}
}

VkCullModeFlagBits igpu::to_vulkan_cull_mode(const cull_mode& mode)
{
	if (!mode.enable)
	{
		return VK_CULL_MODE_NONE;
	}

	switch (mode.mode)
	{
	case cull::BACK:
		return VK_CULL_MODE_BACK_BIT;
	case cull::FRONT:
		return VK_CULL_MODE_FRONT_BIT;
	case cull::BOTH:
		return VK_CULL_MODE_FRONT_AND_BACK;
	};

	if (is_valid(mode.mode))
	{
		LOG_CRITICAL("unhandled format: %s", to_string(mode.mode).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", mode.mode);
	}

	return VK_CULL_MODE_BACK_BIT;
}

VkBlendFactor igpu::to_vulkan_blend(blend blend)
{
	switch (blend)
	{
	case blend::ZERO:
		return VK_BLEND_FACTOR_ZERO;
	case blend::ONE:
		return VK_BLEND_FACTOR_ONE;
	case blend::SRC_COLOR:
		return VK_BLEND_FACTOR_SRC_COLOR;
	case blend::INV_SRC_COLOR:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case blend::DST_COLOR:
		return VK_BLEND_FACTOR_DST_COLOR;
	case blend::INV_DST_COLOR:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case blend::SRC_ALPHA:
		return VK_BLEND_FACTOR_SRC_ALPHA;
	case blend::INV_SRC_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case blend::DST_ALPHA:
		return VK_BLEND_FACTOR_DST_ALPHA;
	case blend::INV_DST_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	};

	if (is_valid(blend))
	{
		LOG_CRITICAL("unhandled blend: %s", to_string(blend).data());
	}
	else
	{
		LOG_CRITICAL("invalid blend: %d", blend);
	}

	return VK_BLEND_FACTOR_ONE;
};

VkCompareOp igpu::to_vulkan_compare(compare compare)
{
	switch (compare)
	{
	case compare::NEVER:
		return VK_COMPARE_OP_NEVER;
	case compare::LESS:
		return VK_COMPARE_OP_LESS;
	case compare::EQUAL:
		return VK_COMPARE_OP_EQUAL;
	case compare::LESS_EQUAL:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case compare::GREATER:
		return VK_COMPARE_OP_GREATER;
	case compare::NOT_EQUAL:
		return VK_COMPARE_OP_NOT_EQUAL;
	case compare::GREATER_EQUAL:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case compare::ALWAYS:
		return VK_COMPARE_OP_ALWAYS;
	};

	if (is_valid(compare))
	{
		LOG_CRITICAL("unhandled compare: %s", to_string(compare).data());
	}
	else
	{
		LOG_CRITICAL("invalid compare: %d", compare);
	}

	LOG_CRITICAL("unhandled compare value: %s", to_string(compare).data());

	return VK_COMPARE_OP_LESS;
};

VkStencilOp igpu::to_vulkan_stencil(stencil stencil)
{
	switch (stencil)
	{
	case stencil::KEEP:
		return VK_STENCIL_OP_KEEP;
	case stencil::ZERO:
		return VK_STENCIL_OP_ZERO;
	case stencil::REPLACE:
		return VK_STENCIL_OP_REPLACE;
	case stencil::INCREMENT:
		return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case stencil::WRAP_INCREMENT:
		return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case stencil::DECREMENT:
		return VK_STENCIL_OP_INVERT;
	case stencil::WRAP_DECREMENT:
		return VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case stencil::INVERT:
		return VK_STENCIL_OP_DECREMENT_AND_WRAP;
	};
	LOG_CRITICAL("unhandled stencil value: %s", to_string(stencil).data());

	if (is_valid(stencil))
	{
		LOG_CRITICAL("unhandled stencil: %s", to_string(stencil).data());
	}
	else
	{
		LOG_CRITICAL("invalid stencil: %d", stencil);
	}

	return VK_STENCIL_OP_KEEP;
};

VkPipelineColorBlendAttachmentState igpu::to_vulkan_color_blend(const blend_mode& mode, uint8_t color_write_mask)
{
	VkPipelineColorBlendAttachmentState vulkan = {};

	vulkan.blendEnable = mode.enable;
	vulkan.colorWriteMask = color_write_mask;

	vulkan.srcColorBlendFactor =
		vulkan.srcAlphaBlendFactor =
		to_vulkan_blend(mode.src);

	vulkan.dstColorBlendFactor =
		vulkan.dstAlphaBlendFactor =
		to_vulkan_blend(mode.dst);

	vulkan.colorBlendOp =
		vulkan.alphaBlendOp =
		VK_BLEND_OP_ADD;

	return vulkan;
}

VkPipelineDepthStencilStateCreateInfo igpu::to_vulkan_depth_stencil(const depth_mode& depth_mode, const stencil_mode& stencil_mode)
{
	VkPipelineDepthStencilStateCreateInfo vulkan = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
	};

	vulkan.depthTestEnable = depth_mode.enable;
	vulkan.depthWriteEnable = depth_mode.write;
	vulkan.depthCompareOp = to_vulkan_compare(depth_mode.compare);
	vulkan.depthBoundsTestEnable = VK_FALSE;

	vulkan.stencilTestEnable = stencil_mode.enable;

	vulkan.front.failOp = to_vulkan_stencil(stencil_mode.fail);
	vulkan.front.passOp = to_vulkan_stencil(stencil_mode.pass);
	vulkan.front.depthFailOp = to_vulkan_stencil(stencil_mode.depth_fail);
	vulkan.front.compareOp = to_vulkan_compare(stencil_mode.compare);
	vulkan.front.compareMask = stencil_mode.read_mask;
	vulkan.front.writeMask = stencil_mode.write_mask;
	vulkan.front.reference = stencil_mode.ref;

	vulkan.back = vulkan.front;

	return vulkan;
}

vulkan_render_states_raw igpu::to_vulkan_render_states(const render_states::config& cfg)
{
	return 	{
		to_vulkan_cull_mode(cfg.cull),
		to_vulkan_color_blend(cfg.blend, cfg.color_write_mask),
		to_vulkan_depth_stencil(cfg.depth, cfg.stencil),
	};
}


vulkan_render_states::vulkan_render_states(const render_states::config& base_cfg)
	: _cfg(to_vulkan(base_cfg))
{
}

