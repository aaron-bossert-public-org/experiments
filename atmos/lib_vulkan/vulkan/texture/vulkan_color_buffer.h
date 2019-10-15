//
//#pragma once
//
//#include <framework/perf/metrics.h>
//#include <igpu/texture/color_buffer.h>
//
//struct VkImage_T;
//struct VkDeviceMemory_T;
//struct VkImageView_T;
//
//namespace igpu
//{
//	class vulkan_color_buffer : public color_buffer
//	{
//	public:
//
//		static std::unique_ptr<vulkan_color_buffer> make(const config&);
//
//		~vulkan_color_buffer();
//
//	protected:
//
//		vulkan_color_buffer(const config&);
//
//	private:
//
//		VkImage_T* _image = nullptr;
//		VkDeviceMemory_T* _image_memory = nullptr;
//		VkImageView_T _image_view = nullptr;
//
//		perf::metric _gpu_mem_metric;
//	};
//}
