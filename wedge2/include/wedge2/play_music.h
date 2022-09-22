#ifndef WEDGE2_PLAY_MUSIC_H
#define WEDGE2_PLAY_MUSIC_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Play_Music_Step : public Step
{
public:
	Play_Music_Step(std::string name, Task *task);
	virtual ~Play_Music_Step();

	void start();
	bool run();

private:
	std::string name;
};

}

#endif // WEDGE2_PLAY_MUSIC_H
