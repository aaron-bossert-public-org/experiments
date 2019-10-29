
#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

// use to store a cache friendly array of values that are searchable by key
// a separate 
template <typename KEY, typename VAL>
class associative_vector
{
public:
	using key_t = KEY;
	using val_t = VAL;
	using map_t = std::unordered_map<key_t, size_t>;
	using iter_t = typename map_t::iterator;

	const val_t* find_value(const key_t& key) const
	{
		auto found = _index_map.find(key);
		if (found == _index_map.end())
		{
			return nullptr;
		}

		return &_elements[found->second];
	}

	val_t* find_value(const key_t& key)
	{
		auto const_this = (const associative_vector*)this;
		const val_t* found = const_this->find_value(key);
		return (val_t*)found;
	}

	iter_t find_iter(const key_t& key)
	{
		return _index_map.find(key);
	}

	iter_t end_iter()
	{
		return _index_map.end();
	}

	template<typename... ARGS>
	std::pair<val_t*, bool> emplace(const key_t& key, ARGS&&...args)
	{
		auto found = _index_map.find(key);
		if (found == _index_map.end())
		{
			auto emplaced = _index_map.emplace(key, _index_table.size());
			_index_table.emplace_back(&emplaced.first->second);
			_elements.emplace_back(std::forward<ARGS>(args)...);
			*_begin_ptr = _elements.data();
			
			return { &_elements.back(), true };
		}

		return { &_elements[found->second], false };
	}

	bool erase(const key_t& key)
	{
		return erase(find_iter(key));
	}

	bool erase(const iter_t& iter)
	{
		if (iter != _index_map.end())
		{
			size_t index = iter->second;

			_elements[index] = std::move(_elements.back());
			_elements.pop_back();

			_index_table[index] = std::move(_index_table.back());
			*_index_table[index] = index;
			_index_table.pop_back();

			_index_map.erase(iter);
			*_begin_ptr = _elements.data();
			return true;
		}

		return false;
	}

	size_t size() const
	{
		return _elements.size();
	}

	const val_t& operator[](size_t i) const
	{
		return _elements[i];
	}

	val_t& operator[](size_t i)
	{
		return _elements[i];
	}

	const val_t* begin() const
	{
		return _elements.data();
	}

	const val_t* end() const
	{
		return _elements.data() + _elements.size();
	}

	val_t* begin()
	{
		return _elements.data();
	}

	val_t* end()
	{
		return _elements.data() + _elements.size();
	}

	const std::shared_ptr<val_t*>& begin_ptr() const
	{
		return _begin_ptr;
	}

	const std::vector<size_t*>& index_table() const
	{
		return _index_table;
	}

private:
	std::shared_ptr<val_t*> _begin_ptr = std::shared_ptr<val_t*>(new val_t*());
	std::vector<val_t> _elements;
	std::vector<size_t*> _index_table;
	map_t _index_map;
};