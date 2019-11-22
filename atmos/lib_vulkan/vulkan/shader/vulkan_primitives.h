
#pragma once

#include "vulkan/shader/vulkan_primitive.h"

#include "igpu/shader/primitives.h"

namespace igpu
{
	class vulkan_poset_fence;

	class vulkan_primitives : public primitives
	{
	public:
		const config& cfg() const override;

		size_t primitive_count() const override;

		const vulkan_primitive& primitive( size_t ) const override;

		const vulkan_primitive* begin() const;

		const vulkan_primitive* end() const;

		template < typename VISITOR >
		void visit( VISITOR&& visitor );

		static std::unique_ptr< vulkan_primitives > make( const config& );


	private:
		vulkan_primitives( const config&, std::vector< vulkan_primitive > );

	private:
		const config _cfg;
		const std::vector< vulkan_primitive > _primitives;
	};

	template < typename VISITOR >
	void vulkan_primitives::visit( VISITOR&& visitor )
	{
		for ( const vulkan_primitive& prim : *this )
		{
			std::visit( visitor, prim.cfg().vk.value );
		}
	}
}
