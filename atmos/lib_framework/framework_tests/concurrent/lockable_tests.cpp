#include "framework_tests/concurrent/lockable_tests.h"
#include "framework/concurrent/lockable_individual.h"
#include "framework/concurrent/lockable_triple.h"

#include "framework_tests/main/tests.h"

using namespace tests;
using namespace concurrent;

namespace {

	enum TestFlags {
		CONCURRENT_READ_READ = 1,
		CONCURRENT_READ_WRITE = 2,
	};

	struct test_type {};

	template <template<typename> class lockable_t>
	void run_test(TestFlags flags, results* res)
	{
		lockable_t<test_type> lockable;
		(const lockable_t<test_type>&)lockable;

		{
			auto write = lockable.write_lock();
			auto write2 = std::move(write);
			
			bool expect_read_lock = flags & CONCURRENT_READ_WRITE;
			bool actual_read_lock = (bool)lockable.try_read_lock();
			CHECK_FAILURE((*res),
				expect_read_lock == actual_read_lock,
				"Should not be able to obtain read lock while write lock is obtained");
		}

		{
			auto read = lockable.read_lock();
			auto read2 = std::move(read);

			bool expect_write_lock = flags & CONCURRENT_READ_WRITE;
			bool actual_write_lock = (bool)lockable.try_write_lock();
			CHECK_FAILURE((*res), 
				expect_write_lock == actual_write_lock,
				"Should not be able to obtain write lock while read lock is obtained");

			bool expect_read_lock = flags & CONCURRENT_READ_READ;
			bool actual_read_lock = (bool)lockable.try_read_lock();
			CHECK_FAILURE((*res),
				expect_read_lock == actual_read_lock,
				"Should not be able to obtain read lock while write lock is obtained")
		}
	}
}

void lockable_tests::run(results* res)
{
	// supports concurrent reads
	// does not support concurrent read and write
	run_test<lockable_individual>(CONCURRENT_READ_READ, res);

	// does not support concurrent reads
	// supports current read+ write
	run_test<lockable_triple>(CONCURRENT_READ_WRITE, res);
}
