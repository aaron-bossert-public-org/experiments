
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_barrier_manager;
	class vulkan_queue_manager;

	class vulkan_managers
	{
	public:
		struct config
		{
			VkPhysicalDeviceProperties device_properties = {};
			VkPhysicalDeviceFeatures device_features = {};
			scoped_ptr< vulkan_barrier_manager > barrier;
			scoped_ptr< vulkan_queue_manager > queues;
		};

		const config& cfg() const;

		~vulkan_managers();

		static std::unique_ptr< vulkan_managers > make( const config& );

	protected:
		vulkan_managers( const config& );

	private:
		const config _cfg;
	};
}
