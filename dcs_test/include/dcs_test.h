#pragma once
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

class dcs_test
{
public:
	struct config
	{
		std::shared_ptr< igpu::context > context;

		std::string model_path = "cooked_assets/models/chalet.obj";
		std::string texture_path = "cooked_assets/textures/chalet.jpg";
		std::string vertex_path = "cooked_assets/shaders/shader.vert.spv";
		std::string fragment_path = "cooked_assets/shaders/shader.frag.spv";
	};

	static std::unique_ptr< dcs_test > make( const config& );

	~dcs_test();

	bool advance();

private:
	struct state
	{
		std::shared_ptr< igpu::program > program;
		std::shared_ptr< igpu::render_states > render_states;
		std::shared_ptr< igpu::geometry > geometry;

		std::shared_ptr< igpu::compute_buffer > batch_data;
		std::shared_ptr< igpu::compute_buffer > instance_data;
		std::shared_ptr< igpu::texture2d > texture;

		std::shared_ptr< igpu::opaque_batch > opaque_batch;
		std::unique_ptr< igpu::batch_binding > batch_binding;
	};

	dcs_test( const config&, state&& );

	static void update( const config&, const state& );

	void handle_input();
	void render();

private:
	const config _cfg;
	state _st;
};