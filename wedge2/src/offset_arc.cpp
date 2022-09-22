#include "wedge2/map_entity.h"
#include "wedge2/offset_arc.h"

using namespace wedge;

namespace wedge {

Offset_Arc_Step::Offset_Arc_Step(Map_Entity *entity, util::Point<float> start_offset, util::Point<float> end_offset, float height, Uint32 duration, Task *task) :
	Step(task),
	entity(entity),
	start_offset(start_offset),
	end_offset(end_offset),
	height(height),
	duration(duration)
{
}

Offset_Arc_Step::~Offset_Arc_Step()
{
}

void Offset_Arc_Step::start()
{
	entity->set_offset(start_offset);
	start_time = GET_TICKS();
}

bool Offset_Arc_Step::run()
{
	Uint32 now = GET_TICKS();
	Uint32 elapsed = now - start_time;
	if (elapsed >= duration) {
		entity->set_offset(end_offset);
		send_done_signal();
		return false;
	}
	float p = elapsed / (float)duration;
	util::Point<float> o = (end_offset-start_offset) * p + start_offset;
	float a = p * M_PI;
	float h = sin(a) * height;
	o.y -= h;
	entity->set_offset(o);
	return true;
}

}
