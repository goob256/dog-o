#ifndef WEDGE2_WAIT_H
#define WEDGE2_WAIT_H

// Wait for signal, call add_monitor on the Step to wait for

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Wait_Step : public Step
{
public:
	Wait_Step(Task *task);
	virtual ~Wait_Step();
	
	bool run();
	
	void done_signal(Step *step);

private:
	bool done;
};

}

#endif // WEDGE2_WAIT_H
