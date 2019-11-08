
#include "vulkan/context/vulkan_context.h"

#include <cstdlib>
#include <dcs_test/include/dcs_test.h>
#include <thread>

int main()
{
	igpu::vulkan_context::config context_cfg = {};
	context_cfg.name = "test vulkan context";

	std::shared_ptr< igpu::vulkan_context > context =
		igpu::vulkan_context::make( context_cfg, glm::ivec2( 800, 600 ) );

	auto test = dcs_test::make( { context } );

	while ( test->advance() )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
	return EXIT_SUCCESS;
}
