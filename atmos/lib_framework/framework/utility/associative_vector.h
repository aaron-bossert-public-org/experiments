
#pragma once

#include <unordered_map>
#include <vector>

// use to store a cache friendly array of values that are searchable by key
// a separate 
template <typename KEY, typename VAL>
class associative_vector
{
public:

	using map_t = std::unordered_map<KEY, size_t>;
	using iter_t = typename map_t::iterator;

	const VAL* find_value(const KEY& key) const
	{
		auto found = _index_map(key);
		if (found == _index_map.end())
		{
			return nullptr;
		}

		return &_elements[found.second];
	}

	VAL* find_value(const KEY& key)
	{
		auto const_this = (const associative_vector*)this;
		const VAL* found = const_this->find_value(key);
		return (VAL*)found;
	}

	iter_t find_iter(const KEY& key)
	{
		return _index_map.find(key);
	}

	iter_t end_iter(const KEY& key) const
	{
		return _index_map.end();
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
		return erase(find_iter(key));
	}

	bool erase(const iter_t& iter)
	{
		if (iter != _index_map.end())
		{
			size_t index = iter.second;

			_elements[index] = std::move(elements.back());
			_elements.pop_back();

			_index_table[index] = std::move(_index_table.back());
			*_index_table[index] = index;
			_index_table.pop_back();

			_index_map.erase(iter);

			return true;
		}

		return false;
	}

	size_t size() const
	{
		return _elements.size();
	}

	const VAL& operator[](size_t i) const
	{
		return _elements[i];
	}

	VAL& operator[](size_t i)
	{
		return _elements[i];
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

	std::vector<VAL> _elements;
	std::vector<size_t*> _index_table;
	map_t _index_map;
};