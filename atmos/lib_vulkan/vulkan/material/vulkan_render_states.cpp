
// #include <vulkan/material/vulkan_render_states.h>

// #include <framework/logging/log.h>

// // Vulkan implementation includes - begin
// #include <vulkan/defines/vulkan_includes.h>
// // Vulkan implementation includes - end

// using namespace igpu;

// namespace
// {   
//     uint16_t to_gl_cull(cull cull)
// 	{
// 		switch (cull)
// 		{
// 		case cull::FRONT:
// 			return GL_FRONT;
// 		case cull::BACK:
// 			return GL_BACK;
// 		case cull::BOTH:
// 			return GL_FRONT_AND_BACK;
// 		};

// 		LOG_CRITICAL("unhandled cull value: %s", to_string(cull).data());

// 		return GL_BACK;
//     };

// 	uint16_t to_gl_blend(blend blend)
// 	{
// 		switch (blend)
// 		{
// 		case blend::ZERO:
// 			return GL_ZERO;
// 		case blend::ONE:
// 			return GL_ONE;
// 		case blend::SRC_COLOR:
// 			return GL_SRC_COLOR;
// 		case blend::INV_SRC_COLOR:
// 			return GL_ONE_MINUS_SRC_COLOR;
// 		case blend::DST_COLOR:
// 			return GL_DST_COLOR;
// 		case blend::INV_DST_COLOR:
// 			return GL_ONE_MINUS_DST_COLOR;
// 		case blend::SRC_ALPHA:
// 			return GL_SRC_ALPHA;
// 		case blend::INV_SRC_ALPHA:
// 			return GL_ONE_MINUS_SRC_ALPHA;
// 		case blend::DST_ALPHA:
// 			return GL_DST_ALPHA;
// 		case blend::INV_DST_ALPHA:
// 			return GL_ONE_MINUS_DST_ALPHA;
// 		};

// 		LOG_CRITICAL("unhandled blend value: %s", to_string(blend).data());

// 		return GL_ONE;
// 	};

// 	uint16_t to_gl_compare(compare compare)
// 	{
// 		switch (compare)
// 		{
// 		case compare::NEVER:
// 			return GL_NEVER;
// 		case compare::LESS:
// 			return GL_LESS;
// 		case compare::EQUAL:
// 			return GL_EQUAL;
// 		case compare::LESS_EQUAL:
// 			return GL_LEQUAL;
// 		case compare::GREATER:
// 			return GL_GREATER;
// 		case compare::NOT_EQUAL:
// 			return GL_NOTEQUAL;
// 		case compare::GREATER_EQUAL:
// 			return GL_GEQUAL;
// 		case compare::ALWAYS:
// 			return GL_ALWAYS;
// 		};

// 		LOG_CRITICAL("unhandled compare value: %s", to_string(compare).data());
		
// 		return GL_LESS;
// 	};

// 	uint16_t to_gl_stencil(stencil stencil)
// 	{
// 		switch (stencil)
// 		{
// 		case stencil::KEEP:
// 			return GL_KEEP;
// 		case stencil::ZERO:
// 			return GL_ZERO;
// 		case stencil::REPLACE:
// 			return GL_REPLACE;
// 		case stencil::INCREMENT:
// 			return GL_INCR;
// 		case stencil::WRAP_INCREMENT:
// 			return GL_INCR_WRAP;
// 		case stencil::DECREMENT:
// 			return GL_DECR;
// 		case stencil::WRAP_DECREMENT:
// 			return GL_DECR_WRAP;
// 		case stencil::INVERT:
// 			return GL_INVERT;
// 		};

// 		LOG_CRITICAL("unhandled stencil value: %s", to_string(stencil).data());

// 		return GL_KEEP;
// 	};
// }

// //------------------------------------------------------------------------------
// //
// //
// int vulkan_render_states::compare(const render_states* other)
// {
// 	ASSERT_CONTEXT(dynamic_cast<const vulkan_render_states*>(other));
//     return memcmp(_packed, ((vulkan_render_states*)other)->_packed);
// }

// gl_cull_mode gl_cull_mode::make(const cull_mode& mode)
// {
// 	return {
// 		mode.enable,
// 		to_gl_cull(mode.mode),
// 	};
// }

// gl_blend_mode gl_blend_mode::make(const blend_mode& mode)
// {
// 	return {
// 		mode.enable,
// 		to_gl_blend(mode.src),
// 		to_gl_blend(mode.dst),
// 	};
// }

// gl_depth_mode gl_depth_mode::make(const depth_mode& mode)
// {
// 	return {
// 		mode.enable,
// 		mode.write,
// 		to_gl_compare(mode.compare),
// 	};

// }

// gl_stencil_mode gl_stencil_mode::make(const stencil_mode& mode)
// {
// 	return {
// 		mode.enable,
// 		to_gl_compare(mode.compare),
// 		to_gl_stencil(mode.pass),
// 		to_gl_stencil(mode.fail),
// 		to_gl_stencil(mode.depth_fail),
// 		mode.read_mask,
// 		mode.write_mask,
// 		mode.ref,
// 	};
// }

// gl_packed_states gl_packed_states::make(const render_states::config& cfg)
// {
// 	return {
// 		cfg.color_write_mask,
// 		gl_cull_mode::make(cfg.cull),
// 		gl_blend_mode::make(cfg.blend),
// 		gl_depth_mode::make(cfg.depth),
// 		gl_stencil_mode::make(cfg.stencil),
// 	};
// }

// vulkan_render_states::vulkan_render_states(const config& cfg)
// : render_states(cfg)
// , _packed(gl_packed_states::make(cfg))
// {
// }
    
// const gl_packed_states& vulkan_render_states::packed() const
// {
//     return _packed;
// }
