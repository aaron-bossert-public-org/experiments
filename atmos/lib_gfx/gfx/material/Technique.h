
#pragma once

namespace gfx
{
    class program_instance;
    
    class scene_stage;
    
    class technique;

    using technique_set_t = std::vector<technique>;
    
    using technique_sets_t = std::vector<technique_set_t>;
    
	class technique
    {
    public:
        
        static const std::string& default_stage_name( bool alpha );
        
    public:
        
        technique(const std::shared_ptr<program_instance>& program_instance,
                  size_t scene_stage_index,
                  std::string scene_stage_name);
        
        const std::shared_ptr<program_instance>& program_instance() const;
        
        size_t scene_stage_index() const;
        
        const std::string& scene_stage_name() const;
        
    private:
        
        std::shared_ptr<program_instance> _program_instance;
        size_t _scene_stage_index;
        std::string _scene_stage_name;
	};
}


