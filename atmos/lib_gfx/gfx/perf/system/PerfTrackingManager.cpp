
#include <framework/logging/log.h>
#include <framework/tweak/tweak.h>

#include <gfx/perf/context/draw_pass_modifier.h>
#include <gfx/perf/context/perf_test_draw_pass_modifier.h>
#include <gfx/scene/general_render_stage.h>
#include <gfx/scene/scene.h>
#include <gfx/scene/sorted_render_stage.h>
#include <gfx/context/stream_output.h>


using namespace gfx;

namespace
{
    static SETTING_BOOL(profile_batching_system_on_startup, false, "Profile batching system on startup");
    
    void enable_stages(bool batching_stage, bool z_sorted_stage, bool cast_shadow_stage)
    {
        auto scene_handle = scene_handle::game_world();
        
        scene_handle->dispatch_async([](const std::shared_ptr<scene> target,
                                      bool batching_stage,
                                      bool z_sorted_stage,
                                      bool cast_shadow_stage) {
            
            for(const auto& scene_stage : target->stages())
            {
                if(scene_stage->msg().name() == "cast_shadow")
                {
                    scene_stage->disabled_by_profiler(!cast_shadow_stage);
                }
                else if(dynamic_cast<general_render_stage*>(scene_stage.get()))
                {
                    scene_stage->disabled_by_profiler(!batching_stage);
                }
                else if(dynamic_cast<sorted_render_stage*>(scene_stage.get()))
                {
                    scene_stage->disabled_by_profiler(!z_sorted_stage);
                }
                else
                {
                    scene_stage->disabled_by_profiler(false);
                }
            }
            
        }, batching_stage, z_sorted_stage, cast_shadow_stage);
    }
    
#if ALLOW_DYNAMIC_TWEAKIN

	TWEAK_VARIABLE_WITH_CALLBACK(perf_render_manager_submit, true, "toggle render_manager::submit, which will affects updating render thread data, and producing the next frme")
    {
        perf_tracking_manager* ptm = ???;
        
        auto config = ptm->config();
        
        config.render_manager_submit = perf_render_manager_submit;
        
        ptm->config(config);
    }
    
    bool s_toggle_batching_stage = true;
    bool s_toggle_sorted_render_stage = true;
    bool s_toggle_cast_shadow_render_stage = true;
    
    TWEAK_CALLBACK(perf_toggle_batching_render_stage, "Toggle rendering of the contents of batching render stages")
    {
        s_toggle_batching_stage = !s_toggle_batching_stage;
        std::cout << "s_toggle_batching_stage : " << s_toggle_batching_stage << std::endl;
        
        enable_stages(s_toggle_batching_stage, s_toggle_sorted_render_stage, s_toggle_cast_shadow_render_stage);
    }
    
    TWEAK_CALLBACK(perf_toggle_sorted_render_stage, "When triggered repeatedly, will toggle rendering of the contents of Z sorted render stages")
    {
        s_toggle_zSorted_render_stage = !s_toggle_zSorted_render_stage;
        std::cout << "s_toggle_zSorted_render_stage : " << s_toggle_zSorted_render_stage << std::endl;
        
        enable_stages(s_toggle_batching_stage, s_toggle_sorted_render_stage, s_toggle_cast_shadow_render_stage);
    }
    
    TWEAK_CALLBACK(perf_toggle_shadow_render_stage, "Toggle rendering of the contents of shadow render stage")
    {
        s_toggle_cast_shadow_render_stage = !s_toggle_cast_shadow_render_stage;
        std::cout << "s_toggle_cast_shadow_render_stage : " << s_toggle_cast_shadow_render_stage << std::endl;
        
        enable_stages(s_toggle_batching_stage, s_toggle_sorted_render_stage, s_toggle_cast_shadow_render_stage);
    }

    TWEAK_CALLBACK(profile_batching_system, "Profile batching system")
    {
        perf_tracking_manager* ptm = ???;
        
        perf_tracking_manager::profile_config profile_config;
        
        profile_config.frames_per_test = 100;
        profile_config.frames_to_filter = 30;
        ptm->batch_system_profile(profile_config);
    }
    
#endif
    
    const int s_min_texture_size = 2;
    const int s_max_texture_size = 2048;
    
