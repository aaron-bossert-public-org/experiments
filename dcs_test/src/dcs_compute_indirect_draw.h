#pragma once
#include "dcs_test/src/dcs_utils.h"
#include <memory>
#include <string>

namespace igpu
{
	class batch_binding;
	class compute_buffer;
	class context;
	class geometry;
	class opaque_batch;
	class program;
	class render_states;
	class texture2d;
	class window;
}

class dcs_compute_indirect_draw
{
public:
	struct config
	{
		std::shared_ptr< igpu::context > context;
		std::string model_path = "cooked_assets/models/suzanne_lods.dae";
		std::string texture_path = "cooked_assets/textures/chalet.jpg";
		std::string vertex_path =
			"cooked_assets/shaders/indirect_draw.vert.spv";
		std::string fragment_path =
			"cooked_assets/shaders/indirect_draw.frag.spv";
	};

	static std::unique_ptr< dcs_compute_indirect_draw > make( const config& );

	~dcs_compute_indirect_draw();

	bool advance();

private:
	struct state
	{
		std::shared_ptr< igpu::program > program;
		std::shared_ptr< igpu::render_states > render_states;

		dcs_utils::model model;

		std::shared_ptr< igpu::compute_buffer > batch_data;
		std::shared_ptr< igpu::compute_buffer > instance_data;
		std::shared_ptr< igpu::texture2d > texture;

		std::shared_ptr< igpu::opaque_batch > opaque_batch;
		std::unique_ptr< igpu::batch_binding > batch_binding;
		std::vector< std::unique_ptr< igpu::batch_binding > > batch_bindings;
	};

	dcs_compute_indirect_draw( const config&, state&& );

	void update();

	void handle_input();
	void render();

private:
	const config _cfg;
	state _st;
};