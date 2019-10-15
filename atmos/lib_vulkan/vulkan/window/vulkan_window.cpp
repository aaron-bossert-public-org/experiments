
#include <vulkan/window/vulkan_window.h>
#include <vulkan/defines/vulkan_includes.h>
#include <GLFW/glfw3.h>

using namespace igpu;

std::unique_ptr<vulkan_window> vulkan_window::make(
	const std::string_view& title,
	glm::ivec2 res,
	on_resize_t on_resize)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto glfw_window = glfwCreateWindow(res.x, res.y, title.data(), nullptr, nullptr);

	auto w = std::unique_ptr<vulkan_window>(
		new vulkan_window(
			glfw_window, 
			std::move(on_resize)));

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

vulkan_window::~vulkan_window()
{
	glfwDestroyWindow(_glfw_window);
	glfwTerminate();
}

glm::ivec2 vulkan_window::res() const
{
	glm::ivec2 res;
	glfwGetWindowSize(_glfw_window, &res.x, &res.y);
	return res;
}

std::vector<const char*> vulkan_window::required_extensions() const
{
	uint32_t count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<const char*>(glfw_extensions, glfw_extensions + count);
}

VkSurfaceKHR vulkan_window::make_surface(VkInstance instance)
{
	VkSurfaceKHR surface_khr = nullptr;
	if (glfwCreateWindowSurface(instance, _glfw_window, nullptr, &surface_khr) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to make window surface");
	}

	return surface_khr;
}

bool vulkan_window::poll_events()
{
	if (!glfwWindowShouldClose(_glfw_window))
	{
		glfwPollEvents();
		return true;
	}
	return false;
}

vulkan_window::vulkan_window(
	GLFWwindow* glfw_window,
	on_resize_t on_resize)
	: _glfw_window{ glfw_window }
	, _on_resize{ on_resize }
{
}
