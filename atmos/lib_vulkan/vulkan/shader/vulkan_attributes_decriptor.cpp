#include "vulkan/shader/vulkan_attributes_decriptor.h"

#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_vertex_parameters.h"

#include "igpu/shader/attribute_indexer.h"

using namespace igpu;

void vulkan_attributes_decriptor::reset( const attribute_indexer& indexer )
{
	_indexer = &indexer;
	_vertex_parameters = ASSERT_CAST(
		const vulkan_vertex_parameters*,
		indexer.vertex_parameters() );
	_geometry = ASSERT_CAST( const vulkan_geometry*, indexer.geometry() );

	for ( uint32_t i = 0; i < _vertex_parameters->count(); ++i )
	{
		const auto& param = _vertex_parameters->parameter( i );
		const auto& source = indexer.indices()[i];

		const auto& buff_cfg =
			_geometry->cfg().vertex_buffers[source.compact_buffer_index]->cfg();
		const auto& attr_cfg = buff_cfg.attributes[source.attribute_index];


		VkVertexInputAttributeDescription* attribute_description =
			&_attribute_descriptions[i];
		attribute_description->binding = source.compact_buffer_index;
		attribute_description->location = (uint32_t)param.cfg().location;
		attribute_description->format = param.cfg().vk.format;
		attribute_description->offset = (uint32_t)attr_cfg.offset;
	}

	for ( uint32_t i = 0; i < indexer.buffer_count(); ++i )
	{
		uint32_t buffer_index = indexer.buffer_indices()[i];
		auto* binding_description = &_binding_descriptions[buffer_index];
		const auto& buff_cfg =
			_geometry->cfg().vertex_buffers[buffer_index]->cfg();

		binding_description->binding = buffer_index;
		binding_description->stride = buff_cfg.stride;
		binding_description->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	_vertex_input_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		(VkFlags)0,
		(uint32_t)indexer.buffer_count(),
		_binding_descriptions.data(),
		(uint32_t)_vertex_parameters->count(),
		_attribute_descriptions.data(),
	};

	_input_assembly = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		(VkFlags)0,
		_input_assembly.topology = _geometry->cfg().vk.topology,
		(bool)"primitiveRestartEnable",
	};
}

const attribute_indexer& vulkan_attributes_decriptor::indexer() const
{
	return *_indexer;
}

const VkPipelineVertexInputStateCreateInfo& vulkan_attributes_decriptor::
	vertex_input_info() const
{
	return _vertex_input_info;
}

const VkPipelineInputAssemblyStateCreateInfo& vulkan_attributes_decriptor::
	input_assembly() const
{
	return _input_assembly;
}

uint32_t vulkan_attributes_decriptor::binding_description_count() const
{
	return (uint32_t)_indexer->buffer_count();
}

const vulkan_attributes_decriptor::binding_descriptions_t&
	vulkan_attributes_decriptor::binding_descriptions() const
{
	return _binding_descriptions;
}

const vulkan_attributes_decriptor::attribute_descriptions_t&
	vulkan_attributes_decriptor::attribute_descriptions() const
{
	return _attribute_descriptions;
}