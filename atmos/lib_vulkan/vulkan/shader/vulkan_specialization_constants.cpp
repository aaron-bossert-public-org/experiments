
#include "vulkan/shader/vulkan_specialization_constants.h"

using namespace igpu;

vulkan_specialization_constants::vulkan_specialization_constants(
	const std::vector< constant_parameter::config >& constants,
	shader_stages stages )
{
	size_t size = 0;

	for ( const auto& constant : constants )
	{
		if ( 0 != ( constant.shader_stages & stages ) )
		{
			size += std::visit(
				[]( const auto& val ) { return sizeof val; },
				constant.constant.value );
		}
	}

	_data.resize( size );
	size = 0;
	for ( const auto& constant : constants )
	{
		if ( 0 != ( constant.shader_stages & stages ) )
		{
			auto value_size = std::visit(
				[]( const auto& val ) { return sizeof val; },
				constant.constant.value );
			memcpy( _data.data() + size, &constant.constant.value, value_size );

			_entries.push_back( {
				(uint32_t)constant.binding,
				(uint32_t)size,
				(uint32_t)value_size,
			} );

			size += value_size;
		}
	}

	_info = {
		(uint32_t)_entries.size(),
		_entries.data(),
		_data.size(),
		_data.data(),
	};
}


const VkSpecializationInfo& vulkan_specialization_constants::info() const
{
	return _info;
}
