#ifndef GLOBALS_H
#define GLOBALS_H

#include <wedge2/globals.h>

#define DOGO_GLOBALS static_cast<Globals *>(wedge::globals)
#define DOGO_INSTANCE static_cast<Globals::Instance *>(DOGO_GLOBALS->instance)
#define DOGO_BATTLE static_cast<Monster_RPG_3_Battle_Game *>(wedge::globals->battle_game)

const int PLEASANT = 0;
const int TOM = 1;
const int TIK = 2;
const int WIT = 3;
const int NUM_PLAYERS = 4;

const int WIN_BORDER = 5;

enum Difficulty
{
	DIFFICULTY_EASY = 0,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD
};

class Globals : public wedge::Globals
{
public:
	Globals();
	virtual ~Globals();

	bool add_title_gui();
	void do_dialogue(std::string tag, std::string text, wedge::Dialogue_Type type, wedge::Dialogue_Position position, wedge::Step *monitor);
	bool dialogue_active(wedge::Game *game, bool only_if_initialised = false, bool wait_for_fade = true);
	void add_notification_gui(std::string text, util::Callback callback = 0, void *callback_data = 0);
	void add_yes_no_gui(std::string text, bool escape_cancels, bool selected, util::Callback callback = 0, void *callback_data = 0);
	bool can_walk();
	bool title_gui_is_top();
	util::Point<float> get_onscreen_button_position(wedge::Onscreen_Button button);
	void run();
	void lost_device();
	void found_device();
	virtual void get_joy_xy(TGUI_Event *event, float joy_axis0, float joy_axis1, int *x, int *y);
	bool should_show_back_arrow();

#ifdef STEAMWORKS
	void handle_steam_input();
	void read_steam_input_handles();
	void run_ingame_actions(bool ups_only);
	void run_menu_actions(bool ups_only);
	void run_combo_actions(bool ups_only);
	void ingame_buttons_up();
	void menu_buttons_up();
	void combo_buttons_up();
#endif

	void blur_fire();
	
	class Instance : public wedge::Globals::Instance
	{
	public:
		Instance(util::JSON::Node *root);
		virtual ~Instance();

		int num_milestones();

		std::string save();
	
		Difficulty difficulty;

		bool saw_sneak;

		std::vector< std::vector<std::string> > combos;

		int sp_replenish_count; // when this reaches 1000000, inc. each player's SP by 1. Battle_Game::run and Area_Game::run increment this at different speeds.

		int enemies_killed;
	};

	gfx::Shader *alpha_shader;
	gfx::Shader *blur_shader;
	gfx::Shader *blur_banded_shader;
	gfx::Shader *solid_colour_shader;
	gfx::Shader *heat_wave_shader;
	gfx::Shader *alpha_test_shader;

	gfx::Font *bold_font;
	gfx::Font *multi_font;

	gfx::Image *up_arrow;
	gfx::Image *down_arrow;
	gfx::Image *nomore;
	gfx::Image *speech_window;
	gfx::Image *message_window;
	gfx::Image *gui_window;
	gfx::Image *battle_window;
	gfx::Image *menu_window;
	gfx::Image *shop_window;
	gfx::Image *tip_window;
	gfx::Image *tip_tri;
	gfx::Image *profile_pics[NUM_PLAYERS];
	gfx::Image *combo_l;
	gfx::Image *combo_r;
	gfx::Image *combo_u;
	gfx::Image *combo_d;
	gfx::Image *coin;
	gfx::Image *xb_a;
	gfx::Image *xb_b;
	gfx::Image *xb_x;
	gfx::Image *xb_y;
	gfx::Image *ps_x;
	gfx::Image *ps_circle;
	gfx::Image *ps_square;
	gfx::Image *ps_triangle;
	std::map<std::string, gfx::Image *> mini_pics;
	gfx::Image *mini_frame;
	gfx::Image *active_frame;
	gfx::Image *yellow_frame;
	gfx::Image *hourglass;
	gfx::Image *ninja_star;
	gfx::Image *randomaxe;
	std::vector<gfx::Image *> fire_blurred[3];
	gfx::Image *defend_img;

	gfx::Sprite *hit_preload; // only here to preload
	gfx::Sprite *fire_big;
	gfx::Sprite *fire_medium;
	gfx::Sprite *fire_small;
	gfx::Sprite *coin_sprite;
	gfx::Sprite *fast_sprite;
	gfx::Sprite *dagger;
	gfx::Sprite *mult1;
	gfx::Sprite *mult2;
	gfx::Sprite *mult3;
	gfx::Sprite *multi_check_checked;
	gfx::Sprite *multi_check_unchecked;
	gfx::Sprite *multi_check_unchecked_selected;
	gfx::Sprite *multi_check_checked_selected;
	gfx::Sprite *heal;
	gfx::Sprite *turn_arrow;
	gfx::Sprite *water_ripple;
	gfx::Sprite *water_ripple2;

