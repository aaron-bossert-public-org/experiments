
#include "vulkan/sync/vulkan_synchronization.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/texture/vulkan_image.h"

using namespace igpu;

const vulkan_synchronization::config& vulkan_synchronization::cfg() const
{
	return _cfg;
}

size_t vulkan_synchronization::compact_queue_family_index(
	uint32_t queue_family_index ) const
{
	return _compact_queue_family_indices[queue_family_index];
}

const std::vector< scoped_ptr< vulkan_queue > >& vulkan_synchronization::
	compact_queues() const
{
	return _compact_queues;
}

VmaAllocator vulkan_synchronization::vma()
{
	return _cfg.vma;
}

vulkan_abandon_manager& vulkan_synchronization::abandon_manager() const
{
	return *_cfg.abandon_manager;
}

vulkan_synchronization::~vulkan_synchronization()
{}

std::unique_ptr< vulkan_synchronization > vulkan_synchronization::make(
	const config& cfg )
{
	std::vector< size_t > compact;
	std::vector< scoped_ptr< vulkan_queue > > compact_queues;

	for ( auto& queue : {
			  cfg.present_queue,
			  cfg.graphics_queue,
			  cfg.compute_queue,
			  cfg.transfer_queue,
		  } )
	{
		uint32_t family_index = queue->cfg().family_index;
		if ( family_index <= compact.size() )
		{
			compact.resize( family_index + 1 );
		}

		if ( 0 == compact[family_index] )
		{
			compact_queues.push_back( queue );
			compact[family_index] = compact_queues.size();
		}
	}

	for ( auto& compact_index : compact )
	{
		--compact_index;
	}

	return std::unique_ptr< vulkan_synchronization >(
		new vulkan_synchronization( cfg, compact, compact_queues ) );
}

vulkan_synchronization::vulkan_synchronization(
	const config& cfg,
	const std::vector< size_t >& compact_queue_family_indices,
	const std::vector< scoped_ptr< vulkan_queue > >& compact_queues )
	: _cfg( cfg )
	, _compact_queue_family_indices( compact_queue_family_indices )
	, _compact_queues( compact_queues )
{}

#include "vulkan/context/vulkan_context.h"

#define GLM_ENABLE_EXPERIMENTAL 1
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#pragma warning( push )
#pragma warning( disable : 4505 )

#include "igpu/context/context.h"

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

const std::string MODEL_PATH = "cooked_assets/models/chalet.obj";
const std::string TEXTURE_PATH = "cooked_assets/textures/chalet.jpg";
const std::string VERTEX_PATH = "cooked_assets/shaders/shader.vert.spv";
const std::string FRAGMENT_PATH = "cooked_assets/shaders/shader.frag.spv";

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

struct UniformBufferBatch
{
	alignas( 16 ) glm::mat4 view;
	alignas( 16 ) glm::mat4 proj;
};


struct UniformBufferInstance
{
	alignas( 16 ) glm::mat4 model;
};

using namespace igpu;

class HelloTriangleApplication
{
private:
	scoped_ptr< context > _context;

	struct new_texture_impl
	{
		HelloTriangleApplication* app;
		std::shared_ptr< texture2d > texture;
		VkSampler _texture_sampler;

		[[nodiscard]] bool create()
		{
			texture2d::config texture_cfg = {
				TEXTURE_PATH,
				sampler::filter::LINEAR,
				sampler::filter::LINEAR,
				sampler::address::WRAP,
				sampler::address::WRAP,
			};
			texture_cfg.can_auto_generate_mips = true;
			texture = app->_context->make_shared( texture_cfg );

			if ( false == app->load_buffer( TEXTURE_PATH, texture.get() ) )
			{
				return false;
			}

			return true;
		}
		void destroy()
		{
			texture = nullptr;
		}
	};

	new_texture_impl texture_impl = {
		this,
	};

