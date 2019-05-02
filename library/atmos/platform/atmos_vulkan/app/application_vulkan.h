#pragma once
#include <atmos/partial.h>
#include <atmos_vulkan/app/application_vulkan.h>
#include <atmos_vulkan/app/window_vulkan.h>
#include <atmos_vulkan/perf/perf_observer_vulkan.h>
#include <atmos_vulkan/third_party/vulkan_includes.h>
#include <memory>

namespace app { 
	class application;

	class application_vulkan
	{
	public:

	protected:
		std::unique_ptr<window> _window;
		std::weak_ptr<perf::perf_observer> _weak_perf_observer;
	};
}

namespace platform
{
	template<>
	class partial<app::application> : public app::application_vulkan
	{
	};
}

#include <atmos/app/application.h>