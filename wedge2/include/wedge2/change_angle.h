#ifndef WEDGE2_CHANGE_ANGLE_H
#define WEDGE2_CHANGE_ANGLE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Change_Angle_Step : public Step
{
public:
	Change_Angle_Step(Map_Entity *entity, float start_angle, float end_angle, Uint32 duration, Task *task);
	virtual ~Change_Angle_Step();

	void start();
	bool run();

private:
	Map_Entity *entity;
	float start_angle;
	float end_angle;
	Uint32 duration;
	Uint32 start_time;
};

}

#endif // WEDGE2_CHANGE_ANGLE_H
