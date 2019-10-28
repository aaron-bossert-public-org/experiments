
#pragma once

#include <igpu/buffer/vertex_parameter.h>

namespace igpu
{
	class vulkan_vertex_parameter : public vertex_parameter
	{
	public:

		struct config : vertex_parameter::config
		{
		};

		const config& cfg() const override;

		vulkan_vertex_parameter(const config&);

	private:
		
		const config _cfg;
	};
}