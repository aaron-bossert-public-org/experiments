
#include "igpu/shader/program_parsing.h"

#include "igpu/shader/constant_parameters.h"
#include "igpu/shader/parameters.h"

#include "framework/utility/buffer_view.h"

#include "igpu_third_party_builtins/SPIRV-Cross/spirv_cross.hpp"
#include "igpu_third_party_builtins/SPIRV-Cross/spirv_parser.hpp"
#include <array>

using namespace igpu;
using namespace spirv_cross;

namespace
{
	const char* spir_type_string( SPIRType::BaseType base_type )
	{
#define CASE_TYPE( TYPE ) \
	case TYPE:            \
		return #TYPE;
		switch ( base_type )
		{
			CASE_TYPE( SPIRType::Unknown );
			CASE_TYPE( SPIRType::Void );
			CASE_TYPE( SPIRType::Boolean );
			CASE_TYPE( SPIRType::SByte );
			CASE_TYPE( SPIRType::UByte );
			CASE_TYPE( SPIRType::Short );
			CASE_TYPE( SPIRType::UShort );
			CASE_TYPE( SPIRType::Int );
			CASE_TYPE( SPIRType::UInt );
			CASE_TYPE( SPIRType::Int64 );
			CASE_TYPE( SPIRType::UInt64 );
			CASE_TYPE( SPIRType::AtomicCounter );
			CASE_TYPE( SPIRType::Half );
			CASE_TYPE( SPIRType::Float );
			CASE_TYPE( SPIRType::Double );
			CASE_TYPE( SPIRType::Struct );
			CASE_TYPE( SPIRType::Image );
			CASE_TYPE( SPIRType::SampledImage );
			CASE_TYPE( SPIRType::Sampler );
			CASE_TYPE( SPIRType::AccelerationStructureNV );
		};
#undef CASE_TYPE

		return "unknown type";
	}

	shader_stages to_stage_flag( spv::ExecutionModel execution_model )
	{
		switch ( execution_model )
		{
		case spv::ExecutionModelVertex:
			return shader_stages::VERTEX;

		case spv::ExecutionModelTessellationControl:
			return shader_stages::TESSELLATION_CONTROL;

		case spv::ExecutionModelTessellationEvaluation:
			return shader_stages::TESSELLATION_EVALUATION;

		case spv::ExecutionModelGeometry:
			return shader_stages::GEOMETRY;

		case spv::ExecutionModelFragment:
			return shader_stages::FRAGMENT;

		case spv::ExecutionModelGLCompute:
			return shader_stages::COMPUTE;
		}

		LOG_CRITICAL( "unhandled execution model %d", (int)execution_model );

		return shader_stages::NONE;
	}

	spv::ExecutionModel to_execution_model( shader_stages flag )
	{
		switch ( flag )
		{
		case shader_stages::VERTEX:
			return spv::ExecutionModelVertex;

		case shader_stages::TESSELLATION_CONTROL:
			return spv::ExecutionModelTessellationControl;

		case shader_stages::TESSELLATION_EVALUATION:
			return spv::ExecutionModelTessellationEvaluation;

		case shader_stages::GEOMETRY:
			return spv::ExecutionModelGeometry;

		case shader_stages::FRAGMENT:
			return spv::ExecutionModelFragment;

		case shader_stages::COMPUTE:
			return spv::ExecutionModelGLCompute;
		}

		if ( is_valid( flag ) )
		{
			LOG_CRITICAL(
				"unhandled stage_flag: %s",
				to_string( flag ).data() );
		}
		else
		{
			LOG_CRITICAL(
				"invalid stage_flag: %d (expected a single flag)",
				flag );
		}

		return (spv::ExecutionModel)-1;
	}
	// specialization_constants =
	//	compiler.get_specialization_constants();

	// success &= parse_spirv_specialization_constants(
	//	compiler,
	//	stage,
	//	specialization_constants,
	//	&parameter_indices,
	//	out_parameters );


