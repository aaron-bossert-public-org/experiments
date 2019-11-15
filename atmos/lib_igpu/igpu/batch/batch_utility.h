
#pragma once

#include "igpu/batch/batch_binding.h"
#include "igpu/batch/batch_nodes.h"

#include "framework/logging/log.h"
#include "framework/utility/associative_vector.h"

namespace igpu
{
	// this is implemented via templates to preserve access to platform specific
	// implementations exposed through covariance.
	namespace batch_utility
	{
		template < typename MAP, typename INSTANCE_CONFIG >
		auto find_child( MAP& map, const INSTANCE_CONFIG& cfg )
			-> decltype( map.find( nullptr ) )
		{
			auto key = typename MAP::val_t::get_key( cfg );

			return map.find( key );
		}

		template < typename ROOT_BATCH, typename DRAW_STATE >
		void render_opaque( ROOT_BATCH& root_batch, DRAW_STATE& draw_state )
		{
			for ( auto& program_batch : root_batch )
			{
				for ( auto& states_batch : program_batch )
				{
					for ( auto& geometry_batch : states_batch )
					{
						if ( false == geometry_batch.pre_draw( &draw_state ) )
						{
							continue;
						}

						for ( auto& material_batch : geometry_batch )
						{
							for ( auto& instance_batch : material_batch )
							{
								if ( !instance_batch.can_render( &draw_state ) )
								{
									continue;
								}

								// only call batch.start_draw if we have
								// instances to render
								if ( !draw_state.batches.material )
								{
									if ( !draw_state.batches.geometry )
									{
										if ( !draw_state.batches.states )
										{
											if ( !draw_state.batches.program )
											{
												if ( !draw_state.batches.root )
												{
													root_batch.start_draw(
														draw_state );
													draw_state.batches.root =
														&root_batch;
												}
												program_batch.start_draw(
													draw_state );
												draw_state.batches.program =
													&program_batch;
											}
											states_batch.start_draw(
												draw_state );
											draw_state.batches.states =
												&states_batch;
										}
										geometry_batch.start_draw( draw_state );
										draw_state.batches.geometry =
											&geometry_batch;
									}
									material_batch.start_draw( draw_state );
									draw_state.batches.material =
										&material_batch;
								}

								draw_state.batches.instance = &instance_batch;
								instance_batch.draw( draw_state );
								draw_state.batches.instance = nullptr;
							}
							if ( draw_state.batches.material )
							{
								draw_state.batches.material = nullptr;
								material_batch.stop_draw();
							}
						}
						if ( draw_state.batches.geometry )
						{
							draw_state.batches.geometry = nullptr;
							geometry_batch.stop_draw();
						}
					}
					if ( draw_state.batches.states )
					{
						draw_state.batches.states = nullptr;
						states_batch.stop_draw();
					}
				}
				if ( draw_state.batches.program )
				{
					draw_state.batches.program = nullptr;
					program_batch.stop_draw();
				}
			}
			if ( draw_state.batches.root )
			{
				draw_state.batches.root = nullptr;
				root_batch.stop_draw();
			}
		}

		template < typename BASE_T, typename ITEM_T, typename CHILD_T >
		class batch_impl_t : public BASE_T
		{
		public:
			using child_t = typename CHILD_T;
			using item_t = typename ITEM_T;
			using key_t = typename child_t::item_t*;
			using map_t = associative_vector< key_t, child_t >;

			template < typename INSTANCE_CONFIG >
			batch_impl_t( const INSTANCE_CONFIG& instance_cfg )
			{
				get_key( instance_cfg, &_item );
			}

			template < typename... ARGS >
			batch_impl_t( ARGS&&... args )
				: T( std::forward< ARGS >( args )... )
			{}

			item_t& item() const override
			{
				return *_item;
			}

			size_t child_count() const override
			{
				return _map.size();
			}

			const child_t& child( size_t i ) const override
			{
				return _map[i];
			}

			child_t& child( size_t i ) override
			{
				return _map[i];
			}

			scoped_ptr< typename map_t::element_ref > ptr( child_t* child )
			{
				return _map.scoped_ptr( child );
			}

			const map_t& map() const
			{
				return _map;
			}

			map_t& map()
			{
				return _map;
			}

