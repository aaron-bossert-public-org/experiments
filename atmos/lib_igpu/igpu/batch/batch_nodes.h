
#pragma once

#include <igpu/batch/batch_binding.h>

#include <framework/logging/log.h>

namespace igpu
{
	class instance;

    class geometry;
	class geometry_batch;

	class material;
	class material_batch;
    
	class program;
	class program_batch;
    
	class render_states;
	class render_state_batch;

	class root_batch;

	// a "tree" of batches used for organizing opaque render passes
	class batch_node
	{
	public:

		virtual ~batch_node() = default;
	};

	class geometry_batch : public batch_node
	{
	public:

		using item_t = geometry;

		virtual item_t& item() const = 0;

		virtual std::unique_ptr<batch_binding> create_instance(const batch_binding::config&) = 0;

		virtual void remove_binding(const batch_binding& binding) = 0;

		virtual const instance* begin() const = 0;

		virtual const instance* end() const = 0;
	};

	class material_batch : public batch_node
	{
	public:

		using child_t = geometry_batch;
		using item_t = material;

		virtual child_t& create_child(const batch_binding::config& cfg) = 0;

		virtual item_t& item() const = 0;

		virtual const child_t* begin() const = 0;

		virtual const child_t* end() const = 0;
	};

	class render_state_batch : public batch_node
	{
	public:

		using child_t = material_batch;
		using item_t = render_states;

		virtual child_t& create_child(const batch_binding::config& cfg) = 0;

		virtual item_t& item() const = 0;

		virtual const child_t* begin() const = 0;

		virtual const child_t* end() const = 0;
	};

	class program_batch : public batch_node
	{
	public:

		using child_t = render_state_batch;
		using item_t = program;

		virtual child_t& create_child(const batch_binding::config& cfg) = 0;

		virtual item_t& item() const = 0;

		virtual const child_t* begin() const = 0;

		virtual const child_t* end() const = 0;
	};

	
	class root_batch : public batch_node
	{
	public:

		using child_t = program_batch;

		virtual child_t& create_child(const batch_binding::config& cfg) = 0;

		virtual const child_t* begin() const = 0;

		virtual const child_t* end() const = 0;
	};
}
