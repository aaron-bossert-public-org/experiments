
#include <igpu/batch/batch.h>
#include <igpu/batch/opaque_batch_nodes.h>

using namespace igpu;

batch_parameters& batch::parameters()
{
	return *_parameters;
}

const batch_parameters& batch::parameters() const
{
	return *_parameters;
}

batch::batch(std::shared_ptr <batch_parameters> parameters)
	:_parameters(std::move(parameters))
{
}