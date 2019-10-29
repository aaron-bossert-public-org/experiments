
#pragma once

#include <framework/utility/associative_vector.h>

#include <igpu/batch/batch_nodes.h>

namespace igpu
{
	// this is implemented via templates to preserve access to platform specific implementations exposed through covariance.
	namespace batch_utility
	{
		void get_key(const batch_binding::config& cfg, const program**);
		void get_key(const batch_binding::config& cfg, const render_states**);
		void get_key(const batch_binding::config& cfg, const material**);
		void get_key(const batch_binding::config& cfg, const geometry**);

		template <
			typename BATCH, 
			typename CHILD = typename BATCH::child_t>
		CHILD& get_or_create_child(BATCH& batch, const batch_binding::config& cfg)
		{
			using token_t = typename CHILD::item_t;
			const token_t* token = nullptr;
			get_key(cfg, &token);

			auto& map = batch.map();
			auto child = map.get(token);
			if (child)
			{
				return *child;
			}

			return *map.emplace(token, cfg).first;
		}

		template <
			typename ROOT_BATCH,
			typename... ARGS>
			auto create_binding(
				ROOT_BATCH& root_batch, 
				const batch_binding::config& cfg,
				ARGS&& ... args)
			-> decltype(
				get_or_create_child(
					get_or_create_child(
						get_or_create_child(
							get_or_create_child(
								get_or_create_child(root_batch, cfg),
								cfg),
							cfg),
						cfg),
					cfg)
				.create_binding(cfg, root_batch, std::forward<ARGS>(args)...))
		{
			return
				get_or_create_child(
					get_or_create_child(
						get_or_create_child(
							get_or_create_child(
								get_or_create_child(root_batch, cfg),
								cfg),
							cfg),
						cfg),
					cfg)
				.create_binding(cfg, root_batch, std::forward<ARGS>(args)...);
		}

		template <
			typename MAP>
			auto find_child(MAP& map, const batch_binding::config& cfg)
			-> decltype(
				map().find_iter(nullptr))
		{
			typename MAP::key_t key = nullptr;
			get_key(cfg, &key);

			return batch.map().find_iter(token);
		}

		template <
			typename ROOT_BATCH>
			void remove_child(ROOT_BATCH& root_batch, const batch_binding& binding)
		{
			const auto& cfg = binding.cfg();

			auto& program_map = root_batch.map();
			auto program_found = find_child(program_map, cfg);
			if (program_found != program_map.end_iter())
			{
				auto program_batch = program_map[program_found.second];

				auto& render_states_map = program_batch.map();
				auto render_states_found = find_child(render_states_map, cfg);
				if (render_states_found != render_states_map.end_iter())
				{
					auto render_states_batch = render_states_map[render_states_found.second];

					auto& material_map = render_states_batch.map();
					auto material_found = find_child(material_map, cfg);
					if (material_found != material_map.end_iter())
					{
						auto material_batch = material_map[material_found.second];

						auto& geometry_map = material_batch.map();
						auto geometry_found = find_child(geometry_map, cfg);
						if (geometry_found != geometry_map.end_iter())
						{
							auto geometry_batch = geometry_map[geometry_found.second];

							auto& instance_map = geometry_batch.map();
							auto instance_found = find_child(instance_map, &binding);
							if (instance_found != instance_map.end_iter())
							{
								// destroy instance
								instance_map.erase(instance_found);

								if (0 == instance_batch.child_count())
								{
									instance_map.erase(instance_found);

									if (0 == geometry_batch.child_count())
									{
										geometry_map.erase(geometry_found);

										if (0 == material_batch.child_count())
										{
											material_map.erase(material_found);

											if (0 == render_states_batch.child_count())
											{
												render_states_map.erase(render_states_found);

												if (0 == program_batch.child_count())
												{
													program_map.erase(program_found);
												}
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
				for (auto& render_state_batch : program_batch)
				{
					stack.render_state_batch = nullptr;
					for (auto& material_batch : render_state_batch)
					{
						stack.material_batch = nullptr;
						for (auto& geometry_batch : material_batch)
						{
							stack.geometry_batch = nullptr;
							for (auto& instance_batch : geometry_batch)
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

		template<typename T>
		class batch_impl_t : public T
		{
		public:

			using child_t = typename T::child_t;
			using item_t = typename T::item_t;
			using key_t = typename child_t::item_t;

			template<typename... ARGS>
			batch_impl_t(
				const batch_binding::config& cfg,
				ARGS&& ... args)
				: T(std::forward<ARGS>(args)...)
				, _cfg(cfg)
			{
				get_key(cfg, &_item);
			}

			const associative_vector<key_t, child_t>& map() const override
			{
				return _map;
			}

			associative_vector<key_t, child_t>& map() override
			{
				return _map;
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
				return _map[i]
			}

			virtual child_t& child(size_t) const override
			{
				return _map[i];
			}

		private:

			item_t* _item;
			associative_vector<key_t, child_t> _map;
		};


		template<typename T, typename ROOT_BATCH>
		class batch_binding_t : public T
		{
		public:

			using config = typename T::config;

			template<typename... ARGS>
			batch_binding_t(
				const config& cfg,
				ROOT_BATCH* root_batch,
				ARGS&& ... args)
				: T(std::forward<ARGS>(args)...)
				, _cfg(cfg)
				, _root_batch(root_batch)
			{
			}

			~batch_binding_t()
			{
				remove_child(_root_batch, *this);
			}

			const config& cfg() const override
			{
				return _cfg;
			}

			ROOT_BATCH* root_batch() const
			{
				return _root_batch;
			}

			size_t instance_count() const override
			{
				reutrn _instance_count;
			}

			void instance_count(size_t instance_count) override
			{
				_instance_count = instance_count;
			}

			const utility::sphere& visibility_sphere() const override
			{
				return _visibility_sphere;
			}

			void visibility_sphere(const utility::sphere& visibility_sphere) override
			{
				_visibility_sphere = visibility_sphere;
			}

		private:

			const config _cfg;
			ROOT_BATCH* const _root_batch;
			size_t _instance_count = 1;
			utility::sphere _sphere = {};
		};
	}
}

