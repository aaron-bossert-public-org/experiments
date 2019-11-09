
#pragma once

#include "framework/utility/scoped_ptr.h"

#include <vector>

namespace igpu
{
	class vulkan_fence;

	class vulkan_gpu_object
	{
	public:
		class state
		{
		public:
			friend class vulkan_gpu_object;

			~state();

		private:
			std::vector< scoped_ptr< vulkan_fence > > fences;
		};

		virtual state& object_state() = 0;

		void add_fence( const scoped_ptr< vulkan_fence >& );

		void wait_on_fences();
	};
}
