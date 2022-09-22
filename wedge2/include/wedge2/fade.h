#ifndef WEDGE2_FADE_H
#define WEDGE2_FADE_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Fade_Step : public Step
{
public:
	Fade_Step(SDL_Colour colour, bool out, int length/*ms*/, Task *task); // in or out
	virtual ~Fade_Step();
	
	bool run();
	void start();

protected:
	SDL_Colour colour;
	bool out;
	int length;
	Uint32 start_time;
};

}

#endif // WEDGE2_FADE_H
