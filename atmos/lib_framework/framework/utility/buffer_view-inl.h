
#pragma once

#include "framework/logging/log.h"

inline
buffer_view_base::buffer_view_base(size_t size, void* data, size_t stride)
	: _size(size)
	, _stride(stride)
	, _data(data)
{
}

template <typename T>
inline
buffer_view_base::buffer_view_base(const T& ary_view)
	: _size(ary_view.size())
	, _stride(sizeof(T::value_type))
	, _data((void*)ary_view.data())
{
}
    
inline
void* buffer_view_base::data() const
{
    return _data;
}
    
inline
void buffer_view_base::reset()
{
    _size = 0;
    _data = nullptr;
}
    
inline
size_t buffer_view_base::size() const
{
    return _size;
}
    
inline
size_t buffer_view_base::byte_size() const
{
    return _size * _stride;
}
    
inline
size_t buffer_view_base::stride() const
{
    return _stride;
}
    
inline
void* buffer_view_base::at(size_t i) const
{
    if(i < _size)
    {
        return _at(i);
    }
    else
    {
		LOG_CRITICAL( "index out of bounds" );
    }
        
    // return null pointer so crash occurs on access
    return nullptr;
}

inline
bool buffer_view_base::valid() const
{
    return _size && _stride && _data;
}
    
inline
void* buffer_view_base::_at(size_t i) const
{
    return (char*)_data + (i * _stride);
}

//------------------------------------------------------------------------------
//
//
template <typename T>
buffer_view<T>::buffer_view()
: buffer_view_base(0, nullptr, sizeof(T))
{
}

template <typename T>
buffer_view<T>::buffer_view(size_t size, T* data)
: buffer_view_base(size, (void*)data, sizeof(T))
{
}

template <typename T>
buffer_view<T>::buffer_view(size_t size, T* data, size_t stride)
	: buffer_view_base(size, (void*)data, stride)
{
}

template <typename T>
const T& buffer_view<T>::operator[](size_t i) const
{
    return *(T*)at(i);
}

template <typename T>
T& buffer_view<T>::operator[](size_t i)
{
    return *(T*)at(i);
}

template <typename T>
typename buffer_view<T>::iter buffer_view<T>::begin()
{
    ASSERT_CONTEXT(_stride);
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    return iter(begin_(), begin_(), end_() , _stride);
#endif
    
    return iter(begin_(), _stride);
}

template <typename T>
typename buffer_view<T>::iter buffer_view<T>::end()
{
    ASSERT_CONTEXT(_stride);
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    return iter(end_(), begin_(), end_() , _stride);
#endif
    
    return iter(end_(), _stride);
}

template <typename T>
const typename buffer_view<T>::iter buffer_view<T>::begin() const
{
    ASSERT_CONTEXT(_stride);
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    return iter(begin_(), begin_(), end_() , _stride);
#else
    return iter(begin_(), _stride);
#endif
}

template <typename T>
const typename buffer_view<T>::iter buffer_view<T>::end() const
{
    ASSERT_CONTEXT(_stride);
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    return iter(end_(), begin_(), end_() , _stride);
#else
    return iter(end_(), _stride);
#endif    
}

template <typename T>
T* buffer_view<T>::begin_() const
{
    return (T*)_data;
}

template <typename T>
T* buffer_view<T>::end_() const
{
    return (T*)_at(_size);
}

//------------------------------------------------------------------------------
//
//
template <typename T>
buffer_view<T>::iter::iter(T* data, size_t stride)
: _data( (char*)data )
, _stride( stride )
{
}

#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
template <typename T>
buffer_view<T>::iter::iter(T* data, T* begin, T* end, size_t stride)
: _data( (char*)data )
, _begin( (char*)begin )
, _end( (char*)end )
, _stride( stride )
{
}
#endif

template <typename T>
bool buffer_view<T>::iter::operator==( const iter& rhs ) const
{
    return _data == rhs._data;
}

template <typename T>
bool buffer_view<T>::iter::operator!=( const iter& rhs ) const
{
    return _data != rhs._data;
}

template <typename T>
typename buffer_view<T>::iter& buffer_view<T>::iter::operator++()
{
    _data += _stride;
    
    return *this;
}

template <typename T>
typename buffer_view<T>::iter buffer_view<T>::iter::operator++(int)
{
    auto ret = *this;
    
    _data += _stride;
    
    return ret;
}

template <typename T>
typename buffer_view<T>::iter& buffer_view<T>::iter::operator+=(int size)
{
    _data += _stride * size;
    
    return *this;
}

template <typename T>
typename buffer_view<T>::iter& buffer_view<T>::iter::operator--()
{
    _data -= _stride;
    
    return *this;
}

template <typename T>
typename buffer_view<T>::iter buffer_view<T>::iter::operator--(int)
{
    auto ret = *this;
    
    _data -= _stride;
    
    return ret;
}

template <typename T>
typename buffer_view<T>::iter& buffer_view<T>::iter::operator-=(int size)
{
    _data -= _stride * size;
    
    return *this;
}

template <typename T>
size_t buffer_view<T>::iter::operator-(const typename buffer_view<T>::iter& other) const
{
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    ASSERT_CONTEXT(_begin == other._begin);
    ASSERT_CONTEXT(_end == other._end);
#endif
    
    return (_data - other._data) / _stride;
}

template <typename T>
T& buffer_view<T>::iter::operator*()
{
    
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    validate();
#endif
    
    return *(T*)_data;
}

template <typename T>
const T& buffer_view<T>::iter::operator*() const
{
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    validate();
#endif
    
    return *(T*)_data;
}

template <typename T>
T* buffer_view<T>::iter::operator->()
{
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    validate();
#endif
    
    return (T*)_data;
}

template <typename T>
const T* buffer_view<T>::iter::operator->() const
{
#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
    validate();
#endif
    
    return (T*)_data;
}

#if ATMOS_BUFFER_VIEW_BOUNDS_CHECK
template <typename T>
void buffer_view<T>::iter::validate() const
{
    ASSERT_CONTEXT( _begin <= _data );
    ASSERT_CONTEXT( _data <= _end );
}
#endif
