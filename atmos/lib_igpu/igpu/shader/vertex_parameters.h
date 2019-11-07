
#pragma once


namespace igpu
{
	class vertex_parameter;

	class vertex_parameters
	{
	public:
		virtual size_t count() const = 0;

		virtual const vertex_parameter& parameter( size_t ) const = 0;
	};
}