#ifndef WEDGE2_BATTLE_COMBO_DRAWER_H
#define WEDGE2_BATTLE_COMBO_DRAWER_H

#include "wedge2/main.h"
#include "wedge2/systems.h"

#include "combo.h"

class Battle_Combo_Drawer_Step : public wedge::Step
{
public:
	Battle_Combo_Drawer_Step(wedge::Task *task);
	virtual ~Battle_Combo_Drawer_Step();
	
	void draw_fore();

	void set(Combo c);
	void set(std::string combo_name);
	void set_cursors(std::vector< std::pair<util::Point<float>, int> > cursors);
	void set_multi_checkboxes(std::vector< std::pair<util::Point<float>, gfx::Sprite *> > multi_checkboxes);
	void end_now();

	void next_caption();

private:
	Uint32 end_draw;
	Uint32 end_name;
	Combo c;
	std::string combo_name;
	int count;
	std::vector< std::pair<util::Point<float>, int> > cursors;
	std::vector< std::pair<util::Point<float>, gfx::Sprite *> > multi_checkboxes;
	std::string caption;
	int last_caption;
	bool ended;
};

#endif // WEDGE2_BATTLE_COMBO_DRAWER_H
