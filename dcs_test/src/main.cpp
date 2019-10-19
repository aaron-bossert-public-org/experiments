
#include <cstdlib>
#include <thread>
#include <dcs_test/include/dcs_test.h>
#include <vulkan/context/vulkan_context.h>

int main() {

	igpu::vertex_constraints::config vertex_constraints = {};
	vertex_constraints.vertex_parameters = {
		{"pos", igpu::components::FLOAT3 },
		{"col", igpu::components::FLOAT3 },
		{"uv0", igpu::components::FLOAT2 },
	};

	igpu::vulkan_context::config context_cfg = {};
	context_cfg.name = "test vulkan context";
	context_cfg.vertex_constraints = vertex_constraints;


	std::shared_ptr < igpu::vulkan_context > context = igpu::vulkan_context::make(
		context_cfg,
		glm::ivec2(800, 600));
		
	auto test = dcs_test::make({ context });

	while (test->advance()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return EXIT_SUCCESS;
}
