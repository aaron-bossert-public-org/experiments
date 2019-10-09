
#include <gfx/mesh/model_geometry.h>

using namespace gfx

model_geometry::model_geometry(std::string file, mesh_geometries_t mesh_geometries, material_indices_t material_indices)
: _file(std::move(file))
, _mesh_geometries(std::move(mesh_geometries))
, _material_indices(std::move(material_indices))
{
}

std::unique_ptr<model_geometry> model_geometry::make(std::string file, mesh_geometries_t mesh_geometries)
{
    material_indices_t material_indices(mesh_geometries.size());
    
    for(int i = 0; i < material_indices.size(); i++)
    {
        material_indices[i] = i;
    }
    
    return make_welded(std::move(file), std::move(mesh_geometries), std::move(material_indices));
}

std::unique_ptr<model_geometry> model_geometry::make_welded(
    std::string file,
    mesh_geometries_t mesh_geometries,
    material_indices_t material_indices)
{
    ASSERT_CONTEXT(material_indices.size() == mesh_geometries.size());
    if(material_indices.size() != mesh_geometries.size()) return nullptr;
    
    return std::unique_ptr<model_geometry>( new model_geometry(std::move(file), std::move(mesh_geometries), std::move(material_indices)));
}

const std::string& model_geometry::file() const
{
    return _file;
}

const model_geometry::mesh_geometries_t& model_geometry::mesh_geometries() const
{
    return _mesh_geometries;
}

const model_geometry::material_indices_t& model_geometry::material_indices() const
{
    return _material_indices;
}
