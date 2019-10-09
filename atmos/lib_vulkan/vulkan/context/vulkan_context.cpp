#include <vulkan/context/vulkan_context.h>

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/material/vulkan_program.h>
#include <vulkan/resource/internal/vulkan_resource_mediator.h>
#include <vulkan/resource/vulkan_compute_resource.h>
#include <vulkan/resource/vulkan_index_resource.h>
#include <vulkan/resource/vulkan_vertex_resource.h>
#include <vulkan/resource/vulkan_geometry.h>
#include <vulkan/window/vulkan_window.h>
// Vulkan implementation includes - end

using namespace igpu;

std::unique_ptr<vulkan_context> vulkan_context::make(const config& cfg)
{
	vulkan_resource_mediator::config resource_mediator_cfg;

#if ATMOS_DEBUG
	resource_mediator_cfg.enable_validation_layers = true;
#else 
	resource_mediator_cfg.enable_validation_layers = false;
#endif

	if (auto resource_mediator = vulkan_resource_mediator::make(resource_mediator_cfg))
	{
		return std::unique_ptr<vulkan_context>(
			new vulkan_context(
				cfg,
				std::move(resource_mediator)));
	}

	return nullptr;
}

std::unique_ptr<program> vulkan_context::make_program(
	const buffer_view<uint8_t>& vertex_code,
	const buffer_view<uint8_t>& pixel_code)
{
	return vulkan_program::make(
		this,
		vertex_code,
		pixel_code);
}

std::unique_ptr<geometry> vulkan_context::make_geometry(
	std::string name,
	topology topology,
	size_t element_start,
	size_t element_count,
	std::vector<std::shared_ptr<vertex_resource>> vertex_resources,
	std::shared_ptr<index_resource> index_resource)
{
	if (vertex_resources.empty())
	{
		LOG_CONTEXT(CRITICAL, "vertex_resources is empty");
	}
	else if (!index_resource)
	{
		LOG_CONTEXT(CRITICAL, "index_resource is null");
	}
	else
	{
		return vulkan_geometry::make(
			vertex_constraints(),
			std::move(name),
			topology,
			element_start,
			element_count,
			std::move(index_resource),
			std::move(vertex_resources));
	}

	return nullptr;
}

std::unique_ptr<vertex_resource> vulkan_context::make_resource(
	const vertex_resource::config& cfg)
{
	return _resource_mediator->make_resource(cfg);
}

std::unique_ptr<index_resource> vulkan_context::make_resource(
	const index_resource::config& cfg)
{
	return _resource_mediator->make_resource(cfg);
}

std::unique_ptr<compute_resource> vulkan_context::make_resource(
	const compute_resource::config& cfg)
{
	return _resource_mediator->make_resource(cfg);
}

const batch_constraints& vulkan_context::batch_constraints() const
{
	return _batch_constraints;
}

const material_constraints& vulkan_context::material_constraints() const
{
	return _material_constraints;
}

const vertex_constraints& vulkan_context::vertex_constraints() const
{
	return _vertex_constraints;
}

const window& vulkan_context::window() const
{
	return *_window;
}

vulkan_context::vulkan_context(
	const config& cfg,
	std::shared_ptr<vulkan_resource_mediator> resource_mediator)
	: _cfg(cfg)
	, _batch_constraints(cfg.batch_constraints)
	, _material_constraints(cfg.material_constraints)
	, _vertex_constraints(cfg.vertex_constraints)
	, _resource_mediator(std::move(resource_mediator))
	, _window(vulkan_window::make("vulkan", glm::ivec2(800, 600)))
#if ATMOS_PERFORMANCE_TRACKING
	, _renderstate_switch_metric(perf::category::SWITCH_RENDER_STATES, "Renderstates")
	, _draw_target_clears_metric(perf::category::CLEAR_DRAW_TARGET, "Draw Target Clears")
	, _draw_target_switch_metric(perf::category::SWITCH_DRAW_TARGET, "Draw Target Changes")
	, _shader_switch_metric(perf::category::SWITCH_SHADERS, "Shaders")
	, _geometry_switch_metric(perf::category::SWITCH_GEOMETRY, "geometry Count")
	, _draw_call_metric(perf::category::DRAW_CALL_COUNT, "Draw Calls")
	, _polycount_metric(perf::category::POLY_COUNT, "Polycount")
#endif
{
}

vulkan_context::~vulkan_context()
{
}
