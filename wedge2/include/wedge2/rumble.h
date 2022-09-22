#ifndef WEDGE2_RUMBLE_H
#define WEDGE2_RUMBLE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Rumble_Step : public Step
{
public:
	Rumble_Step(float force, Uint32 length, Task *task);
	virtual ~Rumble_Step();

	bool run();

private:
	float force;
	Uint32 length;
};

}

#endif // WEDGE2_RUMBLE_H
