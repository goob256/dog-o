#ifndef WEDGE2_LOOK_AROUND_INPUT_H
#define WEDGE2_LOOK_AROUND_INPUT_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Look_Around_Input_Step : public Map_Entity_Input_Step
{
public:
	static const int DELAY = 5000;

	Look_Around_Input_Step(Map_Entity *entity, std::vector<Direction> dont_look, Task *task);
	virtual ~Look_Around_Input_Step();

	bool run(); // return false to pop this Step and advance to the next

protected:
	int ticks;
	std::vector<Direction> dont_look;
};

}

#endif // WEDGE2_LOOK_AROUND_INPUT_H
