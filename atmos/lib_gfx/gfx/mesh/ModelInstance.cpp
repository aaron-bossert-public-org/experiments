
#include <gfx/mesh/model_instance.h>

#include <gfx/mesh/mesh_state.h>
#include <gfx/mesh/model_meshes.h>
#include <gfx/mesh/model_state.h>
#include <gfx/perf/context/draw_pass_modifier.h>
#include <gfx/context/draw_pass_bundle.h>

using namespace gfx

model_instance::model_instance(const std::shared_ptr<scene>& scene)
: _model_state(model_state::make(scene, [this](const glm::mat4x4& model_matrix){
    
        // respond to 'model mat' changes in model state
        const auto& meshes = _model_meshes->meshes();
        
        for (size_t i = 0; i < _draw_pass_bundles.size(); ++i)
        {
            sphere visibility_sphere = math::transform_sphere(meshes[i]->sphere(), model_matrix);
            _draw_pass_bundles[i]->mesh_state()->visibility_sphere(visibility_sphere);
        }
    }))
{
}

std::unique_ptr<model_instance> model_instance::make(const std::shared_ptr<scene>& scene)
{
    return std::unique_ptr<model_instance>( new model_instance(scene) );
}

void model_instance::set_model_meshes (std::shared_ptr<draw_pass_modifier> draw_pass_modifier,
                                    std::shared_ptr<model_meshes> model_meshes,
                                    technique_sets_t technique_sets)
{
    _model_meshes = std::move(model_meshes);
    
    if(_model_meshes)
    {
        const auto& material_indices = _model_meshes->material_indices();
        const auto& meshes = _model_meshes->meshes();
        
        ASSERT_CONTEXT(material_indices.size() == meshes.size());
        
        _draw_pass_bundles.resize( meshes.size() );
        
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            size_t material_index = material_indices[i];
            
            draw_pass_bundle::msg msg = {
                meshes[i],
                draw_pass_modifier,
                std::move(technique_sets[material_index]),
            };
            
            _draw_pass_bundles[i] = std::make_shared<draw_pass_bundle>(msg, _model_state);
        }
        
        _model_state->observers(_draw_pass_bundles);
    }
    else
    {
        _draw_pass_bundles.clear();
    }
}

const std::shared_ptr<model_state>& model_instance::model_state() const
{
    return _model_state;
}

void model_instance::add_draw_passes(const technique_sets_t& technique_sets)
{
    auto material_indices = _model_meshes->material_indices();
    
    if(material_indices.size() != _draw_pass_bundles.size())
    {
        LOG_CONTEXT( CRITICAL, "material_indices.size(%d) != _draw_pass_bundles.size(%d)",
                  material_indices.size(),
                  _draw_pass_bundles.size());
        return;
    }
    
    for( size_t i = 0; i < _draw_pass_bundles.size(); ++i )
    {
        size_t material_index = material_indices[i];
        
        if(material_index >= technique_sets.size())
        {
            LOG_CONTEXT( CRITICAL, "material_index(%d) >= technique_sets.size(%d)",
                      material_index,
                      technique_sets.size());
            return;
        }
        
        const auto& draw_pass_bundle = _draw_pass_bundles[ i ];
        const auto& technique_set = technique_sets[ material_index ];
        
        for( const auto& technique : technique_set )
        {
            draw_pass_bundle->add_pass(technique );
        }
    }
}

void model_instance::remove_draw_passes(const technique_sets_t& technique_sets)
{
    auto material_indices = _model_meshes->material_indices();
    
    if(material_indices.size() != _draw_pass_bundles.size())
    {
        LOG_CONTEXT( CRITICAL, "material_indices.size(%d) != _draw_pass_bundles.size(%d)",
                  material_indices.size(),
                  _draw_pass_bundles.size());
        return;
    }
    
    for( size_t i = 0; i < _draw_pass_bundles.size(); ++i )
    {
        size_t material_index = material_indices[i];
        
        if(material_index >= technique_sets.size())
        {
            LOG_CONTEXT( CRITICAL, "material_index(%d) >= technique_sets.size(%d)",
                      material_index,
                      technique_sets.size());
            return;
        }
        
        const auto& draw_pass_bundle = _draw_pass_bundles[ i ];
        const auto& technique_set = technique_sets[ material_index ];
        
        for( const auto& technique : technique_set )
        {
            draw_pass_bundle->remove_pass(technique);
        }
    }
}

const std::shared_ptr<model_meshes>& model_instance::model_meshes() const
{
    return _model_meshes;
}

const std::vector<std::shared_ptr<draw_pass_bundle>>& model_instance::draw_pass_bundles() const
{
    return _draw_pass_bundles;
}
