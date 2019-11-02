
#pragma once

#include <igpu/utility/utility_types.h>
namespace igpu
{
	class geometry;
	class program;
	class render_states;
	class primitives;
	class instance_batch;

	struct batch_draw_config
	{
		utility::frustum frustum;
	};

	template<typename CHILD_T, typename ITEM_T>
	class batch_api_t
	{
	public:

		using child_t = typename CHILD_T;
		using item_t = typename ITEM_T;

		virtual item_t& item() const = 0;

		virtual size_t child_count() const = 0;

		virtual const child_t& child(size_t) const = 0;

		virtual child_t& child(size_t) = 0;

	protected:
		virtual ~batch_api_t() = default;
		batch_api_t() = default;
		batch_api_t(const batch_api_t&) = default;
		batch_api_t& operator= (const batch_api_t&) = default;
	};

	class geometry_batch : public batch_api_t<instance_batch, geometry>
	{
	};

	class material_batch : public batch_api_t<geometry_batch, primitives>
	{
	};

	class render_states_batch : public batch_api_t<material_batch, render_states>
	{
	};

	class program_batch : public batch_api_t<render_states_batch, program>
	{
	};

	class root_batch : public batch_api_t<program_batch, primitives>
	{
	protected:
		root_batch() = default;
		root_batch(const root_batch&) = delete;
		root_batch& operator= (const root_batch&) = delete;
	};
}
