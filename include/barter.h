#ifndef BUY_SCROLL_H
#define BUY_SCROLL_H

#include <wedge2/systems.h>

class Barter_Step : public wedge::Step
{
public:
	Barter_Step(std::vector<wedge::Object> items, wedge::Task *task);
	virtual ~Barter_Step();

	bool run();
	void done_signal(wedge::Step *step);

private:
	std::vector<wedge::Object> items;
	bool done;
};

#endif // BUY_SCROLL_H