    std::map<int, render_instance<igpu::texture_2d>> replacement_texture_ri_for_config(const perf_test_config::replace_texture_config& config,
                                                                                 std::map<int, std::shared_ptr<texture_handle>>& replacement_textures)
    {
        std::map<int, render_instance<igpu::texture_2d>> replacement_texture_ri;
        
        if(config.replace_textures)
        {
            const int min = s_min_texture_size;
            const int max = std::max(s_max_texture_size >> config.reduction_level_pow2, min);
            const std::string suffix = "compressed.pvr";
            
            for(int size = max; size >= min; size = size >> 1)
            {
                auto texture_name = std::to_string(size) + suffix;
                if(auto texture_handle = get_resource<texture_handle>(texture_name))
                {
                    replacement_textures[size] = texture_handle;
                    replacement_texture_ri[size] = texture_handle->get_render_instance();
                }
            }
            
            ASSERT_CONTEXT(!replacement_texture_ri.empty());
        }
        
        return replacement_texture_ri;
    }
}

//******************************************************************************
#pragma mark - perf_tracking_manager
//******************************************************************************

//------------------------------------------------------------------------------
void perf_tracking_manager::make(const moby::prius<IManager>& prius)
{
    _scheduler = performance_instrument_scheduler::make();
    _config.reset(new perf_test_config);
    
    profile_config profile_config;
    
    // allow game to spool up particle emitters, animations, etc...
    profile_config.delay = 350;
    profile_config.frames_per_test = 100;
    profile_config.frames_to_filter = 30;
    
    if(profile_batching_system_on_startup)
    {
        batch_system_profile(profile_config);
    }
}

