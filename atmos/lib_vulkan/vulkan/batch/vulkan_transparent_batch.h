
// #pragma once

// #include <igpu/batch/transparent_batch.h>

// #include <vector>
// #include <unordered_map>

// namespace igpu
// {
// 	class vulkan_context;

// 	class vulkan_transparent_batch : public transparent_batch
// 	{
// 	public:
        
//         struct binding : batch_binding
//         {
//             binding(
// 				vulkan_transparent_batch*,
// 				const batch_binding::config&,
// 				const utility::sphere& visibility_sphere);

//             ~binding();

//             void unbind() override;
            
// 		private:

//             vulkan_transparent_batch * _vulkan_transparent_batch;
//         };
        
//         using sortable_t = std::pair<float, const binding*>;
//         using sorted_t = std::vector<sortable_t>;
//         using binding_t = std::shared_ptr<const binding>;
//         using bindings_t = std::unordered_map<const binding*, binding_t>;
        
// 		std::shared_ptr<batch_binding> make_binding(
// 			const batch_binding::config&,
// 			const utility::sphere& visibility_sphere) override;

//         void render(const utility::frustum&) override;

// 		bool empty() const override;
        
//         ~vulkan_transparent_batch() override;

//         void unbind(const binding*);

// 		static std::unique_ptr<vulkan_transparent_batch> make(
// 			std::shared_ptr<batch_parameters> parameters, 
// 			vulkan_context* vulkan_context);

//     private:

// 		vulkan_transparent_batch(
// 			std::shared_ptr<batch_parameters> parameters, 
// 			vulkan_context* vulkan_context);

// 	private:
        
// 		vulkan_context* _vulkan_context;
//         bindings_t _bindings;
        
//         // retain set of sortable indices so that they do not need to
//         // be re allocated each frame.
//         sorted_t _sorted_visible_bindings;
// 	};
// }
