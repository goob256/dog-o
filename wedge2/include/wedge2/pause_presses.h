#ifndef WEDGE2_PAUSE_PRESSES_H
#define WEDGE2_PAUSE_PRESSES_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Pause_Presses_Step : public Step
{
public:
	Pause_Presses_Step(bool paused, bool repeat_pressed, Task *this_task);
	virtual ~Pause_Presses_Step();
	
	void start();
	bool run();

private:
	bool paused;
	bool repeat_pressed;
};

}

#endif // WEDGE2_PAUSE_PRESSES_H
