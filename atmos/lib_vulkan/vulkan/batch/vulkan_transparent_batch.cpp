
// #include <vulkan/batch/vulkan_transparent_batch.h>

// // Vulkan implementation includes - begin
// #include <vulkan/context/vulkan_context.h>
// // Vulkan implementation includes - end

// #include <igpu/buffer/geometry.h>
// #include <framework/logging/log.h>
// #include <algorithm>

// using namespace igpu;

// namespace
// {
//     using sortable_t = vulkan_transparent_batch::sortable_t;
//     using sorted_t   = vulkan_transparent_batch::sorted_t;
//     using binding_t  = vulkan_transparent_batch::binding_t;
//     using bindings_t = vulkan_transparent_batch::bindings_t;
    
//     void sort_bindings(sorted_t& out_sorted)
//     {
//         auto predicate = [](sortable_t& a, sortable_t& b)
//         {
//             return a.first > b.first;
//         };
        
//         std::sort(out_sorted.begin(), out_sorted.end(), predicate);
//     }
    
//     void visible_bindings(
// 		sorted_t& out_visible,
//         bindings_t& bindings,
//         const utility::frustum& frustum)
//     {
//         unsigned vis_count = 0;
//         out_visible.resize(bindings.size());
        
//         for(const auto& pair : bindings)
//         {
// 			const vulkan_transparent_batch::binding* binding = pair.second.get();
// 			const utility::sphere& sphere = binding->visibility_sphere();
            
// 			if (utility::intersects(frustum, sphere))
//             {
//                 out_visible[vis_count].first = utility::dist(frustum.near, sphere.center) - sphere.radius;
//                 out_visible[vis_count].second = binding;
//                 ++vis_count;
//             }
//         }
        
//         out_visible.resize(vis_count);
        
//         sort_bindings(out_visible);
//     }
// }

// vulkan_transparent_batch::binding::binding(
// 	vulkan_transparent_batch* vulkan_transparent_batch,
// 	const instance_batch::config& cfg,
// 	const utility::sphere& visibility_sphere)
// 	: batch_binding(
// 		cfg,
// 		visibility_sphere)
// 	, _vulkan_transparent_batch(vulkan_transparent_batch)
// {
// 	ASSERT_CONTEXT(_vulkan_transparent_batch);
// }

// vulkan_transparent_batch::binding::~binding()
// {
// 	if (_vulkan_transparent_batch)
// 	{
// 		LOG_CONTEXT(
// 			CRITICAL,
// 			"%s was destroyed without being unbatched!",
// 			cfg().geometry->name().c_str());
// 	}
// }

// std::shared_ptr<batch_binding> vulkan_transparent_batch::make_binding(
// 	const instance_batch::config& cfg,
// 	const utility::sphere& visibility_sphere)
// {
//     auto binding = std::make_shared<vulkan_transparent_batch::binding>(
// 		this, 
// 		cfg,
// 		visibility_sphere);
    
//     if(binding)
//     {
//     	_bindings[binding.get()] = binding;
//     }
    
//     return binding;
// }

// void vulkan_transparent_batch::render(const utility::frustum& frustum)
// {
//     visible_bindings(_sorted_visible_bindings, _bindings, frustum);
    
// 	_vulkan_context->begin_batch(this);
// 	ASSERT_CONTEXT(false && "not implemented");
// 	//tbd pass/program/material/geometry setup and caching
//     //for (auto& pair : _sorted_visible_bindings)
//     //{
//     //    const vulkan_transparent_batch::binding* binding = pair.second;
//     //}
// 	_vulkan_context->end_batch(this);
// }

// bool vulkan_transparent_batch::empty() const
// {
// 	return _bindings.empty();
// }

// vulkan_transparent_batch::~vulkan_transparent_batch()
// {}

// void vulkan_transparent_batch::unbind(const binding* binding)
// {
//     auto find = _bindings.find(binding);
//     if(find == _bindings.end())
//     {
//         LOG_CRITICAL( CRITICAL, "cannot find binding to unbind" );
//     }
//     else
//     {
//         _bindings.erase(find);
//     }
// }

// void vulkan_transparent_batch::binding::unbind()
// {
//     _vulkan_transparent_batch->unbind(this);
//     _vulkan_transparent_batch = nullptr;
// }

// std::unique_ptr<vulkan_transparent_batch> vulkan_transparent_batch::make(
// 	std::shared_ptr<batch_parameters> parameters, 
// 	vulkan_context* vulkan_context)
// {
// 	return std::unique_ptr<vulkan_transparent_batch>(
// 		new vulkan_transparent_batch(
// 			std::move(parameters),
// 			vulkan_context));
// }

// vulkan_transparent_batch::vulkan_transparent_batch(
// 	std::shared_ptr<batch_parameters> parameters, 
// 	vulkan_context* vulkan_context)
// 	: transparent_batch(std::move(parameters))
// 	, _vulkan_context(vulkan_context)
// {
// }