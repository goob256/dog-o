#ifndef WEDGE2_DELAY_H
#define WEDGE2_DELAY_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Delay_Step : public Step
{
public:
	Delay_Step(int millis, Task *task);
	virtual ~Delay_Step();
	
	void start();
	bool run();

private:
	int millis;
	int start_time;
};

}

#endif // WEDGE2_DELAY_H
