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
			const config&,
			glm::ivec2 res,
			on_resize_t on_resize = nullptr);

		~gl_window() override;

		const config& cfg() const override;

		glm::ivec2 res() const override;

	protected:

		gl_window(
			const config&,
			GLFWwindow*, 
			on_resize_t on_resize);

	protected:

		const config _cfg;
		GLFWwindow* const _glfw_window;
		const on_resize_t _on_resize;
	};
}