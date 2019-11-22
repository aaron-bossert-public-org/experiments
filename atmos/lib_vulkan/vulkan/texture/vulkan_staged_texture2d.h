
#pragma once

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/texture/vulkan_image.h"
#include "vulkan/texture/vulkan_texture2d.h"

#include "framework/perf/metrics.h"
#include "framework/utility/scoped_ptr.h"


namespace igpu
{
	class vulkan_queue_manager;

	class vulkan_staged_texture2d : public vulkan_texture2d
	{
	public:
		const config& cfg() const override;

		vulkan_staged_texture2d( const config& cfg );

		~vulkan_staged_texture2d() override;

		void map( buffer_view_base* ) override;

		void unmap( const texture2d::state& state ) override;

		void unmap() override;

		const texture2d::state& current_state() const override;

		vulkan_image& gpu_object() override;

		const vulkan_image& gpu_object() const override;

	private:
		void upload( const buffer_view< char >&, const texture2d::state& );

	private:
		const config _cfg;
		texture2d::state _current_state = {};
		buffer_view< char > _mapped_view = {};
		vulkan_buffer _staging_buffer;
		vulkan_image _gpu_image;
	};
}
