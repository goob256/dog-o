#ifndef AUTOSAVE_H
#define AUTOSAVE_H

#include <wedge2/main.h>
#include <wedge2/systems.h>

class Autosave_Step : public wedge::Step
{
public:
	Autosave_Step(wedge::Task *task);
	virtual ~Autosave_Step();
	
	bool run();
};

#endif // AUTOSAVE_H
