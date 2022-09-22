#include <wedge2/area.h>
#include <wedge2/area_game.h>
#include <wedge2/battle_game.h>

#include "area_game.h"
#include "dialogue.h"
#include "globals.h"
#include "inventory.h"

Globals::Globals() :
	wedge::Globals()
{
	player_start_positions.push_back(util::Point<int>(1, 3));
	player_start_directions.push_back(wedge::DIR_N);

	// example level, not used in this example
	levels.push_back(Level());
	levels[0].experience = 250;
	levels[0].stat_increases.max_hp = 25;
	levels[0].stat_increases.max_mp = 10;
	levels[0].stat_increases.attack = 5;
	levels[0].stat_increases.defense = 0;

	spell_interface = NULL; // FIXME, you can implement this
	object_interface = new Object_Interface();

	melee = new audio::MML("sfx/melee.mml");

	started = false;
	
	max_battle_steps = 20;
	min_battle_steps = 10;
	
	red_triangle_colour = shim::palette[26];
	gameover_fade_colour = shim::palette[8];
}

Globals::~Globals()
{
	delete melee;
}

void Globals::do_dialogue(std::string tag, std::string text, wedge::Dialogue_Type type, wedge::Dialogue_Position position, wedge::Step *monitor)
{
	wedge::Game *g;
	if (BATTLE) {
		g = BATTLE;
	}
	else {
		g = AREA;
	}
	NEW_SYSTEM_AND_TASK(g)
	Dialogue_Step *d = new Dialogue_Step(tag, text, type, position, new_task);
	if (monitor) {
		d->add_monitor(monitor);
	}
	ADD_STEP(d)
	ADD_TASK(new_task)
	FINISH_SYSTEM(g)
}

bool Globals::add_title_gui()
{
	if (started) {
		return false;
	}

	started = true;

	INSTANCE = new Globals::Instance(NULL);

	// FIXME: implement a title screen if you want one
	AREA = new Area_Game();
	AREA->start_area(NULL);

	return true;
}

bool Globals::dialogue_active(wedge::Game *game, bool only_if_initialised)
{
	// FIXME
	return false;
}

void Globals::add_yes_no_gui(std::string text, bool escape_cancels, bool selected, util::Callback callback, void *callback_data)
{
	wedge::Yes_No_GUI::Callback_Data d;
	d.choice = true;
	d.cancelled = false;
	d.userdata = callback_data;
	callback(&d);
}

bool Globals::can_walk()
{
	// FIXME
	return true;
}

bool Globals::title_gui_is_top()
{
	// FIXME
	return false;
}

// --

Globals::Instance::Instance(util::JSON::Node *root) :
	wedge::Globals::Instance(root)
{
	if (root) {
	}
	else {
		stats.push_back(wedge::Player_Stats());
		stats[0].name = "Player";
		stats[0].sprite = NULL; // fix crash if line below throws exception
		stats[0].sprite = new gfx::Sprite("eny");

		for (size_t i = 0; i < stats.size(); i++) {
			stats[i].level = 1;
			stats[i].experience = 0;
			stats[i].base.fixed.max_hp = 100;
			stats[i].base.fixed.max_mp = 25;
			stats[i].base.fixed.attack = 25;
			stats[i].base.fixed.defense = 25;
			stats[i].base.hp = stats[i].base.fixed.max_hp;
			stats[i].base.mp = stats[i].base.fixed.max_mp;
		}
	}
}

Globals::Instance::~Instance()
{
}
