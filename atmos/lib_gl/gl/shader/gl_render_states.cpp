//
//#include <gl/material/gl_render_states.h>
//
//#include <framework/logging/log.h>
//
//// GL implementation includes - begin
//#include <gl/defines/gl_includes.h>
//// GL implementation includes - end
//
//using namespace igpu;
//
//namespace
//{   
//    uint16_t to_gl_format(cull cull, bool* success)
//	{
//		switch (cull)
//		{
//		case cull::FRONT:
//			return GL_FRONT;
//		case cull::BACK:
//			return GL_BACK;
//		case cull::BOTH:
//			return GL_FRONT_AND_BACK;
//		};
//
//		if (*success)
//		{
//			*success = false;
//		}
//
//		LOG_CRITICAL("unhandled cull value: %s", to_string(cull).data());
//
//		return GL_BACK;
//    };
//
//	uint16_t to_gl_format(blend blend, bool* success)
//	{
//		switch (blend)
//		{
//		case blend::ZERO:
//			return GL_ZERO;
//		case blend::ONE:
//			return GL_ONE;
//		case blend::SRC_COLOR:
//			return GL_SRC_COLOR;
//		case blend::INV_SRC_COLOR:
//			return GL_ONE_MINUS_SRC_COLOR;
//		case blend::DST_COLOR:
//			return GL_DST_COLOR;
//		case blend::INV_DST_COLOR:
//			return GL_ONE_MINUS_DST_COLOR;
//		case blend::SRC_ALPHA:
//			return GL_SRC_ALPHA;
//		case blend::INV_SRC_ALPHA:
//			return GL_ONE_MINUS_SRC_ALPHA;
//		case blend::DST_ALPHA:
//			return GL_DST_ALPHA;
//		case blend::INV_DST_ALPHA:
//			return GL_ONE_MINUS_DST_ALPHA;
//		};
//
//		if (*success)
//		{
//			*success = false;
//		}
//
//		LOG_CRITICAL("unhandled blend value: %s", to_string(blend).data());
//
//		return GL_ONE;
//	};
//
//	uint16_t to_gl_format(compare compare, bool* success)
//	{
//		switch (compare)
//		{
//		case compare::NEVER:
//			return GL_NEVER;
//		case compare::LESS:
//			return GL_LESS;
//		case compare::EQUAL:
//			return GL_EQUAL;
//		case compare::LESS_EQUAL:
//			return GL_LEQUAL;
//		case compare::GREATER:
//			return GL_GREATER;
//		case compare::NOT_EQUAL:
//			return GL_NOTEQUAL;
//		case compare::GREATER_EQUAL:
//			return GL_GEQUAL;
//		case compare::ALWAYS:
//			return GL_ALWAYS;
//		};
//
//		if (*success)
//		{
//			*success = false;
//		}
//
//		LOG_CRITICAL("unhandled compare value: %s", to_string(compare).data());
//		
//		return GL_LESS;
//	};
//
//	uint16_t to_gl_format(stencil stencil, bool* success)
//	{
//		switch (stencil)
//		{
//		case stencil::KEEP:
//			return GL_KEEP;
//		case stencil::ZERO:
//			return GL_ZERO;
//		case stencil::REPLACE:
//			return GL_REPLACE;
//		case stencil::INCREMENT:
//			return GL_INCR;
//		case stencil::WRAP_INCREMENT:
//			return GL_INCR_WRAP;
//		case stencil::DECREMENT:
//			return GL_DECR;
//		case stencil::WRAP_DECREMENT:
//			return GL_DECR_WRAP;
//		case stencil::INVERT:
//			return GL_INVERT;
//		};
//
//		if (*success)
//		{
//			*success = false;
//		}
//
//		LOG_CRITICAL("unhandled stencil value: %s", to_string(stencil).data());
//
//		return GL_KEEP;
//	};
//}
//
////------------------------------------------------------------------------------
////
////
//gl_cull_mode gl_cull_mode::make(const cull_mode& mode, bool* success)
//{
//	return {
//		mode.enable,
//		to_gl_format(mode.mode, success),
//	};
//}
//
//gl_blend_mode gl_blend_mode::make(const blend_mode& mode, bool* success)
//{
//	return {
//		mode.enable,
//		to_gl_format(mode.src, success),
//		to_gl_format(mode.dst, success),
//	};
//}
//
//gl_depth_mode gl_depth_mode::make(const depth_mode& mode, bool* success)
//{
//	return {
//		mode.enable,
//		mode.write,
//		to_gl_format(mode.compare, success),
//	};
//
//}
//
//gl_stencil_mode gl_stencil_mode::make(const stencil_mode& mode, bool* success)
//{
//	return {
//		mode.enable,
//		to_gl_format(mode.compare, success),
//		to_gl_format(mode.pass, success),
//		to_gl_format(mode.fail, success),
//		to_gl_format(mode.depth_fail, success),
//		mode.read_mask,
//		mode.write_mask,
//		mode.ref,
//	};
//}
//
//gl_packed_states gl_packed_states::make(const render_states::config& cfg, bool* success)
//{
//	return {
//		cfg.color_write_mask,
//		gl_cull_mode::make(cfg.cull, success),
//		gl_blend_mode::make(cfg.blend, success),
//		gl_depth_mode::make(cfg.depth, success),
//		gl_stencil_mode::make(cfg.stencil, success),
//	};
//}
// 
//const gl_render_states::config& gl_render_states::cfg() const
//{
//	return _cfg;
//}
//
//int gl_render_states::compare(const render_states* other)
//{
//	auto* gl_other = ASSERT_CAST(const gl_render_states*, other);
//	return memcmp(_packed, gl_other->_packed);
//}
//
//const gl_packed_states& gl_render_states::packed() const
//{
//	return _packed;
//}
//
//std::unique_ptr < gl_render_states > gl_render_states::make(const config& cfg)
//{
//	bool success = true;
//	auto packed = gl_packed_states::make(cfg, &success);
//	if (!success)
//	{
//		LOG_CRITICAL("failed to make gl_render_states");
//	}
//	else
//	{
//		return std::unique_ptr< gl_render_states >(
//			new gl_render_states(cfg, packed));
//	}
//
//	return nullptr;
//}
//
//gl_render_states::gl_render_states(
//	const config& cfg,
//	const gl_packed_states& packed)
//	: _cfg(cfg)
//	, _packed(packed)
//{
//}
