
#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <queue>
#include <tuple>
#include <vector>

namespace igpu
{
	class vulkan_fence;
	class vulkan_queue;

	template < typename... ARGS >
	void abandon( const scoped_ptr< vulkan_queue >&, ARGS... ) = delete;

	template <>
	void abandon<>(
		const scoped_ptr< vulkan_queue >&,
		VmaAllocator,
		VkBuffer,
		VmaAllocation );

	template <>
	void abandon<>(
		const scoped_ptr< vulkan_queue >&,
		VkDevice,
		VkCommandPool,
		VkCommandBuffer );

	template <>
	void abandon<>(
		const scoped_ptr< vulkan_queue >&,
		VkDevice,
		VkCommandPool );

	template <>
	void abandon<>(
		const scoped_ptr< vulkan_queue >&,
		VkDevice,
		VkDescriptorPool );

	template <>
	void abandon<>(
		const scoped_ptr< vulkan_queue >&,
		VkDevice,
		VkDeviceMemory );

	template <>
	void abandon<>( const scoped_ptr< vulkan_queue >&, VkDevice, VkImage );

	template <>
	void abandon<>( const scoped_ptr< vulkan_queue >&, VkDevice, VkSampler );

	template <>
	void abandon<>( const scoped_ptr< vulkan_queue >&, VkDevice, VkImageView );

	template <>
	void abandon<>( const scoped_ptr< vulkan_queue >&, VkDevice, VkPipeline );

	class vulkan_abandon_manager
	{
	public:
		using payload_t = std::tuple< void*, void*, void*, void* >;

		using pending_t = std::vector< std::queue< payload_t > >;

		void trigger_abandon( const std::shared_ptr< vulkan_fence >& );

		static std::unique_ptr< vulkan_abandon_manager > make();

		pending_t& pending();

		bool empty();

		~vulkan_abandon_manager();

	private:
		vulkan_abandon_manager();

	private:
		struct payload_category
		{
			size_t type_index;
			std::queue< payload_t > payloads;
		};

		struct abandoned
		{
			size_t submit_index = 0;
			std::shared_ptr< vulkan_fence > fence;
			std::vector< payload_category > categories;
		};

		pending_t _pending;

		std::queue< abandoned > _abandoned;

		vulkan_abandon_manager( const vulkan_abandon_manager& ) = delete;
		vulkan_abandon_manager& operator=( const vulkan_abandon_manager& ) =
			delete;
	};

}