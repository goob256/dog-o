#ifndef WEDGE2_PAUSE_SPRITE_H
#define WEDGE2_PAUSE_SPRITE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Pause_Sprite_Step : public Step
{
public:
	Pause_Sprite_Step(gfx::Sprite *sprite, bool paused, Task *task);
	virtual ~Pause_Sprite_Step();

	void start();
	bool run();

private:
	gfx::Sprite *sprite;
	bool paused;
};

}

#endif // WEDGE2_PAUSE_SPRITE_H
