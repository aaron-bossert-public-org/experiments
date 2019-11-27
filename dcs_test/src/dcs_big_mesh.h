#pragma once
#include <memory>
#include <string>

namespace igpu
{
	class raster_binding;
	class compute_buffer;
	class context;
	class geometry;
	class opaque_batch;
	class program;
	class render_states;
	class texture2d;
	class window;
}

class dcs_big_mesh
{
public:
	struct config
	{
		std::shared_ptr< igpu::context > context;

		std::string model_path = "cooked_assets/models/chalet.obj";
		std::string texture_path = "cooked_assets/textures/chalet.jpg";
		std::string vertex_path = "cooked_assets/shaders/textured.vert.spv";
		std::string fragment_path = "cooked_assets/shaders/textured.frag.spv";
	};

	static std::unique_ptr< dcs_big_mesh > make( const config& );

	~dcs_big_mesh();

	bool advance();

private:
	struct state
	{
		std::shared_ptr< igpu::program > program;
		std::shared_ptr< igpu::render_states > render_states;
		std::shared_ptr< igpu::geometry > geometry;

		std::shared_ptr< igpu::compute_buffer > raster_data;
		std::shared_ptr< igpu::compute_buffer > instance_data;
		std::shared_ptr< igpu::texture2d > texture;

		std::shared_ptr< igpu::opaque_batch > opaque_batch;
		std::unique_ptr< igpu::raster_binding > raster_binding;
	};

	dcs_big_mesh( const config&, state&& );

	void update();

	void handle_input();
	void render();

private:
	const config _cfg;
	state _st;
};