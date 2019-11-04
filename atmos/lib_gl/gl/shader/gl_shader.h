
#pragma once

#include "gl/defines/gl_includes.h"

#include "igpu/buffer/buffer_raw.h"

namespace igpu
{
	class gl_shader
	{
	public:
		struct config
		{
			buffer_usage usage;
			GLenum type;
		};

		gl_shader( const config& );

		~gl_shader();

		const config& cfg() const;

		GLuint gl_handle() const;

		void map( size_t byte_size, buffer_view_base* );

		void unmap();

		size_t byte_capacity() const;

	private:
		const config _cfg;
		buffer_raw _buffer;
		GLenum _gl_shader;
	};

	template < typename T >
	class gl_shader_t : public T
	{
	public:
		struct config : T::config
		{
			struct gl
			{
				GLenum type;
			};

			gl gl;
		};

		const config& cfg() const
		{
			return _cfg;
		}

		GLuint gl_handle() const override
		{
			return _shader.gl_handle();
		}

		void map( size_t byte_size, buffer_view_base* out_buffer_view ) override
		{
			_shader.map( byte_size, out_buffer_view );
		}

		void unmap() override
		{
			_shader.unmap();
		}

		size_t byte_capacity() const override
		{
			return _shader.byte_capacity();
		}

		static std::unique_ptr< gl_shader_t > make(
			const typename T::config& cfg,
			GLenum type )
		{
			config cfg_t = {
				cfg,
				type,
			};

			switch ( type )
			{
#ifdef GL_COMPUTE_SHADER
			case GL_COMPUTE_SHADER:
#endif
#ifdef GL_TESS_CONTROL_SHADER
			case GL_TESS_CONTROL_SHADER:
#endif
#ifdef GL_TESS_EVALUATION_SHADER
			case GL_TESS_EVALUATION_SHADER:
#endif
#ifdef GL_TESS_EVALUATION_SHADER
			case GL_GEOMETRY_SHADER:
#endif
			case GL_VERTEX_SHADER:
			case GL_FRAGMENT_SHADER:
				return std::unique_ptr< gl_shader_t >(
					new gl_shader_t( cfg_t ) );
			}

			LOG_CRITICAL( "unhandled gl shader type: %d ", type );

			return nullptr;
		}

	private:
		gl_shader_t( const config& cfg )
			: _cfg( cfg )
			, _shader( {
				  cfg.usage,
				  cfg.type,
			  } )
		{}

	private:
		const config _cfg;
		gl_shader _shader;
	};
}
