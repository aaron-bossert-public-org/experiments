
#include <cstdlib>
#include <thread>
#include <dcs_test/include/dcs_test.h>
#include <vulkan/context/vulkan_context.h>

int main() {

	std::shared_ptr < igpu::vulkan_context > context = igpu::vulkan_context::make(
		{ "test vulkan context" },
		glm::ivec2(800, 600));
		
	auto test = dcs_test::make({ context });

	while (test->advance()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return EXIT_SUCCESS;
}
