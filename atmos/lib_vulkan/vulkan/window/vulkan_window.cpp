
#include <vulkan/window/vulkan_window.h>
#include <vulkan/defines/vulkan_includes.h>
#include <GLFW/glfw3.h>

#include <algorithm>

using namespace igpu;

std::unique_ptr<vulkan_window> vulkan_window::make(
	const config& cfg,
	glm::ivec2 res,
	const on_resize_t& on_resize)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto glfw_window = glfwCreateWindow(res.x, res.y, cfg.name.data(), nullptr, nullptr);
	VkSurfaceKHR surface = nullptr;
	VkResult result = glfwCreateWindowSurface(cfg.instance, glfw_window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error(EXCEPTION_CONTEXT(debug::stringify_result(result).c_str()));
	}

	auto w = std::unique_ptr<vulkan_window>(
		new vulkan_window(
			cfg,
			on_resize,
			glfw_window,
			surface));

	glfwSetWindowUserPointer(glfw_window, w.get());

	if (w->_on_resize)
	{
		glfwSetFramebufferSizeCallback(glfw_window, [](GLFWwindow* glfw_window, int /* width */, int /* height */)
		{
			vulkan_window* w = (vulkan_window*)glfwGetWindowUserPointer(glfw_window);
			w->_on_resize(w);
		});
	}

	return w;
}

const vulkan_window::config& vulkan_window::cfg() const
{
	return _cfg;
}

glm::ivec2 vulkan_window::res() const
{
	glm::ivec2 res;
	glfwGetWindowSize(_glfw_window, &res.x, &res.y);
	return res;
}

vulkan_window::~vulkan_window()
{
	vkDestroySurfaceKHR(_cfg.instance, _surface, nullptr);
	glfwDestroyWindow(_glfw_window);
	glfwTerminate();
}

std::vector<const char*> vulkan_window::required_extensions()
{
	glfwInit();

	uint32_t count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<const char*>(glfw_extensions, glfw_extensions + count);
}

bool vulkan_window::poll_events() const
{
	if (!glfwWindowShouldClose(_glfw_window))
	{
		glfwPollEvents();
		return true;
	}
	return false;
}

VkSurfaceKHR vulkan_window::surface()
{
	return _surface;
}

vulkan_window::vulkan_window(
	const config& cfg,
	const on_resize_t& on_resize,
	GLFWwindow* glfw_window,
	VkSurfaceKHR surface)
	: _cfg(cfg)
	, _on_resize(on_resize)
	, _glfw_window(glfw_window)
	, _surface(surface)
{
}
