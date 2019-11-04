
#include "framework/logging/log.h"

#include "gfx/texture/packed_texture.h"

using namespace gfx;

const packing_request::raw_t& packing_request::raw() const
{
	return _raw;
}

const packing_request::packed_t& packing_request::packed() const
{
	return _packed;
}

packing_request::packing_request( const raw_t& raw, const packed_t& packed )
	: _raw( raw )
	, _packed( packed )
{}

const std::shared_ptr< igpu::texture_2d >& packed_texture::atlas() const
{
	return _atlas;
}

const glm::vec2& packed_texture::uv_scale() const
{
	return _uv_scale;
}

const glm::vec2& packed_texture::uv_offset() const
{
	return _uv_offset;
}

void packed_texture::process(
	const std::shared_ptr< igpu::texture_2d >& atlas,
	const glm::vec2& uv_scale,
	const glm::vec2& uv_offset )
{
	_atlas = atlas;
	_uv_scale = uv_scale;
	_uv_offset = uv_offset;
}

std::unique_ptr< packed_texture > packed_texture::make()
{
	return std::unique_ptr< packed_texture >( new packed_texture() );
}
