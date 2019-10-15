
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
			const std::string name;
			const igpu::topology topology;
			std::shared_ptr<igpu::index_buffer> index_buffer;
			std::vector<std::shared_ptr<vertex_buffer>> vertex_buffers;
		};

		virtual ~geometry() {}

		virtual const config& cfg() const = 0;
        
		virtual size_t element_start() const = 0;

		virtual size_t element_count() const = 0;

		virtual void element_start(size_t) = 0;

		virtual void element_count(size_t) = 0;

    protected:
        
		geometry() = default;

	private:

        geometry (const geometry&) = delete;
        geometry& operator= (const geometry&) = delete;
    };
}
