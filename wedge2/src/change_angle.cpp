#include "wedge2/change_angle.h"
#include "wedge2/map_entity.h"

using namespace wedge;

namespace wedge {

Change_Angle_Step::Change_Angle_Step(Map_Entity *entity, float start_angle, float end_angle, Uint32 duration, Task *task) :
	Step(task),
	entity(entity),
	start_angle(start_angle),
	end_angle(end_angle),
	duration(duration)
{
}

Change_Angle_Step::~Change_Angle_Step()
{
}

void Change_Angle_Step::start()
{
	entity->set_angle(start_angle);
	start_time = GET_TICKS();
}

bool Change_Angle_Step::run()
{
	Uint32 now = GET_TICKS();
	Uint32 elapsed = now - start_time;
	if (elapsed >= duration) {
		entity->set_angle(end_angle);
		send_done_signal();
		return false;
	}
	float p = elapsed / (float)duration;
	if (p > 1.0f) {
		p = 1.0f;
	}
	float a = p * (end_angle-start_angle) + start_angle;
	entity->set_angle(a);
	return true;
}

}
