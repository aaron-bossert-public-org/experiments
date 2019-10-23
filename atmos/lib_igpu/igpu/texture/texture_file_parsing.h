
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <framework/utility/buffer_view.h>
#include <igpu/buffer/buffer.h>
#include <igpu/texture/sampler.h>
#include <igpu/texture/texture_format.h>
#include <glm/vec2.hpp>
#include <memory>
#include <string>

namespace igpu
{
	class texture_file_parsing
	{
	public:
		struct compressed_parser
		{
			texture_format format = texture_format::UNDEFINED;
			glm::ivec2 res = {};
			buffer_view<char> decompressed = {};

			compressed_parser(const buffer_view<char>& raw_file_data, int req_component_count = 4);

			~compressed_parser();
			compressed_parser(const compressed_parser&) = delete;
			compressed_parser& operator= (const compressed_parser&) = delete;
		};

		static bool parse_as_ktx(
			const buffer_view<char>& raw_file_data,
			buffer_view<char>* out_buffer_view,
			glm::ivec2* out_res,
			texture_format* out_format,
			size_t* out_mipmap_count);

		static bool parse_as_dds(
			const buffer_view<char>& raw_file_data,
			buffer_view<char>* out_buffer_view,
			glm::ivec2* out_res,
			texture_format* out_format,
			size_t* out_mipmap_count);

		static bool parse_as_pvr(
			const buffer_view<char>& raw_file_data,
			buffer_view<char>* out_buffer_view,
			glm::ivec2* out_res,
			texture_format* out_format,
			size_t* out_mipmap_count);
	};
}
