#ifndef WEDGE2_BATTLE_END_H
#define WEDGE2_BATTLE_END_H

#include "wedge2/main.h"
#include "wedge2/general.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Battle_End_Step : public Step
{
public:
	Battle_End_Step(Task *task);
	virtual ~Battle_End_Step();
	
	bool run();
	void done_signal(Step *step);

private:
	int count;
};

}

#endif // WEDGE2_BATTLE_END_H
