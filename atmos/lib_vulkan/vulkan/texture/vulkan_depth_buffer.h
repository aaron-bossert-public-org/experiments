//
//#pragma once
//
//#include <igpu/texture/depth_buffer.h>
//#include <framework/perf/metrics.h>
//
//struct VkImage_T;
//struct VkDeviceMemory_T;
//struct VkImageView_T
//
//namespace igpu
//{
//	class vulkan_depth_buffer : public depth_buffer
//	{
//	public:
//
//		static std::unique_ptr<vulkan_depth_buffer> make(const config&);
//
//		~vulkan_depth_buffer();
//
//	protected:
//
//		vulkan_depth_buffer(const config&);
//
//	private:
//
//				VkImage_T* _image = nullptr;
//				VkDeviceMemory_T* _image_memory = nullptr;
//				VkImageView_T _image_view = nullptr;
//
//		perf::metric _gpu_mem_metric;
//	};
//}
