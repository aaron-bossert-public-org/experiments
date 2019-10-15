//
//#pragma once
//
//
//#include <igpu/texture/texture2d.h>
//#include <framework/perf/metrics.h>
//#include <glm/vec2.hpp>
//
//struct VkImage_T;
//struct VkDeviceMemory_T;
//struct VkImageView_T
//
//namespace igpu
//{
//	class vulkan_texture2d : public texture2d
//	{
//	public:
//
//		unsigned gl_handle() const;
//
//		static std::unique_ptr<vulkan_texture2d> make(const texel_buffer2d&);
//
//		~vulkan_texture2d() override;
//
//		vulkan_texture2d(
//			const texel_buffer2d::config&,
//			size_t size,
//			unsigned gl_handle);
//
//	private:
//
//		uint32_t _mip_levels;
//		VkImage_T* _image = nullptr;
//		VkDeviceMemory_T* _image_memory = nullptr;
//		VkImageView_T _image_view = nullptr;
//		VkSampler _texture_sampler;
//
//		perf::metric _gpu_mem_metric;
//	};
//}
