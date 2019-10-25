
#pragma once

#include <gl/defines/gl_includes.h>

#include <framework/utility/buffer_view.h>

namespace igpu
{   
    class gl_shader
    {
    public:

		gl_shader(GLenum type);
		
		~gl_shader();

		GLuint gl_handle() const;

		void map(size_t byte_size, buffer_view_base*);

		void unmap();

	private:

		GLenum _type;
		GLenum _gl_handle;

		struct
		{
			std::unique_ptr<char[]> raw;
			buffer_view_base view;
		} _mapped;
    };

	template<typename T>
	class gl_shader_t : public T
	{
	public:

		GLuint gl_handle() const override
		{
			return _shader.gl_handle();
		}

		void map(
			size_t byte_size,
			buffer_view_base* out_buffer_view)
		{
			_shader.map(byte_size, out_buffer_view);
		}

		void unmap()
		{
			_shader.unmap();
		}

		static std::unique_ptr<gl_shader_t> make(GLenum type)
		{
			switch (type)
			{
			case GL_COMPUTE_SHADER:
			case GL_VERTEX_SHADER:
			case GL_TESS_CONTROL_SHADER:
			case GL_TESS_EVALUATION_SHADER:
			case GL_GEOMETRY_SHADER:
			case GL_FRAGMENT_SHADER:
				return std::unique_ptr<gl_shader_t>(
					new gl_shader(type));
			}

			LOG_CRITICAL("unhandled gl shader type: %d ", type);

			return nullptr;
		}

	private:

		gl_shader_t(GLenum type)
			: _shader(type)
		{ }

	private:

		gl_shader _shader;
	};
}
