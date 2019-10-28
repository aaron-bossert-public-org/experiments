
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
			std::shared_ptr<render_states> render_states;
			std::vector<primitive_kv> primitives;
		};

		virtual ~material() = default;

		const config& cfg() const;

		const std::vector<primitive>& primitives() const;

	protected:

		material(config, std::vector<primitive>);
		material(const material&) = delete;
		material& operator= (const material&) = delete;

	private:

		const config _cfg;
		const std::vector<primitive> _primitives;
	};
}
