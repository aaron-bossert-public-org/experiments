
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
			buffer_view_base&,
			size_t byte_size);

		void unmap();

		void release();

		size_t byte_size() const;

		static bool validate(const config&);

	private:
		
		const config _cfg;
		
		std::unique_ptr<vulkan_buffer> _staging_buffer;
		std::unique_ptr<vulkan_buffer> _gpu_buffer;
	};

	template <typename T>
	class vulkan_staged_buffer_t : public T
	{
	public:

		using config = typename T::config;

		~vulkan_staged_buffer_t() override {};

		const config& cfg() const override
		{
			return _cfg;
		}

		void map(
			buffer_view_base& buffer_view,
			size_t byte_size)
			override
		{
			_vulkan_staged_buffer.map(buffer_view, byte_size);
		}

		void unmap() override
		{
			_vulkan_staged_buffer.unmap();
		}

		size_t byte_size() const override
		{
			return _vulkan_staged_buffer.byte_size();
		}

	protected:

		vulkan_staged_buffer_t(
			const config& cfg,
			const vulkan_staged_buffer::config& res_config)
			: _cfg(cfg)
			, _vulkan_staged_buffer(res_config)
		{
		}

	private:

		const config _cfg;
		vulkan_staged_buffer _vulkan_staged_buffer;
	};
}
