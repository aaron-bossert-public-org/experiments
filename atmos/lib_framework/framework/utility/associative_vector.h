
#pragma once

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <unordered_map>

// use to store a cache array of values for cache friendly traversal
// that are searchable by key.
// adds an extra layer of indirection to key lookup.
// also provides support for weak_ptr style references to elements.
template < typename KEY, typename VAL >
class associative_vector
{
public:
	class element_ref;

	using key_t = KEY;
	using val_t = VAL;
	using vec_t = std::vector< val_t >;
	using shared_element_t = std::shared_ptr< element_ref >;
	using vec_element_ptr_t = std::vector< shared_element_t >;
	using map_element_ptr_t = std::unordered_map< key_t, element_ref* >;
	using iter_t = typename map_element_ptr_t::iterator;
	using emplaced_t = std::pair< iter_t, bool >;

	class element_ref
	{
	public:
		const key_t& key() const noexcept;

		const val_t& val() const noexcept;

		val_t& val() noexcept;

	private:
		friend class associative_vector;

		associative_vector* _owner = nullptr;
		key_t _key;
		size_t _index = (size_t)-1;
	};

	/// map interface
	template < typename... ARGS >
	emplaced_t emplace( const key_t& key, ARGS&&... args );

	iter_t find( const key_t& key );

	bool found( const iter_t& iter );

	bool erase( const iter_t& iter );

	// array interface
	size_t size() const;

	const val_t& operator[]( size_t i ) const;

	val_t& operator[]( size_t i );

	const val_t* begin() const;

	const val_t* end() const;

	val_t* begin();

	val_t* end();

	// take a reference that will remain valid after vector resizes
	::scoped_ptr< element_ref > scoped_ptr( const val_t* p_val ) const;

private:
	vec_t _elements;
	vec_element_ptr_t _element_ptrs;
	map_element_ptr_t _key_to_element_ptrs;
};

template < typename KEY, typename VAL >
template < typename... ARGS >
typename associative_vector< KEY, VAL >::emplaced_t associative_vector<
	KEY,
	VAL >::emplace( const KEY& key, ARGS&&... args )
{
	auto found = _key_to_element_ptrs.find( key );
	if ( found == _key_to_element_ptrs.end() )
	{
		auto ref_ptr = new element_ref();
		ref_ptr->_owner = this;
		ref_ptr->_key = key;
		ref_ptr->_index = _element_ptrs.size();

		_element_ptrs.emplace_back( ref_ptr );

		auto emplaced = _key_to_element_ptrs.emplace( key, ref_ptr );

		_elements.emplace_back( std::forward< ARGS >( args )... );

		return emplaced;
	}

	return {
		found,
		false,
	};
}

template < typename KEY, typename VAL >
typename associative_vector< KEY, VAL >::iter_t associative_vector< KEY, VAL >::
	find( const KEY& key )
{
	return _key_to_element_ptrs.find( key );
}

template < typename KEY, typename VAL >
bool associative_vector< KEY, VAL >::found( const iter_t& iter )
{
	return iter != _key_to_element_ptrs.end();
}

template < typename KEY, typename VAL >
bool associative_vector< KEY, VAL >::erase( const iter_t& iter )
{
	if ( iter != _key_to_element_ptrs.end() )
	{
		size_t index = iter->second->_index;

		_key_to_element_ptrs.erase( iter );

		_element_ptrs[index] = std::move( _element_ptrs.back() );
		_element_ptrs[index]->_index = index;
		_element_ptrs.pop_back();

		_elements[index] = std::move( _elements.back() );
		_elements.pop_back();

		return true;
	}

	LOG_CRITICAL( "iter not found" );

	return false;
}

template < typename KEY, typename VAL >
size_t associative_vector< KEY, VAL >::size() const
{
	return _elements.size();
}

template < typename KEY, typename VAL >
const VAL& associative_vector< KEY, VAL >::operator[]( size_t i ) const
{
	return _elements[i];
}

template < typename KEY, typename VAL >
VAL& associative_vector< KEY, VAL >::operator[]( size_t i )
{
	return _elements[i];
}

template < typename KEY, typename VAL >
const VAL* associative_vector< KEY, VAL >::begin() const
{
	return _elements.data();
}

template < typename KEY, typename VAL >
const VAL* associative_vector< KEY, VAL >::end() const
{
	return _elements.data() + _elements.size();
}

template < typename KEY, typename VAL >
VAL* associative_vector< KEY, VAL >::begin()
{
	return _elements.data();
}

template < typename KEY, typename VAL >
VAL* associative_vector< KEY, VAL >::end()
{
	return _elements.data() + _elements.size();
}

template < typename KEY, typename VAL >
::scoped_ptr< typename associative_vector< KEY, VAL >::element_ref >
	associative_vector< KEY, VAL >::scoped_ptr( const VAL* val ) const
{
	size_t index = val - _elements.data();

	if ( index < _elements.size() )
	{
		return _element_ptrs[index];
	}

	ASSERT_CONTEXT( "address of val is not within the vector's active data" );

	return nullptr;
}

template < typename KEY, typename VAL >
const KEY& associative_vector< KEY, VAL >::element_ref::key() const noexcept
{
	return _key;
}

template < typename KEY, typename VAL >
const VAL& associative_vector< KEY, VAL >::element_ref::val() const noexcept
{
	return ( *_owner )[_index];
}

template < typename KEY, typename VAL >
VAL& associative_vector< KEY, VAL >::element_ref::val() noexcept
{
	return ( *_owner )[_index];
}