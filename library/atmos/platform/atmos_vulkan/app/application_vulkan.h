#pragma once
#include <atmos/partial.h>
#include <vulkan/vulkan.h>

#include "window_vulkan.h"
#include <memory>

namespace app { 
	class application;

	class application_vulkan
	{
	public:

	protected:
		std::unique_ptr<app::window> _window;
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