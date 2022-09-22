#ifndef WEDGE2_WAIT_FOR_INTEGER_H
#define WEDGE2_WAIT_FOR_INTEGER_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Wait_For_Integer_Step : public Step
{
public:
	Wait_For_Integer_Step(int *i, int desired_value, Task *task);
	virtual ~Wait_For_Integer_Step();
	
	bool run();

private:
	int *i;
	int desired_value;
};

}

#endif // WEDGE2_WAIT_FOR_INTEGER_H
