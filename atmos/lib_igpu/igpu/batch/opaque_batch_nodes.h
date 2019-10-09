
#pragma once

#include <igpu/batch/batch_binding.h>

#include <memory>
#include <list>
#include <map>
#include <unordered_map>

#include <framework/logging/log.h>

namespace igpu
{
    typedef const void* token;
    class geometry;
    class program;
    class render_states;
    
	// a "tree" of batches used for organizing opaque render passes
    class opaque_batch_node
    {
    public:
        typedef token (*keygen_t)(const batch_binding::config&);

        typedef bool (*compare_t)(token, token);
        
        typedef std::map<token, std::unique_ptr<opaque_batch_node>, compare_t> map_t;
        
        virtual ~opaque_batch_node() { }
        
		token get_key() const
        {
            return _key;
        }

        const opaque_batch_node* parent() const
        {
            return _parent;
        }
        
        opaque_batch_node* parent()
        {
            return _parent;
        }
        
        void insert(std::unique_ptr<opaque_batch_node> opaque_node_batch)
        {
            token key = opaque_node_batch->get_key();
            
            ASSERT_CONTEXT(_map.end() == _map.find(key));
            
            opaque_node_batch->_parent = this;
            _map[key] = std::move(opaque_node_batch);
        }

        opaque_batch_node* find_child(const batch_binding::config& cfg)
        {
            typename map_t::const_iterator find = _map.find(_child_keygen(cfg));
            return find != _map.end() ? find->second.get() : nullptr;
        }

        /// does not delete child, just detaches it...
        void remove_child(opaque_batch_node* child)
        {
            ASSERT_CONTEXT((this == child->_parent), "not a child of this");
            ASSERT_CONTEXT((_map.end() != _map.find(child->get_key())), "not a child of this (some terrible mismatch)");
            
            _map.erase(child->get_key());
            
            // upon reaching an empty state, batches remove themselves from their parents
            if(_map.empty())
            {
                became_empty();
            }
        }
        
        const map_t& map() const
        {
            return _map;
        }
        
        map_t& map()
        {
            return _map;
        }
        
        size_t size() const
        {
            return _map.size();
        }
        
        void clear()
        {
            _map.clear();
        }
                
        bool active() const
        {
            return _active;
        }
        
        void active(bool active)
        {
            _active = active;
        }
        
        virtual void became_empty()
        {
            if(auto* p = parent())
            {
                p->remove_child(this);
            }
        }
        
        static bool compare(token l, token r) { return l < r; }
        
    protected:
        
        opaque_batch_node(token key,
              keygen_t child_keygen,
              compare_t child_compare)
        : _key(key)
        , _child_keygen(child_keygen)
        , _parent(nullptr)
        , _map(child_compare)
        , _active(false)
        {
        }
        
        template< typename batch_t >
        opaque_batch_node(const batch_binding::config& cfg, const batch_t*)
        : opaque_batch_node(batch_t::keygen(cfg), batch_t::child_t::keygen, batch_t::child_t::compare)
        {
        }

    private:

        opaque_batch_node *_parent;
        
        keygen_t _child_keygen;
		token _key;
        map_t _map;
        bool _active;

        opaque_batch_node (const opaque_batch_node&) = delete;
        opaque_batch_node& operator= (const opaque_batch_node&) = delete;
    };

    // this function is provided for safety reasons as it asserts dynamicly up casting
    template<
    typename batch_t,
    typename child_t = typename batch_t::child_t
    >
    child_t* cast_to_child(opaque_batch_node* child)
    {
        ASSERT_CONTEXT(!child || dynamic_cast<child_t*>(child));
        
        return (child_t*) child;
    }

    //-------------------------------------------------------------------------
    //
    //
    
    class geometry_batch : public opaque_batch_node
    {
        
    public:
        
        struct binding : public batch_binding
        {
			const config& cfg() const override;

            binding(
				geometry_batch*,
                const batch_binding::config&,
				const utility::sphere visibility_sphere);
            
            ~binding();
            
            void unbind() override;
            
        private:

			const config _cfg;
            geometry_batch* _geometry_batch;
        };
        
        using item_t = std::shared_ptr<geometry>;
        
        using binding_t = std::shared_ptr<const binding>;
        
        using bindings_t = std::unordered_map<const batch_binding*, binding_t>;
        

        static token keygen(const batch_binding::config&);
        
        geometry_batch(const batch_binding::config&);
        
        const item_t& item() const;
        
        const bindings_t& bindings() const;
        
        bindings_t& bindings();

		bool add(const binding_t&);
        
        void remove(const binding& binding);
        
        size_t size() const;
        
    private:
        
        const item_t _item;
        bindings_t _bindings;
    };
    
    //-------------------------------------------------------------------------
    //
    //

    class material_batch : public opaque_batch_node
    {
    public:
        
        using child_t = geometry_batch;
        
        using item_t = std::shared_ptr<material>;
        
        static token keygen(const batch_binding::config&);
        
        static bool compare(token l_token, token r_token);
        
		material_batch(const batch_binding::config&);
        
        const item_t& item() const;
        
    private:
        const item_t _item;
    };

    //-------------------------------------------------------------------------
    // 
    //
    class render_state_batch : public opaque_batch_node
    {
    public:
        
        using child_t = material_batch;
        
        using item_t = std::shared_ptr<render_states>;
        
        static token keygen(const batch_binding::config&);
        
        static bool compare(token l_token, token r_token);
        
        render_state_batch(const batch_binding::config&);
        
        const item_t& item() const;
        
    private:

        const item_t _item;
        
    };

    //-------------------------------------------------------------------------
    //
    //
    class program_batch : public opaque_batch_node
    {
    public:
        
        using child_t = render_state_batch;
        
        using item_t = std::shared_ptr<program>;
        
        static token keygen(const batch_binding::config&);
        
        program_batch(const batch_binding::config&);

        const item_t& item() const;
        
    private:
        
        const item_t _item;
    };

    //-------------------------------------------------------------------------
    //
    //
    class opaque_batch_root : public opaque_batch_node
    {
    public:
		static std::unique_ptr<opaque_batch_root> make();
        typedef program_batch child_t;
		
	private:

		opaque_batch_root();

    };
}
