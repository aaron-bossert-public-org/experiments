
#pragma once

namespace igpu
{
    void gl_event_marker(const char* message, size_t length);
    
    void gl_push_marker(const char* message, size_t length);
    
    void gl_pop_marker();    
}
