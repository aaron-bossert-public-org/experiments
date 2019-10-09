
#pragma once

#include <memory>

namespace igpu
{
    class depth_buffer;
    class color_buffer;
    
    class back_buffer
    {
    public:
        
		struct config
		{
			std::shared_ptr<color_buffer> color;
			std::shared_ptr<depth_buffer> depth;
		};

        virtual const config& cfg() const = 0;
        
        static std::shared_ptr<back_buffer> instance();
        
    protected:
        
        back_buffer();
        
    private:
        
		back_buffer (const back_buffer&) = delete;
        back_buffer& operator= (const back_buffer&) = delete;
    };
}
