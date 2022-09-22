#include "wedge2/area_game.h"
#include "wedge2/general.h"
#include "wedge2/map_entity.h"
#include "wedge2/slide_entity_offset.h"

using namespace wedge;

namespace wedge {

Slide_Entity_Offset_Step::Slide_Entity_Offset_Step(Map_Entity *entity, util::Point<float> destination_offset, float speed, Task *task) :
	Step(task),
	entity(entity),
	destination_offset(destination_offset)
{
	util::Point<float> eo = entity->get_offset();

	float len = (destination_offset-eo).length();
	count = int(fabsf(len / speed));
	count++;
	inc_x = fabsf((destination_offset.x-eo.x) / count);
	inc_y = fabsf((destination_offset.y-eo.y) / count);

	if (destination_offset.x < eo.x) {
		inc_x = -inc_x;
	}
	if (destination_offset.y < eo.y) {
		inc_y = -inc_y;
	}
}

Slide_Entity_Offset_Step::~Slide_Entity_Offset_Step()
{
}

void Slide_Entity_Offset_Step::start()
{
}

bool Slide_Entity_Offset_Step::run()
{
	util::Point<float> o = entity->get_offset();

	if (count <= 1) {
		o = destination_offset;
	}
	else {
		o.x += inc_x;
		o.y += inc_y;
	}

	entity->set_offset(o);

	count--;

	return (count <= 0) == false;
}

}
