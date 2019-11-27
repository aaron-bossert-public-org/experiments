

#include "dcs_test/src/dcs_compute_indirect_draw.h"

#include "igpu/context/context.h"
#include "igpu/window/window.h"

#include "framework_tests/main/tests.h"

#include "dcs_test/src/dcs_utils.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <iostream>

using namespace igpu;

namespace
{
	const int32_t s_instance_length = 24;
	const int32_t s_instance_volume =
		s_instance_length * s_instance_length * s_instance_length;
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

		st.batch_data = cfg.context->make_shared( compute_buffer::config{} );
		st.instance_data = cfg.context->make_shared( compute_buffer::config{} );

		st.render_states = cfg.context->make_shared( render_states::config{
			0b1111, // color write mask
			{ (bool)"cull_enabled", cull::BACK },
			{ !(bool)"blend_enabled" },
			{ !(bool)"stencil_enabled" },
			{ (bool)"depth_enabled", (bool)"depth_write", compare::LESS },
		} );

		st.texture = cfg.context->make_shared( texture2d::config{
			cfg.texture_path.c_str(),
			sampler::filter::LINEAR,
			sampler::filter::LINEAR,
			sampler::address::WRAP,
			sampler::address::WRAP,
			(bool)"can_auto_generate_mips",
		} );

		dcs_utils::load_buffer( cfg.texture_path.c_str(), st.texture.get() );

		st.program = cfg.context->make_shared( program::config{
			"test program",
			cfg.context->make_shared(
				vertex_shader::config{},
				dcs_utils::load_mem( cfg.vertex_path.c_str() ) ),
			cfg.context->make_shared(
				fragment_shader::config{},
				dcs_utils::load_mem( cfg.fragment_path.c_str() ) ),
			{ { { "MAX_INSTANCE_ID", s_instance_volume } } } } );

		st.model = dcs_utils::load_model(
			cfg.context.get(),
			cfg.model_path,
			dcs_utils::model_flags::JOIN_IDENTICAL_VERTICES |
				dcs_utils::model_flags::FLIP_TEXCOORD_Y );

		auto geo_cfg = st.model.geometry->cfg();
		auto colors = cfg.context->make_shared( vertex_buffer::config{
			(uint32_t)sizeof( glm::vec4 ),
			{ { { "cl0", components::FLOAT4 } } } } );

		buffer_view< glm::vec4 > color_view( st.model.vertex_count, nullptr );
		colors->map( &color_view );
		for ( size_t msh = 0; msh < st.model.meshes.size(); ++msh )
		{
			const auto& mesh = st.model.meshes[msh];
			for ( size_t i = mesh.base_vertex;
				  i < mesh.vertex_count + mesh.base_vertex;
				  ++i )
			{
				color_view[i] = mesh.material_color;
				color_view[i].a = (float)msh;
			}
		}

		colors->unmap();

		geo_cfg.vertex_buffers.push_back( colors );
		st.model.geometry = cfg.context->make_shared( geo_cfg );

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
{
	update();
	struct
	{
		glm::mat4 model[s_instance_length][s_instance_length]
					   [s_instance_length];
	}* instance_ubo = nullptr;

	_st.instance_data->map( &instance_ubo );

	auto& mats = instance_ubo->model;
	float scale = 5.f;
	float offset = scale * s_instance_length * -0.5f;

	glm::mat4x4 ident = glm::mat4( 1.0f );
	for ( size_t z = 0; z < s_instance_length; ++z )
		for ( size_t y = 0; y < s_instance_length; ++y )
			for ( size_t x = 0; x < s_instance_length; ++x )
			{
				ident[3][0] = x * scale + offset;
				ident[3][1] = y * scale + offset;
				ident[3][2] = z * scale + offset;
				mats[x][y][z] = ident;
			}

	_st.instance_data->unmap();

	_st.opaque_batch = cfg.context->make_shared( opaque_batch::config{
		cfg.context->back_buffer(),
		cfg.context->make_shared( primitives::config{ {
			{ "batch_data", _st.batch_data },
		} } ),
	} );

	_st.batch_bindings.push_back( _st.opaque_batch->make_binding( {
		_st.program,
		_st.render_states,
		_st.model.geometry,
		cfg.context->make_shared( primitives::config{ {
			{ "texSampler", _st.texture },
		} } ),
		cfg.context->make_shared( primitives::config{ {
			{ "instance_data", _st.instance_data },
		} } ),
	} ) );

	_st.batch_bindings.back()->instance_batch().draw_params(
		igpu::draw_parameters{
			_st.model.meshes.back().index_count,
			s_instance_volume,
			_st.model.meshes.back().first_index,
			_st.model.meshes.back().base_vertex,
			0,
		} );
}

dcs_compute_indirect_draw::~dcs_compute_indirect_draw()
{}

bool dcs_compute_indirect_draw::advance()
{
	update();

	if ( _cfg.context->window()->poll_events() )
	{
		handle_input();

		render();

		return true;
	}

	return false;
}

void dcs_compute_indirect_draw::update()
{
	// update model transform
	static auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration< float, std::chrono::seconds::period >(
					 current_time - start_time )
					 .count();

	// update view matrices
	auto back_buffer = _cfg.context->back_buffer();
	auto res = back_buffer->res();

	auto rot = glm::rotate(
		glm::mat4( 1.0f ),
		time * glm::radians( 90.0f ) / 16.f,
		glm::vec3( 0.0f, 0.0f, 1.0f ) );

	auto look = glm::lookAt(
		glm::vec3( 50.0f, 0.0f, 50.0f ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ) );

	struct
	{
		glm::mat4 view;
		glm::mat4 proj;
	}* batch_ubo = nullptr;

	_st.batch_data->map( &batch_ubo );

	batch_ubo->view = look * rot;

	batch_ubo->proj = glm::perspective(
		glm::radians( 60.0f ),
		res.x / (float)res.y,
		0.5f,
		1000.0f );

	_st.batch_data->unmap();
}

void dcs_compute_indirect_draw::handle_input()
{}

void dcs_compute_indirect_draw::render()
{
	const auto& back_buffer = _cfg.context->back_buffer();

	back_buffer->begin_raster();

	_st.opaque_batch->raster( back_buffer, {} );

	back_buffer->end_raster();
}