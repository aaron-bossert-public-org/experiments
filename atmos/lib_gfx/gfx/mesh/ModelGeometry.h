
#pragma once


namespace gfx
{
	class mesh_geometry;
	class program_instance;

	class model_geometry
	{
	public:
        
        using mesh_geometry_t = std::shared_ptr< mesh_geometry >;
        using mesh_geometries_t = std::vector< mesh_geometry_t >;
        using material_indices_t = std::vector< size_t >;
        
    public:
        
        const std::string& file() const;
        
        const mesh_geometries_t& mesh_geometries() const;
        
        const material_indices_t& material_indices() const;
        
	public:
        
        static std::unique_ptr<model_geometry> make(std::string file, mesh_geometries_t);
        
        static std::unique_ptr<model_geometry> make_welded(std::string file, mesh_geometries_t, material_indices_t);
        
	private:
        
        model_geometry(std::string file, mesh_geometries_t, material_indices_t);

	private:
        
        std::string _file;
        
		mesh_geometries_t _mesh_geometries;
	
        material_indices_t _material_indices;
        
		model_geometry (const model_geometry&) = delete;
		model_geometry& operator= (const model_geometry&) = delete;
	};
}
