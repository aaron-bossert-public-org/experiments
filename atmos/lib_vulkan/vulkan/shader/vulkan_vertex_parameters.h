
#pragma once

#include "vulkan/shader/vulkan_vertex_parameter.h"

#include "igpu/shader/vertex_parameters.h"

#include <vector>

namespace igpu
{
	class vulkan_vertex_parameters : public vertex_parameters
	{
	public:
		vulkan_vertex_parameters( std::vector< vulkan_vertex_parameter >&& );
		vulkan_vertex_parameters( vulkan_vertex_parameters&& ) = default;
		vulkan_vertex_parameters& operator=( vulkan_vertex_parameters&& ) =
			default;

		size_t count() const override;

		const vulkan_vertex_parameter& parameter( size_t ) const override;

	private:
		std::vector< vulkan_vertex_parameter > _parameters;
	};
}
