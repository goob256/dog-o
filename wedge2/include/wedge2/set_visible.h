#ifndef WEDGE2_SET_VISIBLE_H
#define WEDGE2_SET_VISIBLE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Set_Visible_Step : public Step
{
public:
	Set_Visible_Step(Map_Entity *entity, bool visible, Task *task);
	virtual ~Set_Visible_Step();

	void start();
	bool run();

private:
	Map_Entity *entity;
	bool visible;
};

}

#endif // WEDGE2_SET_VISIBLE_H
