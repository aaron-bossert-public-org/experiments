
#include "gl/window/gl_window.h"
#include "GLFW/glfw3.h"
#include "gl/defines/gl_includes.h"

using namespace igpu;

std::unique_ptr<gl_window> gl_window::make(
	const config& cfg,
	glm::ivec2 res,
	on_resize_t on_resize)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto glfw_window = glfwCreateWindow(res.x, res.y, cfg.name.c_str(), nullptr, nullptr);

	auto w = std::unique_ptr<gl_window>(
		new gl_window(
			cfg,
			glfw_window,
			std::move(on_resize)));

	glfwSetWindowUserPointer(glfw_window, w.get());

	if (w->_on_resize)
	{
		glfwSetFramebufferSizeCallback(glfw_window, [](GLFWwindow* glfw_window, int /* width */, int /* height */)
			{
				gl_window* w = (gl_window*)glfwGetWindowUserPointer(glfw_window);
				w->_on_resize(w);
			});
	}

	return w;
}

gl_window::~gl_window()
{
	glfwDestroyWindow(_glfw_window);
	glfwTerminate();
}

const gl_window::config& gl_window::cfg() const
{
	return _cfg;
}

glm::ivec2 gl_window::res() const
{
	glm::ivec2 res;
	glfwGetWindowSize(_glfw_window, &res.x, &res.y);
	return res;
}

gl_window::gl_window(
	const config& cfg,
	GLFWwindow* glfw_window,
	on_resize_t on_resize)
	: _cfg(cfg)
	, _glfw_window{ glfw_window }
	, _on_resize{ on_resize }
{
}
