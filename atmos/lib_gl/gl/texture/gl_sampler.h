
#pragma once

#include "igpu/texture/sampler.h"

namespace igpu
{
	unsigned to_gl(sampler::filter);
	unsigned to_gl(sampler::address);
}
