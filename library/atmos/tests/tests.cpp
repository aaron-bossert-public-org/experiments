
#include <atmos/tests.h>
#include "lock_ptr_tests.h"
#include "lockable_tests.h"

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