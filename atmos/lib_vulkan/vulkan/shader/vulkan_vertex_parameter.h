
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/components.h"
#include "igpu/shader/program_parsing.h"

namespace igpu
{
	VkFormat to_vulkan_format( components );

	class vulkan_vertex_parameter : public vertex_parameter
	{
	public:
		struct config : vertex_parameter::config
		{
			struct vulkan
			{
				VkFormat format = {};
			};

			vulkan vk;
		};

		const config& cfg() const override;

		vulkan_vertex_parameter( const vertex_parameter::config& );

	private:
		const config _cfg;
	};
}