

#include "dcs_test/src/dcs_compute_indirect_draw.h"

#include "igpu/context/context.h"
#include "igpu/utility/utility_types.h"
#include "igpu/window/window.h"

#include "framework_tests/main/tests.h"

#include "dcs_test/src/dcs_utils.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <iostream>

using namespace igpu;

namespace
{
	const int32_t s_size_1d = 16;
	const int32_t s_size_3d = s_size_1d * s_size_1d * s_size_1d;
	const int32_t s_local_workgroup_size = s_size_1d;
}

std::unique_ptr< dcs_compute_indirect_draw > dcs_compute_indirect_draw::make(
	const config& cfg )
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

		st.raster_data = cfg.context->make_shared( compute_buffer::config{} );

		auto indirect_draw = cfg.context->make_shared(
			compute_buffer::config{ compute_buff_flag::INDIRECT_DRAW } );

		auto instance_data =
			cfg.context->make_shared( compute_buffer::config{} );

		auto lod_data = cfg.context->make_shared( compute_buffer::config{} );

		update( cfg.context->back_buffer()->res(), *st.raster_data );


		auto render_states = cfg.context->make_shared( render_states::config{
			0b1111, // color write mask
			{ (bool)"cull_enabled", cull::BACK },
			{ !(bool)"blend_enabled" },
			{ !(bool)"stencil_enabled" },
			{ (bool)"depth_enabled", (bool)"depth_write", compare::LESS },
		} );

		auto program = cfg.context->make_shared( program::config{
			"test program",
			cfg.context->make_shared(
				vertex_shader::config{},
				dcs_utils::load_mem( cfg.vertex_path.c_str() ) ),
			cfg.context->make_shared(
				fragment_shader::config{},
				dcs_utils::load_mem( cfg.fragment_path.c_str() ) ),
		} );

		auto model = dcs_utils::load_model(
			cfg.context.get(),
			cfg.model_path,
			dcs_utils::model_flags::JOIN_IDENTICAL_VERTICES |
				dcs_utils::model_flags::FLIP_TEXCOORD_Y );

		auto geo_cfg = model.geometry->cfg();
		auto colors = cfg.context->make_shared( vertex_buffer::config{
			(uint32_t)sizeof( glm::vec4 ),
			{ { { "cl0", components::FLOAT4 } } } } );

		buffer_view< glm::vec4 > color_view( model.vertex_count, nullptr );
		colors->map( &color_view );
		for ( size_t msh = 0; msh < model.meshes.size(); ++msh )
		{
			const auto& mesh = model.meshes[msh];
			for ( size_t i = mesh.base_vertex;
				  i < mesh.vertex_count + mesh.base_vertex;
				  ++i )
			{
				color_view[i] = mesh.material_color;
				color_view[i].a = (float)msh;
			}
		}

		colors->unmap();

		struct lod
		{
			uint32_t first_index;
			uint32_t index_count;
			int32_t base_vertex;
			float distance;
		};
		buffer_view< lod > lod_view( model.meshes.size(), nullptr );

		lod_data->map( &lod_view );

		float lod_scale = 10;
		for ( size_t msh = 0; msh < model.meshes.size(); ++msh )
		{
			const auto& mesh = model.meshes[msh];
			lod_view[msh] = {
				mesh.first_index,
				mesh.index_count,
				mesh.base_vertex,
				lod_scale + msh * lod_scale,
			};
		}

		lod_data->unmap();


		geo_cfg.vertex_buffers.push_back( colors );
		model.geometry = cfg.context->make_shared( geo_cfg );

		glm::mat4( *instances )[s_size_1d][s_size_1d][s_size_1d];

		instance_data->map( &instances );

		float scale = 5.f;
		float offset = scale * s_size_1d * -0.5f;

		glm::mat4x4 ident = glm::mat4( 1.0f );
		for ( size_t z = 0; z < s_size_1d; ++z )
			for ( size_t y = 0; y < s_size_1d; ++y )
				for ( size_t x = 0; x < s_size_1d; ++x )
				{
					ident[3][0] = x * scale + offset;
					ident[3][1] = y * scale + offset;
					ident[3][2] = z * scale + offset;
					( *instances )[x][y][z] = ident;
				}

		instance_data->unmap();


		indirect_draw->reset_gpu_only( sizeof( draw_parameters ) * s_size_3d );


		st.opaque_batch = cfg.context->make_shared( opaque_batch::config{
			cfg.context->back_buffer(),
			cfg.context->make_shared( primitives::config{ {
				{ "raster_data", st.raster_data },
			} } ),
		} );

		instance_batch::config instance_cfg = {};
		instance_cfg.program = program;
		instance_cfg.states = render_states;
		instance_cfg.geometry = model.geometry;
		instance_cfg.instance = cfg.context->make_shared( primitives::config{ {
			{ "instance_data", instance_data },
		} } );


		st.batch_binding = st.opaque_batch->make_binding( instance_cfg );
		st.batch_binding->instance_batch().draw_params(
			igpu::draw_indirect_parameters{
				s_size_3d,
				indirect_draw,
			} );


		st.compute_binding =
			cfg.context->make_shared( igpu::compute_binding::config{
				cfg.context->make_shared( igpu::compute_program::config{
					cfg.compute_path,
					cfg.context->make_shared(
						compute_shader::config{},
						dcs_utils::load_mem( cfg.compute_path.c_str() ) ),
					{ { { "LOCAL_SIZE", s_local_workgroup_size } } },
				} ),
				st.opaque_batch->cfg().primitives,
				cfg.context->make_shared( primitives::config{ {
					{ "lod_data", lod_data },
					{ "instance_data", instance_data },
					{ "indirect_draw", indirect_draw },
				} } ),
			} );

		st.compute_binding->compute_params(
			{ s_size_3d / s_local_workgroup_size } );

		return std::unique_ptr< dcs_compute_indirect_draw >(
			new dcs_compute_indirect_draw( cfg, std::move( st ) ) );
	}

	return nullptr;
}

