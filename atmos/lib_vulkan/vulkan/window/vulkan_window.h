#pragma once


#include "vulkan/defines/vulkan_includes.h"

#include "igpu/window/window.h"

#include <functional>

struct GLFWwindow;

namespace igpu
{
	class vulkan_back_buffer;
	class vulkan_queue;

	class vulkan_window : public window
	{
	public:
		struct config : window::config
		{
			struct vulkan
			{
				VkInstance instance;
			};

			vulkan vk;
		};

		using on_resize_t = std::function< void( vulkan_window* ) >;

		static std::unique_ptr< vulkan_window > make(
			const config& cfg,
			glm::ivec2 res,
			const on_resize_t& on_resize = nullptr );

		const config& cfg() const override;

		glm::ivec2 res() const override;

		bool poll_events() const override;

		~vulkan_window() override;

		VkSurfaceKHR surface();

		static std::vector< const char* > required_extensions();

	protected:
		vulkan_window(
			const config&,
			const on_resize_t&,
			GLFWwindow*,
			VkSurfaceKHR );

	protected:
		const config _cfg;
		const on_resize_t _on_resize;
		GLFWwindow* const _glfw_window;
		VkSurfaceKHR _surface;
	};
}
