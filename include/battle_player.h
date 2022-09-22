#ifndef BATTLE_PLAYER_H
#define BATTLE_PLAYER_H

#include <wedge2/main.h>
#include <wedge2/battle_game.h>
#include <wedge2/battle_player.h>
#include <wedge2/stats.h>

class Battle_Player : public wedge::Battle_Player
{
public:
	static const int PROJECTILE_TIME_PER_100 = 333;
	static const int HEAL_AMOUNT = 200;

	static const int AXE_STAY_TIME = 1000;
	static const int MIN_ARCH_TIME = 500;
	static const int MAX_ARCH_TIME = 750;
	static const int AXE_DIG = -4; // would be 3 but axe is drawn with a pivot of 7, 7

	static void end_water_ripple(void *data);

	Battle_Player(int index);
	~Battle_Player();

	void handle_event(TGUI_Event *event);
	void draw();
	void draw_fore();
	void run();
	
	int get_max_turns();
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	void set_attack_done(bool done);
	void set_use_done(bool done);

	bool take_hit(wedge::Battle_Entity *actor, int damage);

	util::Point<float> get_sprite_pos();

	void inc_mult_level();
	void reset_mult_level();
	int get_mult_level();
	void show_mult_inc(int level);

	void set_heal_done(bool done);

private:
	void calc_ninja_star_pos();
	util::Point<float> calc_arch(float p);

	bool turn_started;
	bool attack_done;
	bool use_done;
	Uint32 run_start;
	float tilt_angle;
	bool doing_ninja_star;
	wedge::Battle_Game::Turn *ninja_star_turn;
	int ninja_star_targets_done;
	Uint32 ninja_star_start;
	Uint32 ninja_star_end;
	util::Point<float> ninja_star_start_pos;
	util::Point<float> ninja_star_end_pos;
	bool doing_dagger;
	wedge::Battle_Game::Turn *dagger_turn;
	Uint32 dagger_start;
	Uint32 dagger_end;
	Uint32 dagger_impact;
	util::Point<float> dagger_start_pos;
	util::Point<float> dagger_end_pos;
	bool dagger_hit;
	int throw_id;

	int mult_level;
	float multiplier;
	int mult_stage;

	Uint32 turn_start_time;

	wedge::Battle_Game::Turn *heal_turn;
	bool doing_heal;
	int heal_time;
	bool heal_done;
	
	// Axe is copied from Goblin's Bomb
	Uint32 axe_start;
	bool doing_axe;
	wedge::Battle_Entity *axe_target;
	bool thrown;
	Uint32 arch_time;
	bool played_axe_impact;
	wedge::Battle_Game::Turn *axe_turn;
	bool axe_hit;

	bool weapon_ready;

	int last_sprite_centre;
	int last_sprite_bottom;
	struct ripple_info {
		util::Point<int> pos;
		Battle_Player *player;
	};
	static std::map< gfx::Sprite *, ripple_info > water_ripples;

	std::string arch_anim;
	float axe_bottom;
	bool took_hit;
};

#endif // BATTLE_PLAYER_H
