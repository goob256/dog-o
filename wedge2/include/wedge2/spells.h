#ifndef WEDGE2_SPELLS_H
#define WEDGE2_SPELLS_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

namespace wedge {

class Base_Stats;
class Battle_Entity;

class WEDGE2_EXPORT Spell_Interface
{
public:
	virtual ~Spell_Interface();

	virtual bool is_white_magic(std::string name) = 0;
	virtual bool can_cast_in_menus(std::string name) = 0;
	virtual bool can_multi(std::string name) = 0;
	virtual int get_cost(std::string name) = 0;
	virtual std::vector<int> use(std::string name, std::vector<Base_Stats *> stats, float scale = 1.0f) = 0;
	virtual void play_sound(std::string name) = 0;
	virtual std::vector<Step *> start_effect(std::string spell, std::vector<Battle_Entity *> targets, util::Callback callback, void *callback_data) = 0;
};

}

#endif // WEDGE2_SPELLS_H
