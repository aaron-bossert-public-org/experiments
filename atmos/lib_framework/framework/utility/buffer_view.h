
#pragma once

#define ATMOS_F_BOUNDS_CHECK ATMOS_DEBUG

class buffer_view_base
{
public:
	buffer_view_base() = default;

	buffer_view_base( const buffer_view_base& ) = default;

	buffer_view_base( size_t size, void* data, size_t stride );

	template < typename T >
	buffer_view_base( const T& );

	virtual ~buffer_view_base() = default;

	void* data() const;

	virtual void reset();

	size_t size() const;

	size_t byte_size() const;

	size_t stride() const;

	void* at( size_t i ) const;

	bool valid() const;

protected:
	void* _at( size_t i ) const;

protected:
	size_t _size = 0;
	size_t _stride = 0;
	void* _data = nullptr;
};

//------------------------------------------------------------------------------
// buffer_view - allows safer access of stream type buffers using std container
// like syntax
//
template < typename T >
class buffer_view : public buffer_view_base
{
public:
	typedef T value_type;

	using buffer_view_base::buffer_view_base;

	buffer_view();

	buffer_view( size_t size, T* data );

	buffer_view( size_t size, T* data, size_t stride );

	virtual ~buffer_view()
	{
		reset();
	}

	const T& operator[]( size_t i ) const;

	T& operator[]( size_t i );

	const T* data() const;

	T* data();
	
	class iter;

	iter begin();

	iter end();

	const iter begin() const;

	const iter end() const;

private:
	T* begin_() const;

	T* end_() const;
};

//------------------------------------------------------------------------------
// buffer_view<T>::iter - iterator for buffer_view<T>
//
template < typename T >
class buffer_view< T >::iter
{
public:
	iter( T* data, size_t stride );

#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK

	iter( T* data, T* begin, T* end, size_t stride );

#endif

	bool operator==( const iter& other ) const;

	bool operator!=( const iter& other ) const;

	iter& operator++();

	iter operator++( int );

	iter& operator+=( int );

	iter& operator--();

	iter operator--( int );

	iter& operator-=( int );

	size_t operator-( const iter& ) const;

	T& operator*();

	const T& operator*() const;

	T* operator->();

	const T* operator->() const;

#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK

	void validate() const;

private:
	char *_begin = nullptr, *_end = nullptr;

#endif

private:
	size_t _stride = 0;
	char* _data = nullptr;
};

#include "framework/utility/buffer_view-inl.h"