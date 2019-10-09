
// #pragma once

// #include <igpu/batch/opaque_batch.h>

// namespace igpu
// {
// 	class vulkan_context;

// 	class vulkan_opaque_batch : opaque_batch
// 	{
//     public:

// 		~vulkan_opaque_batch() override;

//         void render(const utility::frustum& frustum) override;
        
//         std::shared_ptr<batch_binding> make_binding(
// 			const batch_binding::config&,
// 			const utility::sphere& visibility_sphere) override;
        
//         static std::unique_ptr<vulkan_opaque_batch> make(
// 			std::shared_ptr<batch_parameters> parameters,
// 			vulkan_context*);

// 	private:
        
//         vulkan_opaque_batch(
// 			std::shared_ptr<batch_parameters>,
// 			std::unique_ptr<opaque_batch_root>,
// 			vulkan_context*);

// 	private:

// 		vulkan_context *const _vulkan_context;
// 	};    
// }
