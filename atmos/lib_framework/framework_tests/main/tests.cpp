
#include <framework_tests/main/tests.h>
#include <framework_tests/concurrent/lock_ptr_tests.h>
#include <framework_tests/concurrent/lockable_tests.h>

using namespace tests;

void results::append_failure(std::string failure)
{
	_failures += "\n" + failure;
}


const std::string& results::failures() const
{
	return _failures;
}

bool results::success() const
{
	return _failures == "";
}

results tests::run()
{
	results res;

	lockable_tests::run(&res);
	
	lock_ptr_tests::run(&res);
	
	return res;
}