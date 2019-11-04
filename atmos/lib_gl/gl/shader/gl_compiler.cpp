//
//#include "gl/material/gl_compiler.h"
//
//#include "gl/context/gl_context.h"
//#include "gl/defines/gl_includes.h"
//#include "gl/material/gl_fragment_shader.h"
//#include "gl/material/gl_vertex_shader.h"
//
//#include "framework/utility/buffer_view.h"
//
//#include "igpu/context/vertex_constraints.h"
//#include "igpu/shader/program.h"
//
//#include <vector>
//#include <sstream>
//
//using namespace igpu;
//
//namespace
//{
//	const int k_info_log_max_length = 2048;
//
//    std::vector<std::string> split_shader_source_lines(const buffer_view_base& source_code)
//    {
//		char* data = (char*)source_code.data();
//		std::string str = std::string(data, data + source_code.byte_size());
//        std::stringstream ss(str);
//        std::string line_str;
//        std::vector<std::string> shader_lines;
//        
//        while(std::getline(ss,line_str))
//        {
//            shader_lines.emplace_back(line_str);
//        }
//        
//        return shader_lines;
//    }
//    
//    
//    std::string make_line_numbered_source(const std::vector<std::string>& shader_lines)
//    {
//        std::string numbered_line_source;
//        
//        int line_number = 0;
//        for(const std::string& shader_line : shader_lines)
//        {
//            numbered_line_source += "(" + std::to_string(++line_number) + "):  \t" + shader_line + "\n";
//        }
//        
//        return numbered_line_source;
//    }
//    
//    std::string make_error_desc(const std::string& info_log, const std::vector<std::string>& shader_lines)
//    {
//        std::string error_desc;
//        
//        for(const std::string& info_log_line : split_shader_source_lines(info_log))
//        {
//            // shader error line numbers start at 1
//            int line_number = 0;
//            sscanf(info_log_line.data(), "ERROR: 0:%d:", &line_number);
//            auto line_number_string = std::to_string(line_number);
//            
//            if(error_desc.size())
//            {
//                error_desc += "\n";
//            }
//            
//            if(0 == line_number)
//            {
//                error_desc += "(Could not find error line number)";
//            }
//            else if(line_number > shader_lines.size())
//            {
//                error_desc += "GLSL line #" + line_number_string + ": out of range " + std::to_string((int)shader_lines.size() + 1);
//            }
//            else
//            {
//                error_desc += "GLSL line #" + line_number_string + ": \"" + shader_lines[line_number - 1] + "\"";
//            }
//            
//            
//            error_desc += "\n" + info_log_line + "\n";
//        }
//        
//        return error_desc;
//    }
//    
//    void output_errors(GLuint shader, const buffer_view_base& source_code)
//    {
//        char info_log[k_info_log_max_length] = {0};
//        GLsizei info_log_length = 0;
//        glGetShaderInfoLog(shader, k_info_log_max_length, &info_log_length, info_log);
//        
//        std::vector<std::string> source_lines = split_shader_source_lines(source_code);
//        
//        std::string err_desc = make_error_desc(std::string(info_log, info_log_length), source_lines);
//        std::string line_numbered_shader_code = make_line_numbered_source(source_lines);
//        
//		LOG_CRITICAL( "%s\n%s\n\n%s", err_desc.c_str(), line_numbered_shader_code.c_str(), err_desc.c_str() );
//    }
//
//	void bind_vertex_attribute_indices(const vertex_constraints& vertex_constraints, GLuint program)
//	{
//		const auto& vertex_parameters = vertex_constraints.cfg().vertex_parameters;
//		for (size_t vp = 0; vp < vertex_parameters.size(); ++vp)
//		{
//			const auto& name = vertex_parameters[vp].name;
//			glBindAttribLocation(program, (GLuint)vertex_constraints.find_location(name), name.c_str());
//		}
//	}
//
//    bool gl_link_program(GLuint program, GLuint vertex_program, GLuint pixel_program)
//    {
//        bool success = true;
//        
//        glAttachShader(program, vertex_program);
//        glAttachShader(program, pixel_program);
//        glLinkProgram(program);
//        
//        GLint result = 0;
//        glGetProgramiv(program, GL_LINK_STATUS, &result);
//        if (GL_FALSE == result)
//        {
//            char info_log[k_info_log_max_length];
//            glGetProgramInfoLog(program, k_info_log_max_length, 0, info_log);
//            
//            LOG_CRITICAL("\n Linker error \n%s",
//                      info_log);
//            
//            success = false;
//        }
//        
//        return success;
//    }
//}
//
//bool gl_compile_shader_code(GLuint shader, const buffer_view_base& source_code)
//{
//	const char* code[] = { (char*)source_code.data() };
//
//	glShaderSource(shader, (GLsizei)1, code, NULL);
//	glCompileShader(shader);
//
//	// peek compile status
//	GLint result = 0;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
//
//	// on fail cleanup and output compile errors
//	if (GL_FALSE == result)
//	{
//		output_errors(shader, source_code);
//	}
//
//	return GL_FALSE != result;
//}
//
//unsigned gl_compile(
//	const gl_program::config& cfg)
//{
//	if (!cfg.vertex)
//	{
//		LOG_CRITICAL("vertex shader is null");
//	}
//	else if (!cfg.fragment)
//	{
//		LOG_CRITICAL("fragment shader is null");
//	}
//	else
//	{
//		auto gl_vertex = ASSERT_CAST(gl_vertex_shader*, cfg.vertex.get());
//		auto gl_Fragment = ASSERT_CAST(gl_vertex_shader*, cfg.fragment.get());
//
//		GLuint vertex_handle = gl_vertex->gl_handle();
//		GLuint fragment_handle = gl_Fragment->gl_handle();
//
//		if (!vertex_handle)
//		{
//			LOG_CRITICAL("vertex handle is 0");
//		}
//		else if (!fragment_handle)
//		{
//			LOG_CRITICAL("fragment handle is 0");
//		}
//		else
//		{
//			GLuint program_handle = glCreateProgram();
//			if (!program_handle)
//			{
//				LOG_CRITICAL("program handle is 0");
//			}
//			else
//			{
//				bool success = false;
//    
//				bind_vertex_attribute_indices(cfg.context->vertex_constraints(), program_handle);
//	
//				if(gl_link_program(program_handle, vertex_handle, fragment_handle))
//				{
//					success = true;
//				}
//    
//				if (!success)
//				{
//					glDeleteProgram(program_handle);
//				}
//				else 
//				{
//					return program_handle;
//				}
//			}
//		}
//	}
//
//	return 0;
//}
