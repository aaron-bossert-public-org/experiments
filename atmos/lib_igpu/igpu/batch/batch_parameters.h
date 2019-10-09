
#pragma once

#include <igpu/batch/primitive_block.h>

#include <memory>

namespace igpu
{
	class batch_constraints;

    class batch_parameters
    {
    public:

		const std::shared_ptr<batch_parameters>& parent() const;
		
		size_t index_of(const std::string_view& name) const;

		const primitive_block::element* find(size_t index) const;

		void set (size_t index, const primitive&);

		void set (size_t index, primitive&&);

		bool erase(size_t index);

    protected:

		batch_parameters(batch_constraints*, std::shared_ptr<batch_parameters> parent);

	private:
		
		primitive_block _block;
		std::shared_ptr<batch_parameters> _parent;
    };
}
