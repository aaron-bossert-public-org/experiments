#pragma once
#include <memory>

namespace igpu
{
	class context;
	class window;
}

class dcs_test
{
public:

	static std::unique_ptr<dcs_test> make(
		std::unique_ptr<igpu::context>);
	~dcs_test();

	bool advance();

private:

	dcs_test(
		std::unique_ptr<igpu::context>);

	void update();
	void handle_input();
	void render();

	std::unique_ptr<igpu::context> _context;
};