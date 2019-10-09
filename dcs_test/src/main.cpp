
#include <cstdlib>
#include <thread>
#include <dcs_test/include/dcs_test.h>
#include <vulkan/context/vulkan_context.h>

int main() {

	auto vulkan = igpu::vulkan_context::make({});

	auto test = dcs_test::make(std::move(vulkan));

	while (test->advance()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return EXIT_SUCCESS;
}
