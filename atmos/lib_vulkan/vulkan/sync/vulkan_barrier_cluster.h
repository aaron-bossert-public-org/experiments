
#include <vector>

namespace igpu
{
	class vulkan_dependency;

	class vulkan_barrier_cluster
	{
	public:
		struct identifier
		{};

	private:
		size_t _generation = 0;
	};
}
