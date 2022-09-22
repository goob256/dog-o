#ifndef WEDGE2_SET_ANIMATION_H
#define WEDGE2_SET_ANIMATION_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Set_Animation_Step : public Step
{
public:
	Set_Animation_Step(Map_Entity *entity, std::string animation, Task *task);
	virtual ~Set_Animation_Step();

	void start();
	bool run();

private:
	Map_Entity *entity;
	std::string animation;
};

}

#endif // WEDGE2_SET_ANIMATION_H
