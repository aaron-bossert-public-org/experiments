#pragma once

#include "framework/concurrent/lock_ptr.h"

#include <atomic>
#include <shared_mutex>

namespace concurrent
{
	template < typename T >
	class lockable_individual
	{
	public:
		using lock = std::unique_lock< std::shared_mutex >;
		using shared_lock = std::shared_lock< std::shared_mutex >;

		class write_lock_ptr;
		class read_lock_ptr;

		write_lock_ptr write_lock();
		write_lock_ptr try_write_lock();

		read_lock_ptr read_lock() const;
		read_lock_ptr try_read_lock() const;

	private:
		mutable std::shared_mutex _mtx;
		T _t;
	};

	template < typename T >
	class lockable_individual< T >::write_lock_ptr : public lock_ptr< T >
	{
	public:
		write_lock_ptr( T* ptr, lock lk )
			: lock_ptr< T >{ ptr }
			, _lk{ std::move( lk ) }
		{}

		write_lock_ptr( write_lock_ptr&& other )
			: lock_ptr< T >{ std::move( other ) }
			, _lk{ std::move( other._lk ) }
		{}

		write_lock_ptr& operator=( write_lock_ptr&& other )
		{
			return *new ( this ) write_lock_ptr{ other };
		}

	private:
		lock _lk;
	};

	template < typename T >
	class lockable_individual< T >::read_lock_ptr : public lock_ptr< const T >
	{
	public:
		read_lock_ptr( const T* ptr, shared_lock lk )
			: lock_ptr< const T >{ ptr }
			, _lk{ std::move( lk ) }
		{}

		read_lock_ptr( read_lock_ptr&& other )
			: lock_ptr< const T >{ std::move( other ) }
			, _lk{ std::move( other._lk ) }
		{}

		read_lock_ptr& operator=( read_lock_ptr&& other )
		{
			return *new ( this ) write_lock_ptr{ other };
		}

	private:
		shared_lock _lk;
	};

	template < typename T >
	typename lockable_individual< T >::write_lock_ptr lockable_individual<
		T >::write_lock()
	{
		return write_lock_ptr{ &_t, lock{ _mtx } };
	}

	template < typename T >
	typename lockable_individual< T >::write_lock_ptr lockable_individual<
		T >::try_write_lock()
	{
		lock lk{ _mtx, std::try_to_lock };
		// auto lk = std::unique_lock<std::shared_mutex>(_mtx,
		// std::try_to_lock);
		T* ptr = lk.owns_lock() ? &_t : nullptr;

		return write_lock_ptr{ ptr, std::move( lk ) };
	}

	template < typename T >
	typename lockable_individual< T >::read_lock_ptr lockable_individual<
		T >::read_lock() const
	{
		return read_lock_ptr{ &_t, shared_lock{ _mtx } };
	}

	template < typename T >
	typename lockable_individual< T >::read_lock_ptr lockable_individual<
		T >::try_read_lock() const
	{
		shared_lock lk{ _mtx, std::try_to_lock };
		const T* ptr = lk.owns_lock() ? &_t : nullptr;

		return read_lock_ptr{ ptr, std::move( lk ) };
	}
}
