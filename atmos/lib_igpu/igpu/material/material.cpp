
#include <igpu/material/material.h>

#include <framework/logging/log.h>

#include <algorithm>

using namespace igpu;

ptrdiff_t material::compare_parameters(const material* other) const
{
	ASSERT_CONTEXT(other);
	ASSERT_CONTEXT(_cfg.program == other->cfg().program);
	
	size_t size = std::min(_primitives.size(), _primitives.size());
	
	using hash_t = std::hash<primitive::variant_t>;

	for (size_t i = 0; i < size; ++i)
	{
		size_t lhs = hash_t()(_primitives[i].variant());
		size_t rhs = hash_t()(other->_primitives[i].variant());
		if (lhs != rhs)
		{
			return ptrdiff_t(lhs - rhs);
		}
	}

	return _primitives.size() - other->_primitives.size();
}

const material::config& material::cfg() const
{
	return _cfg;
}

const std::vector<primitive>& material::primitives() const
{
	return _primitives;
}

material::material(config cfg, std::vector<primitive> primitives)
	: _cfg(std::move(cfg))
	, _primitives(std::move(primitives))
{}