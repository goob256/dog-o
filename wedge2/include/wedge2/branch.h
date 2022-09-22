#ifndef WEDGE2_BRANCH_H
#define WEDGE2_BRANCH_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class WEDGE2_EXPORT Branch_Step : public Step
{
public:
	Branch_Step(int *i, Game *game, std::vector< std::vector< std::vector< Step *> > > steps, Task *task);
	virtual ~Branch_Step();

	bool run();

private:
	int *i;
	Game *game;
	std::vector< std::vector< std::vector< Step *> > > steps;
};

}

#endif // WEDGE2_BRANCH_H
