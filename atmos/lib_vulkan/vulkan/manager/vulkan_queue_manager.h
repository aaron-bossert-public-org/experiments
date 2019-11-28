
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/compute/compute_buffer.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_buffer;
	class vulkan_image;
	class vulkan_queue;

	class vulkan_queue_manager
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_queue > present_queue;
			scoped_ptr< vulkan_queue > graphics_queue;
			scoped_ptr< vulkan_queue > compute_queue;
			scoped_ptr< vulkan_queue > transfer_queue;
			std::vector< scoped_ptr< vulkan_queue > > queue_family_table;
		};

		const config& cfg() const;

		~vulkan_queue_manager();

		static std::unique_ptr< vulkan_queue_manager > make( const config& );

	protected:
		vulkan_queue_manager( const config& );

	private:
		const config _cfg;
	};
}
