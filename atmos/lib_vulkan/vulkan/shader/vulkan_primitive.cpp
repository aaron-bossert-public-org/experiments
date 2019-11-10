
#include "vulkan/shader/vulkan_primitive.h"

#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"

using namespace igpu;

vulkan_primitive::vulkan_primitive( const config& cfg )
	: _cfg( cfg )
{}

const vulkan_primitive::config& vulkan_primitive::cfg() const
{
	return _cfg;
}

vulkan_primitive::gpu_variant std::visit(
	[&]( auto&& prim ) {
		auto gpu_object = &prim->gpu_object();

		std::get< decltype( gpu_object ) >( tup_gpu_object ) = gput_object;
	},
	primitive.cfg().vk.value );