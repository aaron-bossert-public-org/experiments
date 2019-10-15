#pragma once
#include <memory>

namespace igpu
{
	class vulkan_context;
	class window;
}

class dcs_test
{
public:

	struct config
	{
		std::shared_ptr<igpu::vulkan_context> context;
	};

	static std::unique_ptr<dcs_test> make(
		const config&);

	~dcs_test();

	bool advance();

private:

	dcs_test(
		const config&);

	void update();
	void handle_input();
	void render();

	const config _cfg;
};