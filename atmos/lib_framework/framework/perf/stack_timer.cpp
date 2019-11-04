
#include "framework/perf/stack_timer.h"

#if ATMOS_STACK_TIMERS

#include "framework/logging/log.h"

#include <algorithm>
#include <vector>
#include <thread>

namespace
{
    // in milliseconds
    float s_minimum_scope_time = 0.1f;
    
    float s_trap_long_frame_threshold = 50;
    size_t s_profile_frequency = 300;
    
    // this is used for rapid indentation of text
    const char s_spaces[] = "| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | ";
    const size_t s_spaces_size = sizeof(s_spaces) / sizeof(s_spaces[0]) - 4;
    
    const char* get_indent(size_t stack_depth)
    {
        return s_spaces + s_spaces_size - stack_depth * 2;
    }
    
    double get_time_now()
    {
        static auto s_launch_time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        
        auto dt = now - s_launch_time;
        
        return std::chrono::duration<double, std::chrono::milliseconds::period>(dt).count();
    }
}

class thread_callstack_tracker;

class callsite_data
{
    std::string _name;
    thread_callstack_tracker& _thread_callstack_tracker;
    
public:
    
    callsite_data(const std::string& name, thread_callstack_tracker& thread_callstack_tracker)
    : _name(name)
    , _thread_callstack_tracker(thread_callstack_tracker)
    {
    }
    
    const std::string& name() const
    {
        return _name;
    }
    
    thread_callstack_tracker& thread_callstack_tracker() const
    {
        return _thread_callstack_tracker;
    }
};

std::vector<callsite_data> s_callsites;

class stack_entry
{
    const size_t _callsite_index;
    const size_t _stack_depth;
    const std::string _instance_desc;
    
    size_t _start_count = 0;
    double _start_time = 0;
    double _end_time = 0;
    double _tot_runtime = 0;
    double _min_runtime = 0;
    double _max_runtime = 0;
    
public:
    
    stack_entry(size_t callsite_index, size_t stack_depth, const std::string& instance_desc)
    : _callsite_index(callsite_index)
    , _stack_depth(stack_depth)
    , _instance_desc(instance_desc)
    {
    }
    
    size_t callsite_index() const
    {
        return _callsite_index;
    }
    
    size_t stack_depth() const
    {
        return _stack_depth;
    }
    
    double tot_runtime() const
    {
        return _tot_runtime;
    }
    
    void begin()
    {
        _start_count++;
        
        ASSERT_CONTEXT(0 == _start_time);
        _start_time = get_time_now();
    }
    
    void end()
    {
        double runtime = get_time_now() - _start_time;
        
#ifdef ATMOS_DEBUG
		ASSERT_CONTEXT(0 != _start_time);
        _start_time = 0;
#endif
        
        _tot_runtime += runtime;
        _min_runtime = std::min(_min_runtime, runtime);
        _max_runtime = std::max(_max_runtime, runtime);
    }
    
    void out_put(const std::string& callsite_name) const
    {
		double total     = _tot_runtime;
		double maximum   = _max_runtime;
		double minimum   = _min_runtime;
		double average   = _tot_runtime / _start_count;
        
        std::string suffix;
        
        if(_instance_desc.size())
        {
            suffix = "(" + _instance_desc + ")";
        }
        
        if(1 < _start_count)
        {
            suffix += " / " + std::to_string(_start_count);
            suffix += " avg:" + std::to_string(average);
            suffix += " max:" + std::to_string(maximum);
            suffix += " min:" + std::to_string(minimum);
        }
        
		LOG_DEBUG("%s%.2f : %s %s\n", get_indent(_stack_depth), total, callsite_name.c_str(), suffix.c_str() );
    }
};

class thread_callstack_tracker
{
    size_t _frame = 0;
    double _last_time = get_time_now();
    bool _force_output = false;
    bool _next_frame_after_this_callstack = false;
    
    std::string _thread_name;
    std::thread::id _thread_id;
    size_t _stack_depth = 0;
    
    std::vector<size_t> _last_entry_for_stack_depth;
    std::vector<stack_entry> _stack_entries;
    
#define MAX_THREADS 100
    
    static thread_callstack_tracker* s_trackers[MAX_THREADS];
    
public:
    
    thread_callstack_tracker()
    {
    }
    