	audio::Sound *melee;
	audio::Sound *melee2;
	audio::Sound *melee_combo;
	audio::Sound *double_melee;
	audio::Sound *melee_wrestling;
	audio::Sound *melee_spin;
	audio::Sound *jump;
	audio::Sound *big_jump;
	audio::Sound *fall;
	audio::Sound *throw_;
	audio::Sound *shock;
	audio::Sound *hit;
	audio::Sound *rope;
	audio::Sound *shake;
	audio::Sound *spit_water;
	audio::Sound *error;
	Uint32 last_error_play;
	audio::Sound *enemy_attack;
	audio::Sound *combo;
	audio::Sound *run_away;
	audio::Sound *hadouken;
	audio::Sound *land;
	audio::Sound *sensei_jump;
	audio::Sound *ninja_star_loop;
	audio::Sound *octo;
	audio::Sound *buysell;
	audio::Sound *fast;
	audio::Sound *cannonball;
	audio::Sound *stomp;
	audio::Sound *bellyflop;
	audio::Sound *dodge;
	audio::Sound *mult1_s;
	audio::Sound *mult2_s;
	audio::Sound *mult3_s;
	audio::Sound *dive_bomb;
	audio::Sound *seering_slap;
	audio::Sound *thunderclap;
	audio::Sound *cartwheel;
	audio::Sound *beatdown;
	audio::Sound *windmill;
	audio::Sound *the_claw;
	audio::Sound *heal_sound;
	audio::Sound *pickup;
	audio::Sound *trout_slap;
	audio::Sound *twister;
	audio::Sound *kick_flip;
	audio::Sound *big_stomp;
	audio::Sound *nose_breaker;
	audio::Sound *timber;
	audio::Sound *crash;
	audio::Sound *loud_thud;
	audio::Sound *limbs_a_flailin;
	audio::Sound *defend;

	audio::MML *waterfall_sfx;

	audio::Sound *laugh;
	audio::Sound *grunt;
	audio::Sound *bridge_collapse;
	audio::Sound *evil_laugh;
	audio::Sound *coin_hit;
	audio::Sound *elevator_start;
	audio::Sound *elevator_raising;
	audio::Sound *thud;
	audio::Sound *bumper_growl;
	audio::Sound *hoohoo;

	int save_slot;
	bool loaded_autosave;

	std::vector<int> rands;
	int rand_max;

	audio::MML *credits_music;
	gfx::Sprite *jb_credits;
	gfx::Image *nooskewl_logo;

#ifdef STEAMWORKS
	ControllerActionSetHandle_t ingame_action_set;
	ControllerActionSetHandle_t menu_action_set;
	ControllerActionSetHandle_t combo_action_set;
	ControllerDigitalActionHandle_t ingame_up;
	ControllerDigitalActionHandle_t ingame_down;
	ControllerDigitalActionHandle_t ingame_left;
	ControllerDigitalActionHandle_t ingame_right;
	ControllerDigitalActionHandle_t ingame_use;
	ControllerDigitalActionHandle_t ingame_pause;
	ControllerDigitalActionHandle_t ingame_menu;
	ControllerDigitalActionHandle_t menu_up;
	ControllerDigitalActionHandle_t menu_down;
	ControllerDigitalActionHandle_t menu_left;
	ControllerDigitalActionHandle_t menu_right;
	ControllerDigitalActionHandle_t menu_select;
	ControllerDigitalActionHandle_t menu_cancel;
	ControllerDigitalActionHandle_t menu_switch;
	ControllerDigitalActionHandle_t menu_confirm;
	ControllerDigitalActionHandle_t menu_pause;
	ControllerDigitalActionHandle_t menu_menu;
	ControllerDigitalActionHandle_t combo_up;
	ControllerDigitalActionHandle_t combo_down;
	ControllerDigitalActionHandle_t combo_left;
	ControllerDigitalActionHandle_t combo_right;
	ControllerDigitalActionHandle_t combo_punch;
	ControllerDigitalActionHandle_t combo_kick;
	ControllerDigitalActionHandle_t combo_fierce_punch;
	ControllerDigitalActionHandle_t combo_fierce_kick;
	ControllerDigitalActionHandle_t combo_cancel;

	bool ingame_up_data;
	bool ingame_down_data;
	bool ingame_left_data;
	bool ingame_right_data;
	bool ingame_use_data;
	bool ingame_pause_data;
	bool ingame_menu_data;
	bool menu_up_data;
	bool menu_down_data;
	bool menu_left_data;
	bool menu_right_data;
	bool menu_select_data;
	bool menu_cancel_data;
	bool menu_switch_data;
	bool menu_confirm_data;
	bool menu_pause_data;
	bool menu_menu_data;
	bool combo_up_data;
	bool combo_down_data;
	bool combo_left_data;
	bool combo_right_data;
	bool combo_punch_data;
	bool combo_kick_data;
	bool combo_fierce_punch_data;
	bool combo_fierce_kick_data;
	bool combo_cancel_data;

	bool prev_ingame;
	bool prev_menu;
	bool prev_combo;
	Uint32 last_action_set_change;

	bool input_handle_was_null;
#endif
};

extern util::JSON *cfg;

#endif // GLOBALS_H
