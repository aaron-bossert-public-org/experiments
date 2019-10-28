
#include <igpu/batch/opaque_batch_nodes.h>

#include <igpu/buffer/geometry.h>
#include <igpu/material/material.h>
#include <igpu/material/render_states.h>

using namespace igpu;

token geometry_batch::keygen(const batch_binding::config& cfg)
{
    return cfg.geometry.get();
}

geometry_batch::geometry_batch(const batch_binding::config& cfg)
: opaque_batch_node(keygen(cfg), nullptr, nullptr)
, _item(cfg.geometry)
{
}

const geometry_batch::item_t& geometry_batch::item() const
{
    return _item;
}

const geometry_batch::bindings_t& geometry_batch::bindings() const
{
    return _bindings;
}

geometry_batch::bindings_t& geometry_batch::bindings()
{
    return _bindings;
}

bool geometry_batch::add(const binding_t& binding)
{
    const auto* raw_binding = binding.get();
    
    if (_bindings.find(raw_binding) != _bindings.end())
    {
		LOG_CRITICAL( "binding appears to have already been added");
        return false;
    }
    
    _bindings.emplace(raw_binding, binding);
    return true;
}

void geometry_batch::remove(const batch_binding& binding)
{
    auto find = _bindings.find(&binding);
    if (find == _bindings.end())
    {
		LOG_CRITICAL( "binding does not appear to have been added");
        return;
    }
    
    _bindings.erase(find);


	if (_bindings.empty())
	{
		// parents will remove themselves recursively up the batching tree if they are empty as a result of removal
		parent()->remove_child(this);
	}
}

size_t geometry_batch::size() const
{
    return _bindings.size();
}

//-------------------------------------------------------------------------
//
//

token material_batch::keygen(const batch_binding::config& cfg)
{
    return cfg.material.get();
}

bool material_batch::compare(token l_token, token r_token)
{
	auto* l_material = (material*)l_token;
	auto* r_material = (material*)r_token;

	// need to move this to 
	ASSERT_CONTEXT(false && "not implemented, need to compare parameters used by program, also need to modify how parameters a melded");

	return l_material != r_material;
	//size_t size = std::min(_primitives.size(), _primitives.size());

	//using hash_t = std::hash<primitive::variant_t>;

	//for (size_t i = 0; i < size; ++i)
	//{
	//	size_t lhs = hash_t()(_primitives[i].variant());
	//	size_t rhs = hash_t()(other->_primitives[i].variant());
	//	if (lhs != rhs)
	//	{
	//		return ptrdiff_t(lhs - rhs);
	//	}
	//}

	//return _primitives.size() - other->_primitives.size();
}

material_batch::material_batch(const batch_binding::config& cfg)
: opaque_batch_node(cfg, this)
, _item(cfg.material)
{
}

const material_batch::item_t& material_batch::item() const
{
    return _item;
}

//-------------------------------------------------------------------------
//
//

token render_state_batch::keygen(const batch_binding::config& cfg)
{
    return cfg.material->cfg().render_states.get();
}

bool render_state_batch::compare(token l_token, token r_token)
{
    return ((render_states*)l_token)->compare((render_states*)r_token);
}

render_state_batch::render_state_batch(const batch_binding::config& cfg)
: opaque_batch_node(cfg, this)
, _item(cfg.material->cfg().render_states)
{
}

const render_state_batch::item_t& render_state_batch::item() const
{
    return _item;
}

//-------------------------------------------------------------------------
//
//

token program_batch::keygen(const batch_binding::config& cfg)
{
    return cfg.program.get();
}

program_batch::program_batch(const batch_binding::config& cfg)
: opaque_batch_node(cfg, this)
, _item(cfg.program)
{
}

const program_batch::item_t& program_batch::item() const
{
    return _item;
}


//-------------------------------------------------------------------------
//
//

std::unique_ptr<opaque_batch_root> opaque_batch_root::make()
{
	return std::unique_ptr<opaque_batch_root>(
		new opaque_batch_root());
}

opaque_batch_root::opaque_batch_root()
	: opaque_batch_node(nullptr, child_t::keygen, child_t::compare)
{

}