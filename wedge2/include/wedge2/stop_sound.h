#ifndef WEDGE2_STOP_SOUND_H
#define WEDGE2_STOP_SOUND_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Stop_Sound_Step : public Step
{
public:
	Stop_Sound_Step(audio::Sound *sound, Task *task);
	virtual ~Stop_Sound_Step();

	bool run();

private:
	audio::Sound *sound;
};

}

#endif // WEDGE2_STOP_SOUND_H
