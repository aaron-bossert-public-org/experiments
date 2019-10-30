//
//#include <gl/batch/gl_opaque_batch.h>
//
//#include <framework/utility/scoped_ptr.h>
//
//// GL implementation includes - begin
//#include <gl/buffer/gl_geometry.h>
//#include <gl/context/gl_context.h>
//#include <gl/material/gl_program.h>
//// GL implementation includes - end
//
//#include <igpu/batch/opaque_batch_nodes.h>
//#include <igpu/buffer/geometry.h>
//
//using namespace igpu;
//
////-------------------------------------------------------------------------
////
////
//
//namespace
//{
//    std::shared_ptr<batch_binding> gl_make_batches(
//		geometry_batch* geometry_batch, 
//		const instance_batch::config& cfg,
//		const utility::sphere& visibility_sphere)
//    {
//		auto binding = std::shared_ptr<geometry_batch::binding>(
//			new geometry_batch::binding(
//			geometry_batch,
//			cfg,
//			visibility_sphere));
//
//        if(geometry_batch->add(binding))
//        {
//            return binding;
//        }
//        
//		return nullptr;
//    }
//
//    template <typename batch_t>
//	std::shared_ptr<batch_binding> gl_make_batches(
//		batch_t* b,
//		const instance_batch::config& cfg,
//		const utility::sphere& visibility_sphere)
//    {
//        auto* child = cast_to_child<batch_t>(b->find_child(cfg));
//        
//        if (!child)
//        {
//            using child_t = typename batch_t::child_t;
//            
//            child = new child_t(cfg);
//            
//            if(!child->item())
//            {
//				LOG_CRITICAL( "batches must have an item to batch on (program/geometry/etc...)");
//                return nullptr;
//            }
//            
//            b->insert(std::unique_ptr<child_t>(child));
//        }
//        
//        return gl_make_batches(
//			child,
//			cfg,
//			visibility_sphere);
//    }
//
//    //-------------------------------------------------------------------------
//    //
//    //
//    class gl_render_batches
//    {
//    public:
//        
//        gl_render_batches(gl_context* gl_context, opaque_batch_root* root, const utility::frustum& frustum)
//        : _frustum(frustum)
//        , _gl_context(gl_context)
//        {   
//            visit(root);
//        }
//        
//    private:
//        
//        void set(opaque_batch_root*)
//        {}
//
//        void set(render_state_batch* node)
//        {
//            _render_state_batch = node;
//        }
//        
//        void set(program_batch* node)
//        {
//            if(_program_batch && _program_batch->active())
//            {
//				ASSERT_CONTEXT(false && "not implemented");
//                // tbd: program begin _program_batch->begin_batch();
//                //_gl_context->end_program((gl_program*)_program_batch->item().get());
//            }
//            _program_batch = node;
//        }
//
//        void set(material_batch* node)
//        {
//            _material_batch = node;
//        }
//        
//        template <typename batch_t>
//        void visit(batch_t* b)
//        {
//            set(b);
//            
//            for(auto& pair : b->map())
//            {
//                visit(cast_to_child<batch_t>(pair.second.get()));
//            }
//            
//            set((batch_t*)nullptr);
//            b->active(false);
//        }
//        
//        void visit(geometry_batch* geometry_batch)
//        {
//			auto* program = ASSERT_CAST(gl_program*, _program_batch->item().get());
//			auto* geometry = (gl_geometry*)geometry_batch->item().get();
//			int32_t start = (int32_t)geometry->element_start();
//			int32_t count = (int32_t)geometry->element_count();
//			
//			for (const auto& pair : geometry_batch->bindings())
//            {
//                auto& binding = *pair.second;
//				
//                if(utility::intersects(_frustum, binding.visibility_sphere()))
//                {
//                    if(!_program_batch->active())
//                    {
//						_program_batch->active(true);
//						_gl_context->begin_program(program);
//                    }
//                    
//                    if(!_render_state_batch->active())
//                    {
//						_render_state_batch->active(true);
//						const auto* render_states = ASSERT_CAST(gl_render_states*, _render_state_batch->item().get());
//						
//						_gl_context->set_render_states(render_states);
//                    }
//
//                    if(!_material_batch->active())
//                    {
//						_material_batch->active(true);
//						program->update(_material_batch->item().get());
//                    }
//                    
//                    if(!geometry_batch->active())
//                    {
//						geometry_batch->active(true);
//						_gl_context->begin_geometry(geometry);
//                    }
//
//					program->update(&binding);
//					_gl_context->draw(start, count);
//                }
//            }
//            
//            if(geometry_batch->active())
//            {
//                geometry_batch->active(false);
//                // tbd: program begin _geometry_batch->begin_batch();
//                //_gl_context->end_geometry((gl_geometry*)geometry_batch->item().get());
//            }
//        }
//        
//    private:
//        
//        utility::frustum _frustum;
//        gl_context* _gl_context = nullptr;
//        program_batch* _program_batch = nullptr;
//        render_state_batch* _render_state_batch = nullptr;
//		material_batch* _material_batch = nullptr;
//    };
//}
//
//
////------------------------------------------------------------------------------
////
////
//gl_opaque_batch::~gl_opaque_batch()
//{
//}
//
//void gl_opaque_batch::render(const utility::frustum& frustum)
//{
//	_gl_context->begin_batch(this);
//    gl_render_batches( _gl_context, &root(), frustum );
//	_gl_context->end_batch(this);
//}
//
//std::unique_ptr<batch_binding> gl_opaque_batch::make_binding(
//	const instance_batch::config& cfg,
//	const utility::sphere& visibility_sphere)
//{
//	return gl_make_batches(&root(), cfg, visibility_sphere);
//}
//
//std::unique_ptr<gl_opaque_batch> gl_opaque_batch::make(
//	std::shared_ptr<batch_parameters> parameters,
//	gl_context* gl_context)
//{
//	return std::unique_ptr<gl_opaque_batch>(
//		new gl_opaque_batch(
//			std::move(parameters),
//			opaque_batch_root::make(),
//			gl_context));
//}
//
//gl_opaque_batch::gl_opaque_batch(
//	std::shared_ptr<batch_parameters> parameters,
//	std::unique_ptr<opaque_batch_root> root,
//	gl_context* gl_context)
//: opaque_batch(
//	std::move(parameters),
//	std::move(root))
//, _gl_context(gl_context)
//{
//}
