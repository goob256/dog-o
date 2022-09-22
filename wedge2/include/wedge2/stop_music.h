#ifndef WEDGE2_STOP_MUSIC_H
#define WEDGE2_STOP_MUSIC_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Stop_Music_Step : public Step
{
public:
	Stop_Music_Step(Task *task);
	virtual ~Stop_Music_Step();

	void start();
	bool run();
};

}

#endif // WEDGE2_STOP_MUSIC_H
