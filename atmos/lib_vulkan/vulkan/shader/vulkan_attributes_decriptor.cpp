#include "vulkan/shader/vulkan_attributes_decriptor.h"

#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_vertex_parameters.h"

#include "igpu/shader/attribute_indexer.h"

using namespace igpu;

bool vulkan_attributes_decriptor::reset(
	const vulkan_vertex_parameters& vertex_parameters,
	const vulkan_geometry& geometry )
{
	if ( !_indexer.reset( vertex_parameters, geometry ) )
	{
		memset( this, 0, sizeof *this );
		return false;
	}

	_vertex_parameters = &vertex_parameters;
	_geometry = &geometry;
	_binding_description_count = 0;

	std::array< VkVertexInputBindingDescription*, vertex_parameters::MAX_COUNT >
		binding_table = {};

	for ( uint32_t i = 0; i < vertex_parameters.count(); ++i )
	{
		const auto& param = vertex_parameters.parameter( i );
		const auto& source = _indexer.indices()[i];

		const auto& buff_cfg =
			geometry.cfg().vertex_buffers[source.buffer]->cfg();
		const auto& attr_cfg = buff_cfg.attributes[source.attribute];
		auto** pp_binding_description = &binding_table[source.buffer];

		if ( !*pp_binding_description )
		{
			VkVertexInputBindingDescription* binding_description =
				*pp_binding_description =
					&_binding_descriptions[_binding_description_count];

			binding_description->binding = _binding_description_count;
			binding_description->stride = buff_cfg.stride;
			binding_description->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			_binding_description_count++;
		}

		VkVertexInputAttributeDescription* attribute_description =
			&_attribute_descriptions[i];
		attribute_description->binding = ( *pp_binding_description )->binding;
		attribute_description->location = (uint32_t)param.cfg().location;
		attribute_description->format = param.cfg().vk.format;
		attribute_description->offset = attr_cfg.offset;
	}

	_vertex_input_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
	};

	_vertex_input_info.vertexBindingDescriptionCount =
		_binding_description_count;
	_vertex_input_info.pVertexBindingDescriptions =
		_binding_descriptions.data();

	_vertex_input_info.vertexAttributeDescriptionCount =
		(uint32_t)vertex_parameters.count();
	_vertex_input_info.pVertexAttributeDescriptions =
		_attribute_descriptions.data();


	_input_assembly = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
	};
	_input_assembly.topology = geometry.cfg().vk.topology;

	return true;
}

const attribute_indexer& vulkan_attributes_decriptor::indexer() const
{
	return _indexer;
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
	return _binding_description_count;
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