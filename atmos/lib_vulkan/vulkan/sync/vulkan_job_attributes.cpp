
#include "vulkan/sync/vulkan_job_attributes.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_vertex_parameters.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job.h"

#include <array>

using namespace igpu;

const vulkan_job_attributes::config& vulkan_job_attributes::cfg() const
{
	return _cfg;
}

vulkan_job_dependencies::state& vulkan_job_attributes::job_dependency_state()
{
	return _state;
}

const vulkan_job_dependencies::state& vulkan_job_attributes::
	job_dependency_state() const
{
	return _state;
}

const vulkan_job& vulkan_job_attributes::job() const
{
	return *_cfg.job;
}

std::shared_ptr< vulkan_job_attributes > vulkan_job_attributes::make(
	const config& cfg )
{
	if ( !cfg.device )
	{
		LOG_CRITICAL( "device is null" );
	}
	else if ( !cfg.swap_count )
	{
		LOG_CRITICAL( "swap count is zero" );
	}
	else if ( !cfg.geometry )
	{
		LOG_CRITICAL( "geometry is null" );
	}
	else
	{
		auto shared = std::shared_ptr< vulkan_job_attributes >(
			new vulkan_job_attributes( cfg ) );

		auto& index_buffer = shared->_index_buffer;
		auto& index_buffer_offset = shared->_index_buffer_offset;
		auto& index_type = shared->_index_type;
		auto& vertex_buffers = shared->_vertex_buffers;
		auto& vertex_buffer_offsets = shared->_vertex_buffer_offsets;
		auto& read_deps = shared->_state.read_deps;

		VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
		scoped_ptr< vulkan_job_dependencies > scoped_ptr = shared;
		vulkan_geometry* geometry = cfg.geometry;

		vertex_buffers.reserve( cfg.active_vertex_buffers.size() );
		vertex_buffer_offsets.reserve( cfg.active_vertex_buffers.size() );
		read_deps.reserve( cfg.active_vertex_buffers.size() + 1 );

		for ( size_t i : cfg.active_vertex_buffers )
		{
			vulkan_vertex_buffer& vertex_buffer = geometry->vertex_buffer( i );
			vertex_buffers.push_back( vertex_buffer.gpu_object().vk_buffer() );
			vertex_buffer_offsets.push_back(
				geometry->cfg().vbuff_byte_offsets[i] );

			vulkan_resource* resource = &vertex_buffer.gpu_object();
			vulkan_job_scope job_scope = {
				decorator::READABLE,
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			};
			read_deps.emplace_back( resource, layout, job_scope, scoped_ptr );
		}

		index_buffer = geometry->index_buffer().gpu_object().vk_buffer();
		index_buffer_offset = geometry->cfg().ibuff_byte_offset;
		index_type = geometry->index_buffer().cfg().vk.index_type;
		vulkan_resource* resource = &geometry->index_buffer().gpu_object();
		vulkan_job_scope job_scope = {
			decorator::READABLE,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INDEX_READ_BIT,
		};

		read_deps.emplace_back( resource, layout, job_scope, scoped_ptr );

		return shared;
	}

	return nullptr;
}

vulkan_job_attributes ::~vulkan_job_attributes()
{}

vulkan_job_attributes::vulkan_job_attributes( const config& cfg )
	: _cfg( cfg )
{}

void vulkan_job_attributes::on_record_cmds(
	const scoped_ptr< vulkan_command_buffer >& command_buffer )
{
	vkCmdBindVertexBuffers(
		command_buffer->vk_cmds(),
		0,
		(uint32_t)_vertex_buffers.size(),
		_vertex_buffers.data(),
		_vertex_buffer_offsets.data() );

	vkCmdBindIndexBuffer(
		command_buffer->vk_cmds(),
		_index_buffer,
		_index_buffer_offset,
		_index_type );
}

void vulkan_job_attributes::on_resource_reinitialized(
	vulkan_dependency* dependency )
{
	size_t write_index = ( size_t )( dependency - _state.write_deps.data() );
	size_t read_index = ( size_t )( dependency - _state.read_deps.data() );

	if ( write_index < _state.write_deps.size() )
	{
		LOG_CRITICAL(
			"something is awry, draw jobs are not supposed to have any write "
			"dependencies" );
	}
	else if ( read_index >= _state.read_deps.size() )
	{
		LOG_CRITICAL( "dependency does not belong to these job dependencies" );
	}
	else if ( read_index > _vertex_buffers.size() )
	{
		LOG_CRITICAL(
			"something is awry, this read index (%d) does not map to a vertex "
			"or index buffer",
			(int)read_index );
	}
	else if ( read_index < _vertex_buffers.size() )
	{
		size_t buffer_index = _cfg.active_vertex_buffers[read_index];
		_vertex_buffers[read_index] =
			_cfg.geometry->vertex_buffer( buffer_index )
				.gpu_object()
				.vk_buffer();
	}
	else
	{
		_index_buffer = _cfg.geometry->index_buffer().gpu_object().vk_buffer();
	}
}
