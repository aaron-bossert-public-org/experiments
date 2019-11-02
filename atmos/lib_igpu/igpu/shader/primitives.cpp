
#include <igpu/shader/primitives.h>

#include <framework/logging/log.h>

using namespace igpu;

bool primitives::find_expected(const std::string& name, size_t* p_expected) const
{
	size_t count = primitive_count();
	size_t expected = *p_expected;

	if (expected >= count)
	{
		expected = 0;
	}

	size_t at = expected;

	do
	{	
		if (primitive(at).cfg().name == name)
		{
			*p_expected = at;
			return true;
		}
		at++;
	} while (at != expected);

	return false;
}
