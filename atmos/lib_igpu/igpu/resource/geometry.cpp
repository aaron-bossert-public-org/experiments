
#include <igpu/resource/geometry.h>

using namespace igpu;

geometry::~geometry()
{
}

const std::string& geometry::name() const
{
	return _name;
}

topology geometry::topology() const
{
	return _topology;
}

size_t geometry::element_start() const
{
	return _element_start;
}

void geometry::element_start(size_t element_start)
{
	_element_start = element_start;
}

size_t geometry::element_count() const
{
	return _element_count;
}

void geometry::element_count(size_t element_count)
{
	_element_count = element_count;
}

const std::vector<std::shared_ptr<vertex_resource>>& geometry::vertex_resources() const
{
	return _vertex_resources;
}

const std::shared_ptr<index_resource>& geometry::index_resource() const
{
	return _index_resource;
}

geometry::geometry(
	std::string name,
	igpu::topology topology,
	size_t element_start,
	size_t element_count,
	std::shared_ptr<igpu::index_resource> index_resource,
	std::vector<std::shared_ptr<vertex_resource>> vertex_resources)
	: _name(std::move(name))
	, _topology(topology)
	, _element_start(element_start)
	, _element_count(element_count)
	, _index_resource(std::move(index_resource))
	, _vertex_resources(std::move(vertex_resources))
{
}
