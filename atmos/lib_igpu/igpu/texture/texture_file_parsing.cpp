
#include "igpu/texture/texture_file_parsing.h"

#include "igpu/buffer/buffer.h"

using namespace igpu;


#pragma warning( push )
#pragma warning( disable : 4505 )

#define STBI_NO_STDIO
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#pragma warning( pop )

texture_file_parsing::compressed_parser::compressed_parser(
	const buffer_view< char >& raw_file_data,
	int req_component_count )
{
	// if this is 16, stb_image will leave bit depth as it is stored in the
	// image format.
	constexpr int max_bits_per_channel = 16;

	stbi__context s;
	stbi__result_info ri;
	stbi__start_mem(
		&s,
		(const stbi_uc*)raw_file_data.data(),
		(int)raw_file_data.size() );
	int comp = 0;
	void* raw = nullptr;
	if ( stbi__hdr_test( &s ) )
	{
		raw = stbi__hdr_load(
			&s,
			&res.x,
			&res.y,
			&comp,
			req_component_count,
			&ri );
	}
	else
	{
		raw = stbi__load_main(
			&s,
			&res.x,
			&res.y,
			&comp,
			req_component_count,
			&ri,
			max_bits_per_channel );
	}

	if ( raw )
	{
		if ( req_component_count )
		{
			comp = ri.num_channels = req_component_count;
		}

		size_t byte_size = ( res.x * res.y * comp * ri.bits_per_channel ) / 8;
		decompressed = buffer_view< char >( byte_size, (char*)raw );

		ASSERT_CONTEXT( ri.channel_order == STBI_ORDER_RGB );
		ASSERT_CONTEXT( ri.bits_per_channel == 8 || ri.bits_per_channel == 16 );

		if ( ri.bits_per_channel == 8 )
		{
			switch ( comp )
			{
			case STBI_grey:
				format = texture_format::INT_R8;
				break;
			case STBI_grey_alpha:
				format = texture_format::INT_R8G8;
				break;
			case STBI_rgb:
				format = texture_format::INT_R8G8B8;
				break;
			case STBI_rgb_alpha:
				format = texture_format::INT_R8G8B8A8;
				break;
			default:
				LOG_CRITICAL( "unexpected component count: %d", comp );
			}
		}
		else if ( ri.bits_per_channel == 16 )
		{
			switch ( comp )
			{
			case STBI_grey:
				format = texture_format::INT_R16;
				break;
			case STBI_grey_alpha:
				format = texture_format::INT_R16G16;
				break;
			case STBI_rgb:
				format = texture_format::INT_R16G16B16;
				break;
			case STBI_rgb_alpha:
				format = texture_format::INT_R16G16B16A16;
				break;
			default:
				LOG_CRITICAL( "unexpected component count: %d", comp );
			}
		}
	}
}

texture_file_parsing::compressed_parser::~compressed_parser()
{
	if ( decompressed.data() )
		stbi_image_free( decompressed.data() );
}

