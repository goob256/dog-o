#ifndef WEDGE2_PLAYER_INPUT_H
#define WEDGE2_PLAYER_INPUT_H

#include "wedge2/main.h"
#include "wedge2/input.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Player_Input_Step : public Map_Entity_Input_Step
{
public:
	Player_Input_Step(Map_Entity *entity, Task *task);
	virtual ~Player_Input_Step();

	void handle_event(TGUI_Event *event);

	void set_input_paused(bool paused);

private:
	bool input_paused;
};

}

#endif // WEDGE2_PLAYER_INPUT_H
