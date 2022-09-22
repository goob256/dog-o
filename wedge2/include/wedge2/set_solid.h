#ifndef WEDGE2_SET_SOLID_H
#define WEDGE2_SET_SOLID_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Set_Solid_Step : public Step
{
public:
	Set_Solid_Step(Map_Entity *entity, bool solid, Task *task);
	virtual ~Set_Solid_Step();

	void start();
	bool run();

private:
	Map_Entity *entity;
	bool solid;
};

}

#endif // WEDGE2_SET_SOLID_H
