
#pragma once

class buffer_view_base;

namespace igpu
{
    class shader
    {
    public:
		
		virtual void map(size_t byte_size, buffer_view_base*) = 0;

		virtual void unmap() = 0;

		virtual ~shader() = default;
        
    protected:
        
		shader() = default;

    private:

		shader(const shader&) = delete;
		shader& operator= (const shader&) = delete;
    };
}
