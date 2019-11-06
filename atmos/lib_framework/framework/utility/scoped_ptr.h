
#pragma once

#include "framework/logging/log.h"

#include <memory>

// scoped_ptr alerts when references live longer than their shared pointer
// targets.
template < class T >
class scoped_ptr : private std::weak_ptr< T >
{
	using weak_t = std::weak_ptr< T >;

public:
	constexpr scoped_ptr() noexcept {};

	constexpr scoped_ptr( nullptr_t ) noexcept {};

	scoped_ptr( scoped_ptr&& other ) noexcept
	{
		*this = std::move( other );
	}

	template < class T2 >
	scoped_ptr( scoped_ptr< T2 >&& other ) noexcept
	{
		*this = std::move( other );
	}

	scoped_ptr( const scoped_ptr& other ) noexcept
	{
		*this = other;
	}

	template < class T2 >
	scoped_ptr( const scoped_ptr< T2 >& other ) noexcept
	{
		*this = other;
	}

	template < class T2 >
	scoped_ptr( const std::shared_ptr< T2 >& other ) noexcept
	{
		*this = other;
	}

	scoped_ptr& operator=( scoped_ptr&& other ) noexcept
	{
		weak_t::operator=( std::move( other ) );
		std::swap( _ptr, other._ptr );
		return *this;
	}

	template < class T2 >
	scoped_ptr& operator=( scoped_ptr< T2 >&& other ) noexcept
	{
		weak_t::operator=( std::move( other ) );
		std::swap( _ptr, other._ptr );
		return *this;
	}

	scoped_ptr& operator=( const scoped_ptr& other ) noexcept
	{
		weak_t::operator=( other );
		_ptr = other._ptr;
		return *this;
	}

	template < class T2 >
	scoped_ptr& operator=( const scoped_ptr< T2 >& other ) noexcept
	{
		weak_t::operator=( other );
		_ptr = other._ptr;
		return *this;
	}

	template < class T2 >
	scoped_ptr& operator=( const std::shared_ptr< T2 >& other ) noexcept
	{
		weak_t::operator=( other );
		_ptr = other.get();
		return *this;
	}

	scoped_ptr& operator=( nullptr_t ) noexcept
	{
		reset();
		_ptr = nullptr;
		return *this;
	}

	explicit operator bool() const noexcept
	{
		return !expired();
	}

	T* operator->() const noexcept
	{
		ASSERT_CONTEXT(
			!_ptr || !expired(),
			"target has expired while scope_ptr was referencing it" );
#if ATMOS_DEBUG
		return lock().get();
#else
		return _ptr;
#endif
	}

	T& operator*() const noexcept
	{
		ASSERT_CONTEXT(
			!_ptr || !expired(),
			"target has expired while scope_ptr was referencing it" );
#if ATMOS_DEBUG
		return *lock().get();
#else
		return *_ptr;
#endif
	}

	~scoped_ptr() noexcept
	{
		ASSERT_CONTEXT(
			!_ptr || !expired(),
			"target has expired while scope_ptr was referencing it" );
	}

	template < class T2 >
	[[nodiscard]] friend bool operator==(
		const scoped_ptr& left,
		const scoped_ptr< T2 >& right ) noexcept
	{
		return left.lock().get() == right.lock().get();
	}

	template < class T2 >
	[[nodiscard]] friend bool operator==(
		const scoped_ptr& scoped,
		const std::shared_ptr< T2 >& other ) noexcept
	{
		return scoped.lock() == other.get();
	}

	template < class T1, class T2 >
	[[nodiscard]] friend bool operator==(
		const scoped_ptr& scoped,
		const T* other ) noexcept
	{
		return scoped.lock().get() == other;
	}

	template < class T2 >
	[[nodiscard]] friend bool operator==(
		const std::shared_ptr< T2 >& other,
		const scoped_ptr& scoped ) noexcept
	{
		return scoped.lock().get() == other.get();
	}

	template < class T1, class T2 >
	[[nodiscard]] friend bool operator==(
		const T* other,
		const scoped_ptr& scoped ) noexcept
	{
		return scoped.lock().get() == other;
	}

	template < class T2 >
	[[nodiscard]] friend bool operator!=(
		const scoped_ptr& left,
		const scoped_ptr< T2 >& right ) noexcept
	{
		return left.lock() != right.lock();
	}

	template < class T2 >
	[[nodiscard]] friend bool operator!=(
		const scoped_ptr& scoped,
		const std::shared_ptr< T2 >& other ) noexcept
	{
		return scoped.lock().get() != other.get();
	}

	template < class T1, class T2 >
	[[nodiscard]] friend bool operator!=(
		const scoped_ptr& scoped,
		const T* other ) noexcept
	{
		return scoped.lock().get() != other;
	}

	template < class T2 >
	[[nodiscard]] friend bool operator!=(
		const std::shared_ptr< T2 >& other,
		const scoped_ptr& scoped ) noexcept
	{
		return scoped.lock().get() != other.get();
	}

	template < class T1, class T2 >
	[[nodiscard]] friend bool operator!=(
		const T* other,
		const scoped_ptr& scoped ) noexcept
	{
		return scoped.lock().get() != other;
	}

private:
	void* _ptr = nullptr;
};