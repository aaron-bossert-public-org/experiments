
#include <igpu/batch/opaque_batch.h>
#include <igpu/batch/opaque_batch_nodes.h>

using namespace igpu;

namespace
{
    class visit_batches
    {
    public:
        
        using visit_t = std::function<void(const batch_binding&)>;
        
        visit_batches(opaque_batch_root* root, const visit_t& visit_)
		{
			_visit = &visit_;

			visit(root);

			_visit = nullptr;
		}
        
    private:
        
        template <typename batch_t>
        void visit(batch_t* b)
		{
			for (auto& pair : b->map())
			{
				visit(cast_to_child<batch_t>(pair.second.get()));
			}
		}
        
        void visit(geometry_batch* b)
		{
			for (auto& pair : b->bindings())
			{
				(*_visit)(*pair.second);
			}
		}
        
    private:
        
        const visit_t * _visit;
    };
}

void opaque_batch::visit(const std::function<void(const batch_binding&)>& visit)
{
    visit_batches(_root.get(), visit);
}

bool opaque_batch::empty() const
{
    return 0 == _root->size();
}

opaque_batch_root& opaque_batch::root()
{
	return *_root;
}

const opaque_batch_root& opaque_batch::root() const
{
	return *_root;
}

opaque_batch::~opaque_batch()
{
}

opaque_batch::opaque_batch(
	std::shared_ptr<batch_parameters> parameters,
	std::unique_ptr<opaque_batch_root> root)
: batch(std::move(parameters))
, _root(std::move(root))
{
}

