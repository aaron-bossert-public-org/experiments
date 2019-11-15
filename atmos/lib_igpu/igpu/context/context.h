
#pragma once

#include "igpu/batch/opaque_batch.h"
#include "igpu/batch/transparent_batch.h"
#include "igpu/buffer/compute_buffer.h"
#include "igpu/buffer/geometry.h"
#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/topology.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/shader/fragment_shader.h"
#include "igpu/shader/primitives.h"
#include "igpu/shader/program.h"
#include "igpu/shader/render_states.h"
#include "igpu/shader/vertex_shader.h"
#include "igpu/texture/color_format.h"
#include "igpu/texture/depth_buffer.h"
#include "igpu/texture/depth_format.h"
#include "igpu/texture/depth_texture2d.h"
#include "igpu/texture/draw_target.h"
#include "igpu/texture/render_buffer.h"
#include "igpu/texture/render_texture2d.h"
#include "igpu/texture/texture2d.h"

#include <functional>
#include <memory>

namespace igpu
{
	class back_buffer;
	class window;
	class memoizer;

	class context
	{
	public:
		struct config
		{
			std::string name;
			color_format color_format = color_format::INT_R8G8B8A8;
			depth_format depth_format = depth_format::D24S8;
			bool enable_validation
#if ATMOS_DEBUG
				= true;
#else
				= false;
#endif
		};

		virtual const config& cfg() const = 0;

		virtual std::unique_ptr< draw_target > make(
			const draw_target::config& ) = 0;

		virtual std::unique_ptr< render_buffer > make(
			const render_buffer::config& ) = 0;

		virtual std::unique_ptr< render_texture2d > make(
			const render_texture2d::config& ) = 0;

		virtual std::unique_ptr< depth_buffer > make(
			const depth_buffer::config& ) = 0;

		virtual std::unique_ptr< depth_texture2d > make(
			const depth_texture2d::config& ) = 0;

		virtual std::unique_ptr< vertex_shader > make(
			const vertex_shader::config& ) = 0;

		virtual std::unique_ptr< fragment_shader > make(
			const fragment_shader::config& ) = 0;

		virtual std::unique_ptr< vertex_buffer > make(
			const vertex_buffer::config& ) = 0;

		virtual std::unique_ptr< index_buffer > make(
			const index_buffer::config& ) = 0;

		virtual std::unique_ptr< compute_buffer > make(
			const compute_buffer::config& ) = 0;

		virtual std::unique_ptr< texture2d > make(
			const texture2d::config& ) = 0;

		virtual std::unique_ptr< opaque_batch > make(
			const opaque_batch::config& ) = 0;

		virtual std::unique_ptr< transparent_batch > make(
			const transparent_batch::config& ) = 0;

		virtual const window& window() const = 0;

		virtual const back_buffer& back_buffer() const = 0;


		std::shared_ptr< program > make_shared( const program::config& );

		std::shared_ptr< render_states > make_shared(
			const render_states::config& );

		std::shared_ptr< geometry > make_shared( const geometry::config& );

		std::shared_ptr< primitives > make_shared( const primitives::config& );


		virtual ~context() = default;

	protected:
		virtual std::unique_ptr< program > make( const program::config& ) = 0;

		virtual std::unique_ptr< render_states > make(
			const render_states::config& ) = 0;

		virtual std::unique_ptr< geometry > make( const geometry::config& ) = 0;

		virtual std::unique_ptr< primitives > make(
			const primitives::config& ) = 0;

		std::vector< std::shared_ptr< memoizer > > _memoizers;

		context() = default;
		context( const context& ) = delete;
		context& operator=( const context& ) = delete;
	};
}
