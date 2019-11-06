
#pragma once

#include <memory>
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
			std::vector< std::shared_ptr< vulkan_fence > > fences;
		};

		virtual state& object_state() = 0;

		void add_fence( const std::shared_ptr< vulkan_fence >& );

		void wait_on_fences();
	};
}
