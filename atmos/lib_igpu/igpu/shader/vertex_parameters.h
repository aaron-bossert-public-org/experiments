
#pragma once


namespace igpu
{
	class vertex_parameter;

	class vertex_parameters
	{
	public:
		static constexpr size_t MAX_COUNT = 16;

		virtual size_t count() const = 0;

		virtual const vertex_parameter& parameter( size_t ) const = 0;

		virtual ~vertex_parameters() = default;
	};
}