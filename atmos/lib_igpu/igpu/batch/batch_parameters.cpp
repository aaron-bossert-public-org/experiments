
#include <igpu/batch/batch_parameters.h>
#include <igpu/context/batch_constraints.h>

#include <framework/logging/log.h>

using namespace igpu;

size_t batch_parameters::index_of(const std::string_view& name) const
{
	size_t index = _block.get_index_of(name);
	if (-1 == index)
	{
		LOG_CRITICAL("invalid batch parameter name given: %s", name.data());
	}
	return index;
}

const std::shared_ptr<batch_parameters>& batch_parameters::parent() const
{
	return _parent;
}

const primitive_block::element* batch_parameters::find(size_t index) const
{
	ASSERT_CONTEXT(index + 1 <= _block.index_count(), "invalid index: %d must be -1 or less than index_count", index);
	return _block.find(index);
}

void batch_parameters::set(size_t index, const primitive& primitive)
{
	ASSERT_CONTEXT(index + 1 <= _block.index_count(), "invalid index: %d must be -1 or less than index_count", index);
	_block.set(index, primitive);
}

void batch_parameters::set(size_t index, primitive&& primitive)
{
	ASSERT_CONTEXT(index + 1 <= _block.index_count(), "invalid index: %d must be -1 or less than index_count", index);
	_block.set(index, std::move(primitive));
}

bool batch_parameters::erase(size_t index)
{
	ASSERT_CONTEXT(index + 1 <= _block.index_count(), "invalid index: %d must be -1 or less than index_count", index);
	return _block.erase(index);
}

batch_parameters::batch_parameters(batch_constraints* constraints, std::shared_ptr<batch_parameters> parent)
	: _block(
		[constraints](const std::string_view& sv) { return constraints->index_of(sv); },
		[constraints](size_t index) -> const primitive& { return constraints->cfg().parameters[index].second; },
		constraints->cfg().parameters.size())
	, _parent(std::move(parent))
{}