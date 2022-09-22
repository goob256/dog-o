#include "wedge2/achieve.h"

using namespace wedge;

namespace wedge {

Achieve_Step::Achieve_Step(void *id, Task *task) :
	Step(task),
	id(id)
{
}

Achieve_Step::~Achieve_Step()
{
}

void Achieve_Step::start()
{
	util::achieve(id);
}

bool Achieve_Step::run()
{
	send_done_signal();
	return false;
}

}
