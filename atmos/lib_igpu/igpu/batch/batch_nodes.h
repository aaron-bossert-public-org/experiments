
#pragma once

#include <igpu/batch/batch_binding.h>

#include <framework/logging/log.h>

namespace igpu
{
	class batch_parameters;
	class instance;

	// a "tree" of batches used for organizing opaque render passes
	class batch_node
	{
	public:

		virtual size_t child_count() const = 0;
		
		virtual const batch_node& child(size_t) const = 0;

		virtual batch_node& child(size_t) = 0;

	protected:
		virtual ~batch_node() = default;
		batch_node() = default;
		batch_node(const batch_node&) = default;
		batch_node& operator= (const batch_node&) = default;
	};

	class instance_batch : public batch_node
	{
	};

	template<typename CHILD_T, typename ITEM_T>
	class batch_api_t : public batch_node
	{
	public:

		using child_t = typename CHILD_T;
		using item_t = typename ITEM_T;

		virtual item_t& item() const = 0;

		virtual size_t child_count() const = 0;

		virtual const child_t& child(size_t) const = 0;

		virtual child_t& child(size_t) = 0;
	};

	class geometry_batch : public batch_api_t<instance_batch, geometry>
	{
	};

	class material_batch : public batch_api_t<geometry_batch, material>
	{
	};

	class render_state_batch : public batch_api_t<material_batch, render_states>
	{
	};

	class program_batch : public batch_api_t<render_state_batch, program>
	{
	};

	class root_batch : public batch_api_t<program_batch, batch_parameters>
	{
		root_batch(const batch_node&) = delete;
		root_batch& operator= (const root_batch&) = delete;
	};
}