    void thread_name(const std::string& thread_name)
    {
        _thread_name = thread_name;
    }
    
    const std::string& thread_name() const
    {
        return _thread_name;
    }
    
    void thread_id(std::thread::id thread_id)
    {
        _thread_id = thread_id;
    }
    
    std::thread::id thread_id() const
    {
        return _thread_id;
    }
    
    void clear()
    {
        ASSERT_CONTEXT(0 == _stack_depth);
        _stack_depth = 0;
        _last_entry_for_stack_depth.clear();
        _stack_entries.clear();
    }
    
    void next_frame()
    {
        _frame++;
        
        double current_time = get_time_now();
        
        double frame_time_milliseconds = (current_time - _last_time);
        bool traping_enabled = false;
        bool trap_sprung = false;
#if ATMOS_STACK_TIMERS
        traping_enabled = 0.f < s_trap_long_frame_threshold;
        trap_sprung = s_trap_long_frame_threshold < frame_time_milliseconds;
        
        bool is_outputting_performance_results = trap_sprung || _force_output;
        
        // prevent output from spiraling out of control
        if(_frame < 3)
        {
            is_outputting_performance_results = false;
        }
        
        // force output periodically
        if(_frame >= s_profile_frequency)
        {
            is_outputting_performance_results = true;
            _frame = 0;
        }
        
        
#else
        bool trap_active = false;
        bool is_outputting_performance_results = false;
#endif
        
        if(is_outputting_performance_results)
        {
            _force_output = false;
            
            // prevent output collisions between separate thread_callstack_trackers
                
			LOG_DEBUG("\n");
                
            if(trap_sprung)
            {
				LOG_DEBUG("||||||||||||||||||||| long frame trap sprung!!! - \n");
            }
                
			LOG_DEBUG(" %.2f - frame time (milliseconds)\n\n", float(frame_time_milliseconds) );
                
            output_results(frame_time_milliseconds);
        }
        
        clear();
        
        _last_time = get_time_now();
    }
    
    void output_results(double frame_time_milliseconds) const
    {
        const char* thread_name = "<un-named>";
        
        if(_thread_name.size())
        {
            thread_name = _thread_name.c_str();
        }
        
		LOG_DEBUG("%s@%s\n", get_indent(0), thread_name );
        
        // this is to colapse redundant function calls
        size_t last_stack_depth = 0;
        double untracked_frame_time_milliseconds = frame_time_milliseconds;
        
        for(const auto& stack_entry : _stack_entries)
        {
            size_t callsite_index = stack_entry.callsite_index();
            size_t stack_depth = stack_entry.stack_depth();
            
            if(s_minimum_scope_time > stack_entry.tot_runtime()) continue;
            
            if(0 == stack_depth) untracked_frame_time_milliseconds -= stack_entry.tot_runtime();
			
			ASSERT_CONTEXT(callsite_index < s_callsites.size());
            
            if(callsite_index < s_callsites.size())
            {
                if(last_stack_depth > stack_depth)
                {
					LOG_DEBUG("%s\n", get_indent(stack_depth) );
                }
                
                last_stack_depth = stack_depth;
                
                stack_entry.out_put(s_callsites[callsite_index].name());
            }
        }
        
		LOG_DEBUG("%s\n", get_indent(0) );
		LOG_DEBUG("%s%.2f / %.2f : %s untracked time \n\n\n", get_indent(0), untracked_frame_time_milliseconds, frame_time_milliseconds, thread_name );
    }
    
    size_t begin_callsite(size_t callsite_index, const std::string& instance_desc)
    {
		ASSERT_CONTEXT(callsite_index < s_callsites.size());
        
        // if not hitting the same callsite_index as the last time at this call stack depth,
        // clear records of the last callsites below this point in the stack, and make a new stack entry
        if(instance_desc.size() ||
           _stack_depth >= _last_entry_for_stack_depth.size() ||
           _stack_entries[_last_entry_for_stack_depth[_stack_depth]].callsite_index() != callsite_index)
        {
            _last_entry_for_stack_depth.resize(_stack_depth);
            _last_entry_for_stack_depth.push_back(_stack_entries.size());
            _stack_entries.push_back(stack_entry(callsite_index, _stack_depth, instance_desc));
        }
        
        size_t stack_entry_index = _last_entry_for_stack_depth[_stack_depth];
        _stack_entries[stack_entry_index].begin();
        
        _stack_depth++;
        return stack_entry_index;
    }
    
