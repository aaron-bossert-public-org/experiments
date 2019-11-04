//
//#include "gl/buffer/gl_geometry.h"
//
//#include "framework/logging/log.h"
//
//#include "gl/defines/gl_includes.h"
//
//using namespace igpu;
//
//const gl_geometry::config& gl_geometry::cfg() const
//{
//	return _cfg;
//}
//
//index_buffer& gl_geometry::index_buffer()
//{
//	return *_index_buffer;
//}
//
//vertex_buffer& gl_geometry::vertex_buffer(size_t i)
//{
//	return *_vertex_buffers[i];
//}
//
//size_t gl_geometry::element_start() const
//{
//	return _element_start.value_or(0);
//}
//
//void gl_geometry::element_start(size_t element_start)
//{
//	_element_start = element_start;
//}
//
//size_t gl_geometry::element_count() const
//{
//	return _element_count.has_value() ? _element_count.value() : _cfg.index_buffer->count();
//}
//
//void gl_geometry::element_count(size_t element_count)
//{
//	_element_count = element_count;
//}
//
//GLenum gl_geometry::gl_topology() const
//{
//	return _gl_topology;
//}
//
//GLuint gl_geometry::vao() const
//{
//    return _vao;
//}
//
//std::unique_ptr<gl_geometry> gl_geometry::make(
//	const config& cfg)
//{
//	if (nullptr == cfg.index_buffer)
//	{
//		LOG_CRITICAL(
//			"index buffer is null");
//		return nullptr;
//	}
//
//	auto* index_buffer = ASSERT_CAST(gl_index_buffer*, cfg.index_buffer.get());
//	std::vector<gl_vertex_buffer*> vertex_buffers;
//
//	for (size_t i = 0; i < cfg.vertex_buffers.size(); ++i)
//	{
//		const auto& vertex_buffer = cfg.vertex_buffers[i];
//		if (nullptr == vertex_buffer)
//		{
//			LOG_CRITICAL("vertex buffer is null vertex_buffers[%d]",
//				(int)i);
//
//			return nullptr;
//		}
//		
//		vertex_buffers.push_back(ASSERT_CAST(gl_vertex_buffer*, vertex_buffer.get()));
//	}
//
//	GLenum gl_topology = GL_FALSE;
//	switch (cfg.topology)
//	{
//	case topology::TRIANGLE_LIST:
//		gl_topology = GL_TRIANGLES;
//		break;
//	case topology::TRIANGLE_STRIP:
//		gl_topology = GL_TRIANGLE_STRIP;
//		break;
//	case topology::TRIANGLE_FAN:
//		gl_topology = GL_TRIANGLE_FAN;
//		break;
//	case topology::LINE_LIST:
//		gl_topology = GL_LINES;
//		break;
//	case topology::LINE_STRIP:
//		gl_topology = GL_LINE_STRIP;
//		break;
//	case topology::POINT_LIST:
//		gl_topology = GL_POINTS;
//		break;
//	default:
//		LOG_CRITICAL("unhandled topology: %s", to_string(cfg.topology).data());
//		return nullptr;
//	}
//	
//	GLuint vao = 0;
//	glGenVertexArrays(1, &vao);
// vao needs to get moved to geometry_batch, batch system nodes need to be allowed to have platform dependent implementations
//	if (vao)
//	{
//		glBindVertexArray(vao);
//
//		for (const auto& vertex_buffer : vertex_buffers)
//		{
//			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->gl_handle());
//
//			for (const gl_vertex_attribute& attribute : vertex_buffer->gl_format().attributes())
//			{
//				glEnableVertexAttribArray(attribute.location);
//				glVertexAttribPointer(
//					attribute.location,
//					attribute.size,
//					attribute.type, 
//					GL_FALSE/*normalized*/, 
//					vertex_buffer->cfg().format.stride,
//					attribute.offset + (char*)nullptr);
//			}
//		}
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->gl_handle());
//		
//		glBindVertexArray(0);
//	}
//
//    return std::unique_ptr<gl_geometry>(
//		new gl_geometry(
//			cfg,
//			index_buffer,
//			std::move(vertex_buffers),
//			gl_topology,
//			vao));
//}
//
//gl_geometry::~gl_geometry()
//{
//    glDeleteVertexArrays(1, &_vao);
//}
//
//gl_geometry::gl_geometry(
//	const config& cfg,
//	gl_index_buffer* index_buffer,
//	std::vector<gl_vertex_buffer*> vertex_buffers,
//	unsigned gl_topology,
//	unsigned vao)
//: _cfg(cfg)
//, _index_buffer(index_buffer)
//, _vertex_buffers(std::move(vertex_buffers))
//, _gl_topology(gl_topology)
//, _vao(vao)
//{
//}