//------------------------------------------------------------------------------
void perf_tracking_manager::batch_system_profile(const profile_config& profile_config)
{
    _scheduler->queue_idle_frames(profile_config.delay);
    
    _scheduler->queue_log("\n"
                         "///////////////////////////////////////////////////////////\n"
                         "// begin sequencial reduction\n"
                         "///////////////////////////////////////////////////////////\n");
    
    
    // continually adjust this config to reduce the rendering workload of the scene in a sequencial way
    perf_test_config config;
    
    config.scene_parameter_updates = false;
    config.repeat_rendering_instructions = profile_config.frames_per_test;
    
    _scheduler->queue([this, config]{
        
        this->config(config);
        
        return true;
    });
    _scheduler->queue_bench_mark("Setup: switch to rendering batching system offscreen in a tight loop", profile_config.frames_per_test, profile_config.frames_to_filter);
    
    config.scene_parameter_updates = false;
    _scheduler->queue([this, config]{
        
        this->config(config);
        
        return true;
    });
    _scheduler->queue_bench_mark("disable scene parameter updates", profile_config.frames_per_test, profile_config.frames_to_filter);
    
    struct results
    {
        float begin_ms = 0, end_ms = 0, prev_ms = 0;
        
        std::multimap<float, std::string> reductions;
    };
    
    auto results = std::make_shared<results>();
    
    auto init_results = [results](const std::string&, const performance_instrument_scheduler::benchmark_results& benchmark_results) {
        
        results->begin_ms =
        results->end_ms =
        results->prev_ms =
        benchmark_results.render_thread_ms_avg;
    };
    
    auto capture_result = [results](const std::string& benchmark_desc, const performance_instrument_scheduler::benchmark_results& benchmark_results) {
        
        float reduction = 0;
        
        if(results->prev_ms > benchmark_results.render_thread_ms_avg)
        {
            reduction = results->prev_ms - benchmark_results.render_thread_ms_avg;
            results->prev_ms = benchmark_results.render_thread_ms_avg;
            results->end_ms = benchmark_results.render_thread_ms_avg;
        }
        
        results->reductions.emplace(-reduction, benchmark_desc);
    };
    
    _scheduler->queue_bench_mark("Benchmark baseline", profile_config.frames_per_test, profile_config.frames_to_filter, init_results);

    for(int step = 3; step --> 1;)
    {
        config.view_port_scale = step / 3.f;
        
        _scheduler->queue([this, config]{

            this->config(config);

            return true;
        });
        
        std::string view_desc = "scale view to " + std::to_string(config.view_port_scale);
        _scheduler->queue_bench_mark(view_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }

    for(int step = 3; step --> 0;)
    {
        config.polycount_scale = step / 3.f;
        
        _scheduler->queue([this, config]{
            
            this->config(config);
            
            return true;
        });
        
        std::string polycount_desc = "scale polycount to ~" + std::to_string(config.polycount_scale);
        _scheduler->queue_bench_mark(polycount_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }
    
    for(int step = 3; step --> 0;)
    {
        config.batching_config.material_texture_scale = step / 3.f;
        
        _scheduler->queue([this, config]{
            
            this->config(config);
            
            return true;
        });
        
        std::string material_texture_desc = "material texture count reduction factor ~" + std::to_string(config.batching_config.material_texture_scale);
        _scheduler->queue_bench_mark(material_texture_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }

    for(int step = 3; step --> 0;)
    {
        config.batching_config.material_parameter_scale = step / 3.f;
        
        _scheduler->queue([this, config]{
            
            this->config(config);
            
            return true;
        });
        
        std::string material_parameter_desc = "material parameter count reduction factor ~" + std::to_string(config.batching_config.material_parameter_scale);
        _scheduler->queue_bench_mark(material_parameter_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }
    
    for(int step = 3; step --> 0;)
    {
        config.batching_config.program_count_scale = step / 3.f;
        
        _scheduler->queue([this, config]{
            
            this->config(config);
            
            return true;
        });
        
        std::string program_desc = "program count reduction factor ~" + std::to_string(config.batching_config.program_count_scale);
        _scheduler->queue_bench_mark(program_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }
    
    for(int step = 3; step --> 0;)
    {
        config.batching_config.vertex_resource_count_scale = step / 3.f;
        
        _scheduler->queue([this, config]{
            
            this->config(config);
            
            return true;
        });
        
        std::string geometry_desc = "vertex resource count reduction factor ~" + std::to_string(config.batching_config.vertex_resource_count_scale);
        _scheduler->queue_bench_mark(geometry_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }
    
    for(int step = 3; step --> 0;)
    {
        config.batching_config.draw_count_scale = step / 3.f;
        
        _scheduler->queue([this, config]{
            
            this->config(config);
            
            return true;
        });
        
        std::string drawcount_desc = "draw call reduction factor ~" + std::to_string(config.batching_config.draw_count_scale);
        _scheduler->queue_bench_mark(drawcount_desc, profile_config.frames_per_test, profile_config.frames_to_filter, capture_result);
    }
    
    _scheduler->queue([this, results]{
        
		LOG_CONTEXT(INFORM, "finished perf_tracking_manager::sequencial_reduction_profile, frame time reductions:");
        
        float total_reduction = results->begin_ms - results->end_ms;
        
		LOG_CONTEXT(INFORM, "Removed %3.2fms of %3.2fms", total_reduction, results->begin_ms);
        
        for(const auto& pair : results->reductions)
        {
			LOG_CONTEXT(INFORM, "%2.2f%% : %s", 100.f * -pair.first / total_reduction, pair.second.c_str());
        }
        
        this->config({});
        
        return true;
    });
}

void perf_tracking_manager::destroy()
{
}

const scalar_performance_data& perf_tracking_manager::get_fps() const
{
    return _fps_data;
}

f32_t perf_tracking_manager::get_fps_average()
{
    return _fps_data.get_average();
}

void perf_tracking_manager::record_frame_time(const f64_t unthrottled_dt)
{
    if (unthrottled_dt <= 0.0)
    {
        return;
    }
    
    f64_t current_time = get_real_time();
    f64_t fps = 1.0f / unthrottled_dt;
    
    _fps_data.update(fps, current_time);
    
    _scheduler->update();
}

const std::shared_ptr<performance_instrument_scheduler>& perf_tracking_manager::scheduler() const
{
    return _scheduler;
}

void perf_tracking_manager::config(const perf_test_config& config)
{
    *_config = config;
    _replacement_textures.clear();
    
    const auto& scene_handle = render_manager->game_world_scene();
    const auto& view_state_handle = scene_handle->view_state_handle();
    const auto& view_frustum = view_state_handle->view_state().frustum();
    
    view_state_handle->config(view_state_handle->config());
    
    scene_handle->scene_parameters_manager_handle()->enable_scene_parameter_updates(config.scene_parameter_updates);
    
    auto replacement_texture_ri = replacement_texture_ri_for_config(config.replace_texture_config, _replacement_textures);
    
    render_manager->draw_pass_modifier_handle()->dispatch_async([](const std::shared_ptr<draw_pass_modifier> target,
                                                              const perf_test_config& config,
                                                              const frustum& view_frustum,
                                                              const std::map<int, render_instance<igpu::texture_2d>>& replacement_texture_ri) {
        
        std::map<int, std::shared_ptr<igpu::texture_2d>> replacement_textures;
        
        for(const auto& pair : replacement_texture_ri)
        {
            if(pair.second.target_exists())
            {
                replacement_textures[pair.first] = pair.second.resolve_target();
            }
        }
        
        target->stage_binding_modifier(perf_test_draw_pass_modifier::make_stage_binding_modifier(config, view_frustum, replacement_textures));
        
    }, config, view_frustum, replacement_texture_ri);
    
    enable_stages(config.batching_render_stages, config.z_sorted_render_stages, config.cast_shadow_render_stage);

    tweaker_render_perf_requests::geometry_enabled(config.draw_static_meshes, config.draw_dynamic_meshes);
}

const perf_test_config& perf_tracking_manager::config() const
{
    return *_config;
}
