//
//#include "gl/window/gl_back_buffer.h"
//
//#include "framework/logging/log.h"
//
// using namespace igpu;
//
// const gl_back_buffer::config& gl_back_buffer::cfg() const
//{
//	return _cfg;
//}
//
// int gl_back_buffer::gl_handle() const
//{
//    return _gl_handle;
//}
//
// void gl_back_buffer::gl_handle(int gl_handle)
//{
//    _gl_handle = gl_handle;
//}
//
// std::shared_ptr<gl_back_buffer> gl_back_buffer::instance()
//{
//	ASSERT_CONTEXT(false, "need to pipe backbuffer config into surface / swap
// chain creation for gl... probably involves a handshake with window inside the
// gl_context");
//    // this lives as long as the process
//	static auto s_instance = std::shared_ptr<gl_back_buffer>(
//		new gl_back_buffer(
//			{},
//			gl_render_buffer::make_empty(),
//			gl_depth_buffer::make_empty()));
//
//	return s_instance;
//}
//
// const gl_render_buffer& gl_back_buffer::color() const
//{
//	return *_color;
//}
//
// const gl_depth_buffer& gl_back_buffer::depth() const
//{
//	return *_depth;
//}
//
// gl_back_buffer::gl_back_buffer(
//	const config& cfg,
//	const std::shared_ptr <gl_render_buffer>& color,
//	const std::shared_ptr <gl_depth_buffer>& depth)
//: _cfg(cfg)
//, _gl_handle(0)
//, _color(color)
//, _depth(depth)
//{
//}