	[[nodiscard]] bool parse_spirv_specialization_constants(
		Compiler& compiler,
		shader_stages shader_stage,
		SmallVector< SpecializationConstant >& specialization_constants,
		std::array< uint8_t, constant_parameters::MAX_COUNT >* out_indices,
		std::vector< constant_parameter::config >* out_constants )
	{
		// print( c.id ); // The ID of the spec constant, useful for further
		//			   // reflection.
		// print( c.constant_id ); // 20

		// const SPIRConstant& value = comp.get_constant( c.id );
		// print( value.scalar_i32() );	// 40
		// print( comp.get_name( c.id ) ); // Const

		bool success = true;

		for ( const SpecializationConstant& c : specialization_constants )
		{
			constant constant = {
				compiler.get_name( c.id ),
			};
			const SPIRConstant& value = compiler.get_constant( c.id );
			const SPIRType& spir_type =
				compiler.get_type( value.constant_type );
			size_t binding = c.constant_id;

			const SPIRConstant::ConstantVector& vec = value.vector();

			if ( spir_type.basetype == SPIRType::Int )
			{
				switch ( spir_type.vecsize )
				{
				case 1:
					constant.value = glm::ivec1( vec.r[0].i32 );
					break;
				case 2:
					constant.value = glm::ivec2( vec.r[0].i32, vec.r[1].i32 );
					break;

				case 3:
					constant.value =
						glm::ivec3( vec.r[0].i32, vec.r[1].i32, vec.r[2].i32 );
					break;

				case 4:
					constant.value = glm::ivec4(
						vec.r[0].i32,
						vec.r[1].i32,
						vec.r[2].i32,
						vec.r[4].i32 );
					break;

				default:
					success = false;
					LOG_CRITICAL(
						"unhandled component count %d",
						(int)spir_type.vecsize );
				}
			}
			else if ( spir_type.basetype == SPIRType::Float )
			{
				switch ( spir_type.vecsize )
				{
				case 1:
					constant.value = glm::vec1( vec.r[0].f32 );
					break;
				case 2:
					constant.value = glm::vec2( vec.r[0].f32, vec.r[1].f32 );
					break;

				case 3:
					constant.value =
						glm::vec3( vec.r[0].f32, vec.r[1].f32, vec.r[2].f32 );
					break;

				case 4:
					constant.value = glm::vec4(
						vec.r[0].f32,
						vec.r[1].f32,
						vec.r[2].f32,
						vec.r[4].f32 );

				default:
					success = false;
					LOG_CRITICAL(
						"unhandled component count %d",
						(int)spir_type.vecsize );
				}
			}
			else
			{
				success = false;
				LOG_CRITICAL(
					"%s is %s, not supported as shader constant",
					constant.name.c_str(),
					spir_type_string( spir_type.basetype ) );
			}

			if ( 1 < spir_type.array.size() )
			{
				success = false;
				LOG_CRITICAL(
					"multidimensional array constants not currently "
					"supported" );
			}

			if ( spir_type.columns != 1 )
			{
				success = false;
				LOG_CRITICAL( "matrix constants not currently supported" );
			}

			uint8_t* index = &( *out_indices )[binding];
			if ( *index == (uint8_t)-1 )
			{
				*index = static_cast< uint8_t >( out_constants->size() );

				out_constants->push_back( {
					constant,
					binding,
					shader_stage,
				} );
			}
			else
			{
				constant_parameter::config* constant_cfg =
					&out_constants->at( *index );

#define ERR_CHECK( EXPECT, ACTUAL, FMT, F_OP )               \
	if ( EXPECT != ACTUAL )                                  \
	{                                                        \
		success = false;                                     \
		LOG_CRITICAL(                                        \
			"constant %s binding %d previously seen as " FMT \
			" but now seen as " FMT,                         \
			constant.name.c_str(),                           \
			(int)binding,                                    \
			F_OP( EXPECT ),                                  \
			F_OP( ACTUAL ) );                                \
	}

				ERR_CHECK(
					constant_cfg->constant.name,
					constant.name,
					"%s",
					[]( const auto& s ) { return s.c_str(); } );
				ERR_CHECK(
					constant_cfg->constant.type(),
					constant.type(),
					"%s",
					[]( const auto& t ) { return to_string( t ).data(); } );

#undef ERR_CHECK
				constant_cfg->shader_stages |= shader_stage;
			}
		}

		return success;
	}

