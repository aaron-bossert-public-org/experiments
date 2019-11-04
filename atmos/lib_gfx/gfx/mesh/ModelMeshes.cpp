
#include "gfx/mesh/mesh.h"
#include "gfx/mesh/model_meshes.h"

using namespace gfx;

model_meshes::model_meshes(
	meshes_t meshes,
	material_indices_t material_indices )
	: _meshes( std::move( meshes ) )
	, _material_indices( std::move( material_indices ) )
{}

std::unique_ptr< model_meshes > model_meshes::make(
	const std::string& file,
	const mesh_geometries_t& mesh_geometries )
{
	material_indices_t material_indices( mesh_geometries.size() );

	for ( int i = 0; i < material_indices.size(); i++ )
	{
		material_indices[i] = i;
	}

	return make( file, mesh_geometries, material_indices );
}

std::unique_ptr< model_meshes > model_meshes::make(
	const std::string& file,
	const mesh_geometries_t& mesh_geometries,
	const material_indices_t& material_indices )
{
	ASSERT_CONTEXT( material_indices.size() == mesh_geometries.size() );
	if ( material_indices.size() != mesh_geometries.size() )
		return nullptr;


	auto meshes = transform< meshes_t >(
		mesh_geometries,
		[file]( const mesh_geometry_t& mesh_geometry ) {
			return std::shared_ptr< mesh >(
				mesh::make( file, mesh_geometry.get() ) );
		} );

	return std::unique_ptr< model_meshes >(
		new model_meshes( std::move( meshes ), material_indices ) );
}

const model_meshes::meshes_t& model_meshes::meshes() const
{
	return _meshes;
}

const model_meshes::material_indices_t& model_meshes::material_indices() const
{
	return _material_indices;
}

void model_meshes::draw_isolated() const
{
	for ( const mesh_t& mesh : _meshes )
	{
		mesh->begin_batched();
		mesh->draw_batched();
		mesh->end_batched();
	}
}
