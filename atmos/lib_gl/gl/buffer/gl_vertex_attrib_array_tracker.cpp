
#include <gl/buffer/gl_vertex_attrib_array_tracker.h>


// GL implementation includes - begin
#include <gl/buffer/gl_geometry.h>
#include <gl/buffer/gl_vertex_buffer.h>
#include <gl/defines/gl_includes.h>
#include <gl/material/gl_program.h>
// GL implementation includes - end

using namespace igpu;

enum class gl_vertex_attrib_array_tracker::array_state
{
    DISABLED,
    ENABLED,
    STALE,
};

gl_vertex_attrib_array_tracker::gl_vertex_attrib_array_tracker()
{
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_max_vertex_attrib_index);
    _array_state.resize(_max_vertex_attrib_index);
    
    for(auto& state : _array_state)
    {
        state = array_state::DISABLED;
    }
}

gl_vertex_attrib_array_tracker::~gl_vertex_attrib_array_tracker()
{
}

int gl_vertex_attrib_array_tracker::max_vertex_attrib_index() const
{
    return _max_vertex_attrib_index;
}

void gl_vertex_attrib_array_tracker::state_caching_enabled(bool state_caching_enabled)
{
    // Enable state caching to avoid unnecessary calls to glVertexAttribPointer.
    // Disable state caching when there is a chance of contaminating the cached view of
    // glVertexAttribPointer state through external invocations of GL code (eg Scaleform).
    
    ASSERT_CONTEXT(_state_caching_enabled != state_caching_enabled);
    _state_caching_enabled = state_caching_enabled;
    
    if(_state_caching_enabled)
    {
        ASSERT_CONTEXT(_enabled_arrays.empty());
        ASSERT_CONTEXT(_next_enabled_arrays.empty());
        
        for( GLint index = 0; index < _max_vertex_attrib_index; ++index )
        {
            glDisableVertexAttribArray( index );
            ASSERT_CONTEXT(array_state::DISABLED == _array_state[index]);
        }
    }
    else
    {
        for(GLuint index : _enabled_arrays)
        {
            glDisableVertexAttribArray( index );
            _array_state[index] = array_state::DISABLED;
        }
        
        _enabled_arrays.clear();
        
#if ATMOS_DEBUG
        for( GLint index = 0; index < _max_vertex_attrib_index; ++index )
        {
            GLint enabled = false;
            glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
            ASSERT_CONTEXT(!enabled);
        }
#endif
    }
}

bool gl_vertex_attrib_array_tracker::state_caching_enabled() const
{
    return _state_caching_enabled;
}

void gl_vertex_attrib_array_tracker::set_active_vertex_buffers(const gl_geometry& geometry)
{
    for(const auto& vertex_buffer : geometry.cfg().vertex_buffers)
    {
		const gl_vertex_buffer* gl_v_buffer = (const gl_vertex_buffer*)vertex_buffer.get();
        glBindBuffer(GL_ARRAY_BUFFER, gl_v_buffer->gl_handle());

		for (const gl_vertex_attribute& attribute : gl_v_buffer->gl_format().attributes())
		{
			if (!_state_caching_enabled)
			{
				glEnableVertexAttribArray(attribute.location);
			}
			else
			{
				array_state& array_state = _array_state[attribute.location];

				if (array_state::DISABLED == array_state)
				{
					glEnableVertexAttribArray(attribute.location);
				}

				array_state = array_state::ENABLED;
				_next_enabled_arrays.push_back(attribute.location);
			}

			// Tracking and avoiding redundantly setting this state is not effective because it
			// binds the current GL_ARRAY_BUFFER to the given attribute pointer index.
			glVertexAttribPointer(
				attribute.location,
				attribute.size,
				attribute.type,
				GL_FALSE/*normalized*/,
				gl_v_buffer->cfg().format.stride,
				attribute.offset + (char*)nullptr);
		}
	}

    if(_state_caching_enabled)
    {
        // Disable elements that were left in enabled state after previous call to this function
        for(GLint index : _enabled_arrays)
        {
            if(array_state::STALE == _array_state[index])
            {
                glDisableVertexAttribArray( index );
                _array_state[index] = array_state::DISABLED;
            }
        }

		// Mark elements for disable so they can be disabled if not enabled after the next call to this function
		for (size_t index : _next_enabled_arrays)
		{
			_array_state[index] = array_state::STALE;
		}

        _enabled_arrays.swap(_next_enabled_arrays);
        _next_enabled_arrays.clear();
    }
}
