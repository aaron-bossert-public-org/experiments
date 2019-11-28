#pragma once
#include "dcs_test/src/dcs_utils.h"
#include <memory>
#include <string>

namespace igpu
{
	class raster_binding;
	class compute_buffer;
	class compute_binding;
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

		std::string vertex_path =
			"cooked_assets/shaders/indirect_draw.vert.spv";

		std::string fragment_path =
			"cooked_assets/shaders/indirect_draw.frag.spv";

		std::string compute_path =
			"cooked_assets/shaders/indirect_cull.comp.spv";
	};

	static std::unique_ptr< dcs_compute_indirect_draw > make( const config& );

	~dcs_compute_indirect_draw();

	bool advance();

private:
	struct state
	{
		std::shared_ptr< igpu::compute_buffer > raster_data;
		std::shared_ptr< igpu::opaque_batch > opaque_batch;
		std::shared_ptr< igpu::raster_binding > batch_binding;
		std::shared_ptr< igpu::compute_binding > compute_binding;
	};

	dcs_compute_indirect_draw( const config&, state&& );

	static void update(
		const glm::ivec2& res,
		igpu::compute_buffer& raster_data );

	void handle_input();
	void render();

private:
	const config _cfg;
	state _st;
};