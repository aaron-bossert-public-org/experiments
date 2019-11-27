

#include "dcs_test/src/dcs_utils.h"

#include "igpu/buffer/buffer.h"
#include "igpu/buffer/geometry.h"
#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/context/context.h"

#include "framework/meta_programming/va_macro.h"
#include "framework/utility/buffer_view.h"
#include "framework/utility/string_utils.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
//#include "assimp/cimport.h"
#include "assimp/scene.h"
#if defined( ATMOS_BUILD_ANDROID )
#	include <android/asset_manager.h>
#else
#	include <fstream>
#endif


using namespace igpu;
using namespace dcs_utils;
namespace
{
	struct file_handle
	{
		file_handle( const std::string_view& path );

		void read( char*, size_t );

		void close();

		size_t size() const
		{
			return _size;
		}

		~file_handle()
		{
			close();
		}

	private:
		size_t _size = 0;

#if ATMOS_BUILD_ANDROID
		AAsset* _asset = nullptr;
	};

	file_handle::file_handle( const std::string_view& path )
		: _asset( AAssetManager_open(
			  androidApp->activity->assetManager,
			  path.data(),
			  AASSET_MODE_STREAMING ) )
	{
		if ( !_asset )
		{
			LOG_CRITICAL( "Could not \"%s\"!", path.data() );
		}
		else
		{
			_size = AAsset_getLength( _asset );
			ASSERT_CONTEXT( _size > 0 );
		}
	}

	void file_handle::close()
	{
		if ( _asset )
		{
			AAsset_close( _asset );
			_asset = nullptr;
		}
	}

	void file_handle::read( char* out, size_t size )
	{
		if ( _asset )
		{
			AAsset_read( _asset, out, size );
		}
	}
#else
		std::ifstream _ifs;
	};

	file_handle::file_handle( const std::string_view& path )
		: _ifs( path.data(), std::ios::binary | std::ios::ate )
	{
		if ( !_ifs )
		{
			LOG_CRITICAL( "Could not load \"%s\"!", path.data() );
		}
		else
		{
			_size = _ifs.tellg();
			_ifs.seekg( 0, std::ios::beg );

			ASSERT_CONTEXT( _size > 0 );
		}
	}

	void file_handle::close()
	{
		if ( _ifs.is_open() )
		{
			_ifs.close();
		}
	}

	void file_handle::read( char* out, size_t size )
	{
		if ( _ifs.is_open() )
		{
			_ifs.read( out, size );
		}
	}
#endif
}


bool dcs_utils::load_buffer( const std::string_view& path, buffer* out_buffer )
{
	const char* err = nullptr;
	file_handle file( path );

	if ( file.size() )
	{
		if ( !out_buffer )
		{
			err = "out buffer is null";
		}
		else
		{
			buffer_view< char > view( file.size(), nullptr );
			out_buffer->map( &view );

			if ( nullptr == view.data() )
			{
				err =
					"could not obtain mapped memory from buffer to "
					"copy "
					"file "
					"into";
			}
			else
			{
				file.read( view.data(), view.byte_size() );
				file.close();

				out_buffer->unmap();

				return true;
			}
		}

		if ( err )
		{
			LOG_CRITICAL( "%s : file '%s'", err, path.data() );
		}
	}

	return false;
}

std::vector< uint32_t > dcs_utils::load_mem( const std::string_view& path )
{
	const char* err = nullptr;
	file_handle file( path );
	if ( file.size() )
	{
		std::vector< uint32_t > memory( ( file.size() + 3 ) / 4 );

		file.read( (char*)memory.data(), file.size() );

		return memory;
	}


	return {};
}

