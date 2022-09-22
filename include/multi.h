#ifndef MULTI_H
#define MULTI_H

#include <wedge2/main.h>
#include <wedge2/systems.h>

class Multi_Step : public wedge::Step
{
public:
	static const int LENGTH = 1500;

	Multi_Step(int level, wedge::Task *task);
	virtual ~Multi_Step();

	void start();
	bool run();
	void draw_fore();

private:
	Uint32 start_time;
	int level;
};

#endif // MULTI_H
