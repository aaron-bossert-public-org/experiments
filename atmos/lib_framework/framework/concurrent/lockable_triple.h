#pragma once

#include "framework/concurrent/lock_ptr.h"

#include <atomic>
#include <mutex>

namespace concurrent
{
	template < typename T >
	class lockable_triple
	{
	public:
		using lock = std::unique_lock< std::mutex >;

		class write_lock_ptr;
		class read_lock_ptr;

		write_lock_ptr write_lock();
		write_lock_ptr try_write_lock();
		read_lock_ptr read_lock() const;
		read_lock_ptr try_read_lock() const;

	private:
		void write_end();
		void read_begin() const;

		const T* prev_write() const;

		T _t[3];

		std::mutex _write_mtx;
		int _write_idx = 1;
		int _prev_write = 0;

		mutable std::mutex _read_mtx;
		mutable int _read_idx = 0;
		mutable int_fast8_t _prev_read_swap_count = 0;

		mutable std::atomic_int_fast8_t _swap_count = {
			0,
		};
	};

	template < typename T >
	class lockable_triple< T >::write_lock_ptr : public lock_ptr< T >
	{
	public:
		write_lock_ptr( T* ptr, lock lk, lockable_triple< T >* lockable_triple )
			: lock_ptr< T >( ptr )
			, _lk( std::move( lk ) )
			, _lockable_triple( lockable_triple )
		{}

		write_lock_ptr( write_lock_ptr&& other )
			: lock_ptr< T >( std::move( other ) )
			, _lk( std::move( other._lk ) )
			, _lockable_triple( other._lockable_triple )
		{
			other._lockable_triple = nullptr;
		}

		write_lock_ptr& operator=( write_lock_ptr&& other )
		{
			return *new ( this ) write_lock_ptr( other );
		}

		~write_lock_ptr()
		{
			if ( _lockable_triple )
			{
				_lockable_triple->write_end();
			}
		}

		const T* prev() const
		{
			return _lockable_triple ? _lockable_triple->prev_write() : nullptr;
		}


	private:
		lock _lk;
		lockable_triple< T >* _lockable_triple = 0;
	};

	template < typename T >
	class lockable_triple< T >::read_lock_ptr : public lock_ptr< const T >
	{
	public:
		read_lock_ptr( const T* ptr, lock lk )
			: lock_ptr< const T >( ptr )
			, _lk( std::move( lk ) )
		{}

		read_lock_ptr( read_lock_ptr&& other )
			: lock_ptr< const T >( std::move( other ) )
			, _lk( std::move( other._lk ) )
		{}

		read_lock_ptr& operator=( read_lock_ptr&& other )
		{
			return *new ( this ) write_lock_ptr( other );
		}

		~read_lock_ptr()
		{}

	private:
		lock _lk;
	};

	template < typename T >
	typename lockable_triple< T >::write_lock_ptr lockable_triple<
		T >::write_lock()
	{
		return write_lock_ptr( _t + _write_idx, lock( _write_mtx ), this );
	}

	template < typename T >
	typename lockable_triple< T >::write_lock_ptr lockable_triple<
		T >::try_write_lock()
	{
		lock lk( _write_mtx, std::try_to_lock );
		T* ptr = lk.owns_lock() ? _t + _write_idx : nullptr;

		return write_lock_ptr( ptr, std::move( lk ), this );
	}

	template < typename T >
	typename lockable_triple< T >::read_lock_ptr lockable_triple<
		T >::read_lock() const
	{
		lock lk( _read_mtx );
		read_begin();

		return read_lock_ptr( _t + _read_idx, std::move( lk ) );
	}

	template < typename T >
	typename lockable_triple< T >::read_lock_ptr lockable_triple<
		T >::try_read_lock() const
	{
		lock lk( _read_mtx, std::try_to_lock );

		if ( lk.owns_lock() )
		{
			read_begin();
			return read_lock_ptr( _t + _read_idx, std::move( lk ) );
		}

		return read_lock_ptr( nullptr, std::move( lk ) );
	}

	template < typename T >
	const T* lockable_triple< T >::prev_write() const
	{
		return _t + _prev_write;
	}

	// continually oscillate writing between the two buffers not currently being
	// read from as read buffer changes
	template < typename T >
	void lockable_triple< T >::write_end()
	{
		auto swap_count = _swap_count.fetch_add( 1 );
		_prev_write = _write_idx;

		_write_idx += swap_count % 2 ? 1 : 2;
		_write_idx %= 3;
	}

	template < typename T >
	void lockable_triple< T >::read_begin() const
	{
		int_fast8_t dummy = 0;

		if ( !_swap_count
				  .compare_exchange_strong( dummy, _prev_read_swap_count ) )
		{
			auto swap_count = _swap_count.fetch_add( 1 );
			_prev_read_swap_count = swap_count;
			_read_idx += swap_count % 2 ? 2 : 1;
			_read_idx %= 3;
		}
	}
}	 // namespace concurrent
