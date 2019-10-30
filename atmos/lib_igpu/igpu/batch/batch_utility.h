
#pragma once

#include <framework/logging/log.h>
#include <framework/utility/associative_vector.h>

#include <igpu/batch/batch_nodes.h>
#include <igpu/batch/batch_binding.h>

namespace igpu
{

	// this is implemented via templates to preserve access to platform specific implementations exposed through covariance.
	namespace batch_utility
	{
		template<typename ROOT_BATCH>
		class batch_binding_t;

	
		template <
			typename BATCH, 
			typename INSTANCE_CONFIG,
			typename CHILD = typename BATCH::child_t>
		CHILD& get_or_create_child(BATCH& batch, const INSTANCE_CONFIG& cfg)
		{
			auto key = CHILD::get_key(cfg);
			auto& map = batch.map();
			auto found = map.find(key);
			if (found->second)
			{
				return found->second->val();
			}

			return map.emplace(key, cfg).first->second->val();
		}

		template <
			typename ROOT_BATCH,
			typename INSTANCE_CONFIG,
			typename... ARGS>
			std::unique_ptr<batch_binding_t<typename ROOT_BATCH>> create_binding(
				ROOT_BATCH& root_batch, 
				const INSTANCE_CONFIG& cfg,
				ARGS&& ... args)
		{
			auto& geometry_batch =
				get_or_create_child(
					get_or_create_child(
						get_or_create_child(
							get_or_create_child(root_batch, cfg),
							cfg),
						cfg),
					cfg);

			using binding_t = batch_binding_t<typename ROOT_BATCH>;
			return std::unique_ptr<binding_t>(
				new binding_t(
					&root_batch,
					&geometry_batch,
					cfg,
					std::forward<ARGS>(args)...));
		}

		template <
			typename MAP,
			typename INSTANCE_CONFIG>
			auto find_child(MAP& map, const INSTANCE_CONFIG& cfg)
			-> decltype(
				map.find(nullptr))
		{
			auto key = typename MAP::val_t::get_key(cfg);

			return map.find(key);
		}

		template <
			typename ROOT_BATCH,
			typename BINDING = typename batch_binding_t<typename ROOT_BATCH>>
			void remove_child(
				ROOT_BATCH& root_batch, 
				BINDING& binding)
		{
			const auto& cfg = binding.instance_batch().cfg();

			auto& program_map = root_batch.map();
			auto program_found = find_child(program_map, cfg);
			if (program_found->second)
			{
				BINDING::program_batch_t& program_batch = program_found->second->val();

				auto& render_states_map = program_batch.map();
				auto render_states_found = find_child(render_states_map, cfg);
				if (render_states_found->second)
				{
					BINDING::render_state_batch_t& render_states_batch = render_states_found->second->val();

					auto& material_map = render_states_batch.map();
					auto material_found = find_child(material_map, cfg);
					if (material_found->second)
					{
						BINDING::material_batch_t& material_batch = material_found->second->val();

						auto& geometry_map = material_batch.map();
						auto geometry_found = find_child(geometry_map, cfg);
						if (geometry_found->second)
						{
							BINDING::geometry_batch_t& geometry_batch = geometry_found->second->val();

							auto& instance_map = geometry_batch.map();
							auto instance_found = instance_map.find(&binding);
							if (instance_found->second)
							{
								// destroy instance
								instance_map.erase(instance_found);

								if (0 == instance_map.size())
								{
									geometry_map.erase(geometry_found);

									if (0 == geometry_map.size())
									{
										material_map.erase(material_found);

										if (0 == material_map.size())
										{
											render_states_map.erase(render_states_found);

											if (0 == render_states_map.size())
											{
												program_map.erase(program_found);
											}
										}
									}
								}
								
								return;
							}

							LOG_CRITICAL("failed to find batch binding in geometry_batch");
							return;
						}

						LOG_CRITICAL("failed to find batch binding in material_batch");
						return;
					}

					LOG_CRITICAL("failed to find batch binding in render_states_batch");
					return;
				}

				LOG_CRITICAL("failed to find batch binding in program_batch");
				return;
			}

			LOG_CRITICAL("failed to find batch binding in root_batch");
			return;
		}

