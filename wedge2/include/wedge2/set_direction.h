#ifndef WEDGE2_SET_DIRECTION_H
#define WEDGE2_SET_DIRECTION_H

#include "wedge2/main.h"
#include "wedge2/globals.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Set_Direction_Step : public Step
{
public:
	Set_Direction_Step(Map_Entity *entity, Direction direction, bool set_animation, bool moving, Task *task);
	virtual ~Set_Direction_Step();

	void start();
	bool run();

private:
	Map_Entity *entity;
	Direction direction;
	bool set_animation;
	bool moving;
};

}

#endif // WEDGE2_SET_DIRECTION_H
