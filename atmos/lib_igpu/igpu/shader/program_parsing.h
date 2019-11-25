
#pragma once

#include "igpu/shader/constant_parameter.h"
#include "igpu/shader/parameter.h"
#include "igpu/shader/vertex_parameter.h"

#include <vector>

class buffer_view_base;

namespace igpu
{
	[[nodiscard]] bool spirv_parse(
		std::vector< uint32_t > spirv,
		std::string entry_point,
		shader_stages stages,
		std::vector< constant_parameter::config >*,
		std::vector< parameter::config >*,
		std::vector< vertex_parameter::config >* = nullptr );
}