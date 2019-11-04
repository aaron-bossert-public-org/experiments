
#pragma once

#include "igpu/buffer/topology.h"

#include <memory>
#include <string>
#include <vector>

#include <optional>

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
			size_t ibuff_byte_offset = 0;
			std::vector<size_t> vbuff_byte_offsets;
		};

		virtual ~geometry() = default;

		virtual const config& cfg() const = 0;

		virtual void base_vertex(ptrdiff_t) = 0;

		virtual void instance_start(size_t) = 0;

		virtual void instance_count(size_t) = 0;

		virtual void element_start(size_t) = 0;

		virtual void element_count(const std::optional < size_t >&) = 0;

		virtual ptrdiff_t base_vertex() const = 0;

		virtual size_t instance_start() const = 0;

		virtual size_t instance_count() const = 0;

		virtual size_t element_start() const = 0;

		virtual size_t element_count() const = 0;
        
		virtual const igpu::index_buffer& index_buffer() const = 0;

		virtual size_t vertex_buffer_count() const = 0;
		
		virtual const igpu::vertex_buffer& vertex_buffer(size_t) const = 0;

		bool find_expected_vertex_param(
			const std::string& name, 
			size_t* p_expected_buff, 
			size_t* p_expected_attr) const; 
    };
}
