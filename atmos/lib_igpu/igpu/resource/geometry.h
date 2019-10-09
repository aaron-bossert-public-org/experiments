
#pragma once

#include <igpu/resource/topology.h>

#include <memory>
#include <string>
#include <vector>

namespace igpu
{
    class index_resource;
    class vertex_resource;
    
    class geometry
    {
    public:
        
        virtual ~geometry();

		const std::string& name() const;

		topology topology() const;
        
		void element_start(size_t);

		void element_count(size_t);

		const std::shared_ptr<index_resource>& index_resource() const;

		const std::vector<std::shared_ptr<vertex_resource>>& vertex_resources() const;

		size_t element_start() const;

		size_t element_count() const;

    protected:
        
        geometry(
			std::string name,
			igpu::topology,
			size_t element_count,
			size_t element_start,
			std::shared_ptr<igpu::index_resource>,
			std::vector<std::shared_ptr<vertex_resource>>);

	private:

		const std::string _name;
		const igpu::topology _topology;
		size_t _element_start;
		size_t _element_count;
		std::shared_ptr<igpu::index_resource> _index_resource;
		std::vector<std::shared_ptr<vertex_resource>> _vertex_resources;

        geometry (const geometry&) = delete;
        geometry& operator= (const geometry&) = delete;
    };
}