    void end_callsite(size_t stack_entry_index)
    {
        _stack_depth--;
		ASSERT_CONTEXT(_last_entry_for_stack_depth[_stack_depth] == stack_entry_index);
		ASSERT_CONTEXT(_stack_entries[_last_entry_for_stack_depth[_stack_depth]].stack_depth() == _stack_depth);
        
        _stack_entries[stack_entry_index].end();
        
        if(_next_frame_after_this_callstack && _stack_depth == 0)
        {
            _next_frame_after_this_callstack = false;
            next_frame();
        }
    }
    
    void next_frame_after_this_callstack()
    {
        _next_frame_after_this_callstack = true;
    }
    
    static thread_callstack_tracker* aquire_tracker()
    {
        const std::thread::id thread_id = std::this_thread::get_id();
        
        for(thread_callstack_tracker*& tracker : s_trackers)
        {
            if(!tracker)
            {
                tracker = new thread_callstack_tracker();
                tracker->thread_id(thread_id);
            }
            
            if(tracker->thread_id() == thread_id)
            {
                return tracker;
            }
        }
        
		LOG_CRITICAL( "ran out of thread callstack trackers, consider increasing the limit, or using another call timing mechanism.");
        
        return nullptr;
    }
    
    static void force_all_output_this_frame()
    {
        for(auto* tracker : s_trackers)
        {
            if(!tracker) return;
            
            tracker->_force_output = true;
        }
    }
};

stack_timer::stack_timer(size_t callsite_index, const std::string& instance_desc)
: _callsite_index(callsite_index)
{
    if(-1 == _callsite_index) return;
    
    // callsite index must be within valid range
	ASSERT_CONTEXT(callsite_index < s_callsites.size());
    
    // the callsite must not get hit from multiple threads!
    callsite_data& callsite_data = s_callsites[callsite_index];
	ASSERT_CONTEXT(callsite_data.thread_callstack_tracker().thread_id() == std::this_thread::get_id());
    
    _stack_entry_index = callsite_data.thread_callstack_tracker().begin_callsite(callsite_index, instance_desc);
}

stack_timer::~stack_timer()
{
    if(-1 == _callsite_index) return;
    
    if(_stack_entry_index != -1)
    {
        end();
    }
}

void stack_timer::end()
{
    if(-1 == _callsite_index) return;
    
	ASSERT_CONTEXT(_stack_entry_index != -1);
	ASSERT_CONTEXT(_callsite_index < s_callsites.size());
    
    const callsite_data& callsite_data = s_callsites[_callsite_index];
    
    // the callsite must not get hit from multiple threads
	ASSERT_CONTEXT(callsite_data.thread_callstack_tracker().thread_id() == std::this_thread::get_id());
    
    callsite_data.thread_callstack_tracker().end_callsite(_stack_entry_index);
    
    // prevent end slice from being called multiple times
    _stack_entry_index = (size_t)-1;
}

thread_callstack_tracker* stack_timer::aquire_tracker(const std::string& thread_name)
{
    thread_callstack_tracker* tracker = thread_callstack_tracker::aquire_tracker();
    
    // only aquire each tracker once this way
	ASSERT_CONTEXT(tracker->thread_name().empty());
    tracker->thread_name(thread_name);
    
    return tracker;
}

void stack_timer::begin_thread(thread_callstack_tracker* tracker)
{
    tracker->thread_id(std::this_thread::get_id());
}

size_t stack_timer::make_callsite(const std::string& name)
{
    if(thread_callstack_tracker* tracker = thread_callstack_tracker::aquire_tracker())
    {
        s_callsites.push_back(callsite_data(name, *tracker));
        
        return s_callsites.size() - 1;
    }
    
    return (size_t)-1;
}

thread_callstack_tracker* thread_callstack_tracker::s_trackers[100] = {0};

void stack_timer::next_frame()
{
    thread_callstack_tracker::aquire_tracker()->next_frame();
}

void stack_timer::next_frame_after_this_callstack()
{
    thread_callstack_tracker::aquire_tracker()->next_frame_after_this_callstack();
}

void stack_timer::force_frame_output()
{
    thread_callstack_tracker::force_all_output_this_frame();
}

#endif // STACK_TIMERS

