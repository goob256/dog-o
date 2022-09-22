#include "wedge2/area_game.h"
#include "wedge2/globals.h"
#include "wedge2/shop_step.h"

using namespace wedge;

namespace wedge {

Shop_Step::Shop_Step(std::vector<Object> items, Task *task) :
	Step(task),
	items(items)
{
}

Shop_Step::~Shop_Step()
{
}

void Shop_Step::start()
{
	AREA->start_shop(items);
}

bool Shop_Step::run()
{
	send_done_signal();
	return false;
}

}
