
#include "framework/logging/log.h"
#include "gfx/context/stream_output.h"
#include "framework/tweak/tweak.h"

using namespace gfx;

namespace
{
    float get_real_time_ms()
    {
        static std::chrono::high_resolution_clock clock;
        static std::chrono::high_resolution_clock::time_point start = clock.now();
        
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(clock.now() - start);
        
        return us.count() / 1000.0;
    }
    
    using recorder_complete_t = std::function<void(const std::vector<float>& game_tick_ms,
                                                  const std::vector<float>& render_thread_ms,
                                                  const std::vector<float>& polycount,
                                                  const std::vector<float>& program_count,
                                                  const std::vector<float>& program_texture_switches,
                                                  const std::vector<float>& program_parameter_switches,
                                                  const std::vector<float>& unique_pieces_of_geometry,
                                                  const std::vector<float>& draw_calls)>;
    
    void queue_fps_recorder(performance_instrument_scheduler* scheduler, int frame_count, const recorder_complete_t& recorder_complete)
    {
        struct measurements
        {
            float last_real_time = 0;
            int frame_index = 0;
            
            std::vector<float>
            game_tick_ms,
            render_thread_ms,
            polycount,
            program_count,
            program_texture_switches,
            program_parameter_switches,
            unique_pieces_of_geometry,
            draw_calls;
        };
        
        auto measurements = std::make_shared<measurements>();
        measurements->game_tick_ms.resize(frame_count);
        measurements->render_thread_ms.resize(frame_count);
        measurements->polycount.resize(frame_count);
        measurements->program_count.resize(frame_count);
        measurements->program_texture_switches.resize(frame_count);
        measurements->program_parameter_switches.resize(frame_count);
        measurements->unique_pieces_of_geometry.resize(frame_count);
        measurements->draw_calls.resize(frame_count);
        
        scheduler->queue([frame_count, recorder_complete, measurements]{
            
            const auto& perf_state = perf:current_state();
            
            int& frame_index = measurements->frame_index;
            
            if(frame_index < frame_count)
            {
                measurements->polycount[frame_index] = perf_state.last_capture(perf::category::POLY_COUNT);
                measurements->program_count[frame_index] = perf_state.last_capture(perf::category::SWITCH_SHADERS);
                measurements->program_texture_switches[frame_index] = perf_state.last_capture(perf::category::SWITCH_TEXTURES);
                measurements->program_parameter_switches[frame_index] = perf_state.last_capture(perf::category::SWITCH_PARAMETERS);
                measurements->unique_pieces_of_geometry[frame_index] = perf_state.last_capture(perf::category::SWITCH_GEOMETRY);
                measurements->draw_calls[frame_index] = perf_state.last_capture(perf::category::DRAW_CALL_COUNT);
                
                ++frame_index;
                
                return false;
            }
            
            recorder_complete(measurements->game_tick_ms,
                             measurements->render_thread_ms,
                             measurements->polycount,
                             measurements->program_count,
                             measurements->program_texture_switches,
                             measurements->program_parameter_switches,
                             measurements->unique_pieces_of_geometry,
                             measurements->draw_calls);
            
            return true;
        });
        
    }
}

void performance_instrument_scheduler::update()
{
    if(_steps.size())
    {
        const auto& step = _steps.front();
        
        if(step())
        {
            _steps.pop_front();
        }
    }
}

void performance_instrument_scheduler::queue(const instrument_step_t& step)
{
    if(step)
    {
        _steps.emplace_back(step);
    }
}

void performance_instrument_scheduler::queue_log(const std::string& log)
{
    queue([log]{ LOG_CONTEXT(INFORM, log.c_str()); return true; });
}

void performance_instrument_scheduler::queue_idle_frames(int count)
{
    auto frames_elapsed = std::make_shared<int>(0);
    
    queue([frames_elapsed, count]{
        
        if(*frames_elapsed < count)
        {
            ++*frames_elapsed;
			LOG_CONTEXT(INFORM, "performance_instrument_scheduler::add_idle_frame_steps : %d / %d", *frames_elapsed, count);
            return false;
        }
        
        return true;
    });
}

void performance_instrument_scheduler::queue_bench_mark(std::string desc, int frame_count, int frames_to_filter, benchmark_complete_t complete)
{
    
    queue_fps_recorder(this, frame_count, [desc, frames_to_filter, complete](const std::vector<float>& game_tick_ms,
                                                                        const std::vector<float>& render_thread_ms,
                                                                        const std::vector<float>& polycount,
                                                                        const std::vector<float>& program_count,
                                                                        const std::vector<float>& program_texture_switches,
                                                                        const std::vector<float>& program_parameter_switches,
                                                                        const std::vector<float>& unique_pieces_of_geometry,
                                                                        const std::vector<float>& draw_calls){
        
        benchmark game_tick_ms_benchmark(game_tick_ms, frames_to_filter);
        benchmark render_thread_ms_benchmark(render_thread_ms, frames_to_filter);
        
        benchmark_results results;
        
        results.game_tick_ms_avg = game_tick_ms_benchmark.avg();
        results.render_thread_ms_avg = render_thread_ms_benchmark.avg();
        results.polycount_avg = benchmark(polycount, frames_to_filter).avg();
        results.program_count_avg = benchmark(program_count, frames_to_filter).avg();
        results.program_texture_switches_avg = benchmark(program_texture_switches, frames_to_filter).avg();
        results.program_parameter_switches_avg = benchmark(program_parameter_switches, frames_to_filter).avg();
        results.unique_pieces_of_geometry_avg = benchmark(unique_pieces_of_geometry, frames_to_filter).avg();
        results.draw_calls_avg = benchmark(draw_calls, frames_to_filter).avg();
        
        std::ostringstream oss;
        
        oss << std::endl << "///////////////////////////////////////////////////////";
        oss << std::endl << "// benchmark results for: " << desc;
        oss << std::endl << "// game tick ms benchmark: " << game_tick_ms_benchmark;
        oss << std::endl << "// render thread ms benchmark: " << render_thread_ms_benchmark;
        oss << std::endl << "//";
        oss << std::endl << "// polycount: " << results.polycount_avg;
        oss << std::endl << "// program count: " << results.program_count_avg;
        oss << std::endl << "// texture switches: " << results.program_texture_switches_avg;
        oss << std::endl << "// program parameter switches: " << results.program_parameter_switches_avg;
        oss << std::endl << "// unique pieces of geometry: " << results.unique_pieces_of_geometry_avg;
        oss << std::endl << "// draw calls: " << results.draw_calls_avg;
        oss << std::endl << "///////////////////////////////////////////////////////";
        oss << std::endl;
        
		LOG_CONTEXT(INFORM, oss.str().c_str());
        
        if(complete)
        {
            complete(desc, results);
        }
    });
}

std::unique_ptr<performance_instrument_scheduler> performance_instrument_scheduler::make()
{
    return std::unique_ptr<performance_instrument_scheduler>(new performance_instrument_scheduler());
}

