#include "wedge2/map_entity.h"
#include "wedge2/set_milestone_complete.h"

using namespace wedge;

namespace wedge {

Set_Milestone_Complete_Step::Set_Milestone_Complete_Step(int milestone, bool complete, Task *task) :
	Step(task),
	milestone(milestone),
	complete(complete)
{
}

Set_Milestone_Complete_Step::~Set_Milestone_Complete_Step()
{
}

void Set_Milestone_Complete_Step::start()
{
	INSTANCE->set_milestone_complete(milestone, complete);
}

bool Set_Milestone_Complete_Step::run()
{
	send_done_signal();
	return false;
}

}
