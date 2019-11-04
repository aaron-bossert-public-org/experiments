

#include "gfx/perf/context/geometry_debugger.h"

#include "gfx/perf/context/draw_pass_modifier.h"

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
}

geometry_debugger_step::geometry_debugger_step(const std::string& description)
: _description(description)
{
}

const std::string& geometry_debugger_step::description() const
{
    return _description;
}

void geometry_debugger_step::frame_times(const std::vector<float>& frame_times)
{
    ASSERT_CONTEXT(_frame_times.empty(), "%s warning! stomped frame times!");
    _frame_times = frame_times;
}

const std::vector<float>& geometry_debugger_step::frame_times() const
{
    return _frame_times;
}

const std::string& geometry_debugger::description() const
{
    return _description;
}

void geometry_debugger::pre_render(draw_pass_modifier& draw_pass_modifier)
{
    if(_state == geometry_debugger_state::WAITING)
    {
        _state = geometry_debugger_state::STARTED;
        
        _steps = make_steps(draw_pass_modifier);
        
        _last_update_time = get_real_time_ms();
    }
    else if(!_steps.empty() && _current_step < (int)_steps.size())
    {
        double time = get_real_time_ms();
        float delta = time - _last_update_time;
        _last_update_time = time;
        
        _frame_times.emplace_back(delta);
        
        if(_frame_times.size() >= _frames_per_step)
        {
            // set base line or frame times with completed list of frame times
            if(-1 == _current_step)
            {
                _baseline = _frame_times;
            }
            else
            {
                _steps[_current_step]->frame_times(_frame_times);
            }
            
            // tear down current test and start the next one
            if(-1 < _current_step)
            {
                _steps[_current_step]->teardown_step(draw_pass_modifier);
            }
            
            int next_step = _current_step + 1;
            if(next_step < _steps.size())
            {
                _steps[next_step]->setup_step(draw_pass_modifier);
            }
        
            _frame_times.clear();
            ++_current_step;
        }
    }
    else
    {
        // debugger routine complete
        _state = geometry_debugger_state::COMPLETE;
        
        _callback(*this);
    }
}

geometry_debugger_state geometry_debugger::state() const
{
    return _state;
}

const std::vector<float>& geometry_debugger::baseline() const
{
    return _baseline;
}

const geometry_debugger::steps_t& geometry_debugger::steps() const
{
    return _steps;
}

geometry_debugger::geometry_debugger(const std::string& description, unsigned frames_per_step, const callback_t& callback)
: _description(description)
, _frames_per_step(frames_per_step)
, _callback(callback)
{
    ASSERT_CONTEXT(callback);
    _frame_times.reserve(_frames_per_step);
}
