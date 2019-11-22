#include "vulkan/sync/vulkan_job_primitives.h"

#include "vulkan/shader/vulkan_descriptor_pool.h"
#include "vulkan/shader/vulkan_parameters.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_primitives_descriptor.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job.h"

#include "framework/logging/log.h"

using namespace igpu;

const vulkan_job_primitives::config& vulkan_job_primitives::cfg() const
{
	return _cfg;
}

vulkan_job_dependencies::state& vulkan_job_primitives::job_dependency_state()
{
	return _state;
}

const vulkan_job_dependencies::state& vulkan_job_primitives::
	job_dependency_state() const
{
	return _state;
}

vulkan_job& vulkan_job_primitives::job()
{
	return *_cfg.job;
}

const vulkan_job& vulkan_job_primitives::job() const
{
	return *_cfg.job;
}

std::unique_ptr< vulkan_job_primitives > vulkan_job_primitives::make(
	const config& cfg )
{
	if ( !cfg.queue )
	{
		LOG_CRITICAL( "queue is null" );
	}
	else if ( !cfg.swap_count )
	{
		LOG_CRITICAL( "swap count is zero" );
	}
	else if ( !cfg.parameters )
	{
		LOG_CRITICAL( "parameters is null" );
	}
	else if ( !cfg.primitives )
	{
		LOG_CRITICAL( "primitives is null" );
	}
	else
	{
		vulkan_primitives_descriptor primitives_descriptor;

		if ( primitives_descriptor.reset( *cfg.parameters, *cfg.primitives ) )
		{
			auto unique = std::unique_ptr< vulkan_job_primitives >(
				new vulkan_job_primitives( cfg ) );

			auto& descriptor_pool = unique->_descriptor_pool;
			auto& descriptor_sets = unique->_descriptor_sets;
			auto& read_deps = unique->_state.read_deps;
			auto& write_deps = unique->_state.write_deps;
			auto& read_parameter_cfgs = unique->_read_parameter_cfgs;
			auto& write_parameter_cfgs = unique->_write_parameter_cfgs;

			bool success = true;

			descriptor_sets.resize( cfg.swap_count );
			descriptor_pool = vulkan_descriptor_pool::make(
				cfg.queue,
				(VkDescriptorPoolCreateFlags)0,
				*cfg.parameters,
				(uint32_t)cfg.swap_count );

			for ( size_t i = 0; i < cfg.swap_count; ++i )
			{
				descriptor_sets[i] = descriptor_pool->allocate_descriptor_set();

				if ( false ==
					 primitives_descriptor.bind_descriptor_set(
						 descriptor_sets[i] ) )
				{
					success = false;
					break;
				}
			}

			if ( success )
			{
				const auto& indexer = primitives_descriptor.indexer();

				size_t writable_count = 0;
				size_t readable_count = 0;
				for ( size_t i = 0; i < cfg.parameters->count(); ++i )
				{
					const vulkan_parameter& parameter =
						cfg.parameters->parameter( i );
					if ( 0 !=
						 ( parameter.cfg().decorators & decorator::WRITABLE ) )
					{
						++writable_count;
					}
					else
					{
						++readable_count;
					}
				}

				write_deps.reserve( writable_count );
				read_deps.reserve( readable_count );

				for ( size_t i = 0; i < cfg.parameters->count(); ++i )
				{
					// WARNING!
					// If we pass an array larger than the size specified in the
					// shader for a particular binding, the overflow will
					// written to subsequent shader bindings, cascading untill
					// all given descriptors are bound.

					// taken from the spec on VkWriteDescriptorSet:
					// If the dstBinding has fewer than descriptorCount array
					// elements remaining starting from dstArrayElement, then
					// the remainder will be used to update the subsequent
					// binding - dstBinding+1 starting at array element zero. If
					// a binding has a descriptorCount of zero, it is skipped.
					// This behavior applies recursively, with the update
					// affecting consecutive bindings as needed to update all
					// descriptorCount descriptors.

					size_t prim_idx = indexer.indices()[i];
					const vulkan_parameter& parameter =
						cfg.parameters->parameter( i );
					const vulkan_primitive& primitive =
						cfg.primitives->primitive( prim_idx );

					VkAccessFlags access = 0;
					if ( 0 !=
						 ( parameter.cfg().decorators & decorator::READABLE ) )
					{
						access |= VK_ACCESS_SHADER_READ_BIT;
					}
					if ( 0 !=
						 ( parameter.cfg().decorators & decorator::WRITABLE ) )
					{
						access |= VK_ACCESS_SHADER_WRITE_BIT;
					}
					if ( parameter.cfg().type ==
						 parameter::type::UNIFORM_BUFFER )
					{
						access |= VK_ACCESS_UNIFORM_READ_BIT;
					}
					if ( std::holds_alternative<
							 std::shared_ptr< vertex_buffer > >(
							 primitive.cfg().value ) )
					{
						access |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
					}
					if ( std::holds_alternative<
							 std::shared_ptr< index_buffer > >(
							 primitive.cfg().value ) )
					{
						access |= VK_ACCESS_INDEX_READ_BIT;
					}

					vulkan_job_scope job_scope = {
						parameter.cfg().decorators,
						parameter.cfg().vk.pipeline_stages,
						(VkAccessFlagBits)access,
					};


					bool is_writable = job_scope.is_writable();

					auto& deps = is_writable ? write_deps : read_deps;
					auto& cfgs = is_writable ? write_parameter_cfgs
											 : read_parameter_cfgs;

					deps.emplace_back(
						primitive.cfg().vk.resource,
						unique.get(),
						parameter.cfg().vk.image_layout,
						job_scope );

					cfgs.push_back( parameter.cfg() );
				}

				ASSERT_CONTEXT(
					write_deps.size() == writable_count,
					"vector cannot be reallocated once dependencies are "
					"created" );
				ASSERT_CONTEXT(
					read_deps.size() == readable_count,
					"vector cannot be reallocated once dependencies are "
					"created" );

				if ( unique->is_activated() )
				{
					unique->job().activate_dependencies( unique.get() );
				}
				return unique;
			}
		}
	}

	return nullptr;
}

