#pragma once
#include <atmos/partial.h>
#include <vulkan/vulkan.h>
#include <functional>
#include  <string_view>

struct GLFWwindow;

namespace app { 

	class window; 

	class window_vulkan
	{
	public:

		using on_resize_t = std::function<void(size_t, size_t)>;

		std::vector<const char*> required_extensions() const;
		window_vulkan(GLFWwindow* glfw_window, on_resize_t on_resize);
		VkResult create_surface(VkInstance, VkSurfaceKHR*);
		bool poll_events();

	protected:

		GLFWwindow* const _glfw_window;
		const on_resize_t _on_resize;
	};
}

namespace platform
{
	template<>
	class partial<app::window> : public app::window_vulkan
	{
		using window_vulkan::window_vulkan;
	};
}

#include <atmos/app/window.h>