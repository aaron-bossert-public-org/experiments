
#pragma once

#include "igpu/shader/primitive.h"

namespace igpu
{
	class vulkan_compute_buffer;
	class vulkan_texture2d;
	class vulkan_depth_texture2d;
	class vulkan_render_texture2d;

	class vulkan_primitive : public primitive
	{
	public:
		using vulkan_variant_t = std::variant<
			std::shared_ptr< vulkan_compute_buffer >,
			std::shared_ptr< vulkan_texture2d >,
			std::shared_ptr< vulkan_depth_texture2d >,
			std::shared_ptr< vulkan_render_texture2d > >;

		struct config : primitive::config
		{
			struct vulkan
			{
				vulkan_variant_t value;
			};

			vulkan vk;
		};

		vulkan_primitive( const config& );

		const config& cfg() const override;

	private:
		const config _cfg;
	};
}
