#include "vulkan/shader/vulkan_attribute_sequencer.h"

#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_vertex_parameters.h"

#include "igpu/shader/attribute_indexer.h"

using namespace igpu;

bool vulkan_attribute_sequencer::reset(
	const vulkan_vertex_parameters& vertex_parameters,
	const vulkan_geometry& geometry )
{
	_binding_description_count = 0;
	if ( !_indexer.reset( vertex_parameters, geometry ) )
	{
		memset( this, 0, sizeof *this );
		return false;
	}

	std::array<
		VkVertexInputBindingDescription*,
		attribute_indexer::MAX_PARAMETERS >
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
		attribute_description->location = param.cfg().spv.location;
		attribute_description->format = param.cfg().vk.format;
		attribute_description->offset = attr_cfg.offset;
	}

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertex_input_info.vertexBindingDescriptionCount =
		_binding_description_count;
	vertex_input_info.pVertexBindingDescriptions = _binding_descriptions.data();

	vertex_input_info.vertexAttributeDescriptionCount =
		(uint32_t)vertex_parameters.count();
	vertex_input_info.pVertexAttributeDescriptions =
		_attribute_descriptions.data();

	return true;
}

const attribute_indexer& vulkan_attribute_sequencer::indexer() const
{
	return _indexer;
}

uint32_t vulkan_attribute_sequencer::binding_description_count() const
{
	return _binding_description_count;
}

const VkVertexInputBindingDescription* vulkan_attribute_sequencer::
	binding_descriptions() const
{
	return _binding_descriptions.data();
}

const VkVertexInputAttributeDescription* vulkan_attribute_sequencer::
	attribute_descriptions() const
{
	return _attribute_descriptions.data();
}