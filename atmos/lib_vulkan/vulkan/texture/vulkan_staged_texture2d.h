
#pragma once

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/texture/vulkan_texture2d.h"

#include "framework/perf/metrics.h"
#include "framework/utility/scoped_ptr.h"


namespace igpu
{
	class vulkan_synchronization;

	class vulkan_staged_texture2d : public vulkan_texture2d
	{
	public:
		const config& cfg() const override;

		vulkan_staged_texture2d(
			const config& cfg,
			const scoped_ptr< vulkan_synchronization >& );

		~vulkan_staged_texture2d() override;

		void map( size_t, buffer_view_base* ) override;

		void unmap( const texture2d::state& state ) override;

		void unmap() override;

		const texture2d::state& current_state() const override;

		vulkan_image& gpu_resource() override;

		const vulkan_image& gpu_resource() const override;

	private:
		void upload( const buffer_view< char >&, const texture2d::state& );

	private:
		const config _cfg;
		texture2d::state _current_state = {};
		scoped_ptr< vulkan_synchronization > _synchronization;
		buffer_view< char > _mapped_view = {};
		vulkan_buffer _staging_buffer;
		std::unique_ptr< vulkan_image > _gpu_image;
	};
}
