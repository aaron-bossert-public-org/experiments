
#pragma once

#include "vulkan/shader/vulkan_parameter.h"

#include "igpu/shader/parameters.h"

#include <vector>

namespace igpu
{
	class vulkan_parameters : public parameters
	{
	public:
		struct config : parameters::config
		{
			struct vulkan
			{
				VkDevice device = nullptr;
				VkDescriptorSetLayout descriptor_set_layout = nullptr;
				std::vector< vulkan_parameter > parameters;
			};

			vulkan vk;
		};

		vulkan_parameters( config&& );

		~vulkan_parameters();

		const config& cfg() const override;

		size_t count() const override;

		const vulkan_parameter& parameter( size_t ) const override;

		VkDescriptorSetLayout descriptor_set_layout();

	private:
		const config _cfg;
	};
}
