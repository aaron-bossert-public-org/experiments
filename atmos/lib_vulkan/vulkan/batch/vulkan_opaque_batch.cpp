
// #include <vulkan/batch/vulkan_opaque_batch.h>

// // Vulkan implementation includes - begin
// #include <vulkan/context/vulkan_context.h>
// #include <vulkan/material/vulkan_program.h>
// #include <vulkan/buffer/vulkan_geometry.h>
// // Vulkan implementation includes - end

// #include <igpu/batch/opaque_batch_nodes.h>
// #include <igpu/buffer/geometry.h>

// using namespace igpu;

// namespace
// {
//     std::shared_ptr<batch_binding> gl_make_batches(
// 		geometry_batch* geometry_batch, 
// 		const batch_binding::config& cfg,
// 		const utility::sphere& visibility_sphere)
//     {
// 		auto binding = std::make_shared<geometry_batch::binding>(
// 			geometry_batch,
// 			cfg,
// 			visibility_sphere);
        
//         if(geometry_batch->add(binding))
//         {
//             return binding;
//         }
        
//         return std::shared_ptr<batch_binding>();
//     }

//     template <typename batch_t>
//     std::shared_ptr<batch_binding> gl_make_batches(
// 		batch_t* b,
// 		const batch_binding::config& cfg,
// 		const utility::sphere& visibility_sphere)
//     {
//         auto* child = cast_to_child<batch_t>(b->find_child(cfg));
        
//         if (!child)
//         {
//             using child_t = typename batch_t::child_t;
            
//             child = new child_t(cfg);
            
//             if(!child->item())
//             {
//                 LOG_CRITICAL( CRITICAL, "batches must have an item to batch on (program/geometry/etc...)");
//                 return nullptr;
//             }
            
//             b->insert(std::unique_ptr<child_t>(child));
//         }
        
//         return gl_make_batches(
// 			child,
// 			cfg,
// 			visibility_sphere);
//     }

//     //-------------------------------------------------------------------------
//     //
//     //
//     class gl_render_batches
//     {
//     public:
        
//         gl_render_batches(vulkan_context* vulkan_context, opaque_batch_root* root, const utility::frustum& frustum)
//         : _frustum(frustum)
//         , _vulkan_context(vulkan_context)
//         {   
//             visit(root);
//         }
        
//     private:
        
//         void set(opaque_batch_root*)
//         {}

//         void set(render_state_batch* node)
//         {
//             _render_state_batch = node;
//         }
        
//         void set(program_batch* node)
//         {
//             if(_program_batch && _program_batch->active())
//             {
// 				ASSERT_CONTEXT(false && "not implemented");
//                 // tbd: program begin _program_batch->begin_batch();
//                 //_vulkan_context->end_program((vulkan_program*)_program_batch->item().get());
//             }
//             _program_batch = node;
//         }

//         void set(material_batch* node)
//         {
//             _material_batch = node;
//         }
        
//         template <typename batch_t>
//         void visit(batch_t* b)
//         {
//             set(b);
            
//             for(auto& pair : b->map())
//             {
//                 visit(cast_to_child<batch_t>(pair.second.get()));
//             }
            
//             set((batch_t*)nullptr);
//             b->active(false);
//         }
        
//         void visit(geometry_batch* geometry_batch)
//         {
// 			auto* program = ASSERT_CAST(vulkan_program*,  _program_batch->item().get());
// 			auto* geometry = ASSERT_CAST(vulkan_geometry*, geometry_batch->item().get());
// 			int32_t start = (int32_t)geometry->element_start();
// 			int32_t count = (int32_t)geometry->element_count();
			
// 			for (const auto& pair : geometry_batch->bindings())
//             {
//                 auto& binding = *pair.second;
				
//                 if(utility::intersects(_frustum, binding.visibility_sphere()))
//                 {
//                     if(!_program_batch->active())
//                     {
// 						_program_batch->active(true);
// 						_vulkan_context->begin_program(program);
//                     }
                    
//                     if(!_render_state_batch->active())
//                     {
// 						_render_state_batch->active(true);
// 						const auto* render_states = ASSERT_CAST(vulkan_render_states*, _render_state_batch->item().get());
						
// 						_vulkan_context->set_render_states(render_states);
//                     }

//                     if(!_material_batch->active())
//                     {
// 						_material_batch->active(true);
// 						program->update(_material_batch->item().get());
//                     }
                    
//                     if(!geometry_batch->active())
//                     {
// 						geometry_batch->active(true);
// 						_vulkan_context->begin_geometry(geometry);
//                     }

// 					program->update(&binding);
// 					_vulkan_context->draw(start, count);
//                 }
//             }
            
//             if(geometry_batch->active())
//             {
//                 geometry_batch->active(false);
//                 // tbd: program begin _geometry_batch->begin_batch();
//                 //_vulkan_context->end_geometry((vulkan_geometry*)geometry_batch->item().get());
//             }
//         }
        
//     private:
        
//         utility::frustum _frustum;
//         vulkan_context* _vulkan_context = nullptr;
//         program_batch* _program_batch = nullptr;
//         render_state_batch* _render_state_batch = nullptr;
// 		material_batch* _material_batch = nullptr;
//     };
// }


// //------------------------------------------------------------------------------
// //
// //
// vulkan_opaque_batch::~vulkan_opaque_batch()
// {
// }

// void vulkan_opaque_batch::render(const utility::frustum& frustum)
// {
// 	_vulkan_context->begin_batch(this);
//     gl_render_batches( _vulkan_context, &root(), frustum );
// 	_vulkan_context->end_batch(this);
// }

// std::shared_ptr<batch_binding> vulkan_opaque_batch::make_binding(
// 	const batch_binding::config& cfg,
// 	const utility::sphere& visibility_sphere)
// {
// 	return gl_make_batches(&root(), cfg, visibility_sphere);
// }

// std::unique_ptr<vulkan_opaque_batch> vulkan_opaque_batch::make(
// 	std::shared_ptr<batch_parameters> parameters,
// 	vulkan_context* vulkan_context)
// {
// 	return std::unique_ptr<vulkan_opaque_batch>(
// 		new vulkan_opaque_batch(
// 			std::move(parameters),
// 			opaque_batch_root::make(),
// 			vulkan_context));
// }

// vulkan_opaque_batch::vulkan_opaque_batch(
// 	std::shared_ptr<batch_parameters> parameters,
// 	std::unique_ptr<opaque_batch_root> root,
// 	vulkan_context* vulkan_context)
// : opaque_batch(
// 	std::move(parameters),
// 	std::move(root))
// , _vulkan_context(vulkan_context)
// {
// }
