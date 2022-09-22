#ifndef WEDGE2_ACHIEVE_H
#define WEDGE2_ACHIEVE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Achieve_Step : public Step
{
public:
	Achieve_Step(void *id, Task *task);
	virtual ~Achieve_Step();

	void start();
	bool run();

private:
	void *id;
};

}

#endif // WEDGE2_ACHIEVE_H
