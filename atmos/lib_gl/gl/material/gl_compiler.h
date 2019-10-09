
#pragma once

#include <string_view>

template <typename T>
class buffer_view;

namespace igpu
{
	class vertex_constraints;

    unsigned gl_compile(
		const vertex_constraints&,
		const buffer_view<uint8_t>& vertex_code,
		const buffer_view<uint8_t>& pixel_code);
}
