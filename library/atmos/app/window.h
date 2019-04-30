#pragma once
#include <atmos/partial.h>
#include <memory>
#include <string_view>
#include <functional>

namespace app
{
	class window : public platform::partial <window>
	{
	public:

		using platform::partial<window>::partial;
		using on_resize_t = std::function<void(size_t, size_t)>;

		static std::unique_ptr<window> make(
			const std::string_view& title,
			size_t width, size_t height,
			on_resize_t on_resize);

		~window();

		size_t width() const;
		size_t height() const;
	};
}