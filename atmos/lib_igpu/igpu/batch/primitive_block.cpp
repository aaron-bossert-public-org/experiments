
#include <igpu/batch/primitive_block.h>

#include <framework/logging/log.h>

using namespace igpu;

primitive_block::primitive_block(
	get_index_of_t get_index_of,
	get_default_t get_default,
	size_t index_count)
	: get_index_of(std::move(get_index_of))
	, get_default(std::move(get_default))
	, _local_indices(index_count, (size_t)-1)
{
}

size_t primitive_block::index_count() const
{
	return _local_indices.size();
}

const primitive_block::element* primitive_block::find(size_t fixed_index) const
{
	if (fixed_index < _local_indices.size())
	{
		size_t local_index = _local_indices[fixed_index];
		if (local_index < _elements.size())
		{
			return &_elements[local_index];
		}
	}

	return nullptr;
}

void primitive_block::set(size_t fixed_index, const primitive& primitive)
{
	if (fixed_index < _local_indices.size())
	{
		size_t local_index = _local_indices[fixed_index];
		if (local_index >= _elements.size())
		{
			local_index = _elements.size();
			_local_indices[fixed_index] = local_index;
			_elements.push_back({
				fixed_index,
				get_default(fixed_index) });
		}

		_elements[local_index].primitive = primitive;
	}
}

bool primitive_block::erase(size_t fixed_index)
{
	if (fixed_index < _local_indices.size())
	{
		size_t local_index = _local_indices[fixed_index];
		if (local_index < _elements.size())
		{
			ASSERT_CONTEXT(_local_indices[fixed_index] != -1);
			_local_indices[fixed_index] = (size_t)-1;
			_local_indices[_elements.back().fixed_index] = local_index;
			_elements[local_index] = _elements.back();
			_elements.pop_back();

			return true;
		}
	}

	return false;
}
