#ifndef WEDGE2_SCREEN_SHAKE_H
#define WEDGE2_SCREEN_SHAKE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Screen_Shake_Step : public Step
{
public:
	Screen_Shake_Step(float amount, Uint32 length, Task *task);
	virtual ~Screen_Shake_Step();
	
	bool run();

	void set_cancelled(bool cancelled);

private:
	float amount;
	Uint32 length;
	bool cancelled;
};

}

#endif // WEDGE2_SCREEN_SHAKE_H
