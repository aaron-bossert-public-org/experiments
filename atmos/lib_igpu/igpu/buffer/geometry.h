
#pragma once

#include <igpu/buffer/topology.h>

#include <memory>
#include <string>
#include <vector>

namespace igpu
{
    class index_buffer;
    class vertex_buffer;
    
    class geometry
    {
    public:
        
		struct config
		{
			std::string name;
			igpu::topology topology;
			std::shared_ptr<igpu::index_buffer> index_buffer;
			std::vector<std::shared_ptr<vertex_buffer>> vertex_buffers;
		};

		virtual ~geometry() = default;

		virtual const config& cfg() const = 0;
        
		virtual const igpu::index_buffer& index_buffer() const = 0;

		virtual size_t vertex_buffer_count() const = 0;
		
		virtual const igpu::vertex_buffer& vertex_buffer(size_t) const = 0;

		bool find_expected_vertex_param(
			const std::string& name, 
			size_t* p_expected_buff, 
			size_t* p_expected_attr) const; 

    protected:
        
		geometry() = default;

	private:

        geometry (const geometry&) = delete;
        geometry& operator= (const geometry&) = delete;
    };
}
