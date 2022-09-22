#ifndef BATTLE_GAME_H
#define BATTLE_GAME_H

#include <wedge2/battle_game.h>

#include "combo.h"
#include "gui.h"

class Battle_Combo_Drawer_Step;
class Battle_Player;
class Positioned_Multiple_Choice_GUI;
class Player_Stats_GUI;

class Battle_Game : public wedge::Battle_Game
{
public:
	enum Notification_Position {
		LEFT,
		RIGHT
	};
	static const int NOTIFICATION_LIFETIME = 1000;

	Battle_Game(std::string bg, int bg_delay);
	virtual ~Battle_Game();

	bool start();
	void start_transition_in();
	void start_transition_out();
	wedge::Battle_Player *create_player(int index);
	void draw();
	void handle_event(TGUI_Event *event);
	bool run();
	void resize(util::Size<int> new_size);
	
	wedge::Battle_Game::Turn *get_turn(Battle_Player *player);

	void set_turn(int type);
	void set_item(int item);
	void set_easy(int easy);

	int get_num_turns(wedge::Battle_Entity *entity);

	void null_pstats();

	void add_notification(Notification_Position position, std::string text, int lifetime = NOTIFICATION_LIFETIME);

	bool is_detecting_combo();

	void set_player_stats_shown(bool show);
	void set_enemy_stats_shown(bool show);
	void set_window_shown(bool show);

	void close_guis();

	bool is_sneak_attack();

	void startup();

	int get_active_player();

	void add_gold(int gold);

	void multi_confirm(bool confirmed);

	bool get_osc_enabled();

	void turn_sprite_ended(gfx::Sprite *sprite);

	bool in_water();
	float get_water_height();
	
protected:
	virtual void start_turn(wedge::Battle_Entity *entity);

	struct Notification {
		Notification_Position position;
		std::string text;
		Uint32 death_time;
	};

	void fixup_selection();
	int get_player_index(Battle_Player *player);
	void next_player(int direct = -1, bool no_sound = false);
	void end_combo();
	void remove_notification(Notification_Position pos);
	void do_all_dead();

	bool window_shown;
	bool player_stats_shown;
	bool enemy_stats_shown;
	int active_player;
	bool getting_turn;
	wedge::Battle_Game::Turn_Type turn_type;
	Positioned_Multiple_Choice_GUI *turn_gui;
	bool selecting;
	bool selecting_enemy;
	bool selecting_multi;
	wedge::Battle_Entity *selected;
	Battle_Player *acting_player;
	wedge::Battle_Game::Turn *next_turn;
	bool detecting_combo;
	std::vector<Combo> combos;
	Combo_Detector *detector;
	int good_combo;
	Battle_Combo_Drawer_Step *combo_drawer;
	std::vector<int> inventory_indices;
	Positioned_Multiple_Choice_GUI *item_gui;
	Positioned_Multiple_Choice_GUI *easy_gui;
	Battle_Multi_Confirm_GUI *mc_gui;
	int item_index;
	int next_turn_pos;
	int next_turn_top;
	bool turn_gui_gone;
	Player_Stats_GUI *pstats;

	std::vector<Notification> notifications;

	gfx::Tilemap *wall;

	bool just_exited_item_gui;
	bool just_exited_easy_gui;
	bool just_exited_multi_gui;
	bool just_exited_pstats;

	bool has_darkness;
	gfx::Image *darkness_image1;
	gfx::Image *darkness_image2;
	util::Point<float> darkness_offset1;
	util::Point<float> darkness_offset2;

	bool osc_enabled;

	bool sneak_attack;
	
	bool started_with_osc;

	std::vector<wedge::Battle_Entity *> multi_targets;

	std::vector< std::pair<wedge::Battle_Entity *, gfx::Sprite *> > started_turns;
	std::vector<gfx::Sprite *> started_turns_to_delete;

	bool water;
};

#endif // BATTLE_GAME_H
