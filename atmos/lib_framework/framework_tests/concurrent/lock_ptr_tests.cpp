#include <framework_tests/concurrent/lock_ptr_tests.h>

#include <framework/concurrent/lock_ptr.h>
#include <type_traits>

using namespace tests;
using namespace concurrent;

namespace {
	struct test_t {};
	using c_test_t = const test_t;

	template<class T>
	using c_lock_ptr = const lock_ptr<T>;
}

void lock_ptr_tests::run(results* /* res */)
{
	// lock_ptr<test_t>& conversions
	static_assert(true ==  std::is_convertible<  lock_ptr<  test_t>&, c_lock_ptr<  test_t>&>::value, "expect to be able to ref t to const ref t");
	static_assert(true ==  std::is_convertible<  lock_ptr<  test_t>&,   lock_ptr<c_test_t>&>::value, "expect to be able to ref t to ref const t");
	static_assert(true ==  std::is_convertible<  lock_ptr<  test_t>&, c_lock_ptr<c_test_t>&>::value, "expect to be able to ref t to const ref const t");

	// const lock_ptr<test_t>& conversions
	static_assert(false == std::is_convertible<c_lock_ptr<  test_t>&,   lock_ptr<  test_t>&>::value, "expect to not be able to ref t to ref t");
	static_assert(false == std::is_convertible<c_lock_ptr<  test_t>&,   lock_ptr<c_test_t>&>::value, "expect to not be able to ref t to ref const t");
	static_assert(true  == std::is_convertible<c_lock_ptr<  test_t>&, c_lock_ptr<c_test_t>&>::value, "expect to be able to ref t to const ref const t");

	// lock_ptr<const test_t>& conversions
	static_assert(false == std::is_convertible<  lock_ptr<c_test_t>&,   lock_ptr<  test_t>&>::value, "expect to not be able to ref t to ref t");
	static_assert(false == std::is_convertible<  lock_ptr<c_test_t>&, c_lock_ptr<  test_t>&>::value, "expect to not be able to ref t to const ref t");
	static_assert(true  == std::is_convertible<  lock_ptr<c_test_t>&, c_lock_ptr<c_test_t>&>::value, "expect to be able to ref t to const ref const t");

	// const lock_ptr<const test_t>& conversions
	static_assert(false == std::is_convertible<c_lock_ptr<c_test_t>&,   lock_ptr<  test_t>&>::value, "expect to not be able to ref t to ref t");
	static_assert(false == std::is_convertible<c_lock_ptr<c_test_t>&, c_lock_ptr<  test_t>&>::value, "expect to not be able to ref t to const ref t");
	static_assert(false == std::is_convertible<c_lock_ptr<c_test_t>&,   lock_ptr<c_test_t>&>::value, "expect to not be able to ref t to ref const t");
}
