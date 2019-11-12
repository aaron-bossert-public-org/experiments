
#include "vulkan/sync/vulkan_job_dependencies.h"

#include "vulkan/shader/vulkan_parameters.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_primitives_descriptor.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include "framework/logging/log.h"

using namespace igpu;

namespace
{
	static VkDescriptorPool create_descriptor_pool(
		VkDevice device,
		const parameters& parameters,
		uint32_t max_sets )
	{
		uint32_t uniform_buffers = 0;
		uint32_t storage_buffers = 0;
		uint32_t samplers = 0;

		for ( int i = 0; i < parameters.count(); ++i )
		{
			const auto& cfg = parameters.parameter( i ).cfg();
			switch ( cfg.type )
			{
			case parameter::type::UNIFORM_BUFFER:
				uniform_buffers += (uint32_t)cfg.array_size;
				break;

			case parameter::type::STORAGE_BUFFER:
				storage_buffers += (uint32_t)cfg.array_size;
				break;

			case parameter::type::SAMPLER2D:
				samplers += (uint32_t)cfg.array_size;
				break;
			}
		}

		uint32_t pool_type_count = 0;

		std::array< VkDescriptorPoolSize, 3 > pool_sizes = {};

		if ( uniform_buffers )
		{
			pool_sizes[pool_type_count++] = {
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				uniform_buffers * max_sets,
			};
		}

		if ( storage_buffers )
		{
			pool_sizes[pool_type_count++] = {
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				storage_buffers * max_sets,
			};
		}

		if ( samplers )
		{
			pool_sizes[pool_type_count++] = {
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				samplers * max_sets,
			};
		}

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = pool_type_count;
		pool_info.pPoolSizes = pool_sizes.data();
		pool_info.maxSets = static_cast< uint32_t >( max_sets );

		VkDescriptorPool descriptor_pool = nullptr;
		vkCreateDescriptorPool( device, &pool_info, nullptr, &descriptor_pool );
		return descriptor_pool;
	}

	static VkDescriptorSet allocate_descriptor_set(
		VkDevice device,
		VkDescriptorPool descriptor_pool,
		VkDescriptorSetLayout layout )
	{
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &layout;

		VkDescriptorSet set = nullptr;
		vkAllocateDescriptorSets( device, &alloc_info, &set );
		return set;
	}
}

void vulkan_job_dependencies::activate_read_hazard(
	vulkan_dependency* dependency )
{
	size_t write_index = ( size_t )( dependency - _write_deps.data() );
	size_t read_index = ( size_t )( dependency - _read_deps.data() );

	if ( write_index < _write_deps.size() )
	{
		LOG_CRITICAL( "cannot activate write dependency as read dependency" );
	}
	else if ( read_index >= _read_deps.size() )
	{
		LOG_CRITICAL( "read dependency does not belong to this generator" );
	}
	else if ( !dependency->active() )
	{
		dependency->active( true );
		_read_hazards.push_back( dependency );
	}
}

void vulkan_job_dependencies::process_dependencies(
	vulkan_barrier_manager* barrier_manager )
{
	for ( auto& write_dependency : _write_deps )
	{
		barrier_manager->process_dependency( &write_dependency );
	}

	for ( auto* read_hazard : _read_hazards )
	{
		barrier_manager->process_dependency( read_hazard );
	}

	_read_hazards.clear();
}

void vulkan_job_dependencies::on_reallocate_gpu_object( vulkan_dependency* )
{
	if ( false == _descriptors_dirty )
	{
		_descriptors_dirty = true;
		_swap_index = ( _swap_index + 1 ) % _descriptor_sets.size();
	}
}

