//
//#include "gl/batch/gl_transparent_batch.h"
//
//#include "gl/context/gl_context.h"
//
//#include "igpu/buffer/geometry.h"
//#include "framework/logging/log.h"
//#include <algorithm>
//
// using namespace igpu;
//
// namespace
//{
//    using sortable_t = gl_transparent_batch::sortable_t;
//    using sorted_t   = gl_transparent_batch::sorted_t;
//    using binding_t  = gl_transparent_batch::binding_t;
//    using bindings_t = gl_transparent_batch::bindings_t;
//
//    void sort_bindings(sorted_t& out_sorted)
//    {
//        auto predicate = [](sortable_t& a, sortable_t& b)
//        {
//            return a.first > b.first;
//        };
//
//        std::sort(out_sorted.begin(), out_sorted.end(), predicate);
//    }
//
//    void visible_bindings(
//		sorted_t& out_visible,
//        bindings_t& bindings,
//        const utility::frustum& frustum)
//    {
//        unsigned vis_count = 0;
//        out_visible.resize(bindings.size());
//
//        for(const auto& pair : bindings)
//        {
//			const gl_transparent_batch::binding* binding = pair.second.get();
//			const utility::sphere& sphere = binding->visibility_sphere();
//
//			if (utility::intersects(frustum, sphere))
//            {
//                out_visible[vis_count].first = utility::dist(frustum.near,
//                sphere.center) - sphere.radius; out_visible[vis_count].second
//                = binding;
//                ++vis_count;
//            }
//        }
//
//        out_visible.resize(vis_count);
//
//        sort_bindings(out_visible);
//    }
//}
//
// const gl_transparent_batch::binding::config&
// gl_transparent_batch::binding::cfg() const
//{
//	return _cfg;
//}
//
// gl_transparent_batch::binding::binding(
//	gl_transparent_batch* gl_transparent_batch,
//	const instance_batch::config& cfg,
//	const utility::sphere& visibility_sphere)
//	: batch_binding(
//		cfg,
//		visibility_sphere)
//	, _cfg(cfg)
//	, _gl_transparent_batch(gl_transparent_batch)
//{
//	ASSERT_CONTEXT(_gl_transparent_batch);
//}
//
// gl_transparent_batch::binding::~binding()
//{
//	if (_gl_transparent_batch)
//	{
//		LOG_CRITICAL(
//			"%s was destroyed without being unbatched!",
//			cfg().geometry->cfg().name.c_str());
//	}
//}
//
// std::unique_ptr<batch_binding> gl_transparent_batch::make_binding(
//	const instance_batch::config& cfg,
//	const utility::sphere& visibility_sphere)
//{
//	auto binding = std::shared_ptr<gl_transparent_batch::binding>(
//		new gl_transparent_batch::binding(
//			this,
//			cfg,
//			visibility_sphere));
//
//    if(binding)
//    {
//    	_bindings[binding.get()] = binding;
//    }
//
//    return binding;
//}
//
// void gl_transparent_batch::render(const utility::frustum& frustum)
//{
//    visible_bindings(_sorted_visible_bindings, _bindings, frustum);
//
//	_gl_context->begin_batch(this);
//	ASSERT_CONTEXT(false && "not implemented");
//	//tbd pass/program/material/geometry setup and caching
//    //for (auto& pair : _sorted_visible_bindings)
//    //{
//    //    const gl_transparent_batch::binding* binding = pair.second;
//    //}
//	_gl_context->end_batch(this);
//}
//
// bool gl_transparent_batch::empty() const
//{
//	return _bindings.empty();
//}
//
// gl_transparent_batch::~gl_transparent_batch()
//{}
//
// void gl_transparent_batch::unbind(const binding* binding)
//{
//    auto find = _bindings.find(binding);
//    if(find == _bindings.end())
//    {
//		LOG_CRITICAL( "cannot find binding to unbind" );
//    }
//    else
//    {
//        _bindings.erase(find);
//    }
//}
//
// void gl_transparent_batch::binding::unbind()
//{
//    _gl_transparent_batch->unbind(this);
//    _gl_transparent_batch = nullptr;
//}
//
// std::unique_ptr<gl_transparent_batch> gl_transparent_batch::make(
//	std::shared_ptr<batch_parameters> parameters,
//	gl_context* gl_context)
//{
//	return std::unique_ptr<gl_transparent_batch>(
//		new gl_transparent_batch(
//			std::move(parameters),
//			gl_context));
//}
//
// gl_transparent_batch::gl_transparent_batch(
//	std::shared_ptr<batch_parameters> parameters,
//	gl_context* gl_context)
//	: transparent_batch(std::move(parameters))
//	, _gl_context(gl_context)
//{
//}