	struct new_geo_impl
	{
		[[nodiscard]] bool create()
		{
			auto index_buffer =
				app->_context->make_shared( index_buffer::config{
					index_format::UNSIGNED_INT,
				} );

			vertex_buffer::attribute attr = {
				"pos",
				to_components( ( *( (Vertex*)0 ) ).pos ),
				( ( size_t ) & ( ( (Vertex*)0 )->pos ) ),
			};


			auto vertex_buffer = app->_context->make_shared(
				IGPU_VERT_CFG_OF( Vertex, pos, col, uv0 ) );

			geometry = app->_context->make_shared( geometry::config{
				MODEL_PATH,
				topology::TRIANGLE_LIST,
				index_buffer,
				{
					vertex_buffer,
				},
			} );

			return (bool)geometry;
		}

		[[nodiscard]] bool upload(
			const std::vector< Vertex >& vertices,
			const std::vector< uint32_t >& indices )
		{
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
				LOG_CRITICAL(
					"failed to obtain mapped memory from index buffer" );
				return false;
			}

			if ( !vertices_view.data() )
			{
				LOG_CRITICAL(
					"failed to obtain mapped memory from vertex buffer" );
				return false;
			}

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

			return true;
		}

		void destroy()
		{
			geometry = nullptr;
		}