bool texture_file_parsing::parse_as_ktx(
	buffer_view< char >& raw_file_data,
	buffer_view< char >* out_buffer_view,
	texture2d::state* out_state )
{
	// Specification of KTX file container format can be found at
	// https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
	//
	// Locally describe the header definition of a ktx file and data block
	struct KTXFileHeader
	{
		uint8_t identifier[12];
		uint32_t endianness;
		uint32_t gl_type;
		uint32_t glTypeSize;
		uint32_t glFormat;
		uint32_t glInternalFormat;
		uint32_t glBaseInternalFormat;
		uint32_t width;
		uint32_t height;
		uint32_t depth;
		uint32_t numberOfArrayElements;
		uint32_t numberOfFaces;
		uint32_t numberOfMipmapLevels;
		uint32_t bytesOfKeyValueData;
	};

	struct KTXDataBlock
	{
		uint32_t imageSize;
		char pixels[1];
	};

	constexpr uint8_t kKTXidentifier[12] = {
		0xAB,
		0x4B,
		0x54,
		0x58,
		0x20,
		0x31,
		0x31,
		0xBB,
		0x0D,
		0x0A,
		0x1A,
		0x0A,
	};

	KTXFileHeader* ktxHeader = (KTXFileHeader*)raw_file_data.data();
	KTXDataBlock* ktxData =
		(KTXDataBlock*)( sizeof( KTXFileHeader ) + ktxHeader->bytesOfKeyValueData + (char*)ktxHeader );

	if ( raw_file_data.size() < sizeof( KTXFileHeader ) ||
		 memcmp(
			 ktxHeader->identifier,
			 kKTXidentifier,
			 sizeof( kKTXidentifier ) ) )
	{
		return false;
	}
	else if ( ktxHeader->glFormat != 0 )
	{
		// glFormat will be 0 if its a compressed format, ktx must contain a
		// valid format for all raw formats.
		LOG_CRITICAL( "raw texture formats currently unsupported" );
		return false;
	}

	*out_buffer_view =
		buffer_view< char >( ktxData->imageSize, ktxData->pixels );
	*out_state = {
		{
			(int32_t)ktxHeader->width,
			(int32_t)ktxHeader->height,
		},
		texture_format::ETC1_RGB,
		(size_t)ktxHeader->numberOfMipmapLevels,
	};

	return true;
}

bool texture_file_parsing::parse_as_dds(
	buffer_view< char >& raw_file_data,
	buffer_view< char >* out_buffer_view,
	texture2d::state* out_state )
{
	auto dwMagicNumber =
		*reinterpret_cast< const uint32_t* >( raw_file_data.data() );
	constexpr uint32_t DDS_MAGIC = 0x20534444; // "DDS "
	constexpr uint32_t DDSCAPS2_CUBEMAP = 0x00000200;
	constexpr uint32_t DDSCAPS2_VOLUME = 0x00200000;
	constexpr uint32_t DDSF_ALPHAPIXELS = 0x00200000;
	constexpr uint32_t DDSF_RGB = 0x00000040;
	constexpr uint32_t DDSF_RGBA = 0x00000041;
	constexpr uint32_t DDSF_FOURCC = 0x00000004;

	struct DDS_PIXELFORMAT
	{
		uint32_t size;
		uint32_t flags;
		uint32_t fourCC;
		uint32_t RGBBitCount;
		uint32_t RBitMask;
		uint32_t GBitMask;
		uint32_t BBitMask;
		uint32_t ABitMask;
	};
	struct DDS_HEADER
	{
		uint32_t size;
		uint32_t flags;
		uint32_t height;
		uint32_t width;
		uint32_t pitchOrLinearSize;
		uint32_t depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
		uint32_t mipMapCount;
		uint32_t reserved1[11];
		DDS_PIXELFORMAT ddspf;
		uint32_t caps;
		uint32_t caps2;
		uint32_t caps3;
		uint32_t caps4;
		uint32_t reserved2;
	};
	auto makeFourCC = []( char ch0, char ch1, char ch2, char ch3 ) {
		return ( uint32_t( ch0 ) << 0 ) | ( uint32_t( ch1 ) << 8 ) |
			( uint32_t( ch2 ) << 16 ) | ( uint32_t( ch3 ) << 24 );
	};

	if ( raw_file_data.size() <
			 ( sizeof( DDS_MAGIC ) + sizeof( DDS_HEADER ) ) ||
		 dwMagicNumber != DDS_MAGIC )
	{
		return false;
	}

	auto ddsHeader = reinterpret_cast< const DDS_HEADER* >(
		raw_file_data.data() + sizeof( uint32_t ) );

	if ( !( ddsHeader->ddspf.flags & DDSF_FOURCC ) )
	{
		return false;
	}

	// Verify header to validate DDS file
	ASSERT_CONTEXT( ddsHeader->size != sizeof( DDS_HEADER ) );
	ASSERT_CONTEXT( ddsHeader->ddspf.size != sizeof( DDS_PIXELFORMAT ) );
	ASSERT_CONTEXT( 0 == ( ddsHeader->caps2 & DDSCAPS2_CUBEMAP ) );
	ASSERT_CONTEXT( 0 == ( ddsHeader->caps2 & DDSCAPS2_VOLUME ) );

	texture_format format = texture_format::UNDEFINED;
	switch ( ddsHeader->ddspf.fourCC )
	{
	case makeFourCC( 'D', 'X', 'T', '1' ):
		format = texture_format::DXT_1_RGBA;
		break;
	case makeFourCC( 'D', 'X', 'T', '3' ):
		format = texture_format::DXT_3_RGBA;
		break;
	case makeFourCC( 'D', 'X', 'T', '5' ):
		format = texture_format::DXT_5_RGBA;
		break;
	default:
		LOG_CRITICAL( "unhandled format %4s", (char*)&ddsHeader->ddspf.fourCC );
		return false;
	}

	// set texture data for texel_buffer2d
	*out_state = {
		{
			(int32_t)ddsHeader->width,
			(int32_t)ddsHeader->height,
		},
		format,
		(size_t)ddsHeader->mipMapCount,
	};

	*out_buffer_view = buffer_view< char >(
		raw_file_data.size() - sizeof( DDS_HEADER ),
		raw_file_data.data() + sizeof( DDS_HEADER ) );

	return true;
}

