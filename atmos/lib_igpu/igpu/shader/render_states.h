
#pragma once

#include "framework/meta_programming/va_macro.h"

namespace igpu
{
	ENUM_SERIALIZABLE_TYPE(

		cull,
		uint8_t,
		DEFAULT( BACK ),

		( BACK, 0 ),
		( FRONT, 1 ),
		( BOTH, 2 ) );

	ENUM_SERIALIZABLE_TYPE(

		blend,
		uint8_t,
		DEFAULT( ZERO ),

		( ZERO, 0 ),
		( ONE, 1 ),
		( SRC_COLOR, 2 ),
		( INV_SRC_COLOR, 3 ),
		( DST_COLOR, 4 ),
		( INV_DST_COLOR, 5 ),
		( SRC_ALPHA, 6 ),
		( INV_SRC_ALPHA, 7 ),
		( DST_ALPHA, 8 ),
		( INV_DST_ALPHA, 9 ) );

	ENUM_SERIALIZABLE_TYPE(

		compare,
		uint8_t,
		DEFAULT( NEVER ),

		( NEVER, 0 ),
		( LESS, 1 ),
		( EQUAL, 2 ),
		( LESS_EQUAL, 3 ),
		( GREATER, 4 ),
		( NOT_EQUAL, 5 ),
		( GREATER_EQUAL, 6 ),
		( ALWAYS, 7 ) );

	ENUM_SERIALIZABLE_TYPE(

		stencil,
		uint8_t,
		DEFAULT( KEEP ),

		( KEEP, 0 ),
		( ZERO, 1 ),
		( REPLACE, 2 ),
		( INCREMENT, 3 ),
		( WRAP_INCREMENT, 4 ),
		( DECREMENT, 5 ),
		( WRAP_DECREMENT, 6 ),
		( INVERT, 7 ) );


	// raw byte comparison is done to prevent redundant state changes
#pragma pack( push, 1 )

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

#pragma pack( pop )

	class render_states
	{
	public:
#pragma pack( push, 1 )
		struct config
		{
			uint8_t color_write_mask;
			cull_mode cull;
			blend_mode blend;
			stencil_mode stencil;
			depth_mode depth;
		};
#pragma pack( pop )

		virtual const config& cfg() const = 0;

	protected:
		render_states() = default;
		render_states( const render_states& ) = delete;
		render_states& operator=( const render_states& ) = delete;
	};

	int mem_compare(
		const render_states::config& lhs,
		const render_states::config& rhs );
	int mem_compare( const cull_mode& lhs, const cull_mode& rhs );
	int mem_compare( const blend_mode& lhs, const blend_mode& rhs );
	int mem_compare( const stencil_mode& lhs, const stencil_mode& rhs );
	int mem_compare( const depth_mode& lhs, const depth_mode& rhs );
}
