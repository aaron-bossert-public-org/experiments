
#include "vulkan/context/vulkan_context.h"

#include <algorithm>
#include <cstdlib>
#include <dcs_test/src/dcs_test.h>
#include <thread>

#if ATMOS_BUILD_WINDOWS

#	include <windows.h>

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd )

#else

int main()

#endif {
{
	igpu::vulkan_context::config context_cfg = {};
	context_cfg.name = "test vulkan context";

	std::shared_ptr< igpu::vulkan_context > context =
		igpu::vulkan_context::make( context_cfg, glm::ivec2( 800, 600 ) );

	auto test = dcs_test::make( { context } );

	while ( test->advance() )
	{
	}
	return EXIT_SUCCESS;
}
