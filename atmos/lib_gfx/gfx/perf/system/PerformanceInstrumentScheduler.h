
#pragma once

#include <functional>

class performance_instrument_scheduler
{
public:
    
    // step must return true when done
    using instrument_step_t = std::function<bool()>;
    
    using steps_t = std::list<instrument_step_t>;
    
public:
    
    struct msg
    {
        int delay = 0;
        float scale_poly_count = 1;
    };
    
    struct benchmark_results
    {
        float
        game_tick_ms_avg = 0.f,
        render_thread_ms_avg = 0.f,
        polycount_avg = 0.f,
        program_count_avg = 0.f,
        program_texture_switches_avg = 0.f,
        program_parameter_switches_avg = 0.f,
        unique_pieces_of_geometry_avg = 0.f,
        draw_calls_avg = 0.f;
    };
    
    using benchmark_complete_t = std::function<void (const std::string&, const benchmark_results&)>;
    
    void update();
    
    void queue(const instrument_step_t&);
    
    void queue_log(const std::string&);
    
    void queue_idle_frames(int);
    
    void queue_bench_mark(std::string desc, int frame_count, int frames_to_filter, benchmark_complete_t complete = nullptr);
    
    static std::unique_ptr<performance_instrument_scheduler> make();
    
private:
    
    performance_instrument_scheduler() = default;
    
private:
    steps_t _steps;
};

