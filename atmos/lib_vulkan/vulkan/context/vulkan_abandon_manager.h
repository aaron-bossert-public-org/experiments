
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
			size_t swap_count = 0;
		};

		struct payload
		{
			void* _0 = nullptr;
			void* _1 = nullptr;
		};

		template < typename... ARGS >
		void abandon( ARGS... ) = delete;

		void swap_frame();

		static std::unique_ptr< vulkan_abandon_manager > make( const config& );

		~vulkan_abandon_manager();

	private:
		vulkan_abandon_manager( const config& );

		void vulkan_abandon_manager::destroy_abandoned( size_t );

		std::vector< std::vector< payload > >* abandoned_frame();

	private:
		const config _cfg;
		size_t _swap_index = 0;
		std::vector< std::vector< std::vector< payload > > > _abandoned;

		vulkan_abandon_manager( const vulkan_abandon_manager& ) = delete;
		vulkan_abandon_manager& operator=( const vulkan_abandon_manager& ) =
			delete;
	};

	template <>
	void vulkan_abandon_manager::abandon<>( VkBuffer, VmaAllocation );

	template <>
	void vulkan_abandon_manager::abandon<>( VkImage );

	template <>
	void vulkan_abandon_manager::abandon<>( VkSampler );

	template <>
	void vulkan_abandon_manager::abandon<>( VkImageView );

	template <>
	void vulkan_abandon_manager::abandon<>( VkDeviceMemory );

	template <>
	void vulkan_abandon_manager::abandon<>( VkCommandPool, VkCommandBuffer );

	template <>
	void vulkan_abandon_manager::abandon<>( VkCommandPool );
}