#ifndef WEDGE2_BATTLE_GAME_H
#define WEDGE2_BATTLE_GAME_H

#include "wedge2/main.h"
#include "wedge2/globals.h"
#include "wedge2/inventory.h"
#include "wedge2/systems.h"

namespace wedge {

class Battle_End_Step;
class Battle_Enemy;
class Battle_Entity;
class Battle_Player;

class WEDGE2_EXPORT Battle_Game : public Game
{
public:
	enum Turn_Type {
		NONE = 0,
		ATTACK,
		ITEM,
		RUN,
		DIALOGUE // special turn handled by player
	};

	struct Turn {
		Battle_Entity *actor;
		Turn_Type turn_type;
		// These two would be used for Attack and Item, e.g., Punch, or item name and targets
		std::string turn_name;
		std::vector<Battle_Entity *> targets;
		bool started;
		struct Dialogue {
			std::string tag;
			std::string text;
			Dialogue_Type type;
			Dialogue_Position position;
			Step *monitor;
		} dialogue;
		bool targetting_enemies; // filled automatically by Wedge after get_turn
	};

	//-- define these in your subclass
	virtual Battle_Player *create_player(int index) = 0;
	virtual void start_transition_in(); // make sure to call this superclass to get a rumble
	virtual void start_transition_out() = 0;
	virtual void draw() = 0;
	//--

	Battle_Game(std::string bg, int bg_delay);
	virtual ~Battle_Game();

	virtual bool start();
	virtual void handle_event(TGUI_Event *event);
	virtual bool run();
	virtual void draw_fore();
	virtual void lost_device();
	virtual void found_device();
	virtual void resize(util::Size<int> new_size);
	
	virtual void startup();

	std::vector<Battle_Entity *> get_players();
	std::vector<Battle_Entity *> get_enemies();
	std::vector<Battle_Entity *> get_all_entities();

	bool is_boss_battle();

	void add_entity(Battle_Entity *entity);
	
	void add_gold(int gold);
	void add_experience(int experience);
	
	void set_music_backup(audio::MML *music_backup);

	util::Point<int> get_offset();
	
	Battle_Player *get_random_player();
	Battle_Enemy *get_random_enemy();

	void set_done(bool done);

	void battle_end_signal();

	void do_dialogue(Battle_Entity *actor, std::string tag, std::string text, Dialogue_Type type, Dialogue_Position position, Step *monitor);

	void drop_inactive_turns();
	void drop_entity_turns(wedge::Battle_Entity *entity);

	void add_enemy(Battle_Enemy *enemy);

	bool is_done();

protected:
	virtual Object get_found_object();
	bool can_start_turn(Turn *turn);
	bool fixup_targets(wedge::Battle_Entity *actor, std::vector<Battle_Entity *> &v, bool targetting_enemies, bool add_random);
	virtual void start_turn(Battle_Entity *entity) = 0;

	std::vector<Battle_Entity *> entities;
	std::vector<gfx::Image *> backgrounds;
	int bg_delay;
	int gold;
	int experience;
	bool boss_battle;
	audio::MML *music;
	audio::MML *music_backup;
	std::vector<Turn *> turns;
	std::vector<Turn *> active_turns;
	bool done;

	Uint32 startup_time;

	bool gameover;
	Uint32 game_end_time;

	Object won_object;

	int ran_dialogs;

	bool did_dialogs;

	Battle_End_Step *battle_end_step;

	int started_dialogs;

	std::string boss_music_name;

	bool can_die;

	int attack;
	int mp;

	bool started_up;

	std::vector<Battle_Enemy *> add_enemies;
};

}

#endif // WEDGE2_BATTLE_GAME_H
