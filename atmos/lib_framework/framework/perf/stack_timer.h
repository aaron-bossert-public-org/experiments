
#pragma once

#ifndef ATMOS_STACK_TIMERS
#define ATMOS_STACK_TIMERS 1
#endif

#if ATMOS_STACK_TIMERS

#	define STACK_TIMER_THREAD(name) \
static thread_callstack_tracker* ___tracker___ = stack_timer::aquire_tracker(name); stack_timer::begin_thread(___tracker___);

#	define STACK_TIMER(name) STACK_TIMER_ID(__COUNTER__, name, "")

#	define STACK_TIMER_PAIR(name, instance_desc) STACK_TIMER_ID(__COUNTER__, name, instance_desc)

#	define STACK_TIMER_START(token) \
static size_t ___##token##callsite_index___ = stack_timer::make_callsite(#token); stack_timer ___##token##timer___(___##token##callsite_index___);

#	define STACK_TIMER_END(token) \
___##token##timer___.end();

#	define STACK_TIMER_CODE(code) code
#	include <string>

class thread_callstack_tracker;

// provides measurement of time spent in a given scope, and inserts results into unique per thread tracker
class stack_timer
{
    const size_t _callsite_index;
    size_t _stack_entry_index;
    
public:
    
    stack_timer(size_t callsite_index, const std::string& instance_desc = "");
    
    ~stack_timer();
    
    void end();
    
    static thread_callstack_tracker* aquire_tracker(const std::string&);
    
    static void begin_thread(thread_callstack_tracker*);
    
    // thread specific: in debug builds implementation asserts this callsite is becomes active only on the thread it was made
    static size_t make_callsite(const std::string& name);
    
    static void next_frame();
    
    static void next_frame_after_this_callstack();
    
    static void force_frame_output();
};

// helper macros
#	define STACK_TIMER_ID( counter, name, instance_desc )\
static size_t STACK_TIMER_SMASH(___callsite_index___, counter) = stack_timer::make_callsite(name);\
stack_timer STACK_TIMER_SMASH(___scope_timer___, counter) (STACK_TIMER_SMASH(___callsite_index___, counter), instance_desc);

#	define STACK_TIMER_SMASH( x, y ) x##y

#else // ATMOS_STACK_TIMERS

#	define STACK_TIMER_THREAD(name);
#	define STACK_TIMER(name);
#	define STACK_TIMER_PAIR(name, instance_desc);
#	define STACK_TIMER_START(token);
#	define STACK_TIMER_END(token);
#	define STACK_TIMER_CODE(code)

#endif // ATMOS_STACK_TIMERS

