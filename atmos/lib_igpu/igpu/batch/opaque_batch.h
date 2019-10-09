
#pragma once

#include <igpu/utility/utility_types.h>

#include <igpu/batch/batch.h>
#include <igpu/batch/batch_binding.h>

#include <functional>
#include <memory>

namespace igpu
{
    class opaque_batch_root;

	class opaque_batch : public batch
	{
    public:
        
        bool empty() const override;
        
        ~opaque_batch() override;

		void visit(const std::function<void(const batch_binding&)>& visit);

		opaque_batch_root& root();

		const opaque_batch_root& root() const;
        
	protected:
        
        opaque_batch(
			std::shared_ptr<batch_parameters>,
			std::unique_ptr<opaque_batch_root>);
        
    private:
        
        std::unique_ptr<opaque_batch_root> _root;
        
		opaque_batch (const opaque_batch&) = delete;
		opaque_batch& operator= (const opaque_batch&) = delete;
	};    
}
