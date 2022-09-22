#ifndef WEDGE2_SHOP_STEP_H
#define WEDGE2_SHOP_STEP_H

#include "wedge2/main.h"
#include "wedge2/inventory.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Shop_Step : public Step
{
public:
	Shop_Step(std::vector<Object> items, Task *task);
	virtual ~Shop_Step();

	void start();
	bool run();

private:
	std::vector<Object> items;
};

}

#endif // WEDGE2_SHOP_STEP_H
