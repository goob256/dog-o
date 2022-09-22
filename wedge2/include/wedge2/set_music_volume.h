#ifndef WEDGE2_SET_MUSIC_VOLUME_H
#define WEDGE2_SET_MUSIC_VOLUME_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Set_Music_Volume_Step : public Step
{
public:
	Set_Music_Volume_Step(float volume, Task *task);
	virtual ~Set_Music_Volume_Step();

	void start();
	bool run();

private:
	float volume;
};

}

#endif // WEDGE2_SET_MUSIC_VOLUME_H