		template <
			typename ROOT_BATCH,
			typename BATCH_STACK = ROOT_BATCH::batch_stack_t>
			void render_opaque(ROOT_BATCH& root_batch)
		{
			BATCH_STACK stack = {};
			for (auto& program_batch : root_batch)
			{
				stack.program_batch = nullptr;
				for (auto& render_state_batch : program_batch.map())
				{
					stack.render_state_batch = nullptr;
					for (auto& material_batch : render_state_batch.map())
					{
						stack.material_batch = nullptr;
						for (auto& geometry_batch : material_batch.map())
						{
							stack.geometry_batch = nullptr;
							for (auto& instance_batch : geometry_batch.map())
							{
								stack.instance_batch = nullptr;
								if (instance_batch.visible())
								{
									stack.instance_batch = &instance_batch;
									if (!stack.geometry_batch)
									{
										stack.geometry_batch = &geometry_batch;
										if (!stack.material_batch)
										{
											stack.material_batch = &material_batch;
											if (!stack.render_state_batch)
											{
												stack.render_state_batch = &render_state_batch;
												if (!stack.program_batch)
												{
													stack.program_batch = &program_batch;
													if (!stack.root_batch)
													{
														stack.root_batch = &root_batch;

														root_batch.begin(stack);
													}
													program_batch.begin(stack);
												}
												render_state_batch.begin(stack);
											}
											material_batch.begin(stack);
										}
										geometry_batch.begin(stack);
									}
									instance_batch.draw(stack);
								}
							}
							if (stack.geometry_batch)
							{
								geometry_batch.end();
							}
						}
						if (stack.material_batch)
						{
							material_batch.end();
						}
					}
					if (stack.render_state_batch)
					{
						render_state_batch.end();
					}
				}
				if (stack.program_batch)
				{
					program_batch.end();
				}
			}
			if (stack.root_batch)
			{
				root_batch.end()
			}
		}

		template<typename BASE_T, typename CHILD_T, typename ITEM_T>
		class batch_impl_t : public BASE_T
		{
		public:

			using child_t = typename CHILD_T;
			using item_t = typename ITEM_T;
			using key_t = typename child_t::item_t*;
			using map_t = associative_vector<key_t, child_t>;

			template<
				typename INSTANCE_CONFIG,
				typename... ARGS>
			batch_impl_t(
				const INSTANCE_CONFIG& instance_cfg)
			{
				get_key(instance_cfg, &_item);
			}

			template<typename... ARGS>
			batch_impl_t(
				ARGS&& ... args)
				: T(std::forward<ARGS>(args)...)
			{
			}

			item_t& item() const override
			{
				return *_item;
			}

			size_t child_count() const override
			{
				return _map.size();
			}

			const child_t& child(size_t i) const override
			{
				return _map[i];
			}

			child_t& child(size_t i) override
			{
				return _map[i];
			}

			const map_t& map() const
			{
				return _map;
			}

			map_t& map()
			{
				return _map;
			}

			batch_impl_t() = default;
			batch_impl_t(batch_impl_t&&) = default;
			batch_impl_t& operator= (batch_impl_t&&) = default;

		private:

			item_t* _item;
			map_t _map;
		};

		template<typename ROOT_BATCH>
		class batch_binding_t : public batch_binding
		{
		public:

			using root_batch_t = typename ROOT_BATCH;
			using program_batch_t = typename root_batch_t::child_t;
			using render_state_batch_t = typename program_batch_t::child_t;
			using material_batch_t = typename render_state_batch_t::child_t;
			using geometry_batch_t = typename material_batch_t::child_t;
			using instance_batch_t = typename geometry_batch_t::child_t;
			using instance_ptr_t = typename scoped_ptr<
				typename geometry_batch_t::map_t::element_ref >;
			
			template<typename... ARGS>
			batch_binding_t(
				root_batch_t* root_batch,
				geometry_batch_t* geometry_batch,
				ARGS&& ... args)
				: _dependencies(
					make_dependencies(
						this,
						root_batch,
						geometry_batch,
						std::forward<ARGS>(args)...))
			{
			}

			~batch_binding_t()
			{
				remove_child(*_dependencies.root_batch, *this);
			}

			const instance_batch_t& instance_batch() const override
			{
				return _dependencies.instance_batch_ptr->val();
			}

			instance_batch_t& instance_batch() override
			{
				return _dependencies.instance_batch_ptr->val();
			}

		private:

			struct dependencies
			{
				root_batch_t* root_batch;
				instance_ptr_t instance_batch_ptr;
			} _dependencies;

			template<typename... ARGS>
			static dependencies make_dependencies(
				batch_binding_t* ptr_this,
				root_batch_t* root_batch,
				geometry_batch_t* geometry_batch,
				ARGS&& ... args)
			{
				auto& map = geometry_batch->map();
				auto emplaced = map.emplace(
					ptr_this,
					std::forward<ARGS>(args)...);

				ASSERT_CONTEXT(emplaced.second, "this binding appears to have already been added");

				geometry_batch_t::map_t::iter_t& iter = emplaced.first;

				return {
					root_batch,
					map.scoped_ptr(iter->second->val()),
				};
			}
		};
	}
}

