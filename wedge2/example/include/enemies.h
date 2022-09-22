#ifndef ENEMIES_H
#define ENEMIES_H

#include <wedge2/battle_enemy.h>
#include <wedge2/battle_player.h>

class Enemy_Slime : public wedge::Battle_Enemy
{
public:
	Enemy_Slime();
	virtual ~Enemy_Slime();

	bool start();
	bool take_turn();

protected:
};

#endif // ENEMIES_H
