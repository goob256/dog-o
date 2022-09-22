#ifndef WEDGE2_SET_INTEGER_H
#define WEDGE2_SET_INTEGER_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Set_Integer_Step : public Step
{
public:
	Set_Integer_Step(int *i, int value, Task *task);
	virtual ~Set_Integer_Step();

	bool run();

private:
	int *i;
	int value;
};

}

#endif // WEDGE2_SET_INTEGER_H
