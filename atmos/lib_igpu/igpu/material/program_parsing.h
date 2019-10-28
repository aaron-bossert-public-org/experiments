
#pragma once

#include <igpu/material/parameter.h>
#include <igpu/buffer/vertex_parameter.h>

class buffer_view_base;

namespace igpu
{   
	struct spirv_resource
	{
		igpu::parameter::config cfg;
		size_t descriptor_set;
		size_t binding;
	};

	struct spirv_attribute
	{
		igpu::vertex_parameter::config cfg;
		size_t location;
	};

	void parse_spirv(
		std::vector<uint32_t> spirv,
		std::vector<spirv_resource>*,
		std::vector<spirv_attribute>* = nullptr);
}