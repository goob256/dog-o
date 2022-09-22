#ifndef WEDGE2_OFFSET_ARC_H
#define WEDGE2_OFFSET_ARC_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Offset_Arc_Step : public Step
{
public:
	Offset_Arc_Step(Map_Entity *entity, util::Point<float> start_offset, util::Point<float> end_offset, float height, Uint32 duration, Task *task);
	virtual ~Offset_Arc_Step();

	void start();
	bool run();

private:
	Map_Entity *entity;
	util::Point<float> start_offset;
	util::Point<float> end_offset;
	float height;
	Uint32 duration;
	Uint32 start_time;
};

}

#endif // WEDGE2_OFFSET_ARC_H
