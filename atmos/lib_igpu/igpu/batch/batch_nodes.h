
#pragma once

#include "igpu/utility/utility_types.h"

namespace igpu
{
	class program;
	class render_states;
	class geometry;
	class primitives;
	class instance_batch;

	template < typename ITEM_T, typename CHILD_T >
	class batch_api_t
	{
	public:
		using child_t = typename CHILD_T;
		using item_t = typename ITEM_T;

		virtual item_t& item() const = 0;

		virtual size_t child_count() const = 0;

		virtual const child_t& child( size_t ) const = 0;

		virtual child_t& child( size_t ) = 0;

	protected:
		virtual ~batch_api_t() = default;
		batch_api_t() = default;
		batch_api_t( const batch_api_t& ) = default;
		batch_api_t& operator=( const batch_api_t& ) = default;
	};

	class material_batch : public batch_api_t< primitives, instance_batch >
	{};

	class geometry_batch : public batch_api_t< geometry, material_batch >
	{};

	class states_batch : public batch_api_t< render_states, geometry_batch >
	{};

	class program_batch : public batch_api_t< program, states_batch >
	{};

	class raster_batch : public batch_api_t< primitives, program_batch >
	{
	protected:
		raster_batch() = default;
		raster_batch( const raster_batch& ) = delete;
		raster_batch& operator=( const raster_batch& ) = delete;
	};
}
