
#pragma once

#include <igpu/batch/batch_nodes.h>
#include <unordered_map>
#include <vector>

namespace igpu
{
	namespace batch_utility
	{
		void get_token(const batch_binding::config& cfg, const program**);
		void get_token(const batch_binding::config& cfg, const render_states**);
		void get_token(const batch_binding::config& cfg, const material**);
		void get_token(const batch_binding::config& cfg, const geometry**);

		template <
			typename BATCH, 
			typename CHILD = typename BATCH::child_t>
		CHILD& get_or_create_child(BATCH& batch, const batch_binding::config& cfg)
		{
			using token_t = typename CHILD::item_t;
			const token_t* token = nullptr;
			get_token(cfg, &token);

			auto& map = batch.map();
			auto child = map.get(token);
			if (child)
			{
				return *child;
			}

			return *map.emplace(token, cfg).first;
		}

		template <
			typename ROOT_BATCH>
			auto create_binding(ROOT_BATCH& root_batch, const batch_binding::config& cfg)
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
				.create_binding(root_batch, cfg))
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
				.create_binding(root_batch, cfg);
		}

		template <
			typename BATCH,
			typename CHILD = typename BATCH::child_t>
			CHILD* find_child(BATCH& batch, const batch_binding::config& cfg)
		{
			using token_t = typename CHILD::item_t;
			const token_t* token = nullptr;
			get_token(cfg, &token);

			auto& map = batch.map();
			return map.get(token);
		}

		template <
			typename BATCH,
			typename CHILD = typename BATCH::child_t>
			bool z(BATCH& batch, CHILD& child)
		{
			if (child.begin() == child.end())
			{
				batch.map().erase(child.item);
				return true;
			}

			return false;
		}



		template <
			typename ROOT_BATCH>
			void remove_child(ROOT_BATCH& root_batch, const batch_binding& binding)
		{
			const auto& cfg = binding.cfg();

			if (auto* program_batch : find_child(root_batch, cfg))
			{
				if (auto* render_state_batch : find_child(program_batch, cfg))
				{
					if (auto* material_batch : find_child(render_state_batch, cfg))
					{
						if (auto* geometry_batch : find_child(material_batch, cfg))
						{
							geometry_batch.remove_binding(binding);
							if (prune_child_if_empty(material_batch, geometry_batch))
							{
								if (prune_child_if_empty(render_state_batch, material_batch))
								{
									if (prune_child_if_empty(program_batch, render_state_batch))
									{
										prune_child_if_empty(root_batch, program_batch);
									}
								}
							}
						}
					}
				}
			}
		}

		template <
			typename ROOT_BATCH,
			typename BATCH_STACK = ROOT_BATCH::batch_stack_t>
			void render(ROOT_BATCH& root_batch)
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
							for (auto& instance : geometry_batch)
							{
								stack.instance = nullptr;
								if (instance.visible())
								{
									stack.instance = &instance;
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
									instance.draw(stack);
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
	}
}