		HelloTriangleApplication* app;
		std::shared_ptr< geometry > geometry;
	};

	new_geo_impl geo_impl = {
		this,
	};

	struct batch_buffer_impl
	{
		HelloTriangleApplication* app;

		[[nodiscard]] bool create()
		{
			compute_buffer =
				app->_context->make_shared( compute_buffer::config{} );

			if ( !compute_buffer )
			{
				return false;
			}

			update();

			return true;
		}

		void update()
		{
			auto res = app->_context->back_buffer()->res();

			auto view = buffer_view< UniformBufferBatch >( 1, nullptr );
			compute_buffer->map( &view );
			UniformBufferBatch& ubo = view[0] = {};

#pragma warning( push )
#pragma warning( disable : 4127 ) // constant if condition
			ubo.view = glm::lookAt(
				glm::vec3( 2.0f, 2.0f, 2.0f ),
				glm::vec3( 0.0f, 0.0f, 0.0f ),
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
			ubo.proj = glm::perspective(
				glm::radians( 45.0f ),
				res.x / (float)res.y,
				0.1f,
				10.0f );
			ubo.proj[1][1] *= -1;
#pragma warning( pop )

			compute_buffer->unmap();
		}

		void destroy()
		{
			compute_buffer = nullptr;
		}

		std::shared_ptr< compute_buffer > compute_buffer;
	};

	batch_buffer_impl batch = {
		this,
	};


	struct instance_buffer_impl : batch_buffer_impl
	{
		[[nodiscard]] bool create()
		{
			if ( batch_buffer_impl::create() )
			{
				update();
				return true;
			}

			return false;
		}

		void update()
		{
			static auto start_time = std::chrono::high_resolution_clock::now();
			auto current_time = std::chrono::high_resolution_clock::now();
			float time =
				std::chrono::duration< float, std::chrono::seconds::period >(
					current_time - start_time )
					.count();

			auto view = buffer_view< UniformBufferInstance >( 1, nullptr );
			compute_buffer->map( &view );
			UniformBufferInstance& ubo = view[0] = {};

#pragma warning( push )
#pragma warning( disable : 4127 ) // constant if condition
			ubo.model = glm::rotate(
				glm::mat4( 1.0f ),
				time * glm::radians( 90.0f ),
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
#pragma warning( pop )

			compute_buffer->unmap();
		}
	};

	instance_buffer_impl instance = {
		this,
	};


	struct new_program_impl
	{
		[[nodiscard]] bool create()
		{
			std::shared_ptr< vertex_shader > vertex =
				app->_context->make_shared( vertex_shader::config{} );
			std::shared_ptr< fragment_shader > fragment =
				app->_context->make_shared( fragment_shader::config{} );


			if ( !app->load_buffer( VERTEX_PATH, vertex.get() ) )
			{
				return false;
			}

			if ( !app->load_buffer( FRAGMENT_PATH, fragment.get() ) )
			{
				return false;
			}

			program = app->_context->make_shared( program::config{
				"test program",
				vertex,
				fragment,
			} );

			return true;
		}

		void destroy()
		{
			// destroy
			program = nullptr;
		}

		HelloTriangleApplication* app;
		std::shared_ptr< program > program;
	};

	new_program_impl program_impl = {
		this,
	};

	struct new_render_states_impl
	{
		[[nodiscard]] bool create()
		{
			uint8_t color_write_mask = 0b1111;

			cull_mode c = {
				true,
				cull::BACK,
			};

			blend_mode b = {
				false,
			};

			stencil_mode s = {
				false,
			};

			depth_mode d = {
				true,
				true,
				compare::LESS,
			};

			render_states = app->_context->make_shared( {
				color_write_mask,
				c,
				b,
				s,
				d,
			} );

			return (bool)render_states;
		}

		void destroy()
		{
			render_states = nullptr;
		}

		HelloTriangleApplication* app;
		std::shared_ptr< render_states > render_states;
	};

	new_render_states_impl render_states_impl = {
		this,
	};

	struct new_descriptor_set_impl
	{
		[[nodiscard]] bool create()
		{
			const auto& program = app->program_impl.program;
			const auto& render_states = app->render_states_impl.render_states;
			const auto& geometry = app->geo_impl.geometry;

			auto batch_primitives =
				app->_context->make_shared( primitives::config{
					{ { "batch_data", app->batch.compute_buffer } } } );

			auto material_primitives =
				app->_context->make_shared( primitives::config{
					{ { "texSampler", app->texture_impl.texture } } } );

			auto instance_primitives =
				app->_context->make_shared( primitives::config{
					{ { "instance_data", app->instance.compute_buffer } } } );


			if ( _opaque_batch =
					 app->_context->make_shared( opaque_batch::config{
						 app->_context->back_buffer(),
						 batch_primitives,
					 } ) )
			{
				if ( _batch_binding = _opaque_batch->make_binding( {
						 program,
						 render_states,
						 geometry,
						 material_primitives,
						 instance_primitives,
					 } ) )
				{
					return true;
				}
			}

			return false;
		}

		void destroy()
		{
			_batch_binding = nullptr;
			_opaque_batch = nullptr;
		}

		HelloTriangleApplication* app = nullptr;
		std::shared_ptr< opaque_batch > _opaque_batch;
		std::unique_ptr< batch_binding > _batch_binding;
	};


	new_descriptor_set_impl descriptor_set_impl = { this };

public:
	~HelloTriangleApplication()
	{}

	bool init_vulkan( const scoped_ptr< vulkan_context >& context )
	{
		_context = context;

		return batch.create() && instance.create() && texture_impl.create() &&
			program_impl.create() && render_states_impl.create() &&
			geo_impl.create() && descriptor_set_impl.create() && load_model();
	}

	bool main_loop()
	{
		if ( _context->window()->poll_events() )
		{
			draw_frame();
			return true;
		}
		return false;
	}

	void cleanup()
	{
		texture_impl.destroy();
		program_impl.destroy();

		descriptor_set_impl.destroy();

		batch.destroy();
		instance.destroy();

		geo_impl.destroy();
	}

private:
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

	[[nodiscard]] bool load_model()
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
				 MODEL_PATH.c_str() ) )
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

		return geo_impl.upload( vertices, indices );
	}

	void draw_frame()
	{
		auto back_buffer = _context->back_buffer();

		back_buffer->begin_raster();

		descriptor_set_impl._opaque_batch->raster( back_buffer, {} );

		back_buffer->end_raster();
	}
};


HelloTriangleApplication hello_triangle_application;

void port_example();

bool test_init_vulkan_context( const scoped_ptr< vulkan_context >& context )
{
	return hello_triangle_application.init_vulkan( context );
}

bool test_loop_vulkan_context()
{
	return hello_triangle_application.main_loop();
}

void test_cleanup_vulkan_context()
{
	hello_triangle_application.cleanup();
}