model dcs_utils::load_model(
	igpu::context* context,
	const std::string_view& path,
	model_flags flags )
{
	int ai_flags = aiProcess_Triangulate;

	std::pair< model_flags, int > pairs[] = {
		{ model_flags::GEN_NORMALS, aiProcess_GenSmoothNormals },
		{ model_flags::GEN_TANGENTS, aiProcess_CalcTangentSpace },
		{ model_flags::CLOCKWISE_WINDING, aiProcess_FlipWindingOrder },
		{ model_flags::FLIP_TEXCOORD_Y, aiProcess_FlipUVs },
		{ model_flags::PRE_TRANSFORM, aiProcess_PreTransformVertices },
		{ model_flags::JOIN_IDENTICAL_VERTICES,
		  aiProcess_JoinIdenticalVertices },

	};

	for ( const auto& pair : pairs )
	{
		if ( 0 != ( flags & pair.first ) )
		{
			ai_flags |= pair.second;
		}
	}

	Assimp::Importer Importer;
	const aiScene* scene;

	// Load file
	// Meshes are stored inside the apk on Android (compressed)
	// So they need to be loaded via the asset manager
	file_handle file( path );
	if ( !path.size() )
	{
		return {};
	}
	{
		std::unique_ptr< char[] > data( new char[file.size()] );
		file.read( data.get(), file.size() );
		scene =
			Importer.ReadFileFromMemory( data.get(), file.size(), ai_flags );
	}

	if ( !scene )
	{
		LOG_CRITICAL(
			"Error parsing '%s': '%s'\n",
			path.data(),
			Importer.GetErrorString() );
	}
	else if ( 0 == scene->mNumMeshes )
	{
		LOG_CRITICAL( "no meshes were loaded from %s", path.data() );
	}
	else
	{
		model m;
		glm::vec3 scale( 1.0f );
		glm::vec2 uvscale( 1.0f );
		glm::vec3 center( 0.0f );

		std::vector< float > vertexBuffer;
		std::vector< uint32_t > indexBuffer;

		auto get_attr = []( const aiMesh* mesh ) {
			return &mesh->mVertices[0].x;
		};
		auto has_attr = []( const aiMesh* mesh ) {
			return mesh->HasPositions();
		};

		using get_attr_t = std::function< float*( const aiMesh* ) >;
		using has_attr_t = std::function< bool( const aiMesh* ) >;

		size_t num_uvs = scene->mMeshes[0]->GetNumUVChannels();
		size_t num_cls = scene->mMeshes[0]->GetNumColorChannels();
		struct attrib_handler
		{
			vertex_buffer::attribute attr;
			size_t stride = 0;
			get_attr_t get_attr;
			has_attr_t has_attr;
		};

		attrib_handler handlers[] = {
			{
				{ "nrm", components::FLOAT3 },
				sizeof( ( (aiMesh*)0 )->mNormals[0] ),
				[]( const aiMesh* mesh ) { return &mesh->mNormals[0][0]; },
				[]( const aiMesh* mesh ) { return mesh->HasNormals(); },
			},
			{
				{ "tan", components::FLOAT3 },
				sizeof( ( (aiMesh*)0 )->mTangents[0] ),
				[]( const aiMesh* mesh ) { return &mesh->mTangents[0][0]; },
				[]( const aiMesh* mesh ) {
					return mesh->HasTangentsAndBitangents();
				},
			},
			{
				{ "btn", components::FLOAT3 },
				sizeof( ( (aiMesh*)0 )->mBitangents[0] ),
				[]( const aiMesh* mesh ) { return &mesh->mBitangents[0][0]; },
				[]( const aiMesh* mesh ) {
					return mesh->HasTangentsAndBitangents();
				},
			},
		};

		aiMesh* first_mesh = *scene->mMeshes;
		vertex_buffer::config vert_cfg = {};
		m.vertex_count = first_mesh->mNumVertices;
		m.index_count = first_mesh->mNumFaces * 3;

		// verify consistent vertex formats
		std::vector< attrib_handler > active_handlers;
		for ( const auto& handler : handlers )
		{
			if ( handler.has_attr( first_mesh ) )
			{
				active_handlers.push_back( handler );
				vert_cfg.attributes.push_back( handler.attr );
			}
		}

		char uvx_str[] = "uvx";
		for ( size_t i = 0; i < num_uvs; ++i )
		{
			// asset implorter supports max 8 uvs
			uvx_str[2] = '0' + (char)i;

			vert_cfg.attributes.push_back( {
				uvx_str,
				(components)first_mesh->mNumUVComponents[i],
			} );

			active_handlers.push_back( {
				vert_cfg.attributes.back(),
				sizeof( ( (aiMesh*)0 )->mTextureCoords[0][0] ),
				[i]( const aiMesh* mesh ) {
					return &mesh->mTextureCoords[i][0][0];
				},
			} );
		}

		char clx_str[] = "clx";
		for ( size_t i = 0; i < num_cls; ++i )
		{
			// asset implorter supports max 8 colors
			clx_str[2] = '0' + (char)i;

			vert_cfg.attributes.push_back( { clx_str, components::FLOAT3 } );

			active_handlers.push_back( {
				vert_cfg.attributes.back(),
				sizeof( ( (aiMesh*)0 )->mColors[0][0] ),
				[i]( const aiMesh* mesh ) { return &mesh->mColors[i][0][0]; },
			} );
		}

		// fix up offsets of each attribute
		for ( auto& attr : vert_cfg.attributes )
		{
			attr.offset = vert_cfg.stride;
			vert_cfg.stride += sizeof( float ) * (int)attr.components;
		}

		// round up vertex size to the nearest vec4
		uint32_t sv4 = sizeof( glm::vec4 );
		vert_cfg.stride = ( ( vert_cfg.stride - 1 + sv4 ) / sv4 ) * sv4;


		bool consistent_vert_formats = true;
		m.meshes.resize( scene->mNumMeshes );

		for ( size_t i = 1; i < scene->mNumMeshes; ++i )
		{
			auto* mesh = scene->mMeshes[i];
			m.vertex_count += mesh->mNumVertices;
			m.index_count += mesh->mNumFaces * 3;

			if ( num_uvs != mesh->GetNumUVChannels() )
			{
				LOG_CRITICAL(
					"mesh %s index %d does not have same number of uv "
					"channels as first mesh in scene",
					mesh->mName.C_Str(),
					(int)i );
				consistent_vert_formats = false;
			}
			else
			{
				for ( size_t iuv = 0; iuv < num_uvs; ++iuv )
				{
					if ( first_mesh->mNumUVComponents[iuv] !=
						 mesh->mNumUVComponents[iuv] )
					{
						LOG_CRITICAL(
							"mesh:%s index:%d uv chanel:%d does not have same "
							"number of uv "
							"components:%d as first mesh in scene %d",
							mesh->mName.C_Str(),
							i,
							iuv,
							mesh->mNumUVComponents[iuv],
							first_mesh->mNumUVComponents[iuv] );
						consistent_vert_formats = false;
					}
				}
			}
			if ( num_cls != mesh->GetNumColorChannels() )
			{
				LOG_CRITICAL(
					"mesh %s index %d does not have same number of "
					"color "
					"channels as first mesh in scene",
					mesh->mName.C_Str(),
					(int)i );
				consistent_vert_formats = false;
			}
			for ( const auto& active_handler : active_handlers )
			{
				if ( !active_handler.has_attr( mesh ) )
				{
					LOG_CRITICAL(
						"mesh %s index %d does not have attribute %s",
						mesh->mName.C_Str(),
						(int)i,
						active_handler.attr.name.c_str() );
					consistent_vert_formats = false;
				}
			}
		}

		if ( !consistent_vert_formats )
		{
			return {};
		}

		auto pos_buffer = context->make_shared( vertex_buffer::config{
			(uint32_t)sizeof glm::vec4,
			{ { "pos", components::FLOAT4 } },
		} );

		auto vertex_buffer = context->make_shared( vert_cfg );
		auto index_buffer = context->make_shared(
			index_buffer::config{ index_format::UNSIGNED_INT } );

		buffer_view< uint32_t > index_view( m.index_count, nullptr );
		buffer_view< glm::vec4 > pos_view( m.vertex_count, nullptr );

		buffer_view< char > vertex_view(
			m.vertex_count * vert_cfg.stride,
			nullptr );

		pos_buffer->map( &pos_view );
		vertex_buffer->map( &vertex_view );
		index_buffer->map( &index_view );

		int32_t base_vertex = 0;
		uint32_t first_index = 0;

		for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
		{
			const aiMesh* mesh = scene->mMeshes[i];

			utility::aabb aabb = {};
			{
				// append positions
				const aiVector3D* src_at = (aiVector3D*)mesh->mVertices;
				const aiVector3D* src_end = src_at + mesh->mNumVertices;
				glm::vec4* dst_at = base_vertex + pos_view.data();

				for ( ; src_at < src_end; ++src_at, ++dst_at )
				{
					memcpy( dst_at, src_at, sizeof *src_at );
					dst_at->w = 1;

					aabb.max.x = fmax( src_at->x, aabb.max.x );
					aabb.max.y = fmax( src_at->y, aabb.max.y );
					aabb.max.z = fmax( src_at->z, aabb.max.z );

					aabb.min.x = fmin( src_at->x, aabb.min.x );
					aabb.min.y = fmin( src_at->y, aabb.min.y );
					aabb.min.z = fmin( src_at->z, aabb.min.z );
				}
			}

			// append non positional vertex data
			for ( const auto& active_handler : active_handlers )
			{
				size_t csz =
					sizeof( float ) * size_t( active_handler.attr.components );

				const char* src_at = (char*)active_handler.get_attr( mesh );
				const char* src_end =
					src_at + mesh->mNumVertices * active_handler.stride;
				char* dst_at = base_vertex * vert_cfg.stride +
					active_handler.attr.offset + vertex_view.data();

				for ( ; src_at < src_end; src_at += active_handler.stride,
										  dst_at += vert_cfg.stride )
				{
					memcpy( dst_at, src_at, csz );
				}
			}

			{ // append indices
				const aiFace* src_at = mesh->mFaces;
				const aiFace* src_end = mesh->mFaces + mesh->mNumFaces;
				uint32_t* dst_at = first_index + index_view.data();
				for ( ; src_at < src_end; ++src_at, dst_at += 3 )
				{
					if ( src_at->mNumIndices != 3 )
					{
						LOG_CRITICAL( "all meshes must be triangulated" );
						return {};
					}

					memcpy( dst_at, src_at->mIndices, 3 * sizeof( uint32_t ) );
				}
			}

			aiColor3D color( 0.f, 0.f, 0.f );
			scene->mMaterials[mesh->mMaterialIndex]->Get(
				AI_MATKEY_COLOR_DIFFUSE,
				color );

			m.meshes[i].base_vertex = base_vertex;
			m.meshes[i].vertex_count = mesh->mNumVertices;
			m.meshes[i].first_index = first_index;
			m.meshes[i].index_count = mesh->mNumFaces * 3;
			m.meshes[i].aabb = aabb;
			m.meshes[i].material_color =
				glm::vec4( color.r, color.g, color.b, 1 );


			base_vertex += mesh->mNumVertices;
			first_index += 3 * mesh->mNumFaces;
		}

		pos_buffer->unmap();
		vertex_buffer->unmap();
		index_buffer->unmap();

		m.geometry = context->make_shared( geometry::config{
			(std::string)path,
			topology::TRIANGLE_LIST,
			index_buffer,
			{ pos_buffer, vertex_buffer } } );

		return m;
	}

	return {};
}