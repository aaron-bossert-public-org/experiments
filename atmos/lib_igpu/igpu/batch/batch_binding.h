
#pragma once

#include <igpu/batch/instance_batch.h>

namespace igpu
{
    class batch_binding
    {
    public:
        
		virtual ~batch_binding() = default;
        
		virtual igpu::instance_batch& instance_batch() = 0;

		virtual const igpu::instance_batch& instance_batch() const = 0;

    protected:

		batch_binding() = default;
		batch_binding(const batch_binding&) = delete;
		batch_binding& operator= (const batch_binding&) = delete;
    };
}

