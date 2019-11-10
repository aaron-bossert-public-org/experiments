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
	VkDescriptorSet descriptor_set,
	const vulkan_parameters& parameters,
	const vulkan_primitives& primitives )
{
	if ( !_indexer.reset( parameters, primitives ) )
	{
		memset( this, 0, sizeof *this );
		return false;
	}

	_image_count = 0;
	_buffer_count = 0;


	_descriptor_set = descriptor_set;
	_parameters = &parameters;
	_primitives = &primitives;

	bool success = true;

	for ( size_t i = 0; i < parameters.count(); ++i )
	{
		// WARNING!
		// If we pass too many descriptors for a binding, the overflow will
		// written to subsequent bindings, cascading untill all given
		// descriptors are bound.

		// taken from the spec on VkWriteDescriptorSet:
		// If the dstBinding has fewer than descriptorCount array elements
		// remaining starting from dstArrayElement, then the remainder will be
		// used to update the subsequent binding - dstBinding+1 starting at
		// array element zero. If a binding has a descriptorCount of zero, it is
		// skipped. This behavior applies recursively, with the update affecting
		// consecutive bindings as needed to update all descriptorCount
		// descriptors.

		size_t prim_idx = _indexer.indices()[i];
		const auto& parameter = parameters.parameter( i );
		const auto& primitive = primitives.primitive( prim_idx );

		if ( 1 != parameter.cfg().array_size )
		{
			LOG_CRITICAL(
				"binding arrays not implemented, parameter:%s",
				parameter.cfg().name.c_str() );
			success = false;
		}
		else
		{
			std::visit(
				[&]( auto&& gpu_object ) {
					if ( !append( parameter.cfg(), *gpu_object ) )
					{
						success = false;
					}
				},
				primitive.cfg().vk.gpu_value );
		}
	}

	return success;
}

const primitive_indexer& vulkan_primitives_descriptor::indexer() const
{
	return _indexer;
}

uint32_t vulkan_primitives_descriptor::image_count() const
{
	return _image_count;
}

uint32_t vulkan_primitives_descriptor::buffer_count() const
{
	return _buffer_count;
}

const vulkan_primitives_descriptor::image_descriptors_t&
	vulkan_primitives_descriptor::image_descriptors() const
{
	return _image_descriptors;
}

const vulkan_primitives_descriptor::buffer_descriptors_t&
	vulkan_primitives_descriptor::buffer_descriptors() const
{
	return _buffer_descriptors;
}

const vulkan_primitives_descriptor::write_descriptors_t&
	vulkan_primitives_descriptor::write_descriptors() const
{
	return _write_descriptors;
}

const vulkan_parameters* vulkan_primitives_descriptor::parameters() const
{
	return _parameters;
}

const vulkan_primitives* vulkan_primitives_descriptor::primitives() const
{
	return _primitives;
}

VkWriteDescriptorSet* vulkan_primitives_descriptor::next_write_descriptor()
{
	auto* write_descriptor = &_write_descriptors[_image_count + _buffer_count];

	*write_descriptor = {
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		_descriptor_set,
	};

	return write_descriptor;
}

bool vulkan_primitives_descriptor::append(
	const vulkan_parameter& parameter,
	const vulkan_image& image )
{
	VkWriteDescriptorSet* write_descriptor = next_write_descriptor();

	write_descriptor->dstBinding = (uint32_t)parameter.cfg().binding;
	write_descriptor->dstArrayElement = 0;
	write_descriptor->descriptorCount = (uint32_t)parameter.cfg().array_size;
	write_descriptor->descriptorType = parameter.cfg().vk.descriptor_type;

	VkDescriptorImageInfo* image_descriptor = &_image_descriptors[_image_count];
	write_descriptor->pImageInfo = image_descriptor;
	++_image_count;

	*image_descriptor = {
		image.sampler(),
		image.image_view(),
		parameter.cfg().vk.image_layout,
	};

	return true;
}

bool vulkan_primitives_descriptor::append(
	const vulkan_parameter& parameter,
	const vulkan_buffer& buffer )
{
	if ( parameter.cfg().type == parameter::type::UNIFORM_BUFFER &&
		 buffer.mapped_view().byte_size() >
			 buffer.cfg().vk.device_properties->limits.maxUniformBufferRange )
	{
		LOG_CRITICAL(
			"uniform buffer size limit of current GPU exceeded. To fix this "
			"switch from 'uniform' to 'buffer' storage in shader, or reduce "
			"the size of your compute buffer.\n"
			"parameter:%s\nlimit:%d bytes\nbound buffer size: %d bytes",
			parameter.cfg().name.c_str(),
			(int)buffer.cfg()
				.vk.device_properties->limits.maxUniformBufferRange,
			(int)buffer.mapped_view().byte_size() );
		return false;
	}
	VkWriteDescriptorSet* write_descriptor = next_write_descriptor();

	write_descriptor->dstBinding = (uint32_t)parameter.cfg().binding;
	write_descriptor->dstArrayElement = 0;
	write_descriptor->descriptorCount = (uint32_t)parameter.cfg().array_size;
	write_descriptor->descriptorType = parameter.cfg().vk.descriptor_type;

	VkDescriptorBufferInfo* buffer_descriptor =
		&_buffer_descriptors[_buffer_count];
	write_descriptor->pBufferInfo = buffer_descriptor;
	++_buffer_count;

	*buffer_descriptor = {
		buffer.get(),
		0,
		VK_WHOLE_SIZE,
	};

	return true;
}
