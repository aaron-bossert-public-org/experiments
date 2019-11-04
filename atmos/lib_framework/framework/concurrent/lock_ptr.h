#pragma once

#ifndef NDEBUG
#	include <cassert>
#	include <thread>
#endif

namespace concurrent
{
	// Helper view into concrete types of lock pointers, does not extend ownership.
	template < typename T > class lock_ptr
	{

	public:
		T* operator->() const
		{
			return _ptr;
		}

		operator lock_ptr< const T >&()
		{
			return *(lock_ptr< const T >*)this;
		}

		operator const lock_ptr< const T >&() const
		{
			return *(lock_ptr< const T >*)this;
		}

		explicit operator bool() const
		{
			return (bool)_ptr;
		}

	protected:
		lock_ptr( T* ptr ) : _ptr { ptr }
		{}

		lock_ptr( lock_ptr&& other ) : _ptr { other._ptr }
		{
			other._ptr = nullptr;
		}

		lock_ptr& operator=( lock_ptr&& other )
		{
			return *new ( this ) lock_ptr { other };
		}

	private:
		T* _ptr;

		lock_ptr( const lock_ptr& ) = delete;
		lock_ptr& operator=( const lock_ptr& ) = delete;
	};
}	 // namespace concurrent
