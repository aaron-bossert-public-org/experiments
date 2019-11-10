
#pragma once

#include "igpu/batch/instance_batch.h"

namespace igpu
{
	class batch_binding
	{
	public:
		virtual const instance_batch::config& cfg() const = 0;

		virtual igpu::instance_batch& instance_batch() = 0;

		virtual const igpu::instance_batch& instance_batch() const = 0;

		virtual ~batch_binding() = default;
	};
}
