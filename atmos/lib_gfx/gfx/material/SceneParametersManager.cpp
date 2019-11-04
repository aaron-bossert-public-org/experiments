
#include "gfx/material/parameter_block.h"
#include "gfx/material/scene_parameters_manager.h"
#include "gfx/material/scene_program_binding.h"

using namespace gfx;

void scene_parameters_manager::set_scene_parameters(
	const std::shared_ptr< parameter_block >& parameter_block )
{
	if ( !parameter_block )
	{
		LOG_CONTEXT( CRITICAL, "scene parameter block is null" );
	}
	else
	{
		ASSERT_CONTEXT( parameter_block->slow_hash() );
		const auto& primitives = parameter_block->primitives();

		for ( const auto& handle : parameter_block->handles() )
		{
			const std::string& name = handle.first;
			int index = handle.second;

			if ( parameter_block->slow_hash() )
			{
				_set( name, primitives[index] );
			}
			else
			{
				set( name, primitives[index] );
			}
		}
	}
}

const igpu::primitive& scene_parameters_manager::get( const std::string& name )
{
	return _scene_parameters[name];
}

void scene_parameters_manager::_set(
	const std::string& name,
	const igpu::primitive& primitive )
{
	ASSERT_CONTEXT( igpu::primitive::TYPE_UNDEFINED != primitive.get_type() );

	if ( !_enable_scene_parameter_updates )
		return;

	auto& current = _scene_parameters[name];

	if ( primitive.hash() == current.hash() )
		return;

	current = primitive;

	for ( const auto& pair : _scene_program_binding_map )
	{
		auto scene_program_binding = pair.second.lock();
		ASSERT_CONTEXT( scene_program_binding );
		if ( scene_program_binding )
		{
			scene_program_binding->update( name, primitive );
		}
	}
}

void scene_parameters_manager::enable_scene_parameter_updates(
	bool enable_scene_parameter_updates )
{
	if ( enable_scene_parameter_updates && !_enable_scene_parameter_updates )
	{
		// update all programs with the current scene parameters after coming
		// back from disabled parameter updates
		for ( const auto& pair : _scene_program_binding_map )
		{
			auto scene_program_binding = pair.second.lock();
			ASSERT_CONTEXT( scene_program_binding );
			if ( scene_program_binding )
			{
				for ( auto& pair : _scene_parameters )
				{
					scene_program_binding->update( pair.first, pair.second );
				}
			}
		}
	}

	_enable_scene_parameter_updates = enable_scene_parameter_updates;
}

const std::unordered_map< std::string, igpu::primitive >&
	scene_parameters_manager::scene_parameters() const
{
	return _scene_parameters;
}

std::shared_ptr< scene_program_binding > scene_parameters_manager::
	scene_program_binding( const std::shared_ptr< igpu::program >& program )
{
	ASSERT_CONTEXT( program );
	if ( !program )
	{
		return nullptr;
	}

	auto weak_binding = _scene_program_binding_map[program.get()];

	if ( auto binding = weak_binding.lock() )
	{
		return binding;
	}

	std::weak_ptr< scene_parameters_manager > weak = shared_from_this();
	ASSERT_CONTEXT( weak.lock() );

	std::shared_ptr< scene_program_binding > scene_program_binding(
		scene_program_binding::make( weak, program ).release(),
		[weak]( scene_program_binding* binding ) {
			if ( auto this_ = weak.lock() )
			{
				this_->_scene_program_binding_map.erase(
					binding->program().get() );
			}

			delete binding;
		} );

	_scene_program_binding_map[program.get()] = scene_program_binding;

	return scene_program_binding;
}

std::shared_ptr< scene_parameters_manager > scene_parameters_manager::make()
{
	return std::unique_ptr< scene_parameters_manager >(
		new scene_parameters_manager() );
}

scene_parameters_manager::scene_parameters_manager()
{
#define INITIALIZE( param, type ) set( #param, _##param );

	SCENE_PARAMETERS_TUP( INITIALIZE );
}
