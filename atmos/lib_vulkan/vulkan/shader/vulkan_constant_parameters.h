
#pragma once

#include "igpu/shader/constant_parameters.h"

#include <vector>

namespace igpu
{
	class vulkan_constant_parameters : public constant_parameters
	{
	public:
		vulkan_constant_parameters( config&& );

		~vulkan_constant_parameters();

		const config& cfg() const override;

	private:
		const config _cfg;
	};
}
