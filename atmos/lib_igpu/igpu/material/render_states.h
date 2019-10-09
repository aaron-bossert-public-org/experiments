
#pragma once

#include <framework/meta_programming/va_macro.h>

namespace igpu
{
	ENUM_SERIALIZABLE(

		cull, DEFAULT(BACK),
		(BACK, 0),
		(FRONT, 1),
		(BOTH, 2)
	);

	ENUM_SERIALIZABLE(

		blend, DEFAULT(ZERO),
		(ZERO, 0),
		(ONE, 1),
		(SRC_COLOR, 2),
		(INV_SRC_COLOR, 3),
		(DST_COLOR, 4),
		(INV_DST_COLOR, 5),
		(SRC_ALPHA, 6),
		(INV_SRC_ALPHA, 7),
		(DST_ALPHA, 8),
		(INV_DST_ALPHA, 9)
	);

	ENUM_SERIALIZABLE(

		compare, DEFAULT(NEVER),

		(NEVER, 0),
		(LESS, 1),
		(EQUAL, 2),
		(LESS_EQUAL, 3),
		(GREATER, 4),
		(NOT_EQUAL, 5),
		(GREATER_EQUAL, 6),
		(ALWAYS, 7)
	);

	ENUM_SERIALIZABLE(

		stencil, DEFAULT(KEEP),

		(KEEP, 0),
		(ZERO, 1),
		(REPLACE, 2),
		(INCREMENT, 3),
		(WRAP_INCREMENT, 4),
		(DECREMENT, 5),
		(WRAP_DECREMENT, 6),
		(INVERT, 7)
	);

	struct cull_mode
	{
		bool enable = true;
		cull mode = cull::DEFAULT;
	};

	struct blend_mode
	{
		bool enable = false;
		blend src = blend::SRC_ALPHA;
		blend dst = blend::INV_SRC_ALPHA;
	};

	struct depth_mode
	{
		bool enable = true;
		bool write = true;
		compare compare = compare::LESS_EQUAL;
	};

	struct stencil_mode
	{
		bool enable = false;
		uint8_t write_mask = 255;
		uint8_t read_mask = 255;
		uint8_t ref = 0;
		compare compare = compare::NEVER;
		stencil pass = stencil::KEEP;
		stencil fail = stencil::KEEP;
		stencil depth_fail = stencil::KEEP;
	};

	class render_states
	{
	public:
		
		virtual int compare(const render_states* other) = 0;
		
		struct config
		{
			uint8_t color_write_mask;
			cull_mode cull;
			blend_mode blend;
			stencil_mode stencil;
			depth_mode depth;
		};

		const config& cfg() const;

	protected:
		
		render_states(const config&);
		render_states(const render_states&) = default;
		render_states& operator= (const render_states&) = default;

	private:

		config _cfg;
	};
}

