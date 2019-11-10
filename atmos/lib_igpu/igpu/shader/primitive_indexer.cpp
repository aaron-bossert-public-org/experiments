#include "igpu/shader/primitive_indexer.h"

#include "igpu/shader/parameter.h"
#include "igpu/shader/parameters.h"
#include "igpu/shader/primitive.h"
#include "igpu/shader/primitives.h"

#include "framework/logging/log.h"

using namespace igpu;

bool primitive_indexer::reset(
	const parameters& parameters,
	const primitives& primitives )
{
	if ( parameters.count() > parameters::MAX_COUNT )
	{
		LOG_CRITICAL(
			"parameter count (%d) exceeds limit (%d)",
			(int)parameters.count(),
			(int)parameters::MAX_COUNT );
		memset( this, 0, sizeof *this );
		return false;
	}

	bool success = true;
	size_t prim = 0;

	for ( size_t pram = 0; pram < parameters.count(); ++pram )
	{
		const igpu::parameter& parameter = parameters.parameter( pram );
		if ( !primitives.find_expected( parameter.cfg().name, &prim ) )
		{
			LOG_CRITICAL(
				"primitive named (%s) not found",
				parameter.cfg().name.c_str() );
			success = false;
		}
		else
		{
			const igpu::primitive& primitive = primitives.primitive( prim );

			_indices[pram] = (uint8_t)prim;
			++prim;
			if ( parameter.cfg().type == parameter::type::STORAGE_BUFFER )
			{
				if ( primitive.type() != parameter::type::STORAGE_BUFFER &&
					 primitive.type() != parameter::type::UNIFORM_BUFFER )
				{
					LOG_CRITICAL(
						"primitive (%s) expected to be (%s) or (%s) but is "
						"(%s) instead",
						parameter.cfg().name.c_str(),
						parameter::to_string( parameter::type::STORAGE_BUFFER )
							.data(),
						parameter::to_string( parameter::type::UNIFORM_BUFFER )
							.data(),
						parameter::to_string( primitive.type() ).data() );
					success = false;
				}
			}
			else if ( parameter.cfg().type != primitive.type() )
			{
				LOG_CRITICAL(
					"primitive (%s) expected to be (%s) but is (%s) instead",
					parameter.cfg().name.c_str(),
					parameter::to_string( parameter.cfg().type ).data(),
					parameter::to_string( primitive.type() ).data() );
				success = false;
			}
			if ( parameter.cfg().array_size != 1 )
			{
				LOG_CRITICAL(
					"(%s) is an array of primitives which is not yet "
					"supported ",
					parameter.cfg().name.c_str() );
				success = false;
			}
		}
	}

	return success;
}

const std::array< uint8_t, parameters::MAX_COUNT >& primitive_indexer::indices()
	const
{
	return _indices;
}
