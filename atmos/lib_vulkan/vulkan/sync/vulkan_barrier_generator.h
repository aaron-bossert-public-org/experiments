
#include <vector>

namespace igpu
{
	class vulkan_dependency;
	class vulkan_barrier_cluster;

	class vulkan_barrier_generator
	{
	public:
		void activate_read_barrier( vulkan_dependency* );

		void generate_barriers( vulkan_barrier_cluster* );

	private:
		std::vector< vulkan_dependency* > _read_dependencies;
		const std::vector< vulkan_dependency > _write_dependencies;
	};
}