VkDescriptorSet vulkan_job_dependencies::descriptor_set()
{
	VkDescriptorSet descriptor_set = _descriptor_sets[_swap_index];

	if ( _descriptors_dirty )
	{
		_descriptors_dirty = false;

		for ( auto* deps : {
				  &_write_deps,
				  &_read_deps,
			  } )
		{
			for ( const vulkan_dependency& dep : *deps )
			{
				dep.resource().update_descriptor_set(
					descriptor_set,
					dep.parameter_config(),
					0 );
			}
		}
	}
	return descriptor_set;
}

std::shared_ptr< vulkan_job_dependencies > vulkan_job_dependencies::make(
	const config& cfg )
{
	vulkan_primitives_descriptor primitives_descriptor;

	if ( primitives_descriptor.reset( *cfg.parameters, *cfg.primitives ) )
	{
		auto shared = std::make_shared< vulkan_job_dependencies >(
			this_is_private{ cfg } );

		auto& read_deps = shared->_read_deps;
		auto& write_deps = shared->_write_deps;
		auto& descriptor_pool = shared->_descriptor_pool;
		auto& descriptor_sets = shared->_descriptor_sets;

		bool success = true;

		descriptor_pool = create_descriptor_pool(
			cfg.device,
			*cfg.parameters,
			(uint32_t)cfg.swap_count );

		for ( size_t i = 0; i < cfg.swap_count; ++i )
		{
			descriptor_sets[i] = allocate_descriptor_set(
				cfg.device,
				descriptor_pool,
				cfg.parameters->cfg().vk.layout );

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

			for ( size_t i = 0; i < cfg.parameters->count(); ++i )
			{
				// WARNING!
				// If we pass an array larger than the size specified in the
				// shader for a particular binding, the overflow will written to
				// subsequent shader bindings, cascading untill all given
				// descriptors are bound.

				// taken from the spec on VkWriteDescriptorSet:
				// If the dstBinding has fewer than descriptorCount array
				// elements remaining starting from dstArrayElement, then the
				// remainder will be used to update the subsequent binding -
				// dstBinding+1 starting at array element zero. If a binding has
				// a descriptorCount of zero, it is skipped. This behavior
				// applies recursively, with the update affecting consecutive
				// bindings as needed to update all descriptorCount descriptors.

				size_t prim_idx = indexer.indices()[i];
				const vulkan_parameter& parameter =
					cfg.parameters->parameter( i );
				const vulkan_primitive& primitive =
					cfg.primitives->primitive( prim_idx );

				vulkan_job_scope job_scope = {
					parameter.cfg().vk.pipeline_stages,
				};

				if ( 0 != ( parameter.cfg().decorators & decorator::READABLE ) )
				{
					job_scope.access |= VK_ACCESS_SHADER_READ_BIT;
				}
				if ( 0 != ( parameter.cfg().decorators & decorator::WRITABLE ) )
				{
					job_scope.access |= VK_ACCESS_SHADER_WRITE_BIT;
				}
				if ( parameter.cfg().type == parameter::type::UNIFORM_BUFFER )
				{
					job_scope.access |= VK_ACCESS_UNIFORM_READ_BIT;
				}
				if ( std::holds_alternative< std::shared_ptr< vertex_buffer > >(
						 primitive.cfg().value ) )
				{
					job_scope.access |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
				}
				if ( std::holds_alternative< std::shared_ptr< index_buffer > >(
						 primitive.cfg().value ) )
				{
					job_scope.access |= VK_ACCESS_INDEX_READ_BIT;
				}

				auto& deps = job_scope.is_read_only() ? read_deps : write_deps;

				deps.emplace_back(
					parameter.cfg().vk.image_layout,
					job_scope,
					shared,
					parameter.cfg() );
			}

			return shared;
		}
	}

	return nullptr;
}

vulkan_job_dependencies ::~vulkan_job_dependencies()
{
	vkDestroyDescriptorPool( _cfg.device, _descriptor_pool, nullptr );
}

vulkan_job_dependencies::vulkan_job_dependencies( const this_is_private& priv )
	: _cfg( priv.cfg )
{}
