
#include <cstdlib>
#include <thread>
#include <dcs_test.h>


int main() {
	auto test = dcs_test::make();

	while (test->advance()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return EXIT_SUCCESS;
}
