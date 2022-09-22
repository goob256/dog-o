#ifndef WEDGE2_SPECIAL_NUMBER_H
#define WEDGE2_SPECIAL_NUMBER_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Special_Number_Step : public Step
{
public:
	static const int DISPLAY_TIME = 1000;
	static const int RISE_MAX_OFFSET = -5;

	enum Movement_Type {
		SHAKE,
		RISE
	};

	Special_Number_Step(SDL_Colour colour, SDL_Colour shadow_colour, std::string text, util::Point<int> position, Movement_Type movement_type, Task *task, bool draw_last = false);
	virtual ~Special_Number_Step();
	
	bool run();
	void draw_fore();
	void start();
	
	void set_cancelled(bool cancelled);

private:
	SDL_Colour colour;
	SDL_Colour shadow_colour;
	std::string text;
	util::Point<float> position;
	Movement_Type movement_type;
	Uint32 start_time;
	int count;
	bool draw_last;
	bool cancelled;
};

}

#endif // WEDGE2_SPECIAL_NUMBER_H