			const child_t* begin() const
			{
				return _map.begin();
			}

			const child_t* end() const
			{
				return _map.end();
			}

			child_t* begin()
			{
				return _map.begin();
			}

			child_t* end()
			{
				return _map.end();
			}

			~batch_impl_t() override = default;

			batch_impl_t() = default;
			batch_impl_t( batch_impl_t&& ) = default;
			batch_impl_t& operator=( batch_impl_t&& ) = default;

		private:
			item_t* _item;
			map_t _map;
		};

		template < typename ROOT_BATCH >
		class batch_binding_t : public batch_binding
		{
		public:
			using root_batch_t = typename ROOT_BATCH;
			using program_batch_t = typename root_batch_t::child_t;
			using states_batch_t = typename program_batch_t::child_t;
			using geometry_batch_t = typename states_batch_t::child_t;
			using material_batch_t = typename geometry_batch_t::child_t;
			using instance_batch_t = typename material_batch_t::child_t;
			using config_t = typename instance_batch_t::config;

			template < typename T >
			using bptr_t =
				typename scoped_ptr< typename T::map_t::element_ref >;

			using program_ptr_t = bptr_t< root_batch_t >;
			using states_ptr_t = bptr_t< program_batch_t >;
			using geometry_ptr_t = bptr_t< states_batch_t >;
			using material_ptr_t = bptr_t< geometry_batch_t >;
			using instance_ptr_t = bptr_t< material_batch_t >;

			template < typename... ARGS >
			batch_binding_t(
				root_batch_t* root_batch,
				program_batch_t* program_batch,
				states_batch_t* states_batch,
				geometry_batch_t* geometry_batch,
				material_batch_t* material_batch,
				const config_t& cfg,
				ARGS&&... args )
				: _root_batch( root_batch )
				, _program_ptr( root_batch->ptr( program_batch ) )
				, _states_ptr( program_batch->ptr( states_batch ) )
				, _geometry_ptr( states_batch->ptr( geometry_batch ) )
				, _material_ptr( geometry_batch->ptr( material_batch ) )
				, _instance_ptr( make_instance(
					  this,
					  material_batch,
					  cfg,
					  std::forward< ARGS >( args )... ) )
				, _cfg( cfg )
			{}

			~batch_binding_t() override
			{
				material_batch_t& material = _material_ptr->val();
				{
					auto& map = material.map();

					map.erase( map.find( this ) );
					if ( map.size() )
					{
						return;
					}
				}

				geometry_batch_t& geometry = _geometry_ptr->val();
				{
					auto& map = geometry.map();
					map.erase( map.find( &material.item() ) );
					if ( map.size() )
					{
						return;
					}
				}

				states_batch_t& states = _states_ptr->val();
				{
					auto& map = states.map();
					map.erase( map.find( &geometry.item() ) );
					if ( map.size() )
					{
						return;
					}
				}

				program_batch_t& program = _program_ptr->val();
				{
					auto& map = program.map();
					map.erase( map.find( &states.item() ) );
					if ( map.size() )
					{
						return;
					}
				}

				auto& map = _root_batch->map();
				map.erase( map.find( &program.item() ) );
			}

			const instance_batch_t& instance_batch() const override
			{
				return _instance_ptr->val();
			}

			instance_batch_t& instance_batch() override
			{
				return _instance_ptr->val();
			}

			const config_t& cfg() const override
			{
				return _cfg;
			}

		private:
			root_batch_t* _root_batch;
			program_ptr_t _program_ptr;
			states_ptr_t _states_ptr;
			geometry_ptr_t _geometry_ptr;
			material_ptr_t _material_ptr;
			instance_ptr_t _instance_ptr;
			const config_t _cfg;

			template < typename... ARGS >
			static instance_ptr_t make_instance(
				batch_binding_t* ptr_this,
				material_batch_t* material_batch,
				ARGS&&... args )
			{
				auto& map = material_batch->map();
				auto emplaced = map.emplace(
					ptr_this,

					std::forward< ARGS >( args )... );

				ASSERT_CONTEXT(
					emplaced.second,
					"this binding appears to have already been added" );

				material_batch_t::map_t::iter_t& iter = emplaced.first;

				return map.scoped_ptr( &iter->second->val() );
			}
		};
	}
}
