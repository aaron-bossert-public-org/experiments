
#pragma once

#include "igpu/shader/render_states.h"


namespace igpu
{
	// raw byte comparison is done to prevent redundant state changes

#pragma pack( push, 1 )
	struct gl_cull_mode
	{
		static gl_cull_mode gl_cull_mode::make(
			const cull_mode& mode,
			bool* success = nullptr );

		uint8_t enable;
		uint16_t mode;
	};

	struct gl_blend_mode
	{
		static gl_blend_mode gl_blend_mode::make(
			const blend_mode&,
			bool* success = nullptr );

		uint8_t enable;
		uint16_t src;
		uint16_t dst;
	};

	struct gl_depth_mode
	{
		static gl_depth_mode gl_depth_mode::make(
			const depth_mode&,
			bool* success = nullptr );

		uint8_t enable;
		uint8_t write;
		uint16_t compare;
	};

	struct gl_stencil_mode
	{
		static gl_stencil_mode gl_stencil_mode::make(
			const stencil_mode&,
			bool* success = nullptr );

		uint8_t enable;
		uint16_t compare;
		uint16_t pass;
		uint16_t fail;
		uint16_t depth_fail;
		uint8_t read_mask;
		uint8_t write_mask;
		uint8_t ref;
	};

	struct gl_packed_states
	{
		static gl_packed_states gl_packed_states::make(
			const render_states::config&,
			bool* success = nullptr );

		uint8_t _color_write_mask = 0;
		gl_cull_mode _gl_cull_mode = {};
		gl_blend_mode _gl_blend_mode = {};
		gl_depth_mode _gl_depth_mode = {};
		gl_stencil_mode _gl_stencil_mode = {};
	};
#pragma pack( pop )

	class gl_render_states : public render_states
	{
	public:
		const config& cfg() const override;

		int compare( const render_states* other ) override;

		const gl_packed_states& packed() const;

		static std::unique_ptr< gl_render_states > make( const config& );

	private:
		gl_render_states( const config&, const gl_packed_states& );

	private:
		const config _cfg;

		const gl_packed_states _packed;
	};

	int memcmp( const gl_cull_mode& lhs, const gl_cull_mode& rhs );
	int memcmp( const gl_blend_mode& lhs, const gl_blend_mode& rhs );
	int memcmp( const gl_depth_mode& lhs, const gl_depth_mode& rhs );
	int memcmp( const gl_stencil_mode& lhs, const gl_stencil_mode& rhs );
	int memcmp( const gl_packed_states& lhs, const gl_packed_states& rhs );
}
