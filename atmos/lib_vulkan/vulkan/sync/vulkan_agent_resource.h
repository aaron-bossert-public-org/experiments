
#include "vulkan/sync/vulkan_barrier.h"

namespace igpu
{
	class vulkan_dependency;

	class vulkan_agent_resource
	{
	public:
		~vulkan_agent_resource();

	private:
		vulkan_barrier _current_barrier;
		std::vector< vulkan_dependency > _read_agents;
	};
}
