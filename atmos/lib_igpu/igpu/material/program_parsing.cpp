
#include <igpu/material/program_parsing.h>
#include <igpu_third_party_builtins/SPIRV-Cross/spirv_cross.hpp>
#include <igpu_third_party_builtins/SPIRV-Cross/spirv_parser.hpp>
#include <framework/utility/buffer_view.h>

#include <array>

using namespace igpu;
using namespace spirv_cross;

namespace
{

	bool validate(SPIRType::BaseType base_type)
	{
#define ERROR_NOT_SUPPORTED(BASE_TYPE) if(base_type == BASE_TYPE) { LOG_CRITICAL(#BASE_TYPE " not currently supported as spirv vertex atttributes"); return false; }
		ERROR_NOT_SUPPORTED(SPIRType::Unknown);
		ERROR_NOT_SUPPORTED(SPIRType::Void);
		ERROR_NOT_SUPPORTED(SPIRType::Boolean);
		ERROR_NOT_SUPPORTED(SPIRType::SByte);
		ERROR_NOT_SUPPORTED(SPIRType::UByte);
		ERROR_NOT_SUPPORTED(SPIRType::Short);
		ERROR_NOT_SUPPORTED(SPIRType::UShort);
		ERROR_NOT_SUPPORTED(SPIRType::Int);
		ERROR_NOT_SUPPORTED(SPIRType::UInt);
		ERROR_NOT_SUPPORTED(SPIRType::Int64);
		ERROR_NOT_SUPPORTED(SPIRType::UInt64);
		ERROR_NOT_SUPPORTED(SPIRType::AtomicCounter);
		ERROR_NOT_SUPPORTED(SPIRType::Half);
		//ERROR_NOT_SUPPORTED(SPIRType::Float);
		ERROR_NOT_SUPPORTED(SPIRType::Double);
		ERROR_NOT_SUPPORTED(SPIRType::Struct);
		ERROR_NOT_SUPPORTED(SPIRType::Image);
		ERROR_NOT_SUPPORTED(SPIRType::SampledImage);
		ERROR_NOT_SUPPORTED(SPIRType::Sampler);
		ERROR_NOT_SUPPORTED(SPIRType::AccelerationStructureNV);
	
#undef ERROR_NOT_SUPPORTED
		return true;
	}
	
	void parse_spirv_resources(
		Compiler& compiler,
		ShaderResources& resources,
		std::vector<spirv_resource>* out_resources)
	{
		std::array<std::array<bool, 64>, 3> tracked_params = {};

		struct
		{
			igpu::parameter::type type;
			const spirv_cross::SmallVector<spirv_cross::Resource>* p_resources = nullptr;
		} resource_categories[] = {
			{ igpu::parameter::type::COMPUTE_BUFFER, &resources.uniform_buffers },
			{ igpu::parameter::type::TEXTURE2D, &resources.sampled_images },
		};

		// Get all sampled images in the shader.
		for (const auto& resource_category : resource_categories)
		{
			for (auto &resource : *resource_category.p_resources)
			{
				unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

				bool& is_tracked = tracked_params[set][binding];
				if (!is_tracked)
				{
					is_tracked = true;
					out_resources->push_back({
						{resource.name, resource_category.type},
						set, 
						binding });
				}
			}
		}
	}

	void parse_spirv_attributes(
		Compiler& compiler,
		ShaderResources& resources,
		std::vector<spirv_attribute>* out_attributes)
	{
		std::array<bool, 64> tracked_params = {};

		for (auto &resource : resources.stage_inputs)
		{
			const SPIRType& spir_type = compiler.get_type(resource.type_id);
			unsigned location = compiler.get_decoration(resource.id, spv::DecorationLocation);

			if(!validate(spir_type.basetype))
			{
				continue;
			}

			if (spir_type.array.size())
			{
				LOG_CRITICAL(
					"arrays not currently supported");
				continue;
			}

			if (spir_type.columns != 1)
			{
				LOG_CRITICAL(
					"matrices not currently supported");
				continue;
			}

			components comp;
			switch (spir_type.vecsize)
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
				LOG_CRITICAL(
					"unhandled component count %d",
					(int)spir_type.vecsize);
				continue;
			}

			bool& is_tracked = tracked_params[location];
			if (!is_tracked)
			{
				is_tracked = true;


				out_attributes->push_back({
					{resource.name, comp}, 
					location });
			}
		}
	}
}

void igpu::parse_spirv(
	std::vector<uint32_t> spirv,
	std::vector<spirv_resource>* out_resources,
	std::vector<spirv_attribute>* out_attributes)
{
	Parser parser(std::move(spirv));
	parser.parse();

	Compiler compiler(std::move(parser.get_parsed_ir()));

	// The SPIR-V is now parsed, and we can perform reflection on it.
	ShaderResources resources = compiler.get_shader_resources();

	parse_spirv_resources(
		compiler,
		resources, 
		out_resources);

	if (out_attributes)
	{
		parse_spirv_attributes(
			compiler,
			resources,
			out_attributes);
	}
}
