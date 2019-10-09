
#pragma once

#include <igpu/material/primitive.h>

#include <functional>

namespace igpu
{
    // helper for mapping primitives to indexed slots by name
	// all operations o(1), except get_index_of which is defined by owner.
    class primitive_block
    {
    public:
		
		struct element
		{
			size_t fixed_index = (size_t)-1;
			igpu::primitive primitive;
		};
		
		using get_index_of_t = std::function<size_t(const std::string_view&)>;
		using get_default_t = std::function<const primitive&(size_t)>;

		primitive_block(
			get_index_of_t get_index_of,
			get_default_t get_default,
			size_t index_count);

		const get_index_of_t get_index_of;

		const get_default_t get_default;
		
		size_t index_count() const;

		const element* find(size_t fixed_index) const;

		void set(size_t fixed_index, const primitive&);

		bool erase(size_t fixed_index);

	private:

		std::vector<size_t> _local_indices;
		std::vector<element> _elements;
    };
}
