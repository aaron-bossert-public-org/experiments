
#define GLM_ENABLE_EXPERIMENTAL 1

#include "dcs_test/include/dcs_test.h"

#include "igpu/context/context.h"
#include "igpu/window/window.h"

#include "framework/logging/log.h"
#include "framework/utility/buffer_view.h"
#include "framework/utility/scoped_ptr.h"

#include "framework_tests/main/tests.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION

#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include <tinyobjloader/tiny_obj_loader.h>

using namespace igpu;

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 uv0;

	bool operator==( const Vertex& other ) const
	{
		utility::center( {} );
		return pos == other.pos && col == other.col && uv0 == other.uv0;
	}
};

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

namespace
{
	[[nodiscard]] bool load_buffer(
		const std::string& path,
		buffer* out_buffer )
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

	[[nodiscard]] std::shared_ptr< geometry > load_model(
		const dcs_test::config& cfg )
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
				 cfg.model_path.c_str() ) )
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
		auto index_buffer = cfg.context->make_shared( index_buffer::config{
			index_format::UNSIGNED_INT,
		} );

		vertex_buffer::attribute attr = {
			"pos",
			to_components( ( *( (Vertex*)0 ) ).pos ),
			( ( size_t ) & ( ( (Vertex*)0 )->pos ) ),
		};

		auto vertex_buffer = cfg.context->make_shared(
			IGPU_VERT_CFG_OF( Vertex, pos, col, uv0 ) );

		auto geometry = cfg.context->make_shared( geometry::config{
			cfg.model_path.c_str(),
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
			memcpy(
				indices_view.data(),
				indices.data(),
				indices_view.byte_size() );
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
}


void port_example();


std::unique_ptr< dcs_test > dcs_test::make( const config& cfg )
{
	auto results = tests::run();
	if ( results.success() )
	{
		std::cout << "tests successful" << std::endl;
	}
	else
	{
		LOG_CRITICAL( results.failures().c_str() );
	}

	if ( !cfg.context )
	{
		LOG_CRITICAL( "context is null" );
	}
	else
	{
		state st;

		auto v_shader = cfg.context->make_shared( vertex_shader::config{} );
		auto f_shader = cfg.context->make_shared( fragment_shader::config{} );
		st.batch_data = cfg.context->make_shared( compute_buffer::config{} );
		st.instance_data = cfg.context->make_shared( compute_buffer::config{} );

		st.render_states = cfg.context->make_shared( render_states::config{
			0b1111, // color write mask
			{ "cull_enabled", cull::BACK },
			{ !"blend_enabled" },
			{ !"stencil_enabled" },
			{ "depth_enabled", "depth_write", compare::LESS },
		} );

		st.texture = cfg.context->make_shared( texture2d::config{
			cfg.texture_path.c_str(),
			sampler::filter::LINEAR,
			sampler::filter::LINEAR,
			sampler::address::WRAP,
			sampler::address::WRAP,
			"can_auto_generate_mips",
		} );

		load_buffer( st.texture->cfg().name, st.texture.get() );
		load_buffer( cfg.vertex_path.c_str(), v_shader.get() );
		load_buffer( cfg.fragment_path.c_str(), f_shader.get() );

		st.program = cfg.context->make_shared( program::config{
			"test program",
			v_shader,
			f_shader,
		} );

		st.geometry = load_model( cfg );

		struct
		{
		}* test = nullptr;

		st.batch_data->map( &test );
		st.batch_data->unmap();

		st.instance_data->map( &test );
		st.instance_data->unmap();

		st.opaque_batch = cfg.context->make_shared( opaque_batch::config{
			cfg.context->back_buffer(),
			cfg.context->make_shared( primitives::config{ {
				{ "batch_data", st.batch_data },
			} } ),
		} );

		st.batch_binding = st.opaque_batch->make_binding( {
			st.program,
			st.render_states,
			st.geometry,
			cfg.context->make_shared( primitives::config{ {
				{ "texSampler", st.texture },
			} } ),
			cfg.context->make_shared( primitives::config{ {
				{ "instance_data", st.instance_data },
			} } ),
		} );

		return std::unique_ptr< dcs_test >(
			new dcs_test( cfg, std::move( st ) ) );
	}

	return nullptr;
}

dcs_test::dcs_test( const config& cfg, state&& st )
	: _cfg( cfg )
	, _st( std::move( st ) )
{}

dcs_test::~dcs_test()
{}

bool dcs_test::advance()
{
	update( _cfg, _st );

	if ( _cfg.context->window()->poll_events() )
	{
		handle_input();

		render();

		return true;
	}

	return false;
}

void dcs_test::update( const config& cfg, const state& st )
{
	// update view matrices
	auto back_buffer = cfg.context->back_buffer();
	auto res = cfg.context->back_buffer()->res();

	struct
	{
		alignas( 16 ) glm::mat4 view;
		alignas( 16 ) glm::mat4 proj;
	}* batch_ubo = nullptr;

	st.batch_data->map( &batch_ubo );

	batch_ubo->view = glm::lookAt(
		glm::vec3( 2.0f, 2.0f, 2.0f ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ) );
	batch_ubo->proj = glm::perspective(
		glm::radians( 45.0f ),
		res.x / (float)res.y,
		0.1f,
		10.0f );

	st.batch_data->unmap();

	// update model transform
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration< float, std::chrono::seconds::period >(
					 current_time - start_time )
					 .count();

	struct
	{
		alignas( 16 ) glm::mat4 model;
	}* instance_ubo = nullptr;

	st.instance_data->map( &instance_ubo );

	instance_ubo->model = glm::rotate(
		glm::mat4( 1.0f ),
		time * glm::radians( 90.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ) );

	st.instance_data->unmap();
}

void dcs_test::handle_input()
{}

void dcs_test::render()
{
	const auto& back_buffer = _cfg.context->back_buffer();

	back_buffer->begin_raster();

	_st.opaque_batch->raster( back_buffer, {} );

	back_buffer->end_raster();
}
