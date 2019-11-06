
#pragma once

#include "igpu/buffer/index_buffer.h"

namespace igpu
{
	class gl_index_buffer : public index_buffer
	{
	public:
		virtual size_t byte_size() const = 0;

		virtual unsigned gl_handle() const = 0;

		unsigned gl_format() const;

		size_t element_count() const override;

		static std::unique_ptr< gl_index_buffer > make( const config& );

	protected:
		gl_index_buffer( unsigned gl_format );

	private:
		const unsigned _gl_format;
	};
}
