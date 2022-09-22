#include "wedge2/general.h"
#include "wedge2/rumble.h"

using namespace wedge;

namespace wedge {

Rumble_Step::Rumble_Step(float force, Uint32 length, Task *task) :
	Step(task),
	force(force),
	length(length)
{
}

Rumble_Step::~Rumble_Step()
{
}

bool Rumble_Step::run()
{
	rumble(force, length);
	send_done_signal();
	return false;
}

} // End namespace wedge
