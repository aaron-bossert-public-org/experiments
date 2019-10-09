
#include <gfx/perf/texture/replace_texture_2ds_by_size.h>

#include <igpu/material/parameter.h>
#include <igpu/texture/texture_2d.h>
#include <gfx/material/program_instance.h>

using namespace gfx;

replace_texture_2ds_by_size::draw_pass_args_t replace_texture_2ds_by_size::modify(draw_pass_args_t draw_pass_args)
{
    const auto& program = draw_pass_args.binding_args.program;
    const auto& original = draw_pass_args.binding_args.program_instance;
    
    auto& replaced_program_instance = _replaced_program_instances[original];
    
    if(!replaced_program_instance)
    {
        std::vector<std::pair<parameter_id, igpu::primitive>> primitives;
        
        for(const auto& handle : program->parameters())
        {
            const auto& name = handle->name();
            if(const igpu::primitive* primitive = original->primitive(name))
            {
                if(igpu::primitive::TYPE_TEXTURE_2D == primitive->get_type())
                {
                    std::shared_ptr<igpu::texture_2d> replacement;
                    for(const auto& pair : _replacements)
                    {
                        std::shared_ptr<igpu::texture_2d> replacement_candidate = pair.second;
                        
                        if(!replacement || replacement_candidate->res().x < primitive->get_texture_2d()->res().x)
                        {
                            // if this is the first time through this loop, take the texture without checking sizes
                            // or
                            // if the replacement candidate is smaller than the actual texture
                            //
                            //switch replacement to the candidate
                            replacement = replacement_candidate;
                        }
                        else
                        {
                            // if iterated to textures larger than the actual texture break and leave replacement to the last acceptable one
                            break;
                        }
                    }
                    
                    primitives.emplace_back(name, replacement);
                }
                else
                {
                    primitives.emplace_back(name, *primitive);
                }
            }
        }
        
        auto desc = "Replaced textures from: " + original->desc();
        
        auto parameter_block = std::make_shared<parameter_block>(primitives, true);
        
        replaced_program_instance = program_instance::make(desc, original->types(), original->program(), original->render_states(), parameter_block);
    }
    
    draw_pass_args.binding_args.program_instance = replaced_program_instance;
    
    return draw_pass_args;
}

std::shared_ptr<replace_texture_2ds_by_size> replace_texture_2ds_by_size::make(std::map<int, std::shared_ptr<igpu::texture_2d>> replacements)
{
    if(replacements.empty())
    {
        return nullptr;
    }
    
    for(const auto& pair : replacements)
    {
        if(!pair.second)
        {
            LOG_CONTEXT( CRITICAL, "texture for size(%d) is null", pair.first );
            return nullptr;
        }
    }

    return std::shared_ptr<replace_texture_2ds_by_size>(new replace_texture_2ds_by_size(std::move(replacements)));
}

replace_texture_2ds_by_size::replace_texture_2ds_by_size(std::map<int, std::shared_ptr<igpu::texture_2d>> replacements)
: _replacements(std::move(replacements))
{
}