#pragma warning( push )
#pragma warning( disable : 4005 )
#include "igpu_third_party_builtins/pvr/tools/PVRTTexture.h"
#pragma warning( pop )

bool texture_file_parsing::parse_as_pvr(
	buffer_view< char >& raw_file_data,
	buffer_view< char >* out_buffer_view,
	texture2d::state* out_state )
{
	PVRTextureHeaderV3 scratch;
	const auto* pvr_header = (const PVRTextureHeaderV3*)raw_file_data.data();
	const auto* legacy_header = (const PVR_Texture_Header*)raw_file_data.data();
	buffer_view< char > texture_data;

	if ( raw_file_data.size() > sizeof( PVRTextureHeaderV3 ) &&
		 pvr_header->u32Version == PVRTEX3_IDENT )
	{
		// set texture data for texel_buffer2d
		texture_data = buffer_view< char >(
			raw_file_data.size() - sizeof( PVRTextureHeaderV3 ),
			raw_file_data.data() + sizeof( PVRTextureHeaderV3 ) );
	}
	else if (
		raw_file_data.size() > sizeof( PVR_Texture_Header ) &&
		legacy_header->dwPVR == PVRTEX_IDENTIFIER )
	{
		// set texture data for texel_buffer2d
		// & upgrade header from pvr 1 to pvr 3
		texture_data = buffer_view< char >(
			raw_file_data.size() - sizeof( PVR_Texture_Header ),
			raw_file_data.data() + sizeof( PVR_Texture_Header ) );
		PVRTConvertOldTextureHeaderToV3( legacy_header, scratch, nullptr );
		pvr_header = &scratch;
	}

	// translate texture format and make texel_buffer2d
	if ( texture_data.size() )
	{
		texture_format format = texture_format::UNDEFINED;

		switch ( pvr_header->u64PixelFormat )
		{
		case ePVRTPF_PVRTCI_2bpp_RGB:
			format = texture_format::PVR1_2BPP_RGB;
			break;
		case ePVRTPF_PVRTCI_2bpp_RGBA:
			format = texture_format::PVR1_2BPP_RGBA;
			break;
		case ePVRTPF_PVRTCI_4bpp_RGB:
			format = texture_format::PVR1_4BPP_RGB;
			break;
		case ePVRTPF_PVRTCI_4bpp_RGBA:
			format = texture_format::PVR1_4BPP_RGBA;
			break;
		case ePVRTPF_PVRTCII_2bpp:
			format = texture_format::PVR2_2BPP;
			break;
		case ePVRTPF_PVRTCII_4bpp:
			format = texture_format::PVR2_4BPP;
			break;
		case ePVRTPF_ETC1:
			format = texture_format::ETC1_RGB;
			break;
		}

		*out_state = {
			{
				(int32_t)pvr_header->u32Width,
				(int32_t)pvr_header->u32Height,
			},
			format,
			(size_t)pvr_header->u32MIPMapCount,
		};

		*out_buffer_view = texture_data;

		return true;
	}

	return false;
}