	[[nodiscard]] bool parse_spirv_resources(
		Compiler& compiler,
		shader_stages shader_stage,
		ShaderResources& resources,
		std::array< std::array< uint8_t, parameters::MAX_COUNT >, 3 >*
			out_indices,
		std::vector< parameter::config >* out_parameters )
	{
		struct
		{
			const parameter::type type;
			const spirv_cross::SmallVector< spirv_cross::Resource >*
				p_resources;
		} resource_categories[] = {
			{
				parameter::type::STORAGE_BUFFER,
				&resources.storage_buffers,
			},
			{
				parameter::type::UNIFORM_BUFFER,
				&resources.uniform_buffers,
			},
			{
				parameter::type::SAMPLER2D,
				&resources.sampled_images,
			},
		};

		bool success = true;

		// Get all sampled images in the shader.
		for ( const auto& resource_category : resource_categories )
		{
			for ( auto& resource : *resource_category.p_resources )
			{
				const SPIRType& spir_type =
					compiler.get_type( resource.type_id );

				size_t descriptor_set = compiler.get_decoration(
					resource.id,
					spv::DecorationDescriptorSet );

				size_t binding = compiler.get_decoration(
					resource.id,
					spv::DecorationBinding );


				decorator decorators = decorator::READABLE;

				if ( resource_category.type == parameter::type::STORAGE_BUFFER )
				{
					decorators = decorator::NOTHING;

					Bitset ssbo_mask =
						compiler.get_buffer_block_flags( resource.id );

					if ( !ssbo_mask.get( spv::DecorationNonWritable ) )
					{
						decorators |= decorator::WRITABLE;
					}
					if ( !ssbo_mask.get( spv::DecorationNonReadable ) )
					{
						decorators |= decorator::READABLE;
					}
				}

				if ( 0 == ( decorators & decorator::READ_WRITE ) )
				{
					success = false;
					LOG_CRITICAL(
						"%s must be readable or writable",
						resource.name.c_str() );
				}

				if ( spir_type.basetype != SPIRType::Struct &&
					 spir_type.basetype != SPIRType::SampledImage )
				{
					success = false;
					LOG_CRITICAL(
						"%s is %s, not supported as shader uniform",
						resource.name.c_str(),
						spir_type_string( spir_type.basetype ) );
				}

				if ( 1 < spir_type.array.size() )
				{
					success = false;
					LOG_CRITICAL(
						"multidimensional array resources (eg uniform UBO{} "
						"ubo[10][10]) not currently supported" );
				}

				if ( spir_type.columns != 1 )
				{
					success = false;
					LOG_CRITICAL( "matrices not currently supported" );
				}

				size_t array_size =
					spir_type.array.size() ? spir_type.array[0] : 1;

				uint8_t* index = &( *out_indices )[descriptor_set][binding];
				if ( *index == (uint8_t)-1 )
				{
					*index = static_cast< uint8_t >( out_parameters->size() );

					out_parameters->push_back( {
						resource.name,
						resource_category.type,
						decorators,
						array_size,
						descriptor_set,
						binding,
						shader_stage,
					} );
				}
				else
				{
					parameter::config* parameter =
						&out_parameters->at( *index );

#define ERR_CHECK( EXPECT, ACTUAL, FMT, F_OP )                     \
	if ( EXPECT != ACTUAL )                                        \
	{                                                              \
		success = false;                                           \
		LOG_CRITICAL(                                              \
			"descriptor set %d binding %d previously seen as " FMT \
			" but now seen as " FMT,                               \
			(int)descriptor_set,                                   \
			(int)binding,                                          \
			F_OP( EXPECT ),                                        \
			F_OP( ACTUAL ) );                                      \
	}

					ERR_CHECK(
						parameter->name,
						resource.name,
						"%s",
						[]( auto& s ) { return s.c_str(); } );
					ERR_CHECK(
						parameter->type,
						resource_category.type,
						"%s",
						[]( auto& t ) {
							return parameter::to_string( t ).data();
						} );
					ERR_CHECK( parameter->array_size, array_size, "%d", int );
#undef ERR_CHECK
					parameter->shader_stages |= shader_stage;
					parameter->decorators |= decorators;
				}
			}
		}

		return success;
	}

