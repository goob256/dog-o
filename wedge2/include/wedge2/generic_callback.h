#ifndef WEDGE2_GENERIC_CALLBACK_H
#define WEDGE2_GENERIC_CALLBACK_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Generic_Callback_Step : public Step
{
public:
	Generic_Callback_Step(util::Callback callback, void *callback_data, Task *task);
	virtual ~Generic_Callback_Step();
	
	bool run();
	void done_signal(Step *step);

private:
	util::Callback callback;
	void *callback_data;
	bool done;
};

}

#endif // WEDGE2_GENERIC_CALLBACK_H
