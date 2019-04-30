#pragma once
#include <atmos/app.h>
#include <memory>

class dcs_test
{
public:

	static std::unique_ptr<dcs_test> make();
	~dcs_test();

	bool advance();

private:

	dcs_test();
	void update();
	void handle_input();
	void render();

	std::unique_ptr<app::application> _application;
	app::time _time;

};