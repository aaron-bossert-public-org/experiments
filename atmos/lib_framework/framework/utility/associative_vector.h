
#pragma once

#include <unordered_map>
#include <vector>

// use to store a cache friendly array of values that are searchable by key
// a separate 
template <typename KEY, typename VAL>
class associative_vector
{
public:

	const T* get(const KEY& key) const
	{
		auto found = _index_map(key);
		if (found == _index_map.end())
		{
			return nullptr;
		}

		return &_elements[found.second];
	}

	T* get(const KEY& key)
	{
		auto const_this = (const associative_vector*)this;
		const VAL* found = const_this->find(key);
		return (VAL*)found;
	}

	template<typename... ARGS>
	std::pair<VAL*, bool> emplace(const KEY& key, ARGS&&...args)
	{
		auto found = _index_map(key);
		if (found == _index_map.end())
		{
			_elements.emplace_back(std::forward<ARGS>(args)...);
			auto emplaced = _index_map.emplace(key, _index_table.size());
			_index_table.emplace_back(&emplaced.first);
			return 
		}

		return { &_elements[found.first], false };
	}

	bool erase(const KEY& key)
	{
		auto found = _index_map(key);
		if (found != _index_map.end())
		{
			size_t index = found.second;

			_elements[index] = std::move(elements.back());
			_elements.pop_back();

			_index_table[index] = std::move(_index_table.back());
			*_index_table[index] = index;
			_index_table.pop_back();

			_index_map.erase(found);
			
			return true;
		}

		return false;
	}

	const VAL* begin() const
	{
		return _elements.data();
	}

	const VAL* end() const
	{
		return _elements.data() + _elements.size();
	}

	VAL* begin() const
	{
		return _elements.data();
	}

	VAL* end() const
	{
		return _elements.data() + _elements.size();
	}

private:
	std::Vector<T> _elements;
	std::Vector<size_t*> _index_table;
	std::unordered_map<void*, size_t> _index_map;
};