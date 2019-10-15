
#include <igpu/material/primitive.h>

#include <igpu/texture/depth_texture2d.h>
#include <igpu/texture/texture2d.h>

using namespace igpu;

primitive::primitive(variant_t other)
	: _variant(std::move(other))
{
}

primitive& primitive::operator=(primitive&& other)
{
	if (type() == other.type())
	{
		_variant = std::move(other._variant);
	}
	else
	{
		LOG_CONTEXT(
			CRITICAL,
			"expected type: %s but instead recieved primitive of type: %s",
			parameter::to_string(type()).data(),
			parameter::to_string(other.type()).data());
	}

	return *this;
}

primitive& primitive::operator=(const primitive& other)
{
	if (type() == other.type())
	{
		_variant = other._variant;
	}
	else
	{
		LOG_CONTEXT(
			CRITICAL,
			"expected type: %s but instead recieved primitive of type: %s",
			parameter::to_string(type()).data(),
			parameter::to_string(other.type()).data());
	}

	return *this;
}

parameter::type primitive::type() const
{
#define STATIC_ASSERT_TYPE(E, T) {\
	using t = std::decay<decltype(std::get<(size_t)E>(_variant))>::type;\
	static_assert(std::is_same<T,t>::value);}

	constexpr parameter::type DEPTH_TEXTURE = parameter::type((size_t)parameter::type::TEXTURE2D + 1);
	constexpr parameter::type RENDER_TEXTURE = parameter::type((size_t)parameter::type::TEXTURE2D + 2);

	STATIC_ASSERT_TYPE(parameter::type::COMPUTE_BUFFER, std::shared_ptr<compute_buffer>);
	STATIC_ASSERT_TYPE(parameter::type::TEXTURE2D, std::shared_ptr<texture2d>);
	STATIC_ASSERT_TYPE(DEPTH_TEXTURE, std::shared_ptr<depth_texture2d>);
	STATIC_ASSERT_TYPE(RENDER_TEXTURE, std::shared_ptr<render_texture2d>);

	parameter::type type = (parameter::type)_variant.index();
	type = type == DEPTH_TEXTURE || type == RENDER_TEXTURE
		? parameter::type::TEXTURE2D
		: type;

	return (parameter::type)type;
}

const primitive::variant_t& primitive::variant() const
{
	return _variant;
}

void primitive::set(std::shared_ptr<compute_buffer> val)
{
	if (std::holds_alternative<std::shared_ptr<compute_buffer>>(_variant))
	{
		_variant = std::move(val);
	}
	else
	{
		LOG_CONTEXT(CRITICAL, "primitive type is unexpected: %s", parameter::to_string(type()).data());
	}
}

void primitive::set(std::shared_ptr<texture2d> val)
{
	if (std::holds_alternative<std::shared_ptr<texture2d>>(_variant) ||
		std::holds_alternative<std::shared_ptr<depth_texture2d>>(_variant) ||
		std::holds_alternative<std::shared_ptr<render_texture2d>>(_variant))
	{
		_variant = std::move(val);
		_variant = val;
	}
	else
	{
		LOG_CONTEXT(CRITICAL, "primitive type is unexpected: %s", parameter::to_string(type()).data());
	}
}

void primitive::set(std::shared_ptr<depth_texture2d> val)
{
	if (std::holds_alternative<std::shared_ptr<texture2d>>(_variant) ||
		std::holds_alternative<std::shared_ptr<depth_texture2d>>(_variant) ||
		std::holds_alternative<std::shared_ptr<render_texture2d>>(_variant))
	{
		_variant = std::move(val);
	}
	else
	{
		LOG_CONTEXT(CRITICAL, "primitive type is unexpected: %s", parameter::to_string(type()).data());
	}
}

void primitive::set(std::shared_ptr<render_texture2d> val)
{
	if (std::holds_alternative<std::shared_ptr<texture2d>>(_variant) ||
		std::holds_alternative<std::shared_ptr<depth_texture2d>>(_variant) ||
		std::holds_alternative<std::shared_ptr<render_texture2d>>(_variant))
	{
		_variant = std::move(val);
	}
	else
	{
		LOG_CONTEXT(CRITICAL, "primitive type is unexpected: %s", parameter::to_string(type()).data());
	}
}
