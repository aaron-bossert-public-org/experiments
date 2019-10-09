#pragma once

#include <string>
#include <sstream>

#define APPEND_FAILURE(RESULTS, REASON) RESULTS.append_failure((std::ostringstream() << __FILE__ << "(" << __LINE__ << ") " << __func__ << " failure: " << REASON).str())
#define CHECK_FAILURE(RESULTS, CONDITION, REASON) if(!(CONDITION)) { APPEND_FAILURE(RESULTS, REASON#CONDITION); }

namespace tests
{
	class results
	{
	public:

		void append_failure(std::string);

		const std::string& failures() const;

		bool success() const;

	private:
		std::string _failures;
	};

	results run();
}