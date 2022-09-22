#ifndef WEDGE2_PAUSE_TASK_H
#define WEDGE2_PAUSE_TASK_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Pause_Task_Step : public Step
{
public:
	Pause_Task_Step(Task *task_to_pause, bool paused, Task *this_task);
	virtual ~Pause_Task_Step();
	
	void start();
	bool run();

private:
	Task *task_to_pause;
	bool paused;
};

}

#endif // WEDGE2_PAUSE_TASK_H
