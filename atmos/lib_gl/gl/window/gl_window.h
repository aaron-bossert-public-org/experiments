#pragma once

#include <igpu/window/window.h>

#include <functional>
#include  <string_view>

struct GLFWwindow;

namespace igpu { 

	class gl_window : public window
	{
	public:

		using on_resize_t = std::function<void(gl_window*)>;

		static std::unique_ptr<gl_window> make(
			const std::string_view& title,
			glm::ivec2 res,
			on_resize_t on_resize = nullptr);

		~gl_window() override;

		glm::ivec2 res() const override;

	protected:

		gl_window(GLFWwindow* glfw_window, on_resize_t on_resize);

	protected:

		GLFWwindow* const _glfw_window;
		const on_resize_t _on_resize;
	};
}