	[[nodiscard]] bool parse_vertex_parameters(
		Compiler& compiler,
		ShaderResources& resources,
		std::array< uint8_t, parameters::MAX_COUNT >* out_indices,
		std::vector< vertex_parameter::config >* out_vertex_parameters )
	{
		bool success = true;

		for ( auto& resource : resources.stage_inputs )
		{
			const SPIRType& spir_type = compiler.get_type( resource.type_id );
			uint32_t location =
				compiler.get_decoration( resource.id, spv::DecorationLocation );

			if ( spir_type.basetype != SPIRType::Float )
			{
				success = false;
				LOG_CRITICAL(
					"%s is %s, not supported as vertex attribute",
					resource.name.c_str(),
					spir_type_string( spir_type.basetype ) );
			}

			if ( spir_type.array.size() )
			{
				success = false;
				LOG_CRITICAL(
					"array vertex attributes (eg attribute vec3[10]) not "
					"currently supported" );
			}

			if ( spir_type.columns != 1 )
			{
				success = false;
				LOG_CRITICAL( "matrices not currently supported" );
			}

			components comp = components::UNDEFINED;

			switch ( spir_type.vecsize )
			{
			case 1:
				comp = components::FLOAT1;
				break;
			case 2:
				comp = components::FLOAT2;
				break;

			case 3:
				comp = components::FLOAT3;
				break;

			case 4:
				comp = components::FLOAT4;
				break;

			default:
				success = false;
				LOG_CRITICAL(
					"unhandled component count %d",
					(int)spir_type.vecsize );
			}

			uint8_t* index = &( *out_indices )[location];
			if ( *index == (uint8_t)-1 )
			{
				*index =
					static_cast< uint8_t >( out_vertex_parameters->size() );

				out_vertex_parameters->push_back( {
					resource.name,
					comp,
					location,
				} );
			}
			else
			{
				vertex_parameter::config* config =
					&out_vertex_parameters->at( *index );
#define ERR_CHECK( EXPECT, ACTUAL, FMT, F_OP )            \
	if ( EXPECT != ACTUAL )                               \
	{                                                     \
		success = false;                                  \
		LOG_CRITICAL(                                     \
			"vertex attribute %d previously seen as " FMT \
			" but now seen as " FMT,                      \
			(int)location,                                \
			F_OP( EXPECT ),                               \
			F_OP( ACTUAL ) );                             \
	}
				ERR_CHECK( config->name, resource.name, "%s", []( auto& s ) {
					return s.c_str();
				} );
				ERR_CHECK( config->components, comp, "%s", []( auto& c ) {
					return to_string( c ).data();
				} );
				ERR_CHECK( config->location, location, "%d", int );
#undef ERR_CHECK
			}
		}

		return success;
	}
}

bool igpu::spirv_parse(
	std::vector< uint32_t > spirv,
	std::string entry_point,
	shader_stages stages,
	std::vector< constant_parameter::config >* out_constants,
	std::vector< parameter::config >* out_parameters,
	std::vector< vertex_parameter::config >* out_vertex_parameters )
{
	bool success = true;
	Parser parser( std::move( spirv ) );
	parser.parse();


	std::array< uint8_t, parameters::MAX_COUNT > vertex_parameter_indices;
	std::array< uint8_t, constant_parameters::MAX_COUNT > constant_indices;
	std::array< std::array< uint8_t, parameters::MAX_COUNT >, 3 >
		parameter_indices;
	memset( &parameter_indices, -1, sizeof parameter_indices );
	memset( &constant_indices, -1, sizeof constant_indices );
	memset( &vertex_parameter_indices, -1, sizeof vertex_parameter_indices );

	Compiler compiler( std::move( parser.get_parsed_ir() ) );
	SmallVector< EntryPoint > entry_points =
		compiler.get_entry_points_and_stages();

	for ( const auto& point : compiler.get_entry_points_and_stages() )
	{
		if ( point.name == entry_point )
		{
			auto stage = to_stage_flag( point.execution_model );
			if ( shader_stages::NONE != ( stage & stages ) )
			{
				compiler.set_entry_point( point.name, point.execution_model );
				auto active = compiler.get_active_interface_variables();
				ShaderResources resources =
					compiler.get_shader_resources( active );

				SmallVector< SpecializationConstant > specialization_constants =
					compiler.get_specialization_constants();

				success &= parse_spirv_specialization_constants(
					compiler,
					stage,
					specialization_constants,
					&constant_indices,
					out_constants );

				success &= parse_spirv_resources(
					compiler,
					stage,
					resources,
					&parameter_indices,
					out_parameters );

				if ( out_vertex_parameters && stage == shader_stages::VERTEX )
				{
					success &= parse_vertex_parameters(
						compiler,
						resources,
						&vertex_parameter_indices,
						out_vertex_parameters );
				}
			}
		}
	}

	return success;
}
