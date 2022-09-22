#ifndef WEDGE2_PLAY_ANIMATION_H
#define WEDGE2_PLAY_ANIMATION_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Play_Animation_Step : public Step
{
public:
	Play_Animation_Step(gfx::Sprite *sprite, std::string anim_name, Task *task);
	virtual ~Play_Animation_Step();
	
	void start();
	bool run();

	void set_done(bool done);

private:
	gfx::Sprite *sprite;
	std::string anim_name;
	bool done;
};

}

#endif // WEDGE2_PLAY_ANIMATION_H
