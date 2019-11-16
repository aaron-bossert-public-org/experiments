
#include "vulkan/defines/vulkan_includes.h"

#include <memory>

namespace igpu
{
	class vulkan_abandon_manager
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			VmaAllocator vma = nullptr;
		};

		struct payload
		{
			void* _0 = nullptr;
			void* _1 = nullptr;
		};

		template < typename... ARGS >
		void abandon( ARGS... );

		void destroy_abandoned();

		std::unique_ptr< vulkan_abandon_manager > make( const config& );

		~vulkan_abandon_manager();

	private:
		vulkan_abandon_manager( const config& );

	private:
		const config _cfg;
		std::vector< std::vector< payload > > _abandoned;
	};

	template <>
	void vulkan_abandon_manager::abandon< VkBuffer, VmaAllocation >(
		VkBuffer,
		VmaAllocation );

	template <>
	void vulkan_abandon_manager::abandon< VkImage >( VkImage );
}