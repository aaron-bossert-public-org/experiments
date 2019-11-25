#pragma once

#include "glm/glm.hpp"
#include <memory>
#include <string>
#include <vector>

namespace igpu
{
	class buffer;
	class context;
	class geometry;
}

namespace dcs_utils
{
	bool load_buffer( const std::string& path, igpu::buffer* out_buffer );

	std::vector< uint32_t > load_mem( const std::string& path );

	std::shared_ptr< igpu::geometry > load_model(
		igpu::context*,
		const std::string_view& model_path );
}
