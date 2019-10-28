//
//#include <gl/material/gl_program.h>
//
//// GL implementation includes - begin
//#include <gl/buffer/gl_compute_buffer.h>
//#include <gl/buffer/gl_vertex_parameter.h>
//#include <gl/context/gl_context.h>
//#include <gl/defines/gl_includes.h>
//#include <gl/material/gl_compiler.h>
//#include <gl/material/gl_parameter.h>
//#include <gl/texture/gl_render_texture2d.h>
//#include <gl/texture/gl_depth_texture2d.h>
//#include <gl/texture/gl_texture2d.h>
//// GL implementation includes - end
//
//#include <igpu/batch/batch.h>
//#include <igpu/batch/batch_parameters.h>
//#include <igpu/buffer/vertex_format.h>
//#include <igpu/material/material.h>
//
//#include <framework/logging/log.h>
//
//#include <algorithm>
//
//using namespace igpu;
//
//namespace
//{
//	std::vector<gl_parameter> discover_parameters(GLuint program)
//    {
//
//		GLuint count;
//		glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, (GLint*)&count);
//		std::vector<gl_parameter> parameters;
//		parameters.reserve(count);
//
//		for (GLuint index = 0; index < count; ++index)
//		{
//			GLint len;
//			GLint binding_index;
//			//(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
//			glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &len);
//			glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_BINDING, &binding_index);
//			
//			std::vector<GLchar> name(len);
//			glGetActiveUniformBlockName(program, index, len, NULL, &name[0]);
//
//			parameter::config cfg;
//			cfg.name.assign(name.begin(), name.end() - 1);
//			cfg.type = parameter::type::COMPUTE_BUFFER;
//			parameters.push_back(gl_parameter(
//				cfg,
//				(size_t)-1,
//				index,
//				binding_index));
//		}
//
//        GLint uniform_count = 0;
//        const int name_max_size = 100;
//        GLchar name_raw[name_max_size + 1];
//        GLsizei name_len = 0;
//        GLint       size = 0;
//        GLenum    gl_type = 0;
//        GLint   location = 0;
//		GLenum   binding = 0;
//        
//        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
//		        
//        for(GLint i = 0; i < uniform_count; ++i)
//        {
//            glGetActiveUniform( program, i, name_max_size, &name_len, &size, &gl_type, name_raw );
//            location = glGetUniformLocation( program, name_raw );
//            
//            std::string name = name_raw;
//            
//            // gl likes to return array element names instead of array names,
//            // eg: "things[0]" instead of "things"
//            // lets strip off that array stuff here:
//            name = name.substr(0, name.find('['));
//
//			// if parameter is a texture, set its texture stage to a non zero value
//			GLenum binding_index = 0;
//			
//			if (gl_type == GL_SAMPLER_2D)
//			{
//				binding_index = binding + GL_TEXTURE0;
//				ASSERT_CONTEXT(
//					binding_index <= GL_TEXTURE7,
//					"sample count overflow: binding_index(%d) <= GL_TEXTURE7(%d)",
//					binding_index, GL_TEXTURE7);
//				glUniform1i(location, binding);
//
//				++binding;
//			}
//
//			parameter::type type;
//			if (1 == size)
//			{
//#define CASE_NOT_IMPLEMENTED(GL_TYPE) \
//	case GL_TYPE: \
//		LOG_CRITICAL("unhandled gl_type(%d)", #GL_TYPE); continue;
//				switch (gl_type)
//				{
//				case GL_SAMPLER_2D:
//					type = parameter::type::TEXTURE2D;
//					break;
//				CASE_NOT_IMPLEMENTED(GL_INT);
//				CASE_NOT_IMPLEMENTED(GL_INT_VEC2);
//				CASE_NOT_IMPLEMENTED(GL_INT_VEC3);
//				CASE_NOT_IMPLEMENTED(GL_INT_VEC4);
//				CASE_NOT_IMPLEMENTED(GL_BOOL);
//				CASE_NOT_IMPLEMENTED(GL_BOOL_VEC2);
//				CASE_NOT_IMPLEMENTED(GL_BOOL_VEC3);
//				CASE_NOT_IMPLEMENTED(GL_BOOL_VEC4);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT_VEC2);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT_VEC3);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT_VEC4);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT_MAT2);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT_MAT3);
//				CASE_NOT_IMPLEMENTED(GL_FLOAT_MAT4);
//				default: LOG_CRITICAL("unrecognized gl_type(%d)", gl_type); continue;
//				}
//#undef CASE_NOT_IMPLEMENTED
//			}
//			else
//			{
//				LOG_CRITICAL("arrays not implemented, ignorning uniform:%s", name.c_str());
//				continue;
//			}
//			parameters.emplace_back(parameter::config{ name, type }, (size_t)-1, location, binding_index);
//        }
//
//		return parameters;
//    }
//
//	std::vector<gl_vertex_parameter> discover_vertex_parameters(GLuint program)
//	{
//        GLint attribute_count = 0;
//        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribute_count);
//		std::vector<gl_vertex_parameter> vertex_parameters;
//		vertex_parameters.reserve(attribute_count);
//        
//        for(GLint i = 0; i < attribute_count; ++i)
//        {
//            const int name_max_size = 100;
//            GLchar name_raw[name_max_size + 1];
//            GLsizei name_len = 0;
//            GLint size = 0;
//            GLenum type = 0;
//            
//            glGetActiveAttrib( program, i, name_max_size, &name_len, &size, &type, name_raw );
//
//			igpu::components components;
//			switch (type)
//			{
//			case GL_FLOAT:
//				components = igpu::components::FLOAT1;
//				break;
//
//			case GL_FLOAT_VEC2:
//				components = igpu::components::FLOAT2;
//				break;
//
//			case GL_FLOAT_VEC3:
//				components = igpu::components::FLOAT3;
//				break;
//
//			case GL_FLOAT_VEC4:
//				components = igpu::components::FLOAT4;
//				break;
//			default:
//				LOG_CRITICAL("Unhandled vertex attrib gl type used in program: %d", type);
//				components = igpu::components::FLOAT4;
//				break;
//			}
//            int attrib_location = glGetAttribLocation( program, name_raw );
//
//			// gl likes to return array element names instead of array names,
//			// eg: "things[10]" instead of "things"
//			// lets strip off that array stuff here:
//			std::string name = name_raw;
//			name = name.substr(0, name.find('['));
//			vertex_parameters.emplace_back(name, components, attrib_location);
//        }
//
//		return vertex_parameters;
//    }
//}
//
////------------------------------------------------------------------------------
////
////
//const gl_program::config& gl_program::cfg() const
//{
//	return _cfg;
//}
//
//gl_program::~gl_program()
//{
//    glDeleteProgram(_gl_handle);
//}
//
//void gl_program::update(size_t batch_drawpass_id, const batch* batch)
//{
//	// apppears to be up to date with batch parameters, nothing to do here
//	if (_batch_drawpass_id == batch_drawpass_id)
//	{
//		return;
//	}
//
//	const auto& cfg_constraints = _cfg.context->batch_constraints().cfg();
//	const auto& batch_parameters = batch->parameters();
//
//	for (size_t param_index = 0; param_index < _batch_parameters.size(); ++param_index)
//	{
//		const auto& parameter = _batch_parameters[param_index];
//		size_t binding = parameter.binding();
//		if (binding < cfg_constraints.parameters.size())
//		{
//			if (const primitive_block::element* element = batch_parameters.find(binding))
//			{
//				update(parameter, element->primitive);
//			}
//			else
//			{
//				// use fallback value
//				update(parameter, cfg_constraints.parameters[binding].second);
//			}
//		} 
//		else
//		{
//			LOG_CRITICAL(
//				"binding: %d out of bounds: %d",
//				binding,
//				cfg_constraints.parameters.size());
//		}
//	}
//}
//
//void gl_program::update(const material* material)
//{
//	const auto& material_primitives = material->primitives();
//	if (material_primitives.size() != _material_parameters.size())
//	{
//		LOG_CRITICAL(
//			"material primitive count: %d does not match count expected by program: %d",
//			material_primitives.size(),
//			_material_parameters.size());
//	}
//	else
//	{
//		for (size_t i = 0; i < _material_parameters.size(); ++i)
//		{
//			const primitive& primitive = material_primitives[i];
//			update(_material_parameters[i], primitive);
//		}
//	}
//}
//
//void gl_program::update(const batch_binding* binding)
//{
//	const auto& primitive_block = binding->primitive_block();
//
//	for (size_t param_index = 0; param_index < _instance_parameters.size(); ++param_index)
//	{
//		const auto& parameter = _instance_parameters[param_index];
//		if (const primitive_block::element* element = primitive_block.find(param_index))
//		{
//			update(parameter, element->primitive);
//		}
//		else
//		{	
//			update(parameter, default_instance_primitive(param_index));
//		}
//	}
//}
//
//void gl_program::update(const gl_parameter& parameter, const primitive& primitive) const
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	if (primitive.type()  == parameter::type::TEXTURE2D)
//	{
//		_texture_switch_metric.add(1);
//	}
//	else
//	{
//		_parameter_switch_metric.add(1);
//	}
//#endif
//
//	switch (primitive.type())
//	{
//	case parameter::type::COMPUTE_BUFFER:
//	{
//		const auto* compute_buffer = (const gl_compute_buffer*)
//			std::get<std::shared_ptr<igpu::compute_buffer>>(
//				primitive.variant()).get();
//
//		GLuint gl_handle = compute_buffer
//			? compute_buffer->gl_handle()
//			: 0;
//			
//		glBindBufferBase(GL_UNIFORM_BUFFER, parameter.binding_index(), gl_handle);
//	}
//	case parameter::type::TEXTURE2D:
//	{
//		GLuint gl_handle = 0;
//		const auto& variant = primitive.variant();
//		if (std::holds_alternative<std::shared_ptr<texture2d>>(variant))
//		{
//			const auto* texture2d = (const gl_texture2d*)
//				std::get<std::shared_ptr<igpu::texture2d>>(
//					primitive.variant()).get();
//			
//			gl_handle = texture2d
//				? texture2d->gl_handle()
//				: 0;
//		}
//		else if (std::holds_alternative<std::shared_ptr<depth_texture2d>>(variant))
//		{
//			const auto* depth_texture2d = (const gl_depth_texture2d*)
//				std::get<std::shared_ptr<igpu::depth_texture2d>>(
//					primitive.variant()).get();
//
//			gl_handle = depth_texture2d
//				? depth_texture2d->gl_handle()
//				: 0;
//		}
//		else if (std::holds_alternative<std::shared_ptr<render_texture2d>>(variant))
//		{
//			const auto* render_texture2d = (const gl_render_texture2d*)
//				std::get<std::shared_ptr<igpu::render_texture2d>>(
//					primitive.variant()).get();
//
//			gl_handle = render_texture2d
//				? render_texture2d->gl_handle()
//				: 0;
//		}
//		else
//		{
//			LOG_CRITICAL("unhandled variant index: %d", primitive.variant().index());
//		}
//
//		_cfg.context->active_texture(parameter.binding_index(), gl_handle);
//	}
//	default:
//		LOG_CRITICAL("unhandled type %s", parameter::to_string(primitive.type()).data());
//	}
//}
//
//GLuint gl_program::gl_handle() const
//{
//    return _gl_handle;
//}
//
//size_t gl_program::batch_parameter_count() const
//{
//	return _batch_parameters.size();
//}
//
//const parameter& gl_program::batch_parameter(size_t i) const
//{
//	return _batch_parameters[i];
//}
//
//size_t gl_program::material_parameter_count() const
//{
//	return _material_parameters.size();
//}
//
//const parameter& gl_program::material_parameter(size_t i) const
//{
//	return _material_parameters[i];
//}
//
//size_t gl_program::instance_parameter_count() const
//{
//	return _instance_parameters.size();
//}
//
//const parameter& gl_program::instance_parameter(size_t i) const
//{
//	return _instance_parameters[i];
//}
//
//size_t gl_program::index_of_instance_parameter(const std::string_view& name) const
//{
//	auto find = _instance_parameter_lookup.find(name);
//	if (find != _instance_parameter_lookup.end())
//	{
//		return find->second;
//	}
//
//	return (size_t)-1;
//}
//
//const primitive& gl_program::default_instance_primitive(size_t instance_parameter_index) const
//{
//	static primitive default_compute = (primitive::variant_t)std::shared_ptr<compute_buffer>();
//	static primitive default_texture = (primitive::variant_t)std::shared_ptr<texture2d>();
//
//	if (instance_parameter_index < _instance_parameters.size())
//	{
//		const parameter& instance_parameter = _instance_parameters[instance_parameter_index];
//		parameter::type type = instance_parameter.cfg().type;
//		switch (type)
//		{
//		case parameter::type::COMPUTE_BUFFER:
//			return default_compute;
//		case parameter::type::TEXTURE2D:
//			return default_texture;
//		default:
//			LOG_CRITICAL( "unhandled type %s, value will not be reset before the next draw call", parameter::to_string(type).data());
//		}
//	}
//
//	return default_texture;
//}
//
//std::unique_ptr<gl_program> gl_program::make(
//	const config& cfg)
//{
//    GLuint gl_handle = gl_compile(cfg);
//
//    if(!gl_handle)
//    {
//		LOG_CRITICAL( "Error compiling shaders");
//        return false;
//    }
//
//	ASSERT_CONTEXT(nullptr == cfg.context->active_program());
//
//	glUseProgram(gl_handle);
//
//    std::vector<gl_parameter> parameters = discover_parameters(gl_handle);
//    discover_vertex_parameters(gl_handle);
//
//	glUseProgram(0);
//
//	std::vector<gl_parameter> batch_parameters;
//	std::vector<gl_parameter> material_parameters;
//	std::vector<gl_parameter> instance_parameters;
//	std::unordered_map<std::string_view, size_t> instance_parameter_lookup;
//
//	for (size_t param_index = 0; param_index < parameters.size(); ++param_index)
//	{
//		gl_parameter& parameter = parameters[param_index];
//		std::vector<gl_parameter>* dest_parameters;
//		size_t binding;
//
//		const std::string_view& name = parameters[param_index].cfg().name;
//		binding = cfg.context->batch_constraints().index_of(name);
//		if (-1 != binding)
//		{
//			dest_parameters = &batch_parameters;
//		}
//		else
//		{
//			binding = cfg.context->material_constraints().index_of(name);
//			if (-1 != binding)
//			{
//				dest_parameters = &material_parameters;
//			}
//			else
//			{
//				dest_parameters = &instance_parameters;
//				binding = instance_parameters.size();
//			}
//		}
//
//		dest_parameters->push_back(gl_parameter(
//			parameter.cfg(),
//			binding,
//			parameter.location(),
//			parameter.binding_index()));
//	}
//	
//	for (const auto& instance_parameter : instance_parameters)
//	{
//		const std::string& name = instance_parameter.cfg().name;
//		ASSERT_CONTEXT(instance_parameter_lookup.find(name) == instance_parameter_lookup.end());
//		instance_parameter_lookup[name] = instance_parameter.binding();
//	}
//
//	return std::unique_ptr<gl_program>(
//		new gl_program(
//			cfg,
//			gl_handle,
//			std::move(batch_parameters),
//			std::move(material_parameters),
//			std::move(instance_parameters),
//			std::move(instance_parameter_lookup)));
//}
//
//gl_program::gl_program(
//	const config& cfg,
//	GLuint gl_handle,
//	std::vector<gl_parameter> batch_parameters,
//	std::vector<gl_parameter> material_parameters,
//	std::vector<gl_parameter> instance_parameters,
//	std::unordered_map<std::string_view, size_t> instance_parameter_lookup)
//	: _cfg(cfg)
//	, _gl_handle(gl_handle)
//	, _batch_parameters(std::move(batch_parameters))
//	, _material_parameters(std::move(material_parameters))
//	, _instance_parameters(std::move(instance_parameters))
//	, _instance_parameter_lookup(instance_parameter_lookup)
//	, _texture_switch_metric(perf::category::SWITCH_TEXTURES, "Shader Texture Switches")
//	, _parameter_switch_metric(perf::category::SWITCH_PARAMETERS, "Shader parameter Switches")
//{}
//
