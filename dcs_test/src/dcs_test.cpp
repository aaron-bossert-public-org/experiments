

#include "dcs_test/src/dcs_test.h"

#include "igpu/context/context.h"
#include "igpu/window/window.h"

#include "framework_tests/main/tests.h"

#include "dcs_test/src/dcs_utils.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <iostream>

using namespace igpu;


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
		} );

		st.geometry = dcs_utils::load_model(
						  cfg.context.get(),
						  cfg.model_path,
						  dcs_utils::model_flags::JOIN_IDENTICAL_VERTICES |
							  dcs_utils::model_flags::FLIP_TEXCOORD_Y )
						  .geometry;

		return std::unique_ptr< dcs_test >(
			new dcs_test( cfg, std::move( st ) ) );
	}

	return nullptr;
}

dcs_test::dcs_test( const config& cfg, state&& st )
	: _cfg( cfg )
	, _st( std::move( st ) )
{
	update();

	_st.opaque_batch = cfg.context->make_shared( opaque_batch::config{
		cfg.context->back_buffer(),
		cfg.context->make_shared( primitives::config{ {
			{ "batch_data", _st.batch_data },
		} } ),
	} );

	_st.batch_binding = _st.opaque_batch->make_binding( {
		_st.program,
		_st.render_states,
		_st.geometry,
		cfg.context->make_shared( primitives::config{ {
			{ "texSampler", _st.texture },
		} } ),
		cfg.context->make_shared( primitives::config{ {
			{ "instance_data", _st.instance_data },
		} } ),
	} );
}

dcs_test::~dcs_test()
{}

bool dcs_test::advance()
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

void dcs_test::update()
{
	// update view matrices
	auto back_buffer = _cfg.context->back_buffer();
	auto res = back_buffer->res();

	struct
	{
		alignas( 16 ) glm::mat4 view;
		alignas( 16 ) glm::mat4 proj;
	}* batch_ubo = nullptr;

	_st.batch_data->map( &batch_ubo );

	batch_ubo->view = glm::lookAt(
		glm::vec3( 2.0f, 2.0f, 2.0f ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ) );
	batch_ubo->proj = glm::perspective(
		glm::radians( 45.0f ),
		res.x / (float)res.y,
		0.1f,
		10.0f );

	_st.batch_data->unmap();

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

	_st.instance_data->map( &instance_ubo );

	instance_ubo->model = glm::rotate(
		glm::mat4( 1.0f ),
		time * glm::radians( 90.0f ),
		glm::vec3( 0.0f, 0.0f, 1.0f ) );

	_st.instance_data->unmap();
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
