
#include "gfx/material/parameter_block_bindings.h"

namespace gfx;

parameter_block_bindings::parameter_block_bindings(
	const parameter_block& parameter_block )
	: _parameter_block( parameter_block )
{}

const parameter_block& parameter_block_bindings::parameter_block() const
{
	return _parameter_block;
}

const std::vector< parameter_binding >& parameter_block_bindings::
	parameter_bindings( const igpu::program& program )
{
	const auto program_binding_id = program.binding_id();
	const auto parameter_block_binding_id = _parameter_block.binding_id();

	if ( program_binding_id != _program_binding_id ||
		 parameter_block_binding_id != _parameter_block_binding_id )
	{
		_program_binding_id = program_binding_id;
		_parameter_block_binding_id = parameter_block_binding_id;

		const auto& primitives = _parameter_block.primitives();
		_parameter_bindings.resize( primitives.size() );

		for ( const auto& handle : _parameter_block.handles() )
		{
			const auto& name = handle.first;
			const auto& index = handle.second;
			const auto* primitive = &primitives[handle.second];

			_parameter_bindings[index].set( name, primitive, program );
		}
	}

	return _parameter_bindings;
}
