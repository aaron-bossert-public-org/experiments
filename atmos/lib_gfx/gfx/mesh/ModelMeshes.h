
#pragma once

#include "gfx/material/technique.h"

namespace gfx
{
	class mesh;
	class mesh_geometry;
	class program_instance;

	class model_meshes
	{
	public:
		using mesh_geometry_t = std::shared_ptr< mesh_geometry >;
		using mesh_geometries_t = std::vector< mesh_geometry_t >;
		using mesh_t = std::shared_ptr< mesh >;
		using meshes_t = std::vector< mesh_t >;
		using material_indices_t = std::vector< size_t >;

	public:
		const meshes_t& meshes() const;

		const material_indices_t& material_indices() const;

		void draw_isolated() const;

	public:
		static std::unique_ptr< model_meshes > make(
			const std::string& file,
			const mesh_geometries_t& );

		static std::unique_ptr< model_meshes > make(
			const std::string& file,
			const mesh_geometries_t&,
			const material_indices_t& );

	private:
		model_meshes( meshes_t, material_indices_t );

	private:
		meshes_t _meshes;

		material_indices_t _material_indices;

		model_meshes( const model_meshes& ) = delete;
		model_meshes& operator=( const model_meshes& ) = delete;
	};
}
