#ifndef WEDGE2_PAUSE_PLAYER_INPUT_H
#define WEDGE2_PAUSE_PLAYER_INPUT_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Pause_Player_Input_Step : public Step
{
public:
	Pause_Player_Input_Step(bool paused, Task *this_task);
	virtual ~Pause_Player_Input_Step();
	
	void start();
	bool run();

private:
	bool paused;
};

}

#endif // WEDGE2_PAUSE_PLAYER_INPUT_H
