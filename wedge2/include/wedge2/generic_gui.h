#ifndef WEDGE2_GENERIC_GUI_H
#define WEDGE2_GENERIC_GUI_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Generic_GUI_Step : public Step
{
public:
	Generic_GUI_Step(gui::GUI *gui, bool resize, Task *task);
	virtual ~Generic_GUI_Step();

	void start();
	bool run();

	void set_done(bool done);

private:
	bool done;
	gui::GUI *gui;
	bool resize;
};

}

#endif // WEDGE2_GENERIC_GUI_H
