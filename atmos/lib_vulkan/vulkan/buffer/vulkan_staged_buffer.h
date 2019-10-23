
#pragma once

#include <igpu/buffer/buffer_usage.h>
#include <igpu/utility/scoped_ptr.h>

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end


namespace igpu
{
	class vulkan_buffer_mediator;
	class vulkan_queue;
	class vulkan_buffer;


	class vulkan_staged_buffer
	{
	public:

		struct config
		{
			buffer_usage usage;
			VkBufferUsageFlagBits vk_usage_flags = (VkBufferUsageFlagBits)0;
			scoped_ptr < vulkan_buffer_mediator > buffer_mediator;
		};
		
		vulkan_staged_buffer(
			const config&);

		~vulkan_staged_buffer();

		const config& cfg() const;

		void map(
			size_t byte_size,
			buffer_view_base*);

		void unmap();

		size_t byte_size() const;
		
		void release();

		static bool validate(const config&);

		// todo: delete access to this
		VkBuffer get();

	private:
		
		const config _cfg;
		uint32_t _byte_size = 0;
		std::unique_ptr<vulkan_buffer> _staging_buffer;
		std::unique_ptr<vulkan_buffer> _gpu_buffer;
	};

	template <typename T>
	class vulkan_staged_buffer_t : public T
	{
	public:

		~vulkan_staged_buffer_t() override {};

		void map(
			size_t byte_size,
			buffer_view_base* out_buffer_view)
			override
		{
			_vulkan_staged_buffer.map(byte_size, out_buffer_view);
		}

		void unmap() override
		{
			_vulkan_staged_buffer.unmap();
		}

		size_t byte_size() const override
		{
			return _vulkan_staged_buffer.byte_size();
		}

		VkBuffer get() override
		{
			return _vulkan_staged_buffer.get();
		}

		template<typename... Args>
		vulkan_staged_buffer_t(
			const vulkan_staged_buffer::config& res_config,
			Args... args)
			: T(args...)
			, _vulkan_staged_buffer(res_config)
		{
		}

	private:

		vulkan_staged_buffer _vulkan_staged_buffer;
	};
}
