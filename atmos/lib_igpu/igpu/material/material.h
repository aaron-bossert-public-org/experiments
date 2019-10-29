
#pragma once

#include <igpu/material/primitive.h>
#include <igpu/material/render_states.h>

namespace igpu
{
	class program;

	class material
	{
	public:

		struct config
		{
			std::string name;
			std::vector<primitive_kv> primitives;
		};

		virtual ~material() = default;

		const config& cfg() const;

	protected:

		material(config, std::vector<primitive>);
		material(const material&) = delete;
		material& operator= (const material&) = delete;

	private:

		const config _cfg;
	};
}
