
#pragma once


namespace gfx
{
    enum class geometry_debugger_state
    {
        WAITING,
        STARTED,
        COMPLETE,
    };
    
    class draw_pass_modifier;
    
    class geometry_debugger_step
    {
    public:
        
        virtual ~geometry_debugger_step () {}
        
        virtual void setup_step(draw_pass_modifier&) const = 0;
        
        virtual void teardown_step(draw_pass_modifier&) const = 0;
        
    public:
        
		geometry_debugger_step(const std::string& description);
        
        const std::string& description() const;
        
        void frame_times(const std::vector<float>&);
        
        const std::vector<float>& frame_times() const;
        
    private:
        
        std::string _description;
        
        std::vector<float> _frame_times;
        
		geometry_debugger_step (const geometry_debugger_step&) = delete;
		geometry_debugger_step& operator= (const geometry_debugger_step&) = delete;
    };
    
    //--------------------------------------------------------------------------
    //
    //
    class geometry_debugger
    {
    public:
        
        using step_t = std::shared_ptr<geometry_debugger_step>;
        
        using steps_t = std::vector<step_t>;
        
        using callback_t = std::function<void(const geometry_debugger&)>;
        
    public:
        
        const std::string& description() const;
        
        void pre_render(draw_pass_modifier&);
        
		geometry_debugger_state state() const;
        
        const std::vector<float>& baseline() const;
        
        const steps_t& steps() const;
        
		geometry_debugger(const std::string& description, unsigned frames_per_step, const callback_t&);
        
        virtual steps_t make_steps(draw_pass_modifier&) = 0;
        
    private:
        
        std::string _description;

		geometry_debugger_state _state = geometry_debugger_state::WAITING;
        
        const unsigned _frames_per_step = 0;
        
        int _current_step = -1;
        
        double _last_update_time = 0.0;
        
        std::vector<float> _baseline;
        
        std::vector<float> _frame_times;
        
        steps_t _steps;
        
        callback_t _callback;
    };
}

