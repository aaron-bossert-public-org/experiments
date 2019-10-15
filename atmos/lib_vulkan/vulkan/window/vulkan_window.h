#pragma once


#include <igpu/window/window.h>

#include <functional>
#include  <string_view>

struct GLFWwindow;
struct VkInstance_T;
struct VkSurfaceKHR_T;

namespace igpu { 

	class vulkan_window : public window
	{
	public:

		using on_resize_t = std::function<void(vulkan_window*)>;

		static std::unique_ptr<vulkan_window> make(
			const std::string_view& title,
			glm::ivec2 res,
			on_resize_t on_resize = nullptr);

		~vulkan_window() override;

		glm::ivec2 res() const override;

		std::vector<const char*> required_extensions() const;
		
		VkSurfaceKHR_T* make_surface(VkInstance_T*);
		
		bool poll_events();

	protected:

		vulkan_window(GLFWwindow* glfw_window, on_resize_t on_resize);

	protected:

		GLFWwindow* const _glfw_window;
		const on_resize_t _on_resize;
	};
}
