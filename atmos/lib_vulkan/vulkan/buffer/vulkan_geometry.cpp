
#include "vulkan/buffer/vulkan_geometry.h"

#include "vulkan/buffer/vulkan_buffer.h"

#include "framework/logging/log.h"

using namespace igpu;

namespace
{}

VkPrimitiveTopology igpu::to_vulkan_topology( topology topology )
{
	switch ( topology )
	{
	case topology::POINT_LIST:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case topology::LINE_LIST:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case topology::LINE_STRIP:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case topology::TRIANGLE_LIST:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case topology::TRIANGLE_STRIP:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case topology::TRIANGLE_FAN:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	};

	if ( is_valid( topology ) )
	{
		LOG_CRITICAL( "unhandled format: %s", to_string( topology ).data() );
	}
	else
	{
		LOG_CRITICAL( "invalid format: %d", topology );
	}

	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

const vulkan_geometry::config& vulkan_geometry::cfg() const
{
	return _cfg;
}

igpu::draw_parameters vulkan_geometry::draw_parameters() const
{
	return { (uint32_t)_index_buffer->element_count(), 1, 0, 0, 0 };
}

vulkan_index_buffer& vulkan_geometry::index_buffer()
{
	return *_index_buffer;
}

const vulkan_index_buffer& vulkan_geometry::index_buffer() const
{
	return *_index_buffer;
}

size_t vulkan_geometry::vertex_buffer_count() const
{
	return _vertex_buffers.size();
}

vulkan_vertex_buffer& vulkan_geometry::vertex_buffer( size_t i )
{
	return *_vertex_buffers[i];
}

const vulkan_vertex_buffer& vulkan_geometry::vertex_buffer( size_t i ) const
{
	return *_vertex_buffers[i];
}

std::unique_ptr< vulkan_geometry > vulkan_geometry::make( const config& cfg )
{
	if ( nullptr == cfg.index_buffer )
	{
		LOG_CRITICAL( "index buffer is null" );
	}

	if ( 0 == cfg.vertex_buffers.size() )
	{
		LOG_CRITICAL( "vertex_buffers is empty" );
	}

	auto* vulkan_index_buffer =
		ASSERT_CAST( igpu::vulkan_index_buffer*, cfg.index_buffer.get() );

	std::vector< vulkan_vertex_buffer* > vertex_buffers;
	vertex_buffers.reserve( cfg.vertex_buffers.size() );

	for ( uint32_t i = 0; i < cfg.vertex_buffers.size(); ++i )
	{
		const auto& vertex_buffer = cfg.vertex_buffers[i];
		if ( nullptr == vertex_buffer )
		{
			LOG_CRITICAL( "vertex buffer is null vertex_buffers[%d]", (int)i );

			continue;
		}
		auto vulkan_vertex_buffer =
			ASSERT_CAST( igpu::vulkan_vertex_buffer*, vertex_buffer.get() );
		vertex_buffers.push_back( vulkan_vertex_buffer );
	}

	if ( vulkan_index_buffer && vertex_buffers.size() &&
		 vertex_buffers.size() == cfg.vertex_buffers.size() )
	{
		return std::unique_ptr< vulkan_geometry >( new vulkan_geometry(
			cfg,
			vulkan_index_buffer,
			std::move( vertex_buffers ) ) );
	}

	return nullptr;
}

vulkan_geometry::~vulkan_geometry()
{}

vulkan_geometry::vulkan_geometry(
	const config& cfg,
	igpu::vulkan_index_buffer* const vulkan_index_buffer,
	std::vector< vulkan_vertex_buffer* > vertex_buffers )
	: _cfg( cfg )
	, _index_buffer( vulkan_index_buffer )
	, _vertex_buffers( std::move( vertex_buffers ) )
{}
