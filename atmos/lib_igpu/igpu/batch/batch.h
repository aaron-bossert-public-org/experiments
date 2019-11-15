
#pragma once

#include "igpu/batch/batch_binding.h"
#include "igpu/shader/primitives.h"
#include "igpu/utility/utility_types.h"

#include "framework/utility/scoped_ptr.h"

#include <functional>
#include <memory>

namespace igpu
{
	class draw_target;

	class batch
	{
	public:
		struct config
		{
			std::shared_ptr< draw_target > draw_target;
			std::vector< primitives::config > primitives;
		};

		virtual const config& cfg() const = 0;

		virtual std::unique_ptr< batch_binding > make_binding(
			const instance_batch::config& ) = 0;

		virtual void render( const utility::frustum& frustum ) = 0;

		void visit(
			const std::function< void( const instance_batch& ) >& visit );

		virtual void on_back_buffer_resized( const glm::ivec2& ) = 0;

		virtual ~batch() = default;

	protected:
		batch() = default;
		batch( const batch& ) = delete;
		batch& operator=( const batch& ) = delete;
	};
}
