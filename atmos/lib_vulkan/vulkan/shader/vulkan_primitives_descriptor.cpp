#include "vulkan/shader/vulkan_primitives_descriptor.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/shader/vulkan_parameters.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/texture/vulkan_image.h"

#include "igpu/shader/primitive_indexer.h"

#include "framework/logging/log.h"

using namespace igpu;

namespace
{}

bool vulkan_primitives_descriptor::reset(
	const vulkan_parameters& parameters,
	const vulkan_primitives& primitives )
{
	if ( !_indexer.reset( parameters, primitives ) )
	{
		memset( this, 0, sizeof *this );
		return false;
	}

	_parameters = &parameters;
	_primitives = &primitives;

	return true;
}

bool vulkan_primitives_descriptor::bind_descriptor_set(
	VkDescriptorSet descriptor_set ) const
{
	bool success = true;

	for ( size_t i = 0; i < _parameters->count(); ++i )
	{
		// WARNING!
		// If we pass an array larger than the size specified in the shader for
		// a particular binding, the overflow will written to subsequent
		// shader bindings, cascading untill all given descriptors are bound.

		// taken from the spec on VkWriteDescriptorSet:
		// If the dstBinding has fewer than descriptorCount array elements
		// remaining starting from dstArrayElement, then the remainder will be
		// used to update the subsequent binding - dstBinding+1 starting at
		// array element zero. If a binding has a descriptorCount of zero, it is
		// skipped. This behavior applies recursively, with the update affecting
		// consecutive bindings as needed to update all descriptorCount
		// descriptors.

		size_t prim_idx = _indexer.indices()[i];
		const auto& parameter = _parameters->parameter( i );
		const auto& primitive = _primitives->primitive( prim_idx );

		if ( 1 != parameter.cfg().array_size )
		{
			LOG_CRITICAL(
				"binding arrays not implemented, parameter:%s",
				parameter.cfg().name.c_str() );
			success = false;
		}
		else
		{
			primitive.cfg().vk.resource->update_descriptor_set(
				descriptor_set,
				parameter.cfg(),
				0 );
		}
	}
	return success;
}

const primitive_indexer& vulkan_primitives_descriptor::indexer() const
{
	return _indexer;
}

const vulkan_parameters* vulkan_primitives_descriptor::parameters() const
{
	return _parameters;
}

const vulkan_primitives* vulkan_primitives_descriptor::primitives() const
{
	return _primitives;
}