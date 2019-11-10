
#pragma once


namespace igpu
{
	class parameter;

	class parameters
	{
	public:
		static constexpr size_t MAX_COUNT = 64;

		struct config
		{
			// placeholder
		};

		virtual const config& cfg() const = 0;

		virtual size_t count() const = 0;

		virtual const parameter& parameter( size_t ) const = 0;
	};
}