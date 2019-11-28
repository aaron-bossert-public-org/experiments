#pragma once

#include "igpu/batch/opaque_batch.h"
#include "igpu/batch/transparent_batch.h"
#include "igpu/buffer/geometry.h"
#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/topology.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/compute/compute_binding.h"
#include "igpu/compute/compute_buffer.h"
#include "igpu/compute/compute_pipeline.h"
#include "igpu/compute/compute_program.h"
#include "igpu/compute/compute_shader.h"
#include "igpu/shader/fragment_shader.h"
#include "igpu/shader/graphics_pipeline.h"
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
#include "igpu/window/window.h"

#include <functional>
#include <memory>

namespace igpu
{
	class window;
	class tracker;

	class context
	{
	public:
		struct config
		{
			std::string name;
			color_format color_format = color_format::INT_R8G8B8A8;
			depth_format depth_format = depth_format::D24S8;
			bool vsync = true;
			bool enable_validation
#if ATMOS_DEBUG
				= true;
#else
				= false;
#endif
		};

		template < typename T >
		using sp_t = std::shared_ptr< T >;

		template < typename T >
		using config_t = const typename T::config&;

		virtual const config& cfg() const = 0;

		virtual scoped_ptr< draw_target > back_buffer() = 0;

		virtual scoped_ptr< window > window() const = 0;

		virtual ~context() = default;

		sp_t< program > make_shared( config_t< program > );

		sp_t< compute_program > make_shared( config_t< compute_program > );

		sp_t< render_states > make_shared( config_t< render_states > );

		sp_t< geometry > make_shared( config_t< geometry > );

		sp_t< primitives > make_shared( config_t< primitives > );

		sp_t< graphics_pipeline > make_shared( config_t< graphics_pipeline > );

		sp_t< compute_pipeline > make_shared( config_t< compute_pipeline > );

		sp_t< draw_target > make_shared( config_t< draw_target > );

		sp_t< render_buffer > make_shared( config_t< render_buffer > );

		sp_t< render_texture2d > make_shared( config_t< render_texture2d > );

		sp_t< depth_buffer > make_shared( config_t< depth_buffer > );

		sp_t< depth_texture2d > make_shared( config_t< depth_texture2d > );

		sp_t< vertex_buffer > make_shared( config_t< vertex_buffer > );

		sp_t< index_buffer > make_shared( config_t< index_buffer > );

		sp_t< compute_buffer > make_shared( config_t< compute_buffer > );

		sp_t< texture2d > make_shared( config_t< texture2d > );

		sp_t< opaque_batch > make_shared( config_t< opaque_batch > );

		sp_t< transparent_batch > make_shared( config_t< transparent_batch > );

		sp_t< compute_binding > make_shared( config_t< compute_binding > );

		sp_t< vertex_shader > make_shared(
			config_t< vertex_shader >,
			std::vector< uint32_t >&& );

		sp_t< fragment_shader > make_shared(
			config_t< fragment_shader >,
			std::vector< uint32_t >&& );

		sp_t< compute_shader > make_shared(
			config_t< compute_shader >,
			std::vector< uint32_t >&& );

		template < typename T >
		void visit( const std::function< void( T* ) >& );

	protected:
		virtual std::unique_ptr< program > make( const program::config& ) = 0;

		virtual std::unique_ptr< compute_program > make(
			const compute_program::config& ) = 0;

		virtual std::unique_ptr< render_states > make(
			const render_states::config& ) = 0;

		virtual std::unique_ptr< geometry > make( const geometry::config& ) = 0;

		virtual std::unique_ptr< primitives > make(
			const primitives::config& ) = 0;

		virtual std::unique_ptr< graphics_pipeline > make(
			const graphics_pipeline::config& ) = 0;

		virtual std::unique_ptr< compute_pipeline > make(
			const compute_pipeline::config& ) = 0;

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

		virtual std::unique_ptr< compute_binding > make(
			const compute_binding::config& ) = 0;

		virtual std::unique_ptr< vertex_shader > make(
			const vertex_shader::config&,
			std::vector< uint32_t >&& ) = 0;

		virtual std::unique_ptr< fragment_shader > make(
			const fragment_shader::config&,
			std::vector< uint32_t >&& ) = 0;

		virtual std::unique_ptr< compute_shader > make(
			const compute_shader::config&,
			std::vector< uint32_t >&& ) = 0;

		std::vector< std::shared_ptr< tracker > > _trackers;

		context() = default;
		context( const context& ) = delete;
		context& operator=( const context& ) = delete;
	};

	template <>
	void context::visit< program >( const std::function< void( program* ) >& );

	template <>
	void context::visit< render_states >(
		const std::function< void( render_states* ) >& );

	template <>
	void context::visit< geometry >(
		const std::function< void( geometry* ) >& );

	template <>
	void context::visit< primitives >(
		const std::function< void( primitives* ) >& );

	template <>
	void context::visit< graphics_pipeline >(
		const std::function< void( graphics_pipeline* ) >& );

	template <>
	void context::visit< draw_target >(
		const std::function< void( draw_target* ) >& );

	template <>
	void context::visit< render_buffer >(
		const std::function< void( render_buffer* ) >& );

	template <>
	void context::visit< render_texture2d >(
		const std::function< void( render_texture2d* ) >& );

	template <>
	void context::visit< depth_buffer >(
		const std::function< void( depth_buffer* ) >& );

	template <>
	void context::visit< depth_texture2d >(
		const std::function< void( depth_texture2d* ) >& );

	template <>
	void context::visit< vertex_shader >(
		const std::function< void( vertex_shader* ) >& );

	template <>
	void context::visit< fragment_shader >(
		const std::function< void( fragment_shader* ) >& );

	template <>
	void context::visit< vertex_buffer >(
		const std::function< void( vertex_buffer* ) >& );

	template <>
	void context::visit< index_buffer >(
		const std::function< void( index_buffer* ) >& );

	template <>
	void context::visit< compute_buffer >(
		const std::function< void( compute_buffer* ) >& );

	template <>
	void context::visit< texture2d >(
		const std::function< void( texture2d* ) >& );

	template <>
	void context::visit< opaque_batch >(
		const std::function< void( opaque_batch* ) >& );

	template <>
	void context::visit< transparent_batch >(
		const std::function< void( transparent_batch* ) >& );
}
