#include <wedge2/globals.h>

#include "battle_game.h"
#include "battle_transition_in2.h"
#include "transition.h"

Battle_Transition_In2_Step::Battle_Transition_In2_Step(wedge::Task *task) :
	Transition_Step(false, task)
{
}

Battle_Transition_In2_Step::~Battle_Transition_In2_Step()
{
}

bool Battle_Transition_In2_Step::run()
{
	bool ret = Transition_Step::run();
	return ret;
}
