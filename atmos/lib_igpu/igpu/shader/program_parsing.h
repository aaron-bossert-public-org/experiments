
#pragma once

#include "igpu/shader/parameter.h"
#include "igpu/shader/shader_stages.h"
#include "igpu/shader/vertex_parameter.h"

class buffer_view_base;

namespace igpu
{
	namespace spirv
	{
		struct parameter : igpu::parameter::config
		{
			struct spirv
			{
				uint32_t descriptor_set = 0;
				uint32_t binding = 0;
				shader_stages stages = NONE;
			};

			spirv spv;
		};

		struct vertex_parameter : igpu::vertex_parameter::config
		{
			struct spirv
			{
				uint32_t location;
			};

			spirv spv;
		};

		void parse(
			std::vector< uint32_t > spirv,
			std::string entry_point,
			shader_stages stages,
			std::vector< parameter >*,
			std::vector< vertex_parameter >* = nullptr );
	}
}