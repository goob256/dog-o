#ifndef WEDGE2_GENERIC_IMMEDIATE_CALLBACK_H
#define WEDGE2_GENERIC_IMMEDIATE_CALLBACK_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Generic_Immediate_Callback_Step : public Step
{
public:
	Generic_Immediate_Callback_Step(util::Callback callback, void *callback_data, Task *task);
	virtual ~Generic_Immediate_Callback_Step();

	void start();
	bool run();

private:
	util::Callback callback;
	void *callback_data;
};

}

#endif // WEDGE2_GENERIC_IMMEDIATE_CALLBACK_H
