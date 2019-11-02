
// #include <vulkan/shader/vulkan_compiler.h>

// // Vulkan implementation includes - begin
// #include <vulkan/defines/vulkan_includes.h>
// // Vulkan implementation includes - end


// #include <vector>
// #include <sstream>

// using namespace igpu;

// namespace
// {
// 	const int k_info_log_max_length = 2048;

//     std::vector<std::string> split_shader_source_lines(const std::string& shader_source)
//     {
//         std::stringstream ss(shader_source);
//         std::string line_str;
//         std::vector<std::string> shader_lines;
        
//         while(std::getline(ss,line_str))
//         {
//             shader_lines.emplace_back(line_str);
//         }
        
//         return shader_lines;
//     }
    
    
//     std::string make_line_numbered_source(const std::vector<std::string>& shader_lines)
//     {
//         std::string numbered_line_source;
        
//         int line_number = 0;
//         for(const std::string& shader_line : shader_lines)
//         {
//             numbered_line_source += "(" + std::to_string(++line_number) + "):  \t" + shader_line + "\n";
//         }
        
//         return numbered_line_source;
//     }
    
//     std::string make_error_desc(const std::string& info_log, const std::vector<std::string>& shader_lines)
//     {
//         std::string error_desc;
        
//         for(const std::string& info_log_line : split_shader_source_lines(info_log))
//         {
//             // shader error line numbers start at 1
//             int line_number = 0;
//             sscanf(info_log_line.data(), "ERROR: 0:%d:", &line_number);
//             auto line_number_string = std::to_string(line_number);
            
//             if(error_desc.size())
//             {
//                 error_desc += "\n";
//             }
            
//             if(0 == line_number)
//             {
//                 error_desc += "(Could not find error line number)";
//             }
//             else if(line_number > shader_lines.size())
//             {
//                 error_desc += "GLSL line #" + line_number_string + ": out of range " + std::to_string((int)shader_lines.size() + 1);
//             }
//             else
//             {
//                 error_desc += "GLSL line #" + line_number_string + ": \"" + shader_lines[line_number - 1] + "\"";
//             }
            
            
//             error_desc += "\n" + info_log_line + "\n";
//         }
        
//         return error_desc;
//     }
    
//     void output_errors(GLuint program, const std::string& shader_code)
//     {
//         char info_log[k_info_log_max_length] = {0};
//         GLsizei info_log_length = 0;
//         glGetShaderInfoLog(program, k_info_log_max_length, &info_log_length, info_log);
        
//         std::vector<std::string> source_lines = split_shader_source_lines(shader_code);
        
//         std::string err_desc = make_error_desc(std::string(info_log, info_log_length), source_lines);
//         std::string line_numbered_shader_code = make_line_numbered_source(source_lines);
        
//         LOG_CRITICAL( CRITICAL, "%s\n%s\n\n%s", err_desc.c_str(), line_numbered_shader_code.c_str(), err_desc.c_str() );
//     }
    
//     GLuint compile_shader_code(GLenum type, const std::string_view& shader_code)
//     {
//         // make c-array of char* with shader chunks
//         // make shader with c-array of char*
//         GLuint program = glCreateShader(type);
//         const char* code[] = { shader_code.data() };
        
//         glShaderSource(program, (GLsizei)1, code, NULL);
//         glCompileShader(program);
        
//         // peek compile status
//         GLint result = 0;
//         glGetShaderiv(program, GL_COMPILE_STATUS, &result);
        
//         // on fail cleanup and output compile errors
//         if (GL_FALSE == result)
//         {
//             output_errors(program, (std::string)shader_code);
//             glDeleteShader(program);
//             program = 0;
//         }
        
//         return program;
//     }

// 	void bind_vertex_attribute_indices(const vertex_constraints& vertex_constraints, GLuint program)
// 	{
// 		const auto& vertex_parameters = vertex_constraints.cfg().vertex_parameters;
// 		for (size_t vp = 0; vp < vertex_parameters.size(); ++vp)
// 		{
// 			const auto& name = vertex_parameters[vp].name;
// 			glBindAttribLocation(program, (GLuint)vertex_constraints.find_location(name), name.c_str());
// 		}
// 	}

//     bool gl_link_program(GLuint program, GLuint vertex_program, const std::string_view& vertex_code, GLuint pixel_program, const std::string_view& pixel_code)
//     {
//         bool success = true;
        
//         glAttachShader(program, vertex_program);
//         glAttachShader(program, pixel_program);
//         glLinkProgram(program);
        
//         GLint result = 0;
//         glGetProgramiv(program, GL_LINK_STATUS, &result);
//         if (GL_FALSE == result)
//         {
//             char info_log[k_info_log_max_length];
//             glGetProgramInfoLog(program, k_info_log_max_length, 0, info_log);
            
//             std::string line_numbered_vertex_code = make_line_numbered_source(split_shader_source_lines((std::string)vertex_code));
//             std::string line_numbered_pixel_code = make_line_numbered_source(split_shader_source_lines((std::string)pixel_code));
            
//             LOG_CRITICAL( CRITICAL,
//                       "\n Linker error \n%s\n%s\n%s\n%s",
//                       info_log,
//                       line_numbered_vertex_code.c_str(), line_numbered_pixel_code.c_str(), info_log);
            
//             success = false;
//         }
        
//         return success;
//     }
// }

// unsigned gl_compile(
// 	const vertex_constraints& vertex_constraints,
// 	const std::string_view& vertex_code,
//     const std::string_view& pixel_code)
// {
//     GLuint vertex_program = compile_shader_code(GL_VERTEX_SHADER, vertex_code);
//     GLuint pixel_program = compile_shader_code(GL_FRAGMENT_SHADER, pixel_code);
//     GLuint program = glCreateProgram();
//     bool success = false;
    
// 	bind_vertex_attribute_indices(vertex_constraints, program);
	
//     if(vertex_program && pixel_program && program && gl_link_program(program, vertex_program, vertex_code, pixel_program, pixel_code))
//     {
//         success = true;
//     }
    
//     glDeleteShader(vertex_program);
//     glDeleteShader(pixel_program);
//     vertex_program = pixel_program = 0;
    
//     if(!success)
//     {
//         glDeleteProgram(program);
//         program = 0;
//     }
    
//     return program;
// }
