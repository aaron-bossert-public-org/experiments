
#pragma once
#include <type_traits>

namespace platform
{
	// Class exists as a way to have partial class definitions in c++.
	// Platform implementations of T must specialize partial<T> and replace these behaviours 
	// in compilation units that are allowed to define lifetimes.
	template <typename T>
	class partial
	{
		partial() = delete;
		partial(const partial&) = delete;
		partial& operator = (const partial&) = delete;
		~partial() = delete;
	};
}