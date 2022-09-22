#ifndef WEDGE2_DELETE_MAP_ENTITY_H
#define WEDGE2_DELETE_MAP_ENTITY_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Delete_Map_Entity_Step : public Step
{
public:
	Delete_Map_Entity_Step(Map_Entity *entity, Task *task);
	virtual ~Delete_Map_Entity_Step();
	
	bool run();
	void start();

private:
	Map_Entity *entity;
};

}

#endif // WEDGE2_DELETE_MAP_ENTITY_H
