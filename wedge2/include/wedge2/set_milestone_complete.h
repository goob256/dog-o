#ifndef WEDGE2_SET_MILESTONE_COMPLETE_H
#define WEDGE2_SET_MILESTONE_COMPLETE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Set_Milestone_Complete_Step : public Step
{
public:
	Set_Milestone_Complete_Step(int milestone, bool complete, Task *task);
	virtual ~Set_Milestone_Complete_Step();

	void start();
	bool run();

private:
	int milestone;
	bool complete;
};

}

#endif // WEDGE2_SET_MILESTONE_COMPLETE_H
