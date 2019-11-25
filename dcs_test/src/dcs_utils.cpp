
#define GLM_ENABLE_EXPERIMENTAL 1
#define TINYOBJLOADER_IMPLEMENTATION

#include "dcs_test/src/dcs_utils.h"

#include "igpu/buffer/buffer.h"
#include "igpu/buffer/geometry.h"
#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/context/context.h"

#include "framework/utility/buffer_view.h"

#include "glm/gtx/hash.hpp"
#include <fstream>
#include <tinyobjloader/tiny_obj_loader.h>

using namespace igpu;
using namespace dcs_utils;
namespace
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 col;
		glm::vec2 uv0;

		bool operator==( const Vertex& other ) const
		{
			return pos == other.pos && col == other.col && uv0 == other.uv0;
		}
	};
}

namespace std
{
	template <>
	struct hash< Vertex >
	{
		size_t operator()( Vertex const& vertex ) const
		{
			return ( ( hash< glm::vec3 >()( vertex.pos ) ^
					   ( hash< glm::vec3 >()( vertex.col ) << 1 ) ) >>
					 1 ) ^
				( hash< glm::vec2 >()( vertex.uv0 ) << 1 );
		}
	};
}


bool dcs_utils::load_buffer( const std::string& path, buffer* out_buffer )
{
	const char* err = nullptr;
	std::ifstream ifs( path.c_str(), std::ios::binary | std::ios::ate );

	if ( !out_buffer )
	{
		err = "out buffer is null";
	}
	else if ( !ifs )
	{
		err = "failed to find file";
	}
	else
	{
		buffer_view< char > view( ifs.tellg(), nullptr );
		out_buffer->map( &view );

		if ( nullptr == view.data() )
		{
			err =
				"could not obtain mapped memory from buffer to copy file "
				"into";

			ifs.close();
		}
		else
		{
			ifs.seekg( 0, std::ios::beg );
			ifs.read( view.data(), view.byte_size() );
			ifs.close();

			out_buffer->unmap();
		}
	}

	if ( err )
	{
		LOG_CRITICAL( "%s : file '%s'", err, path.c_str() );
	}


	return true;
}

std::vector< uint32_t > dcs_utils::load_mem( const std::string& path )
{
	const char* err = nullptr;
	std::ifstream ifs( path.c_str(), std::ios::binary | std::ios::ate );

	if ( !ifs )
	{
		err = "failed to find file";
	}
	else
	{
		size_t byte_size = ifs.tellg();
		std::vector< uint32_t > memory( ( byte_size + 3 ) / 4 );

		ifs.seekg( 0, std::ios::beg );
		ifs.read( (char*)memory.data(), byte_size );
		ifs.close();

		return memory;
	}

	if ( err )
	{
		LOG_CRITICAL( "%s : file '%s'", err, path.c_str() );
	}


	return {};
}

std::shared_ptr< geometry > dcs_utils::load_model(
	igpu::context* context,
	const std::string_view& model_path )
{
	std::vector< Vertex > vertices;
	std::vector< uint32_t > indices;

	tinyobj::attrib_t attrib;
	std::vector< tinyobj::shape_t > shapes;
	std::vector< tinyobj::material_t > materials;
	std::string warn, err;

	if ( !tinyobj::LoadObj(
			 &attrib,
			 &shapes,
			 &materials,
			 &warn,
			 &err,
			 model_path.data() ) )
	{
		if ( !warn.empty() )
		{
			LOG_WARNING( "tinyobj :", warn.c_str() );
		}
		if ( !err.empty() )
		{
			LOG_CRITICAL( "tinyobj :", err.c_str() );
		}

		return false;
	}

	std::unordered_map< Vertex, size_t > unique_vertices = {};

	for ( const auto& shape : shapes )
	{
		for ( const auto& index : shape.mesh.indices )
		{
			auto emplace = unique_vertices.emplace(
				Vertex( {
					{
						// pos
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					},
					{
						// color
						1.0f,
						1.0f,
						1.0f,
					},
					{
						// texcorrd
						attrib.texcoords[2 * index.texcoord_index + 0],
						1 - attrib.texcoords[2 * index.texcoord_index + 1],
					},
				} ),
				unique_vertices.size() );

			if ( emplace.second )
			{
				vertices.push_back( emplace.first->first );
			}

			indices.push_back( (uint32_t)emplace.first->second );
		}
	}
	auto index_buffer = context->make_shared( index_buffer::config{
		index_format::UNSIGNED_INT,
	} );

	auto vertex_buffer =
		context->make_shared( IGPU_VERT_CFG_OF( Vertex, pos, col, uv0 ) );

	auto geometry = context->make_shared( geometry::config{
		model_path.data(),
		topology::TRIANGLE_LIST,
		index_buffer,
		{
			vertex_buffer,
		},
	} );

	buffer_view< char > vertices_view(
		sizeof( vertices[0] ) * vertices.size(),
		nullptr );

	buffer_view< char > indices_view(
		sizeof( indices[0] ) * indices.size(),
		nullptr );

	geometry->index_buffer().map( &indices_view );
	geometry->vertex_buffer( 0 ).map( &vertices_view );

	if ( !indices_view.data() )
	{
		LOG_CRITICAL( "failed to obtain mapped memory from index buffer" );
	}
	else if ( !vertices_view.data() )
	{
		LOG_CRITICAL( "failed to obtain mapped memory from vertex buffer" );
	}
	else
	{
		memcpy( indices_view.data(), indices.data(), indices_view.byte_size() );
		memcpy(
			vertices_view.data(),
			vertices.data(),
			vertices_view.byte_size() );

		geometry->index_buffer().unmap();
		geometry->vertex_buffer( 0 ).unmap();

		return geometry;
	}

	return nullptr;
}