vulkan_job_primitives ::~vulkan_job_primitives()
{
	if ( is_activated() )
	{
		job().deactivate_dependencies( this );
	}
}

vulkan_job_primitives::vulkan_job_primitives( const config& cfg )
	: _cfg( cfg )
{}

void vulkan_job_primitives::record_cmds(
	const scoped_ptr< vulkan_command_buffer >& command_buffer )
{
	VkDescriptorSet descriptor_set = _descriptor_sets[_swap_index];

	if ( _descriptors_dirty )
	{
		_descriptors_dirty = false;

		for ( size_t i = 0; i < _state.write_deps.size(); ++i )
		{
			const auto& dep = _state.write_deps[i];
			const auto& cfg = _write_parameter_cfgs[i];

			dep.resource().update_descriptor_set( descriptor_set, cfg, 0 );
		}

		for ( size_t i = 0; i < _state.read_deps.size(); ++i )
		{
			const auto& dep = _state.read_deps[i];
			const auto& cfg = _read_parameter_cfgs[i];

			dep.resource().update_descriptor_set( descriptor_set, cfg, 0 );
		}
	}

	vkCmdBindDescriptorSets(
		command_buffer->vk_cmds(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_cfg.pipeline_layout,
		(uint32_t)_cfg.descriptor_index,
		1,
		&descriptor_set,
		0,		   // dynamic offset count
		nullptr ); // dynamic offsets
}

void vulkan_job_primitives::on_resource_reinitialized(
	vulkan_dependency* dependency )
{
	size_t write_index = ( size_t )( dependency - _state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - _state.read_deps.data() );

	if ( write_index >= _state.write_deps.size() &&
		 read_index >= _state.read_deps.size() )
	{
		LOG_CRITICAL( "dependency does not belong to these job dependencies" );
	}
	else if ( false == _descriptors_dirty )
	{
		_descriptors_dirty = true;
		_swap_index = ( _swap_index + 1 ) % _descriptor_sets.size();
	}
}