#include "wedge2/area.h"
#include "wedge2/delete_map_entity.h"
#include "wedge2/map_entity.h"
#include "wedge2/systems.h"

using namespace wedge;

namespace wedge {

Delete_Map_Entity_Step::Delete_Map_Entity_Step(Map_Entity *entity, Task *task) :
	Step(task),
	entity(entity)
{
}

Delete_Map_Entity_Step::~Delete_Map_Entity_Step()
{
}

bool Delete_Map_Entity_Step::run()
{
	return false;
}

void Delete_Map_Entity_Step::start()
{
	Area *area = entity->get_area();
	area->remove_entity(entity, true);
	send_done_signal();
}

}
