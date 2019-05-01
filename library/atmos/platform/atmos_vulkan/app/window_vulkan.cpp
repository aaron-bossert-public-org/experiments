
#pragma once
#include "window_vulkan.h"
#include <GLFW/glfw3.h>
using namespace app;
using namespace platform;

window_vulkan::window_vulkan(
	GLFWwindow* glfw_window,
	on_resize_t on_resize)
	: _glfw_window{ glfw_window }
	, _on_resize{ on_resize }
{
}

VkResult window_vulkan::create_surface(VkInstance instance, VkSurfaceKHR* surface)
{
	return glfwCreateWindowSurface(instance, _glfw_window, nullptr, surface);
}

bool window_vulkan::poll_events() {
	if (!glfwWindowShouldClose(_glfw_window)) {
		glfwPollEvents();
		return true;
	}
	return false;
}

std::vector<const char*> window_vulkan::required_extensions() const
{
	uint32_t count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<const char*>(glfw_extensions, glfw_extensions + count);
}

std::unique_ptr<window> window::make(
	const std::string_view& title,
	size_t width, size_t height,
	on_resize_t on_resize)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto glfw_window = glfwCreateWindow((int)width, (int)height, "Vulkan", nullptr, nullptr);

	auto w = std::unique_ptr<window>(new window(glfw_window, [](size_t, size_t) {}));

	glfwSetWindowUserPointer(glfw_window, w.get());
	glfwSetFramebufferSizeCallback(glfw_window, [](GLFWwindow* glfw_window, int width, int height)
	{
		reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_window))->_on_resize(width, height);
	});

	return w;
}

window::~window()
{
	glfwDestroyWindow(_glfw_window);
	glfwTerminate();
}

size_t window::width() const
{
	int w = 0, h = 0;
	glfwGetWindowSize(_glfw_window, &w, &h);
	return w;
}

size_t window::height() const
{
	int w = 0, h = 0;
	glfwGetWindowSize(_glfw_window, &w, &h);
	return h;
}