dcs_compute_indirect_draw::dcs_compute_indirect_draw(
	const config& cfg,
	state&& st )
	: _cfg( cfg )
	, _st( std::move( st ) )
{}

dcs_compute_indirect_draw::~dcs_compute_indirect_draw()
{}

bool dcs_compute_indirect_draw::advance()
{
	auto back_buffer = _cfg.context->back_buffer();
	auto res = back_buffer->res();
	update( res, *_st.raster_data );

	if ( _cfg.context->window()->poll_events() )
	{
		handle_input();

		render();

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------
//
//

void dcs_compute_indirect_draw::update(
	const glm::ivec2& res,
	igpu::compute_buffer& buffer )
{
	// update model transform
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration< float, std::chrono::seconds::period >(
					 current_time - start_time )
					 .count();

	// update view matrices
	auto rot = glm::rotate(
		glm::mat4( 1.0f ),
		time * glm::radians( 90.0f ) / 16.f,
		glm::vec3( 0.0f, 0.0f, 1.0f ) );

	auto look = glm::lookAt(
		glm::vec3( 50.0f, 0.0f, 50.0f ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ) );

	auto view = look * rot;

	auto proj = glm::perspective(
		glm::radians( 60.0f ),
		res.x / (float)res.y,
		0.5f,
		1000.0f );
	auto cam = glm::inverse( view ) * glm::vec4( 0, 0, 0, 1 );

	struct
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 cam;
		utility::frustum frustum;
	}* raster_data = nullptr;

	buffer.map( &raster_data );

	raster_data->proj = proj;
	raster_data->view = view;
	raster_data->cam = cam;
	raster_data->frustum = utility::make_frustum( proj * view );

	buffer.unmap();
}

void dcs_compute_indirect_draw::handle_input()
{}

void dcs_compute_indirect_draw::render()
{
	_st.compute_binding->dispatch();

	const auto& back_buffer = _cfg.context->back_buffer();

	back_buffer->begin_raster();

	_st.opaque_batch->raster( back_buffer, {} );

	back_buffer->end_raster();
}