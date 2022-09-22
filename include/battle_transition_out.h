#ifndef BATTLE_TRANSITION_OUT_H
#define BATTLE_TRANSITION_OUT_H

#include <wedge2/main.h>
#include <wedge2/systems.h>

#include "transition.h"

class Battle_Transition_Out_Step : public Transition_Step
{
public:
	Battle_Transition_Out_Step(wedge::Task *task);
	virtual ~Battle_Transition_Out_Step();
	
	bool run();
};

#endif // BATTLE_TRANSITION_OUT_H
