#include <wedge2/area_game.h>
#include <wedge2/battle_enemy.h>
#include <wedge2/general.h>
#include <wedge2/generic_callback.h>
#include <wedge2/globals.h>
#include <wedge2/input.h>
#include <wedge2/map_entity.h>
#include <wedge2/pause_task.h>

#include "battle_combo_drawer.h"
#include "battle_game.h"
#include "battle_player.h"
#include "battle_transition_in.h"
#include "battle_transition_out.h"
#include "enemies.h"
#include "general.h"
#include "globals.h"
#include "gui.h"
#include "hit.h"
#include "inventory.h"
#include "milestones.h"
#include "widgets.h"

static int get_y(wedge::Battle_Entity *e)
{
	wedge::Battle_Enemy *enemy;

	int y;

	if ((enemy = dynamic_cast<wedge::Battle_Enemy *>(e)) != nullptr) {
		y = enemy->get_position().y;
	}
	else {
		Battle_Player *player = dynamic_cast<Battle_Player *>(e);
		y = player->get_draw_pos().y;
	}
	
	util::Point<int> topleft, bottomright;
	e->get_sprite()->get_bounds(topleft, bottomright);
	y += topleft.y + (bottomright.y-topleft.y);
	
	return y;
}

static bool entity_y_compare(wedge::Battle_Entity *a, wedge::Battle_Entity *b)
{
	int y1 = get_y(a);
	int y2 = get_y(b);

	return y1 < y2;
}

static bool enemy_x_compare(wedge::Battle_Entity *a, wedge::Battle_Entity *b)
{
	wedge::Battle_Enemy *a_enemy = static_cast<wedge::Battle_Enemy *>(a);
	wedge::Battle_Enemy *b_enemy = static_cast<wedge::Battle_Enemy *>(b);
	util::Point<int> a_pos = a_enemy->get_position();
	util::Point<int> b_pos = b_enemy->get_position();
	gfx::Sprite *a_sprite = a->get_sprite();
	gfx::Sprite *b_sprite = b->get_sprite();
	gfx::Image *a_img;
	gfx::Image *b_img;
	gfx::Sprite::Animation *a_idle = a_sprite->get_animation("idle");
	gfx::Sprite::Animation *b_idle = b_sprite->get_animation("idle");
	if (a_idle == nullptr) {
		a_img = a_sprite->get_current_image();
	}
	else {
		a_img = a_idle->images[0];
	}
	if (b_idle == nullptr) {
		b_img = b_sprite->get_current_image();
	}
	else {
		b_img = b_idle->images[0];
	}
	util::Point<int> a_topleft, a_bottomright;
	util::Point<int> b_topleft, b_bottomright;
	a_img->get_bounds(a_topleft, a_bottomright);
	b_img->get_bounds(b_topleft, b_bottomright);
	a_pos.x += a_topleft.x + (a_bottomright.x - a_topleft.x);
	b_pos.x += b_topleft.x + (b_bottomright.x - b_topleft.x);
	return a_pos.x > b_pos.x;
}

static void turn_callback(void *data)
{
	auto d = static_cast<Multiple_Choice_GUI::Callback_Data *>(data);
	Battle_Game *b = static_cast<Battle_Game *>(d->userdata);
	b->set_turn(d->choice);
}

static void item_callback(void *data)
{
	auto d = static_cast<Multiple_Choice_GUI::Callback_Data *>(data);
	Battle_Game *b = static_cast<Battle_Game *>(d->userdata);
	b->set_item(d->choice);
}

static void easy_callback(void *data)
{
	auto d = static_cast<Multiple_Choice_GUI::Callback_Data *>(data);
	Battle_Game *b = static_cast<Battle_Game *>(d->userdata);
	b->set_easy(d->choice);
}

static void stats_callback(void *data)
{
	auto d = static_cast<Player_Stats_GUI::Callback_Data *>(data);
	Battle_Game *b = static_cast<Battle_Game *>(d->userdata);
	b->null_pstats();
}

static void multi_callback(void *data)
{
	auto d = static_cast<Battle_Multi_Confirm_GUI::Callback_Data *>(data);
	Battle_Game *b = static_cast<Battle_Game *>(d->userdata);
	b->multi_confirm(d->confirmed);
}

static void turn_sprite_ended(void *data)
{
	gfx::Sprite *sprite = static_cast<gfx::Sprite *>(data);
	static_cast<Battle_Game *>(BATTLE)->turn_sprite_ended(sprite);
}

Battle_Game::Battle_Game(std::string bg, int bg_delay) :
	wedge::Battle_Game(bg, bg_delay),
	window_shown(true),
	player_stats_shown(true),
	enemy_stats_shown(true),
	active_player(0),
	getting_turn(false),
	turn_type(wedge::Battle_Game::NONE),
	turn_gui(nullptr),
	selecting(false),
	selecting_enemy(false),
	selecting_multi(false),
	selected(nullptr),
	acting_player(nullptr),
	next_turn(nullptr),
	detecting_combo(false),
	detector(nullptr),
	good_combo(-1),
	item_gui(nullptr),
	easy_gui(nullptr),
	mc_gui(nullptr),
	item_index(-1),
	next_turn_pos(0),
	next_turn_top(0),
	turn_gui_gone(true),
	pstats(nullptr),
	just_exited_item_gui(false),
	just_exited_easy_gui(false),
	just_exited_multi_gui(false),
	just_exited_pstats(false),
	has_darkness(false),
	osc_enabled(false),
	water(false)
{
	osc_enabled = GLOBALS->onscreen_controller_was_enabled;

	NEW_SYSTEM_AND_TASK(this)
	combo_drawer = new Battle_Combo_Drawer_Step(new_task);
	ADD_STEP(combo_drawer)
	ADD_TASK(new_task)
	FINISH_SYSTEM(this)

	try {
		wall = new gfx::Tilemap("battle_" + bg + ".wm2");
	}
	catch (util::Error &) {
		wall = nullptr;
	}
}

Battle_Game::~Battle_Game()
{
	if (getting_turn) {
		if (turn_gui_gone == false) {
			turn_gui->exit();
		}
	}
	if (item_gui) {
		item_gui->exit();
	}
	if (easy_gui) {
		easy_gui->exit();
	}
	if (mc_gui) {
		mc_gui->exit();
	}
	if (pstats) {
		pstats->exit();
		pstats = nullptr;
	}

	delete wall;
	
	for (auto it = started_turns.begin(); it != started_turns.end(); it++) {
		auto p = *it;
		delete p.second;
	}
			
	// Make sure any items that were used but canceled before being removed get reset
	INSTANCE->inventory.unuse_all();
}

bool Battle_Game::start()
{
	if (wedge::Battle_Game::start() == false) {
		return false;
	}

	if (boss_battle) {
		sneak_attack = false;
	}
	else {
		sneak_attack = util::rand(0, 11) == 11;
		if (sneak_attack) {
			gfx::add_notification(GLOBALS->game_t->translate(374)/* Originally: Sneak attack! */);
		}
	}

	// FIXME:
	//sneak_attack = true;

	if (sneak_attack) {
		for (auto e : entities) {
			if (dynamic_cast<Battle_Player *>(e)) {
				e->set_inverse_x(true);
			}
		}
	}

	return true;
}

void Battle_Game::start_transition_in()
{
	wedge::Battle_Game::start_transition_in();

	NEW_SYSTEM_AND_TASK(AREA)
	wedge::Map_Entity *player = AREA->get_player(0);
	wedge::Pause_Task_Step *pause1 = new wedge::Pause_Task_Step(player->get_input_step()->get_task(), true, new_task);
	Battle_Transition_In_Step *battle_step = new Battle_Transition_In_Step(this, new_task);
	wedge::Pause_Task_Step *pause2 = new wedge::Pause_Task_Step(player->get_input_step()->get_task(), false, new_task);
	ADD_STEP(pause1)
	ADD_STEP(battle_step)
	ADD_STEP(pause2)
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

void Battle_Game::start_transition_out()
{
	NEW_SYSTEM_AND_TASK(this)
	Battle_Transition_Out_Step *step = new Battle_Transition_Out_Step(new_task);
	ADD_STEP(step)
	ADD_TASK(new_task)
	FINISH_SYSTEM(this)
}

wedge::Battle_Player *Battle_Game::create_player(int index)
{
	return new Battle_Player(index);
}

void Battle_Game::draw()
{
	auto players = get_players();

	int frame;
	Uint32 div = (int)backgrounds.size() * bg_delay;
	if (div != 0) {
		Uint32 num = GET_TICKS() % div;
		frame = num / bg_delay;
	}
	else {
		frame = 0;
	}
	backgrounds[frame]->draw(util::Point<int>(shim::screen_size.w/2-backgrounds[0]->size.w/2, shim::screen_size.h-backgrounds[0]->size.h+3));

	if (wall) {
		wall->draw(0, wall->get_num_layers()-1, util::Point<int>(-3, -18), false);
	}

	std::vector<wedge::Battle_Entity *> sorted_entities = entities;

	std::sort(sorted_entities.begin(), sorted_entities.end(), entity_y_compare);

	for (size_t i = 0; i < sorted_entities.size(); i++) {
		wedge::Battle_Entity *entity = sorted_entities[i];
		entity->draw_back();
	}

	for (size_t i = 0; i < sorted_entities.size(); i++) {
		wedge::Battle_Entity *entity = sorted_entities[i];
		entity->draw();
	}

	bool all_dead = true;
	auto v = get_enemies();
	for (auto &e : v) {
		if (e->get_stats()->hp > 0) {
			all_dead = false;
		}
	}
	if (all_dead == false) {
		all_dead = true;
		for (auto &e : players) {
			if (e->get_stats()->hp > 0) {
				all_dead = false;
			}
		}
	}

	if (GET_TICKS() < startup_time || all_dead) {
		Game::draw();

		if (all_dead) {
			do_all_dead(); // run it here too so turn_gui doesn't show up with no window (9patch below)
			for (size_t i = 0; i < sorted_entities.size(); i++) {
				wedge::Battle_Entity *entity = sorted_entities[i];
				entity->draw_fore();
			}
		}

		if (has_darkness) {
			real_draw_darkness(darkness_image1, darkness_image2, darkness_offset1, darkness_offset2, {0.0f, 0.0f}, 0.5f);
		}

		return;
	}
	
	bool running = false;
	for (auto p : players) {
		if (static_cast<Battle_Player *>(p)->is_running()) {
			running = true;
			break;
		}
	}
	if (running) {
		return;
	}

	if (selecting) {
		util::Point<float> pos;
		gfx::Image *cursor = DOGO_GLOBALS->cursor->get_current_image();
		gfx::Image *multi_image = DOGO_GLOBALS->multi_check_checked->get_current_image();
		std::vector< std::pair<util::Point<float>, gfx::Sprite *> > v2;
		int flag;
		if (selecting_multi) {
			fixup_targets(players[active_player], multi_targets, selecting_enemy, false);
			for (auto e : multi_targets) {
				gfx::Sprite *anim;
				if (selecting_enemy) {
					auto enemy = static_cast<Battle_Enemy *>(e);
					if (enemy->get_draw_as_player()) {
						pos = enemy->get_sprite_pos();
					}
					else {
						pos = enemy->get_position();
					}
					gfx::Sprite *sprite = e->get_sprite();
					gfx::Image *img = sprite->get_current_image();
					util::Point<int> topleft, bottomright;
					img->get_bounds(topleft, bottomright);
					//sprite->get_bounds(topleft, bottomright);
					pos += enemy->get_draw_as_player() ? util::Point<int>(img->size.w-topleft.x-(bottomright.x-topleft.x), topleft.y) : topleft;
					pos.x += (bottomright.x - topleft.x);
					pos.x += shim::tile_size / 4.0f;
					pos.y -= 6;
				}
				else {
					pos = static_cast<Battle_Player *>(e)->get_draw_pos();
					pos.x -= multi_image->size.w;
					pos.y -= 6;
					gfx::Sprite *sprite = selected->get_sprite();
					gfx::Image *img = sprite->get_current_image();
					util::Point<int> topleft, bottomright;
					img->get_bounds(topleft, bottomright);
					pos.x += topleft.x;
					pos.x -= shim::tile_size/4.0f;
				}
				if (e == selected) {
					anim = DOGO_GLOBALS->multi_check_checked_selected;
				}
				else {
					anim = DOGO_GLOBALS->multi_check_checked;
				}
				if (sneak_attack) {
					pos.x = shim::screen_size.w - pos.x - multi_image->size.w;
				}
				v2.push_back({pos, anim});
			}
			std::vector<wedge::Battle_Entity *> all_targets;
			if (selecting_enemy) {
				all_targets = get_enemies();
			}
			else {
				all_targets = get_players();
			}
			for (auto ee : all_targets) {
				if (std::find(multi_targets.begin(), multi_targets.end(), ee) == multi_targets.end() && ee->get_stats()->hp < 1000000) {
					if (selecting_enemy) {
						auto e = static_cast<wedge::Battle_Enemy *>(ee);
						pos = e->get_position();
						gfx::Sprite *sprite = e->get_sprite();
						gfx::Image *img = sprite->get_current_image();
						util::Point<int> topleft, bottomright;
						img->get_bounds(topleft, bottomright);
						//sprite->get_bounds(topleft, bottomright);
						pos += topleft;
						pos.x += (bottomright.x - topleft.x);
						pos.x += shim::tile_size / 4.0f;
						pos.y -= 6;
					}
					else {
						auto p = static_cast<Battle_Player *>(ee);
						pos = p->get_draw_pos();
						pos.x -= multi_image->size.w;
						pos.y -= 6;
						gfx::Sprite *sprite = ee->get_sprite();
						gfx::Image *img = sprite->get_current_image();
						util::Point<int> topleft, bottomright;
						img->get_bounds(topleft, bottomright);
						pos.x += topleft.x;
						pos.x -= shim::tile_size/4.0f;
					}
					if (sneak_attack) {
						pos.x = shim::screen_size.w - pos.x - multi_image->size.w;
					}

					if (ee == selected) {
						v2.push_back({pos, DOGO_GLOBALS->multi_check_unchecked_selected});
					}
					else {
						v2.push_back({pos, DOGO_GLOBALS->multi_check_unchecked});
					}
				}
			}

			combo_drawer->set_multi_checkboxes(v2);
		}
		else if (shim::guis.size() == 0) {
			if (selecting_enemy) {
				auto e = static_cast<wedge::Battle_Enemy *>(selected);
				if (e->get_draw_as_player()) {
					pos = e->get_sprite_pos();
				}
				else {
					pos = e->get_position();
				}
				gfx::Sprite *sprite = e->get_sprite();
				gfx::Image *img = sprite->get_current_image();
				util::Point<int> topleft, bottomright;
				img->get_bounds(topleft, bottomright);
				//sprite->get_bounds(topleft, bottomright);
				pos += e->get_draw_as_player() ? util::Point<int>(img->size.w-topleft.x-(bottomright.x-topleft.x), topleft.y) : topleft;
				pos.x += (bottomright.x - topleft.x);
				pos.x += shim::tile_size / 4.0f;
				pos.y -= shim::tile_size/4.0f;
				flag = gfx::Image::FLIP_H;

				if (INSTANCE->is_milestone_complete(MS_DID_AN_ATTACK) == false) {
#ifdef STEAMWORKS
					EControllerActionOrigin *origins = nullptr;
					ControllerHandle_t handle = 0;
					if (shim::steam_init_failed == false) {
						origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];
						handle = input::get_controller_handle();
					}
#endif

					std::string black = "|";

					int black_i = 0;

					for (int i = 0; i < 256; i++) {
						if (shim::palette[i].r == 0 && shim::palette[i].g == 0 && shim::palette[i].b == 0) {
							black_i = i;
							break;
						}
					}

					char buf[10];
					snprintf(buf, 10, "%02x", black_i);
					black += util::uppercase(buf);

					std::string text;
					std::string button;

					if (input::system_has_keyboard() == false) {
						if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
							button = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_A) + input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_A) + black;
#else
							button = input::get_joystick_button_colour_code(TGUI_B_A) + input::get_joystick_button_name(TGUI_B_A) + black;
#endif
						}
						else {
							text = GLOBALS->game_t->translate(399)/* Originally: Select a target! */;
						}
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_kick, origins);
							if (count > 0) {
								button = input::get_joystick_button_colour_code(origins[0]) + input::get_joystick_button_name(origins[0]) + black;
							}
							else {
								button = get_key_name(GLOBALS->key_action);
							}
						}
						else
#endif
						{
							button = input::get_joystick_button_colour_code(GLOBALS->joy_action) + input::get_joystick_button_name(GLOBALS->joy_action) + black;
						}
					}
					else {
						button = get_key_name(GLOBALS->key_action);
					}

					if (text == "") {
						text = GLOBALS->game_t->translate(399)/* Originally: Select a target! */;
					}

					gfx::Image *cursor_img = DOGO_GLOBALS->cursor->get_current_image();
					int xx = pos.x + cursor_img->size.w+10;
					int y = pos.y + cursor_img->size.h/2 - shim::font->get_height()/2;

					int t = GET_TICKS() % 1000;
					int tip_xo = t < 500 ? 1 : 0;
					gfx::draw_9patch(DOGO_GLOBALS->tip_window, util::Point<int>(xx+tip_xo, y-2), util::Size<int>(shim::font->get_text_width(text) + 10, shim::font->get_height() + 3));
					DOGO_GLOBALS->tip_tri->draw({float(xx-DOGO_GLOBALS->tip_tri->size.w+2+tip_xo), float(y+shim::font->get_height()/2-DOGO_GLOBALS->tip_tri->size.h/2-1)});
					shim::font->draw(shim::black, text, util::Point<int>(xx+5+tip_xo, y));

#ifdef STEAMWORKS
					if (shim::steam_init_failed == false) {
						delete[] origins;
					}
#endif

				}
			}
			else {
				auto p = static_cast<Battle_Player *>(selected);
				pos = p->get_draw_pos();
				pos.x -= cursor->size.w;
				pos.y -= shim::tile_size/4.0f;
				gfx::Sprite *sprite = selected->get_sprite();
				gfx::Image *img = sprite->get_current_image();
				util::Point<int> topleft, bottomright;
				img->get_bounds(topleft, bottomright);
				pos.x += topleft.x;
				pos.x -= shim::tile_size/4.0f;
				flag = 0;
			}

			if (sneak_attack) {
				pos.x = shim::screen_size.w - pos.x - cursor->size.w;
				if (flag == 0) {
					flag = gfx::Image::FLIP_H;
				}
				else {
					flag = 0;
				}
			}
			std::vector< std::pair<util::Point<float>, int> > v;
			v.push_back({pos, flag});
			combo_drawer->set_cursors(v);
		}
	}

	if (simple_turn_display == false && GLOBALS->dialogue_active(BATTLE) == false) {
		std::vector<wedge::Battle_Game::Turn *> upcoming_turns = active_turns;
		upcoming_turns.insert(upcoming_turns.end(), turns.begin(), turns.end());

		for (auto it = upcoming_turns.begin(); it != upcoming_turns.end();) {
			auto t = *it;
			if (std::find(entities.begin(), entities.end(), t->actor) == entities.end()) {
				it = upcoming_turns.erase(it);
			}
			else {
				it++;
			}
		}

		if (upcoming_turns.size() > 0) {
			int i = 1;
			int max = int(get_players().size() + get_enemies().size());
			std::map< wedge::Battle_Entity *, std::vector<int> > m;
			for (auto t : upcoming_turns) {
				/*
				if (t->turn_type == wedge::Battle_Game::DIALOGUE) {
					continue;
				}
				*/
				auto e = t->actor;
				auto it = m.find(e);
				if (it == m.end()) {
					std::vector<int> v;
					v.push_back(i);
					m[e] = v;
				}
				else {
					// if using numbers (1, 2, 3) uncomment below. if using dots, keep this commented
					auto &v = *it;
					v.second.push_back(i);
				}
				if (i == max) {
					break;
				}
				i++;
			}
			for (auto &p : m) {
				auto e = p.first;
				auto &v = p.second;
				std::string text;
				for (size_t i = 0; i < v.size(); i++) {
					int n = v[i];
					/*
					for (int i = 0; i < n; i++) {
						text += ".";
					}
					*/
					// if using numbers (1, 2, 3) uncomment below. if using dots, keep this commented
					text += util::itos(n);
					if (i+1 < v.size()) {
						text += ",";
					}
				}
				int tw = shim::font->get_text_width(text);
				int th = shim::font->get_height();
				auto player = dynamic_cast<Battle_Player *>(e);
				util::Point<float> pos;
				bool skip = false;
				if (player) {
					pos = player->get_sprite_pos();
					pos.x -= tw;
					gfx::Sprite *sprite = player->get_sprite();
					gfx::Image *img = sprite->get_current_image();
					util::Point<int> topleft, bottomright;
					img->get_bounds(topleft, bottomright);
					pos.x += topleft.x;
					pos.x -= shim::tile_size/4.0f;
					sprite->get_bounds(topleft, bottomright);
					pos.y += (bottomright.y-topleft.y)/2.0f-th/2.0f;
				}
				else {
					auto enemy = dynamic_cast<Battle_Enemy *>(e);
					gfx::Sprite *sprite = e->get_sprite();
					auto img = sprite->get_current_image();
					util::Point<int> topleft, bottomright;
					img->get_bounds(topleft, bottomright);
					// check for blank frames and skip turn order display if it's blank
					if (topleft.x >= bottomright.x || topleft.y >= bottomright.y) {
						skip = true;
					}
					if (enemy->get_draw_as_player()) {
						pos = enemy->get_sprite_pos();
					}
					else {
						pos = enemy->get_position();
					}
					pos += enemy->get_draw_as_player() ? util::Point<int>(img->size.w-topleft.x-(bottomright.x-topleft.x), topleft.y): topleft;
					pos += util::Point<int>((bottomright.x-topleft.x)/2.0f, (bottomright.y-topleft.y)/2.0f);
					pos -= util::Point<int>(tw/2, th/2);
					pos.y += 4;
					pos += enemy->get_turn_order_display_offset();
					/*
					pos.x += (bottomright.x - topleft.x);
					pos.x += shim::tile_size / 4;
					pos.y += (bottomright.y-topleft.y)/2-th/4;
					*/
				}
				if (skip == false) {
					if (sneak_attack) {
						pos.x = shim::screen_size.w - pos.x - tw;
					}
					shim::font->enable_shadow(shim::black, gfx::Font::FULL_SHADOW);
					shim::font->draw(shim::palette[36], text, pos);
					shim::font->disable_shadow();
				}
			}
		}

		for (auto p : started_turns) {
			gfx::Image *arrow = p.second->get_current_image();
			auto player = dynamic_cast<Battle_Player *>(p.first);
			util::Point<float> pos;
			if (player) {
				pos = player->get_sprite_pos();
				gfx::Sprite *sprite = player->get_sprite();
				gfx::Image *img = sprite->get_current_image();
				util::Point<int> topleft, bottomright;
				img->get_bounds(topleft, bottomright);
				//pos.x += topleft.x;
				pos += topleft;
				pos.x += (bottomright.x-topleft.x)/2.0f;
				pos.x -= arrow->size.w/2.0f;
				pos.y -= (arrow->size.h+1);
			}
			else {
				auto enemy = dynamic_cast<Battle_Enemy *>(p.first);
				if (enemy->get_draw_as_player()) {
					pos = enemy->get_sprite_pos();
				}
				else {
					pos = enemy->get_position();
				}
				gfx::Sprite *sprite = enemy->get_sprite();
				gfx::Image *img = sprite->get_current_image();
				util::Point<int> topleft, bottomright;
				img->get_bounds(topleft, bottomright);
				pos.x += enemy->get_draw_as_player() ? img->size.w-topleft.x-(bottomright.x-topleft.x) : topleft.x;
				pos.y += topleft.y;
				pos.x += (bottomright.x-topleft.x)/2.0f;
				pos.x -= arrow->size.w/2.0f;
				pos.y -= (arrow->size.h+1);
			}
			if (sneak_attack) {
				pos.x = shim::screen_size.w - pos.x - arrow->size.w;
			}
			pos.x += 0.5f; // arrow sprite frames are 20px even though only fills 19 - this centres it better
			arrow->draw(pos);
		}
	}

	gfx::Font::end_batches();

	const int EDGE_X = shim::screen_size.w * 0.05f;
	const int EDGE_Y = shim::screen_size.h * 0.05f;

	int w = (shim::screen_size.w - EDGE_X*2) / 2;
	int h = shim::font->get_height() * 3 + 6; // 6 = border
	int y = shim::screen_size.h-EDGE_Y-h;

	for (size_t i = 0; i < sorted_entities.size(); i++) {
		wedge::Battle_Entity *entity = sorted_entities[i];
		entity->draw_fore();
	}

	if (has_darkness) {
		real_draw_darkness(darkness_image1, darkness_image2, darkness_offset1, darkness_offset2, {0.0f, 0.0f}, 0.5f);
	}

	if (window_shown) {
		gfx::draw_9patch(DOGO_GLOBALS->battle_window, util::Point<int>(EDGE_X, y), util::Size<int>(w*2, h));

		if (enemy_stats_shown || player_stats_shown) {
			gfx::draw_line(shim::palette[25], util::Point<int>(EDGE_X+w-1, y+3), util::Point<int>(EDGE_X+w-1, y+h-3));
			gfx::draw_line(shim::palette[34], util::Point<int>(EDGE_X+w, y+2), util::Point<int>(EDGE_X+w, y+h-2));
			gfx::draw_line(shim::palette[23], util::Point<int>(EDGE_X+w+1, y+3), util::Point<int>(EDGE_X+w+1, y+h-3));
		}

		std::map<std::string, int> enemies;

		for (size_t i = 0; i < entities.size(); i++) {
			if (entities[i]->get_type() == wedge::Battle_Entity::ENEMY) {
				wedge::Battle_Enemy *enemy = static_cast<wedge::Battle_Enemy *>(entities[i]);
				enemies[enemy->get_name()]++;
			}
		}
		
		shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);

		int o = 4 + ((h - 6) / 3) / 2 - shim::font->get_height() / 2;
		if (enemy_stats_shown) {
			for (auto &p : enemies) {
				shim::font->draw(shim::white, p.first, util::Point<int>(EDGE_X+WIN_BORDER, y+o));
				std::string num = util::itos(p.second);
				shim::font->draw(shim::white, num, util::Point<int>(EDGE_X+w-WIN_BORDER-shim::font->get_text_width(num), y+o));
				o += (h - 6) / 3;
			}
		}

		std::vector<wedge::Battle_Entity *> players = get_players();

		// Use the maximum possible value, 3 digits
		std::string sp_text = GLOBALS->game_t->translate(145)/* Originally: SP */;
		std::string sp = util::itos(888);
		int max_sp_len = shim::font->get_text_width(sp)+shim::font->get_text_width(sp_text + ":");
#if 0
		int max_sp_len = 0;

		for (auto &p : players) {
			auto s = p->get_stats();
			std::string sp_text = GLOBALS->game_t->translate(145)/* Originally: SP */;
			std::string sp = util::itos(s->mp);
			int len = shim::font->get_text_width(sp)+shim::font->get_text_width(sp_text + ":");
			if (len > max_sp_len) {
				max_sp_len = len;
			}
		}
#endif

		int index = 0;
		o = 4 + ((h - 6) / 3) / 2 - shim::font->get_height() / 2;
		if (player_stats_shown) {
			for (auto &p : players) {
				SDL_Colour text_colour;
				bool do_shadow;
				if (index == active_player && getting_turn) {
					DOGO_GLOBALS->yellow_frame->draw(util::Point<int>(EDGE_X+w+WIN_BORDER-2, y+o));
					text_colour = shim::palette[8];
					do_shadow = false;
				}
				else {
					text_colour = shim::white;
					do_shadow = true;
				}
				gfx::Image *mini_pic = DOGO_GLOBALS->mini_pics[util::lowercase(INSTANCE->stats[index].base.get_name())];
				if (do_shadow) {
					draw_tinted_shadowed_image(shim::white, shim::palette[23], mini_pic, util::Point<int>(EDGE_X+w+WIN_BORDER, y+o+2), gfx::Font::DROP_SHADOW);
				}
				else {
					mini_pic->draw(util::Point<int>(EDGE_X+w+WIN_BORDER, y+o+2));
				}
				std::string hp_text = GLOBALS->game_t->translate(144)/* Originally: HP */;
				std::string sp_text = GLOBALS->game_t->translate(145)/* Originally: SP */;
				auto s = p->get_stats();
				float hp_p = s->hp / (float)s->fixed.max_hp;
				float sp_p = s->mp / (float)s->fixed.max_mp;
				SDL_Colour hp_colour;
				SDL_Colour mp_colour;
				SDL_Colour ignored;
				get_gauge_colour(hp_p, hp_colour, ignored);
				get_gauge_colour(sp_p, mp_colour, ignored);
				int full_len = shim::font->get_text_width(hp_text + ":") + 2 + shim::font->get_text_width(util::itos(s->fixed.max_hp));
				int line_w;
				line_w = full_len*hp_p;
				if (line_w < 1 && s->hp > 0) {
					line_w = 1;
				}
				gfx::draw_line(hp_colour, util::Point<int>(EDGE_X+w+WIN_BORDER+mini_pic->size.w+3, y+o), util::Point<int>(EDGE_X+w+WIN_BORDER+mini_pic->size.w+3+line_w, y+o));
				shim::font->draw(text_colour, hp_text + ":", util::Point<int>(EDGE_X+w+WIN_BORDER+mini_pic->size.w+3, y+o));
				shim::font->draw(text_colour, util::itos(s->hp), util::Point<int>(EDGE_X+w+WIN_BORDER+mini_pic->size.w+3+shim::font->get_text_width(hp_text + ":")+2, y+o));
				std::string sp = util::itos(s->mp);
				shim::font->draw(text_colour, sp, util::Point<int>(shim::screen_size.w-EDGE_X-WIN_BORDER-max_sp_len+shim::font->get_text_width(sp_text+":"), y+o));
				full_len = shim::font->get_text_width(sp_text + ":") + 2 + shim::font->get_text_width(util::itos(s->fixed.max_mp));
				line_w = full_len*sp_p;
				if (line_w < 1 && s->mp > 0) {
					line_w = 1;
				}
				gfx::draw_line(mp_colour, util::Point<int>(shim::screen_size.w-EDGE_X-WIN_BORDER-max_sp_len-2, y+o), util::Point<int>(shim::screen_size.w-EDGE_X-WIN_BORDER-max_sp_len-2+line_w, y+o));
				shim::font->draw(text_colour, sp_text + ":", util::Point<int>(shim::screen_size.w-EDGE_X-WIN_BORDER-max_sp_len-2, y+o));
				index++;
				o += (h - 6) / 3;
			}
		}

		shim::font->disable_shadow();
	}

	for (auto it = notifications.begin(); it != notifications.end();) {
		const int win_border_x = 5;
		const int win_border_y = 3;
		Notification n = *it;
		util::Point<int> pos;
		util::Size<int> size;
		size.h = shim::font->get_height() + win_border_y * 2;
		size.w = shim::font->get_text_width(n.text) + win_border_x * 2;
		if (n.position == LEFT) {
			pos.x = shim::screen_size.w * 0.03f;
		}
		else {
			pos.x = shim::screen_size.w - shim::screen_size.w * 0.03f - size.w;
		}
		pos.y = shim::screen_size.h * 0.03f;
		gfx::draw_9patch(DOGO_GLOBALS->gui_window, pos, size);
		shim::font->draw(shim::white, n.text, pos+util::Point<int>(win_border_x, win_border_y));
		if (GET_TICKS() > n.death_time) {
			it = notifications.erase(it);
		}
		else {
			it++;
		}
	}

	Game::draw();

	std::vector<wedge::Battle_Game::Turn *> t = active_turns;
	t.insert(t.end(), turns.begin(), turns.end());

	for (auto it = t.begin(); it != t.end();) {
		wedge::Battle_Game::Turn *turn = *it;
		// Enemy could have been deleted/killed
		if (std::find(entities.begin(), entities.end(), turn->actor) == entities.end()) {
			it = t.erase(it);
		}
		else {
			it++;
		}
	}

	if (simple_turn_display) {
		int nt = (int)t.size();

		if (nt > 0) {
			const int nvis = 3;
		
			int full = MIN(nvis, nt);

			int tw = full * DOGO_GLOBALS->mini_frame->size.w - (full-1) * 2 + 2 * (nt-full);

			int start_x = shim::screen_size.w / 2 - tw/2;

			if (nt > nvis) {
				int count = 0;
				for (int i = nt-1; i >= nvis; i--) {
					gfx::Image *img;
					if (i < (int)active_turns.size()) {
						img = DOGO_GLOBALS->active_frame;
					}
					else {
						img = DOGO_GLOBALS->mini_frame;
					}
					img->draw(util::Point<int>(start_x + tw - img->size.w - 2*count, shim::screen_size.h*0.05f));
					count++;
				}
			}

			for (int i = MIN(nvis-1, nt-1); i >= 0; i--) {
				int x = start_x + i * (DOGO_GLOBALS->mini_frame->size.w-2);
				gfx::Image *img;
				if (i < (int)active_turns.size()) {
					img = DOGO_GLOBALS->active_frame;
				}
				else {
					img = DOGO_GLOBALS->mini_frame;
				}
				img->draw(util::Point<int>(x, shim::screen_size.h*0.05f));

				if (t[i]->actor->get_type() == wedge::Battle_Entity::PLAYER) {
					std::string name = util::lowercase(t[i]->actor->get_name());
					gfx::Image *mini = DOGO_GLOBALS->mini_pics[name];
					mini->draw(util::Point<int>(x+2, shim::screen_size.h*0.05f+2));
				}
				else {
					std::string name = t[i]->actor->get_name();
					std::string first = util::utf8_substr(name, 0, 1);
					SDL_Colour colour;
					if (i < (int)active_turns.size()) {
						colour = shim::palette[5];
					}
					else {
						colour = shim::white;
					}
					shim::font->draw(colour, first, util::Point<int>(x+img->size.w/2-shim::font->get_text_width(first)/2, shim::screen_size.h*0.05f+img->size.h/2-shim::font->get_height()/2));
				}
			}
		}
	}

	if (detecting_combo) {
		Combo c;
		for (auto d : detector->get_detected()) {
			Combo_Event e;
			e.button = d.button;
			c.push_back(e);
		}

		combo_drawer->set(c);
	}
}

wedge::Battle_Game::Turn *Battle_Game::get_turn(Battle_Player *player)
{
	if (player->get_sprite()->get_animation() == "attack_defend") {
		return nullptr;
	}

	if (GLOBALS->is_mini_paused()) {
		// can be here if sneak attack notification comes up
		return nullptr;
	}

	auto v = get_players();
	bool all_dead = true;
	for (auto p : v) {
		if (p->get_stats()->hp > 0) {
			all_dead = false;
			break;
		}
	}
	if (all_dead) {
		if (getting_turn) {
			getting_turn = false;
			if (turn_gui_gone == false) {
				turn_gui->exit();
				turn_gui_gone = true;
			}
			enemy_stats_shown = true;
			player_stats_shown = true;
			if (item_gui) {
				item_gui->exit();
				item_gui = nullptr;
			}
			if (easy_gui) {
				easy_gui->exit();
				easy_gui = nullptr;
			}
			if (mc_gui) {
				mc_gui->exit();
				mc_gui = nullptr;
			}
			if (pstats) {
				pstats->exit();
				pstats = nullptr;
			}
			end_combo();
			good_combo = -1;
			remove_notification(RIGHT);
		}
		return nullptr;
	}
		
	int index = get_player_index(player);

	auto p = dynamic_cast<Battle_Player *>(v[active_player]);
	// If active player has died...
	if (p->get_stats()->hp <= 0) {
		next_player(-1, true);
		getting_turn = false;
		if (turn_gui_gone == false) {
			turn_gui->exit();
			turn_gui_gone = true;
		}
		enemy_stats_shown = true;
		player_stats_shown = true;
		if (item_gui) {
			item_gui->exit();
			item_gui = nullptr;
		}
		if (easy_gui) {
			easy_gui->exit();
			easy_gui = nullptr;
		}
		if (mc_gui) {
			mc_gui->exit();
			mc_gui = nullptr;
		}
		if (pstats) {
			pstats->exit();
			pstats = nullptr;
		}
		end_combo();
		good_combo = -1;
		remove_notification(RIGHT);
		// this return moved into if
		return nullptr;
	}

	if (index != active_player) {
		return nullptr;
	}

	if (next_turn != nullptr) {
		// Need to set next_turn to nullptr
		wedge::Battle_Game::Turn *t = next_turn;
		next_turn = nullptr;
		getting_turn = false;
		return t;
	}
		
	const int EDGE_X = shim::screen_size.w * 0.05f;
	const int EDGE_Y = shim::screen_size.h * 0.05f;

	if (getting_turn == false) {
		getting_turn = true;
		enemy_stats_shown = false;
		turn_type = wedge::Battle_Game::NONE;
	
		std::vector<std::string> choices;
		choices.push_back(GLOBALS->game_t->translate(169)/* Originally: Combo */);
		choices.push_back(GLOBALS->game_t->translate(170)/* Originally: Item */);
		//if (false) {//boss_battle == false) {
		//	choices.push_back(GLOBALS->game_t->translate(172)/* Originally: Run */);
		//}
		choices.push_back(GLOBALS->game_t->translate(181)/* Originally: Info */);
		turn_gui = new Positioned_Multiple_Choice_GUI(false, "", choices, -1, -1, 1, EDGE_X, 0, 0, EDGE_Y, 0, 0, turn_callback, this, 3, 1, true,  0.571f, NULL, false, 3, INSTANCE->is_milestone_complete(MS_DID_AN_ATTACK) == false);
		turn_gui->set_transition(false);
		Widget_List *list = turn_gui->get_list();
		list->set_text_shadow_colour(shim::palette[23]);
		list->set_row_h(shim::font->get_height());
		//list->set_text_offset(util::Point<int>(0, 4));
		list->set_arrow_colour(shim::palette[8]);
		list->set_text_shadow_colour(shim::palette[23]);
		list->set_selected(next_turn_pos);
		list->set_top(next_turn_top);
		next_turn_pos = 0;
		next_turn_top = 0;
		turn_gui->resize(shim::screen_size); // Multiple choice guis always need a resize right away
		shim::guis.push_back(turn_gui);
		turn_gui_gone = false;
	}
	else if (turn_type == wedge::Battle_Game::ATTACK) {
		if (good_combo < 0) {
			if (easy_combos) {
				if (easy_gui == nullptr) {
					enemy_stats_shown = false;
					player_stats_shown = false;

					std::vector<int> quantities;
					std::vector<std::string> names;
					std::vector<std::string> descriptions; // keep this empty, we don't do descriptions in battle

					for (auto cmbo : DOGO_INSTANCE->combos[active_player]) {
						int cost = get_combo_cost(cmbo);
						quantities.push_back(cost);
						names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(cmbo)));
					}

					if (names.size() == 0) {
						getting_turn = false;
						gfx::add_notification(GLOBALS->game_t->translate(375)/* Originally: No combos... */);
						DOGO_GLOBALS->error->play(false);
						enemy_stats_shown = true;
						player_stats_shown = true;
					}
					else {
						Widget_Combo_List *list = new Widget_Combo_List(1.0f, 3 * shim::font->get_height());
						list->set_text_shadow_colour(shim::palette[23]);

						list->set_items_extra(quantities, names, descriptions);
						list->set_text_shadow_colour(shim::palette[23]);
						list->set_row_h(shim::font->get_height());
						//list->set_text_offset(util::Point<int>(0, 4));
						list->set_arrow_colour(shim::palette[8]);
						list->set_arrow_shadow_colour(shim::black);
						list->set_text_shadow_colour(shim::palette[23]);
						list->set_arrow_offsets(util::Point<int>(0, -5), util::Point<int>(0, 0));
						if (easy_combos && INSTANCE->is_milestone_complete(MS_DID_AN_ATTACK) == false) {
							std::vector<std::string> tips;
							tips.push_back(GLOBALS->game_t->translate(408)/* Originally: Pick a move! */);
							list->set_tips(tips);
						}

						int width = shim::screen_size.w - shim::screen_size.w * 0.1f - 3*2/*battle window border aka win_border */ - WIN_BORDER*2;

						std::vector<std::string> choices;
						choices.insert(choices.end(), names.begin(), names.end());
						easy_gui = new Positioned_Multiple_Choice_GUI(false, "", choices, -3, -1, 1, EDGE_X, 0, 0, EDGE_Y, 0, 0, easy_callback, this, 3, width, false,  0.75f, list, false, 3);
						easy_gui->set_transition(false);
						easy_gui->resize(shim::screen_size); // Multiple choice guis always need a resize right away
						shim::guis.push_back(easy_gui);
					}
				}
			}
			else {
				if (detecting_combo == false) {
					detecting_combo = true;
					GLOBALS->allow_global_mini_pause = false;
					GLOBALS->getting_combo = true;
					shim::convert_directions_to_focus_events = false;
					osc_enabled = GLOBALS->onscreen_controller_was_enabled;

					if (input::is_joystick_connected() == false && input::system_has_keyboard() == false) {
						GLOBALS->onscreen_controller_was_enabled = true;
						started_with_osc = true;
					}
					else {
						GLOBALS->onscreen_controller_was_enabled = false;
						started_with_osc = false;
					}
					wedge::set_onscreen_controller_generates_b1_b2(true);
					wedge::set_onscreen_controller_b2_enabled(true);
					wedge::set_onscreen_controller_b3_enabled(true, GLOBALS->key_b3);
					wedge::set_onscreen_controller_b4_enabled(true, GLOBALS->key_b4);

					combos.clear();

					for (size_t i = 0; i < DOGO_INSTANCE->combos[active_player].size(); i++) {
						std::string name = DOGO_INSTANCE->combos[active_player][i];
						Combo c = get_combo(name);
						combos.push_back(c);
					}

					detector = new Combo_Detector(combos);
				}
			}
		}
		else if (selecting == false) {
			end_combo();
			
			acting_player = player;

			int player_index = get_player_index(acting_player);
			std::string cmbo = DOGO_INSTANCE->combos[player_index][good_combo];
			std::string lower = util::lowercase(cmbo);

			if (lower == "defend") {
				getting_turn = false;

				if (get_num_turns(acting_player) > 0) {
					DOGO_GLOBALS->error->play(false);
					gfx::add_notification(GLOBALS->game_t->translate(573)/* Originally: Can't defend while in action... */);
				}
				else {
					DOGO_GLOBALS->defend->play(false);
					acting_player->get_sprite()->set_animation("attack_defend");
					acting_player->get_stats()->mp -= 100;
				}

				good_combo = -1;

				return nullptr;
			}
			else {
				if (get_combo_friendly(cmbo)) {
					selecting_enemy = false;
					selected = get_players()[0];
				}
				else {
					selecting_enemy = true;
					auto enemies = get_enemies();
					std::sort(enemies.begin(), enemies.end(), enemy_x_compare);
					if (enemies.size() > 1) {
						for (auto it = enemies.begin(); it != enemies.end();) {
							auto e = *it;
							if (e->get_stats()->hp >= 1000000) {
								it = enemies.erase(it);
							}
							else {
								it++;
							}
						}
					}
					selected = enemies[0];
				}

				selecting = true;

				selecting_multi = get_combo_multi(cmbo);

				if (selecting_multi) {
					if (wedge::is_onscreen_controller_enabled()) {
						wedge::set_onscreen_controller_b3_enabled(true, GLOBALS->key_confirm);
					}
					mc_gui = new Battle_Multi_Confirm_GUI(multi_callback, this);
					mc_gui->resize(shim::screen_size);
					shim::guis.push_back(mc_gui);
				}
			}
		}
		return nullptr;
	}
	else if (turn_type == wedge::Battle_Game::ITEM) {
		if (item_gui == nullptr && item_index < 0) {
			enemy_stats_shown = false;
			player_stats_shown = false;

			inventory_indices.clear();

			std::vector<int> quantities;
			std::vector<std::string> names;
			std::vector<std::string> descriptions; // keep this empty, we don't do descriptions in battle

			wedge::Object *inv = INSTANCE->inventory.get_all();

			for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
				if (inv[i].type == wedge::OBJECT_ITEM) {
					if (inv[i].quantity > inv[i].used) {
						inventory_indices.push_back(i);
						quantities.push_back(inv[i].quantity - inv[i].used);
						names.push_back(inv[i].name);
					}
				}
			}

			if (names.size() == 0) {
				getting_turn = false;
				gfx::add_notification(GLOBALS->game_t->translate(174)/* Originally: Inventory empty... */);
				DOGO_GLOBALS->error->play(false);
				enemy_stats_shown = true;
				player_stats_shown = true;
			}
			else {
				Widget_Quantity_List *list = new Widget_Quantity_List(1.0f, 3 * shim::font->get_height());
				list->set_text_shadow_colour(shim::palette[23]);

				list->set_items_extra(quantities, names, descriptions);
				list->set_text_shadow_colour(shim::palette[23]);
				list->set_row_h(shim::font->get_height());
				//list->set_text_offset(util::Point<int>(0, 4));
				list->set_arrow_colour(shim::palette[8]);
				list->set_arrow_shadow_colour(shim::black);
				list->set_text_shadow_colour(shim::palette[23]);
				list->set_arrow_offsets(util::Point<int>(0, -5), util::Point<int>(0, 0));
		
				int width = shim::screen_size.w - shim::screen_size.w * 0.1f - 3*2/*battle window border aka win_border */ - WIN_BORDER*2;

				std::vector<std::string> choices;
				choices.insert(choices.end(), names.begin(), names.end());
				item_gui = new Positioned_Multiple_Choice_GUI(false, "", choices, -3, -1, 1, EDGE_X, 0, 0, EDGE_Y, 0, 0, item_callback, this, 3, width, false,  0.571f, list, false, 3);
				item_gui->set_transition(false);
				item_gui->resize(shim::screen_size); // Multiple choice guis always need a resize right away
				shim::guis.push_back(item_gui);
			}
		}
		else if (item_index >= 0 && selecting == false) {
			wedge::Object *inv = INSTANCE->inventory.get_all();
			int i = inventory_indices[item_index];
			if (inv[i].id == ITEM_RANDOMAXE) {
				auto enemies = get_enemies();
				std::sort(enemies.begin(), enemies.end(), enemy_x_compare);
				std::vector<wedge::Battle_Entity *> v;
				for (auto e : enemies) {
					// FIXME: Randomaxe don't work if have massive HP
					// Bumper has 2000 max hp, no other enemy will have that much
					if (e->get_stats()->hp > 0 && e->get_stats()->fixed.max_hp < 2000) {
						v.push_back(e);
					}
					else if (dynamic_cast<Enemy_BigTuna *>(e)) {
						v.push_back(e);
					}
				}

				if (v.size() > 0) {
					inv[i].used++;

					auto t = new wedge::Battle_Game::Turn;

					t->actor = player;
					t->turn_type = wedge::Battle_Game::ITEM;
					t->turn_name = util::itos(i);
					wedge::Object *inv = INSTANCE->inventory.get_all();
					add_notification(RIGHT, inv[i].name);
					t->targets.push_back(v[util::rand(0, int(v.size()-1))]);
					t->started = false;

					item_index = -1;

					getting_turn = false;

					return t;
				}
				else {
					item_index = -1;
					getting_turn = false;
					return nullptr;
				}
			}
			else if (inv[i].id == ITEM_NINJA_STAR) {
				inv[i].used++;

				auto t = new wedge::Battle_Game::Turn;

				t->actor = player;
				t->turn_type = wedge::Battle_Game::ITEM;
				t->turn_name = util::itos(i);
				wedge::Object *inv = INSTANCE->inventory.get_all();
				add_notification(RIGHT, inv[i].name);
				auto enemies = get_enemies();
				std::sort(enemies.begin(), enemies.end(), enemy_x_compare);
				for (auto e : enemies) {
					// FIXME: ninja stars don't work if have massive HP
					if (e->get_stats()->hp > 0 && e->get_stats()->hp < 1000000) {
						t->targets.push_back(e);
					}
				}
				t->started = false;

				item_index = -1;

				getting_turn = false;

				return t;
			}
			else if (inv[i].id == ITEM_DAGGER) {
				selecting = true;
				selecting_enemy = true;
				selecting_multi = false;
				auto enemies = get_enemies();
				std::sort(enemies.begin(), enemies.end(), enemy_x_compare);
				if (enemies.size() > 1) {
					for (auto it = enemies.begin(); it != enemies.end();) {
						auto e = *it;
						if (e->get_stats()->hp >= 1000000) {
							it = enemies.erase(it);
						}
						else {
							it++;
						}
					}
				}
				selected = enemies[0];
				acting_player = player;
			}
			else {
				selecting = true;
				selecting_enemy = false;
				selecting_multi = false;
				selected = get_players()[0];
				acting_player = player;
			}
		}
	}
	else if (turn_type == wedge::Battle_Game::RUN) {
		getting_turn = false;

		auto t = new wedge::Battle_Game::Turn;

		t->actor = player;
		t->turn_type = wedge::Battle_Game::RUN;
		t->turn_name = "";
		t->started = false;

		return t;
	}

	return nullptr;
}

void Battle_Game::set_turn(int type)
{
	turn_gui_gone = true;

	enemy_stats_shown = true;
	player_stats_shown = true;

	auto v = get_players();
	bool all_dead = true;
	for (auto p : v) {
		if (p->get_stats()->hp > 0) {
			all_dead = false;
			break;
		}
	}
	if (all_dead) {
		getting_turn = false;
		return;
	}

	// Adjust for missing 'Run' option
	if (/*boss_battle &&*/ type == 2) {
		type = 3;
	}

	if (type < 0) {
		getting_turn = false;
	}
	else if (type <= 2) {
		turn_type = (wedge::Battle_Game::Turn_Type)(type+1);
	}
	else {
		pstats = new Player_Stats_GUI(active_player, stats_callback, this);
		shim::guis.push_back(pstats);
		GLOBALS->allow_global_mini_pause = false;
	}
}

void Battle_Game::set_item(int item)
{
	item_gui = nullptr;

	enemy_stats_shown = true;
	player_stats_shown = true;

	auto v = get_players();
	bool all_dead = true;
	for (auto p : v) {
		if (p->get_stats()->hp > 0) {
			all_dead = false;
			break;
		}
	}
	if (all_dead) {
		getting_turn = false;
		return;
	}

	if (item < 0) {
		just_exited_item_gui = true;

		getting_turn = false;
		next_turn_pos = 1;
	}
	else {
		item_index = item;
		wedge::Object *inv = INSTANCE->inventory.get_all();
		add_notification(RIGHT, inv[item_index].name, 1000000000);
	}
}

void Battle_Game::set_easy(int easy)
{
	easy_gui = nullptr;

	enemy_stats_shown = true;
	player_stats_shown = true;

	auto v = get_players();
	bool all_dead = true;
	for (auto p : v) {
		if (p->get_stats()->hp > 0) {
			all_dead = false;
			break;
		}
	}
	if (all_dead) {
		getting_turn = false;
		return;
	}

	if (easy < 0) {
		just_exited_easy_gui = true;

		getting_turn = false;
		next_turn_pos = 0;
	}
	else {
		good_combo = easy;
		std::string cmbo = DOGO_INSTANCE->combos[active_player][good_combo];
		int cost = get_combo_cost(cmbo);
		if (cost > INSTANCE->stats[active_player].base.mp) {
			auto v = get_players();
			auto p = dynamic_cast<Battle_Player *>(v[active_player]);
			p->reset_mult_level();
			DOGO_GLOBALS->error->play(false);
			gfx::add_notification(util::string_printf(GLOBALS->game_t->translate(230)/* Originally: Not enough SP! Need %d! */.c_str(), cost));
			close_guis();
		}
		else {
			DOGO_GLOBALS->button->play(false);
			cmbo = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(cmbo));
			std::string lower = util::lowercase(cmbo);
			if (lower == "defend") {
				add_notification(RIGHT, cmbo, NOTIFICATION_LIFETIME);
			}
			else {
				add_notification(RIGHT, cmbo, 1000000000);
			}
		}
	}
}

void Battle_Game::handle_event(TGUI_Event *event)
{
	bool used = false;
	
	const int EDGE_X = shim::screen_size.w * 0.05f;
	const int EDGE_Y = shim::screen_size.h * 0.05f;
	int w = (shim::screen_size.w - EDGE_X*2) / 2;
	int h = shim::font->get_height() * 3 + 6; // 6 = border
	int y = shim::screen_size.h-EDGE_Y-h;
	int xx = EDGE_X + w;
	int yy = y + 3;

	if (selecting == false && pstats == nullptr) {
		auto v = get_players();
		int pcount = 0;
		for (auto p : v) {
			auto pos = static_cast<Battle_Player *>(p)->get_sprite_pos();
			auto img = p->get_sprite()->get_current_image();
			if (sneak_attack) {
				pos.x = shim::screen_size.w - pos.x - img->size.w;
			}
			if (event->type == TGUI_MOUSE_DOWN && event->mouse.x >= pos.x && event->mouse.y >= pos.y && event->mouse.x < pos.x+img->size.w && event->mouse.y < pos.y+img->size.h) {
				if (GLOBALS->dialogue_active(BATTLE) == false) {
					auto v = get_players();
					int max = v[active_player]->get_max_turns();
					int turn_count = 0;
					for (auto t : active_turns) {
						if (t->actor == v[active_player]) {
							turn_count++;
						}
					}
					for (auto t : turns) {
						if (t->actor == v[active_player]) {
							turn_count++;
						}
					}
					bool defending = v[active_player]->get_sprite()->get_animation() == "attack_defend";
					if ((getting_turn && turn_type == wedge::Battle_Game::NONE) || turn_count >= max || defending) {
						next_player(pcount);
					}
				}
			}
			pcount++;
		}
	}
	
	if (done == false) {
		if (pstats == nullptr && ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_switch) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_switch) || (selecting == false && detecting_combo == false && wedge::is_onscreen_controller_enabled() && event->type == TGUI_FOCUS && event->focus.type == TGUI_FOCUS_RIGHT))) {
			auto v = get_players();
			int max = v[active_player]->get_max_turns();
			int turn_count = 0;
			for (auto t : active_turns) {
				if (t->actor == v[active_player]) {
					turn_count++;
				}
			}
			for (auto t : turns) {
				if (t->actor == v[active_player]) {
					turn_count++;
				}
			}
			if ((getting_turn && turn_type == wedge::Battle_Game::NONE) || turn_count >= max || v[active_player]->get_sprite()->get_animation() == "attack_defend") {
				next_player();
			}
			used = true;
		}
		else if (selecting) {
			fixup_selection();
			if (selecting) {
				auto enemies = get_enemies();
				if (event->type == TGUI_MOUSE_DOWN) {
					std::sort(enemies.begin(), enemies.end(), entity_y_compare);
				}
				else {
					std::sort(enemies.begin(), enemies.end(), enemy_x_compare);
				}
				gfx::Image *multi_image = DOGO_GLOBALS->multi_check_checked->get_current_image();
				if (event->type == TGUI_MOUSE_DOWN) {
					bool go = false;
					if (selecting_enemy) {
						//for (size_t i = 0; i < enemies.size(); i++) {
						for (int i = (int)enemies.size()-1; i >= 0; i--) {
							wedge::Battle_Enemy *enemy = static_cast<wedge::Battle_Enemy *>(enemies[i]);
							util::Point<float> pos = enemy->get_position();
							gfx::Sprite *sprite = enemy->get_sprite();
							gfx::Image *img = sprite->get_current_image();
							util::Point<int> topleft, bottomright;
							img->get_bounds(topleft, bottomright);
							pos += topleft;
							util::Size<int> sz(bottomright.x-topleft.x, bottomright.y-topleft.y);
							int click_zone_inc = enemy->get_click_zone_inc();
							pos.x -= click_zone_inc;
							pos.y -= click_zone_inc;
							sz.w += click_zone_inc*2;
							sz.h += click_zone_inc*2;
							if (sneak_attack) {
								pos.x = shim::screen_size.w - pos.x - sz.w;
							}
							util::Point<int> m(event->mouse.x, event->mouse.y);
							if (m.x >= pos.x && m.y >= pos.y && m.x < pos.x+sz.w && m.y < pos.y+sz.h) {
								if (enemies[i]->get_stats()->hp < 1000000 || dynamic_cast<Enemy_Sensei1 *>(enemies[i])) {
									selected = enemies[i];
									go = true;
									break;
								}
							}

							if (selecting_multi) {
								pos = enemy->get_position();
								gfx::Sprite *sprite = enemy->get_sprite();
								gfx::Image *img = sprite->get_current_image();
								util::Point<int> topleft, bottomright;
								img->get_bounds(topleft, bottomright);
								//sprite->get_bounds(topleft, bottomright);
								pos += topleft;
								pos.x += (bottomright.x - topleft.x);
								pos.x += shim::tile_size / 4.0f;
								pos.y -= shim::tile_size/4.0f;
								if (sneak_attack) {
									pos.x = shim::screen_size.w - pos.x - multi_image->size.w;
								}
								if (m.x >= pos.x && m.y >= pos.y && m.x < pos.x+multi_image->size.w && m.y < pos.y+multi_image->size.h) {
									if (enemies[i]->get_stats()->hp < 1000000 || dynamic_cast<Enemy_Sensei1 *>(enemies[i])) {
										selected = enemies[i];
										go = true;
										break;
									}
								}
							}
						}
					}
					else {
						auto players = get_players();
						for (size_t i = 0; i < players.size(); i++) {
							wedge::Battle_Player *p = static_cast<wedge::Battle_Player *>(players[i]);
							util::Point<int> pos = p->get_draw_pos();
							gfx::Sprite *sprite = p->get_sprite();
							gfx::Image *img = sprite->get_current_image();
							if (sneak_attack) {
								pos.x = shim::screen_size.w - pos.x - img->size.w;
							}
							util::Point<int> m(event->mouse.x, event->mouse.y);
							if (m.x >= pos.x && m.y >= pos.y && m.x < pos.x+img->size.w && m.y < pos.y+img->size.h) {
								selected = players[i];
								go = true;
								break;
							}

							if (selecting_multi) {
								pos = p->get_draw_pos();
								pos.x -= multi_image->size.w;
								pos.y -= shim::tile_size/4.0f;
								gfx::Image *img = sprite->get_current_image();
								util::Point<int> topleft, bottomright;
								img->get_bounds(topleft, bottomright);
								pos.x += topleft.x;
								pos.x -= shim::tile_size/4.0f;
								if (sneak_attack) {
									pos.x = shim::screen_size.w - pos.x - multi_image->size.w;
								}
								if (m.x >= pos.x && m.y >= pos.y && m.x < pos.x+multi_image->size.w && m.y < pos.y+multi_image->size.h) {
									selected = players[i];
									go = true;
									break;
								}
							}
						}
					}
					if (go) {
						// Fake an event so the if below runs
						event->type = TGUI_KEY_DOWN;
						event->keyboard.code = GLOBALS->key_action;
						event->keyboard.simulated = true;
					}
				}
				if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_action) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_action)) {
					if (selecting_multi) {
						std::vector<wedge::Battle_Entity *>::iterator it;
						if ((it = std::find(multi_targets.begin(), multi_targets.end(), selected)) != multi_targets.end()) {
							multi_targets.erase(it);
						}
						else {
							multi_targets.push_back(selected);
						}
						DOGO_GLOBALS->button->play(false);
					}
					else {
						if (turn_type == wedge::Battle_Game::ATTACK) {
							if (INSTANCE->is_milestone_complete(MS_DID_AN_ATTACK) == false) {
								gfx::add_notification(GLOBALS->game_t->translate(409)/* Originally: Good job! */);
								INSTANCE->set_milestone_complete(MS_DID_AN_ATTACK, true);
							}

							auto t = new wedge::Battle_Game::Turn;

							t->actor = acting_player;
							t->turn_type = wedge::Battle_Game::ATTACK;
							int player_index = get_player_index(acting_player);
							std::string cmbo = DOGO_INSTANCE->combos[player_index][good_combo];
							std::string bak = cmbo;
							int pos;
							if ((pos = cmbo.find('\'')) != std::string::npos) {
								cmbo = cmbo.substr(0, pos) + cmbo.substr(pos+1);
							}
							int cost = get_combo_cost(cmbo);
							INSTANCE->stats[player_index].base.mp -= cost;
							t->turn_name = std::string("attack_") + util::lowercase(cmbo);
							for (size_t i = 0; i < t->turn_name.length(); i++) {
								if (t->turn_name[i] == ' ') {
									t->turn_name[i] = '_';
								}
							}
							add_notification(RIGHT, GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(bak)));
							good_combo = -1;
							t->targets.push_back(selected);
							t->started = false;

							next_turn = t;

							selecting = false;
							
							GLOBALS->button->play(false);
						}
						else if (turn_type == wedge::Battle_Game::ITEM) {
							auto t = new wedge::Battle_Game::Turn;
					
							t->actor = acting_player;
							t->turn_type = wedge::Battle_Game::ITEM;
							int i = inventory_indices[item_index];
							t->turn_name = util::itos(i);
							wedge::Object *inv = INSTANCE->inventory.get_all();
							inv[i].used++;
							add_notification(RIGHT, inv[i].name);
							t->targets.push_back(selected);
							t->started = false;

							next_turn = t;

							selecting = false;
							
							GLOBALS->button->play(false);

							item_index = -1;
						}
					}
					used = true;
				}
				else if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
					selecting = false;
					getting_turn = false;
					GLOBALS->button->play(false);
					item_gui = nullptr;
					easy_gui = nullptr;
					mc_gui = nullptr;
					if (turn_type == wedge::Battle_Game::ATTACK) {
						next_turn_pos = 0;
					}
					else {
						next_turn_pos = 1;
					}
					item_index = -1;
					good_combo = -1;
					used = true;
					remove_notification(RIGHT);
					if (wedge::is_onscreen_controller_enabled()) {
						wedge::set_onscreen_controller_b3_enabled(false, -1);
					}
				}
				else if (event->type == TGUI_FOCUS) {
					auto players = get_players();
					std::vector<wedge::Battle_Entity *> v;
					if (selecting_enemy) {
						v = enemies;
						if (v.size() > 1) {
							for (auto it = v.begin(); it != v.end();) {
								auto e = *it;
								if (e->get_stats()->hp >= 1000000) {
									it = v.erase(it);
								}
								else {
									it++;
								}
							}
						}
					}
					else {
						v = players;
					}
					int index = 0;
					for (size_t i = 0; i < v.size(); i++) {
						if (v[i] == selected) {
							index = (int)i;
							break;
						}
					}
					if (sneak_attack) {
						if (event->focus.type == TGUI_FOCUS_RIGHT || event->focus.type == TGUI_FOCUS_UP) {
							index += selecting_enemy ? 1 : -1;
						}
						else {
							index -= selecting_enemy ? 1 : -1;
						}
					}
					else {
						if (event->focus.type == TGUI_FOCUS_LEFT || event->focus.type == TGUI_FOCUS_UP) {
							index += selecting_enemy ? 1 : -1;
						}
						else {
							index -= selecting_enemy ? 1 : -1;
						}
					}
					if (index >= (int)v.size()) {
						index = 0;
					}
					if (index < 0) {
						index = int(v.size()-1);
					}
					selected = v[index];
					shim::widget_mml->play(false);
					used = true;
				}
			}
		}
		else if (event->type == TGUI_MOUSE_DOWN && event->mouse.x >= xx && event->mouse.y >= yy && event->mouse.x < xx+w && event->mouse.y < yy+shim::font->get_height()*3) {
			if (GLOBALS->dialogue_active(BATTLE) == false) {
				int row = (event->mouse.y - yy) / shim::font->get_height();
				if (row < (int)INSTANCE->stats.size()) {
					next_player(row);
				}
			}
		}
		else if (detecting_combo == false) {
			if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_pause) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_pause)) {
				if (just_exited_item_gui == false && just_exited_easy_gui == false && just_exited_multi_gui == false && just_exited_pstats == false) {
					used = true;
					GLOBALS->mini_pause();
				}
			}
		}
	}

	if (detecting_combo) {
		if (!(input::is_joystick_connected() && event->type == TGUI_KEY_DOWN && get_key_name(event->keyboard.code) == "")) {
			if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_cancel) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_cancel)) {
				getting_turn = false;

				end_combo();
				DOGO_GLOBALS->button->play(false);
				combo_drawer->end_now();
				next_turn_pos = 0;
			}
			else {
				int nok = detector->num_ok();
				detector->handle_event(event);
				detector->check();
				int nok2 = detector->num_ok();
				if (nok2 > nok) {
					/*
					auto v = get_players();
					auto p = dynamic_cast<Battle_Player *>(v[active_player]);
					p->inc_mult_level();
					*/
				}
				if (detector->error()) {
					auto v = get_players();
					auto p = dynamic_cast<Battle_Player *>(v[active_player]);
					p->reset_mult_level();
					good_combo = -1;
					Uint32 now = GET_TICKS();
					Uint32 elap = now - DOGO_GLOBALS->last_error_play;
					if ((int)elap >= 500) {
						DOGO_GLOBALS->last_error_play = now;
						DOGO_GLOBALS->error->play(false);
					}
					detector->reset();
					combo_drawer->next_caption();
				}
				else if (detector->good() != -1) {
					good_combo = detector->good();
					std::string cmbo = DOGO_INSTANCE->combos[active_player][good_combo];
					int cost = get_combo_cost(cmbo);
					if (cost > INSTANCE->stats[active_player].base.mp) {
						auto v = get_players();
						auto p = dynamic_cast<Battle_Player *>(v[active_player]);
						p->reset_mult_level();
						detector->reset();
						DOGO_GLOBALS->error->play(false);
						gfx::add_notification(util::string_printf(GLOBALS->game_t->translate(230)/* Originally: Not enough SP! Need %d! */.c_str(), cost));
						close_guis();
					}
					else {
						DOGO_GLOBALS->error->stop();
						DOGO_GLOBALS->combo->play(false);
						detector->reset();
						end_combo();
						DOGO_GLOBALS->button->play(false);
						std::string cmbo_t = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(cmbo));
						combo_drawer->set(cmbo);
						add_notification(RIGHT, cmbo_t, 1000000000);
					}
				}
			}
		}
	}
	else if (used == false) {
		Game::handle_event(event);
	}

	just_exited_item_gui = false;
	just_exited_easy_gui = false;
	just_exited_multi_gui = false;
	just_exited_pstats = false;
}

bool Battle_Game::run()
{
	if (detecting_combo &&
		((started_with_osc && (input::is_joystick_connected() || input::system_has_keyboard())) ||
		(started_with_osc == false && (input::is_joystick_connected() == false && input::system_has_keyboard() == false)))
	) {
		TGUI_Event e;
		e.type = TGUI_KEY_DOWN;
		e.keyboard.code = GLOBALS->key_cancel;
		e.keyboard.is_repeat = false;
		e.keyboard.simulated = true;
		shim::push_event(e);
		e.type = TGUI_KEY_UP;
		shim::push_event(e);
	}

	if (detecting_combo) {
		detector->check();
		if (detector->error()) {
			auto v = get_players();
			auto p = dynamic_cast<Battle_Player *>(v[active_player]);
			p->reset_mult_level();
			good_combo = -1;
			Uint32 now = GET_TICKS();
			Uint32 elap = now - DOGO_GLOBALS->last_error_play;
			if ((int)elap >= 500) {
				DOGO_GLOBALS->last_error_play = now;
				DOGO_GLOBALS->error->play(false);
			}
			detector->reset();
			combo_drawer->next_caption();
		}
		else if (detector->good() != -1) {
			good_combo = detector->good();
			std::string cmbo = DOGO_INSTANCE->combos[active_player][good_combo];
			int cost = get_combo_cost(cmbo);
			if (cost > INSTANCE->stats[active_player].base.mp) {
				auto v = get_players();
				auto p = dynamic_cast<Battle_Player *>(v[active_player]);
				p->reset_mult_level();
				detector->reset();
				DOGO_GLOBALS->error->play(false);
				gfx::add_notification(util::string_printf(GLOBALS->game_t->translate(230)/* Originally: Not enough SP! Need %d! */.c_str(), cost));
				close_guis();
			}
			else {
				DOGO_GLOBALS->error->stop();
				DOGO_GLOBALS->combo->play(false);
				detector->reset();
				end_combo();
				DOGO_GLOBALS->button->play(false);
				cmbo = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(cmbo));
				combo_drawer->set(cmbo);
				add_notification(RIGHT, cmbo, 1000000000);
			}
		}
	}

	if (done == false) {
		int max = 0;
		for (auto &s : INSTANCE->stats) {
			if (s.base.fixed.max_mp > max) {
				max = s.base.fixed.max_mp;
			}
		}
		// Fill all SP after 1 minute
		int inc = 1000000 / 60 / 60 * max;
		DOGO_INSTANCE->sp_replenish_count += inc;
	}

	bool running = false;
	for (auto p : get_players()) {
		if (static_cast<Battle_Player *>(p)->is_running()) {
			running = true;
			break;
		}
	}
	if (running) {
		if (getting_turn) {
			if (turn_gui_gone == false) {
				turn_gui->exit();
				turn_gui_gone = true;
			}
			getting_turn = false;
		}
		if (item_gui) {
			item_gui->exit();
			item_gui = nullptr;
		}
		if (easy_gui) {
			easy_gui->exit();
			easy_gui = nullptr;
		}
		if (mc_gui) {
			mc_gui->exit();
			mc_gui = nullptr;
		}
		if (pstats) {
			pstats->exit();
			pstats = nullptr;
		}
		end_combo();
		good_combo = -1;
		remove_notification(LEFT);
		remove_notification(RIGHT);
	}

	bool all_dead = true;
	auto v = get_enemies();
	for (auto &e : v) {
		if (e->get_stats()->hp > 0) {
			all_dead = false;
		}
	}
	if (all_dead == false) {
		all_dead = true;
		auto v = get_players();
		for (auto &e : v) {
			if (e->get_stats()->hp > 0) {
				all_dead = false;
			}
		}
	}

	if (all_dead) {
		do_all_dead();
	}

	bool ret = wedge::Battle_Game::run();
	if (ret == false) {
		save_play_time();
		return ret;
	}

	if (selecting) {
		fixup_selection();
	}
	else if (getting_turn && (turn_type == wedge::Battle_Game::NONE || selecting || detecting_combo || item_gui != nullptr || easy_gui != nullptr || mc_gui != nullptr)) {
		auto v = get_players();
		auto p = dynamic_cast<Battle_Player *>(v[active_player]);
		// If active player has died...
		if (p->get_stats()->hp <= 0) {
			next_player(-1, true);
			getting_turn = false;
			if (turn_gui_gone == false) {
				turn_gui->exit();
				turn_gui_gone = true;
			}
			enemy_stats_shown = true;
			player_stats_shown = true;
			if (item_gui) {
				item_gui->exit();
				item_gui = nullptr;
			}
			if (easy_gui) {
				easy_gui->exit();
				easy_gui = nullptr;
			}
			if (mc_gui) {
				mc_gui->exit();
				mc_gui = nullptr;
			}
			if (pstats) {
				pstats->exit();
				pstats = nullptr;
			}
			end_combo();
			good_combo = -1;
			remove_notification(RIGHT);
		}
		else {
			auto v2 = get_enemies();
			bool all_enemies_dead = v2.size() == 0;
			if (all_enemies_dead == false) {
				all_enemies_dead = true;
				for (auto e : v2) {
					if (e->get_stats()->hp > 0) {
						all_enemies_dead = false;
						break;
					}
				}
			}
			// not else
			if (all_enemies_dead) {
				getting_turn = false;
				if (turn_gui_gone == false) {
					turn_gui->exit();
					turn_gui_gone = true;
				}
				enemy_stats_shown = true;
				player_stats_shown = true;
				if (item_gui) {
					item_gui->exit();
					item_gui = nullptr;
				}
				if (easy_gui) {
					easy_gui->exit();
					easy_gui = nullptr;
				}
				if (mc_gui) {
					mc_gui->exit();
					mc_gui = nullptr;
				}
				if (pstats) {
					pstats->exit();
					pstats = nullptr;
				}
				end_combo();
				good_combo = -1;
			}
		}
	}

	int st_count = 0;
	while (st_count < (int)started_turns_to_delete.size()) {
		gfx::Sprite *s = started_turns_to_delete[st_count];
		bool found = false;
		for (auto it = started_turns.begin(); it != started_turns.end(); it++) {
			auto p = *it;
			if (p.second == s) {
				started_turns.erase(it);
				delete s;
				found = true;
				break;
			}
		}
		if (found == false) {
			st_count++;
		}
	}

	started_turns_to_delete.clear();

	return true;
}

void Battle_Game::fixup_selection()
{
	if (selecting) {
		auto players = get_players();
		auto p = dynamic_cast<Battle_Player *>(players[active_player]);
		if (p->get_stats()->hp <= 0) {
			next_player();
			if (selecting) {
				selecting = false;
				getting_turn = false;
				if (wedge::is_onscreen_controller_enabled()) {
					wedge::set_onscreen_controller_b3_enabled(false, -1);
				}
			}
			if (item_gui) {
				item_gui->exit();
				item_gui = nullptr;
				getting_turn = false;
				selecting = false;
			}
			if (easy_gui) {
				easy_gui->exit();
				easy_gui = nullptr;
				getting_turn = false;
				selecting = false;
			}
			if (mc_gui) {
				mc_gui->exit();
				mc_gui = nullptr;
				getting_turn = false;
				selecting = false;
			}
			if (pstats) {
				pstats->exit();
				pstats = nullptr;
			}
			end_combo();
			good_combo = -1;
			remove_notification(RIGHT);
		}
		else if (selecting_enemy) {
			auto enemies = get_enemies();
			std::sort(enemies.begin(), enemies.end(), enemy_x_compare);
			if (enemies.size() > 1) {
				for (auto it = enemies.begin(); it != enemies.end();) {
					auto e = *it;
					if (e->get_stats()->hp >= 1000000) {
						it = enemies.erase(it);
					}
					else {
						it++;
					}
				}
			}
			if (enemies.size() == 0) {
				selecting = false;
				turn_type = wedge::Battle_Game::NONE;
				if (wedge::is_onscreen_controller_enabled()) {
					wedge::set_onscreen_controller_b3_enabled(false, -1);
				}
			}
			else {
				if (std::find(enemies.begin(), enemies.end(), selected) == enemies.end()) {
					selected = enemies[0];
				}
				else {
					if (selected->get_stats()->hp <= 0) {
						bool found = false;
						for (auto e : enemies) {
							if (e != selected) {
								if (e->get_stats()->hp > 0 && e->get_stats()->hp < 1000000) {
									found = true;
									selected = e;
									break;
								}
							}
						}
						if (found == false) {
							selecting = false;
							turn_type = wedge::Battle_Game::NONE;
						}
						if (wedge::is_onscreen_controller_enabled()) {
							wedge::set_onscreen_controller_b3_enabled(false, -1);
						}
					}
				}
			}
		}
	}
}

int Battle_Game::get_player_index(Battle_Player *player)
{
	int index = 0;
	auto v = get_players();

	for (size_t i = 0; i < v.size(); i++) {
		if (v[i] == player) {
			index = (int)i;
			break;
		}
	}

	return index;
}

void Battle_Game::next_player(int direct, bool no_sound)
{
	auto v = get_players();
	auto start_p = dynamic_cast<Battle_Player *>(v[active_player]);
	if (direct >= 0) {
		active_player = direct;
	}
	else {
		active_player++;
	}
	active_player %= v.size();
	auto p = dynamic_cast<Battle_Player *>(v[active_player]);
	int count = 1;
	while (p->get_stats()->hp <= 0 || p->get_sprite()->get_animation() == "attack_defend") {
		if (count >= (int)v.size()) {
			break;
		}
		active_player++;
		active_player %= v.size();
		p = dynamic_cast<Battle_Player *>(v[active_player]);
		count++;
	}
	if (p->get_stats()->hp > 0 && p != start_p && no_sound == false) {
		shim::widget_mml->play(false);
	}
}

void Battle_Game::end_combo()
{
	detecting_combo = false;
	GLOBALS->allow_global_mini_pause = true;
	GLOBALS->getting_combo = false;
	shim::convert_directions_to_focus_events = true;
	GLOBALS->onscreen_controller_was_enabled = osc_enabled;
	wedge::set_onscreen_controller_generates_b1_b2(false);
	wedge::set_onscreen_controller_b2_enabled(false);
	wedge::set_onscreen_controller_b3_enabled(false, GLOBALS->key_b3);
	wedge::set_onscreen_controller_b4_enabled(false, GLOBALS->key_b4);
	delete detector;
	detector = nullptr;
}

int Battle_Game::get_num_turns(wedge::Battle_Entity *e)
{
	int turn_count = 0;
	for (auto t : active_turns) {
		if (t->actor == e) {
			turn_count++;
		}
	}
	for (auto t : turns) {
		if (t->actor == e) {
			turn_count++;
		}
	}
	return turn_count;
}

void Battle_Game::resize(util::Size<int> new_size)
{
	wedge::Battle_Game::resize(new_size);
	
	const int EDGE_X = shim::screen_size.w * 0.05f;
	const int EDGE_Y = shim::screen_size.h * 0.05f;

	if (item_gui) {
		item_gui->set_padding(EDGE_X, 0, 0, EDGE_Y);
		item_gui->resize(new_size);
		int width = shim::screen_size.w - shim::screen_size.w * 0.1f - 3*2/*battle window border aka win_border */ - WIN_BORDER*2;
		item_gui->get_list()->set_width(width);
	}
	else if (easy_gui) {
		easy_gui->set_padding(EDGE_X, 0, 0, EDGE_Y);
		easy_gui->resize(new_size);
		int width = shim::screen_size.w - shim::screen_size.w * 0.1f - 3*2/*battle window border aka win_border */ - WIN_BORDER*2;
		easy_gui->get_list()->set_width(width);
	}
	else if (turn_gui) {
		turn_gui->set_padding(EDGE_X, 0, 0, EDGE_Y);
		turn_gui->resize(new_size);
	}
}

void Battle_Game::null_pstats()
{
	pstats = nullptr;
	getting_turn = false;
#if 0
	next_turn_pos = /*boss_battle*/true ? 2 : 3;
	next_turn_top = /*boss_battle*/true ? 0 : 1;
#endif
    next_turn_pos = 2;
    next_turn_top = 0;

    just_exited_pstats = true;

    GLOBALS->allow_global_mini_pause = true;
}

void Battle_Game::add_notification(Notification_Position position, std::string text, int lifetime)
{
	if (sneak_attack) {
		if (position == LEFT) {
			position = RIGHT;
		}
		else {
			position = LEFT;
		}
	}
	Notification n;
	n.position = position;
	n.text = text;
	n.death_time = lifetime + GET_TICKS();
	// Delete any in the same position
	for (auto it = notifications.begin(); it != notifications.end();) {
		if ((*it).position == position) {
			it = notifications.erase(it);
		}
		else {
			it++;
		}
	}
	notifications.push_back(n);
}

bool Battle_Game::is_detecting_combo()
{
	return detecting_combo;
}
	
void Battle_Game::set_player_stats_shown(bool show)
{
	player_stats_shown = show;
}

void Battle_Game::set_enemy_stats_shown(bool show)
{
	enemy_stats_shown = show;
}

void Battle_Game::set_window_shown(bool show)
{
	window_shown = show;
}

void Battle_Game::close_guis()
{
	getting_turn = false;
	if (turn_gui_gone == false) {
		turn_gui->exit();
		turn_gui_gone = true;
	}
	enemy_stats_shown = true;
	player_stats_shown = true;
	if (item_gui) {
		item_gui->exit();
		item_gui = nullptr;
	}
	if (easy_gui) {
		easy_gui->exit();
		easy_gui = nullptr;
	}
	if (mc_gui) {
		mc_gui->exit();
		mc_gui = nullptr;
	}
	if (pstats) {
		pstats->exit();
		pstats = nullptr;
	}
	end_combo();
	good_combo = -1;
	item_index = -1;
	selecting = false;
	remove_notification(LEFT);
	remove_notification(RIGHT);
	multi_targets.clear();
	if (wedge::is_onscreen_controller_enabled()) {
		wedge::set_onscreen_controller_b3_enabled(false, -1);
	}
}

void Battle_Game::remove_notification(Notification_Position pos)
{
	for (auto it = notifications.begin(); it != notifications.end();) {
		Notification &n = *it;
		if (n.position == pos) {
			it = notifications.erase(it);
		}
		else {
			it++;
		}
	}
}

void Battle_Game::do_all_dead()
{
	if (getting_turn) {
		getting_turn = false;
		if (turn_gui_gone == false) {
			turn_gui->exit();
			turn_gui_gone = true;
		}
		enemy_stats_shown = true;
		player_stats_shown = true;
		if (item_gui) {
			item_gui->exit();
			item_gui = nullptr;
		}
		if (easy_gui) {
			easy_gui->exit();
			easy_gui = nullptr;
		}
		if (mc_gui) {
			mc_gui->exit();
			mc_gui = nullptr;
		}
		if (pstats) {
			pstats->exit();
			pstats = nullptr;
		}
		end_combo();
		good_combo = -1;
		remove_notification(RIGHT);
	}
}

bool Battle_Game::is_sneak_attack()
{
	return sneak_attack;
}

void Battle_Game::startup()
{
	if (sneak_attack && DOGO_INSTANCE->saw_sneak == false && easy_combos == false) {
		DOGO_INSTANCE->saw_sneak = true;
		GLOBALS->mini_pause(false, true);
	}
}

int Battle_Game::get_active_player()
{
	return active_player;
}

void Battle_Game::add_gold(int gold)
{
	this->gold += gold;
}

void Battle_Game::multi_confirm(bool confirmed)
{
	auto players = get_players();

	fixup_targets(players[active_player], multi_targets, selecting_enemy, false);

	if (multi_targets.size() == 0) {
		confirmed = false;
	}

	if (confirmed) {
		if (turn_type == wedge::Battle_Game::ATTACK) {
			if (INSTANCE->is_milestone_complete(MS_DID_AN_ATTACK) == false) {
				gfx::add_notification(GLOBALS->game_t->translate(409)/* Originally: Good job! */);
				INSTANCE->set_milestone_complete(MS_DID_AN_ATTACK, true);
			}

			auto t = new wedge::Battle_Game::Turn;

			t->actor = acting_player;
			t->turn_type = wedge::Battle_Game::ATTACK;
			int player_index = get_player_index(acting_player);
			std::string cmbo = DOGO_INSTANCE->combos[player_index][good_combo];
			std::string bak = cmbo;
			int pos;
			if ((pos = cmbo.find('\'')) != std::string::npos) {
				cmbo = cmbo.substr(0, pos) + cmbo.substr(pos+1);
			}
			int cost = get_combo_cost(cmbo);
			INSTANCE->stats[player_index].base.mp -= cost;
			t->turn_name = std::string("attack_") + util::lowercase(cmbo);
			for (size_t i = 0; i < t->turn_name.length(); i++) {
				if (t->turn_name[i] == ' ') {
					t->turn_name[i] = '_';
				}
			}
			add_notification(RIGHT, GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(bak)));
			good_combo = -1;
			for (auto e : multi_targets) {
				t->targets.push_back(e);
			}
			t->started = false;

			next_turn = t;

			selecting = false;
			
			GLOBALS->button->play(false);
		}
		else if (turn_type == wedge::Battle_Game::ITEM) {
			auto t = new wedge::Battle_Game::Turn;

			t->actor = acting_player;
			t->turn_type = wedge::Battle_Game::ITEM;
			int i = inventory_indices[item_index];
			t->turn_name = util::itos(i);
			wedge::Object *inv = INSTANCE->inventory.get_all();
			inv[i].used++;
			add_notification(RIGHT, inv[i].name);
			for (auto e : multi_targets) {
				t->targets.push_back(e);
			}
			t->started = false;

			next_turn = t;

			selecting = false;
			
			GLOBALS->button->play(false);

			item_index = -1;
		}
	}
	else {
		selecting = false;
		getting_turn = false;
		GLOBALS->button->play(false);
		item_gui = nullptr;
		easy_gui = nullptr;
		if (turn_type == wedge::Battle_Game::ATTACK) {
			next_turn_pos = 0;
		}
		else {
			next_turn_pos = 1;
		}
		item_index = -1;
		good_combo = -1;
		remove_notification(RIGHT);
		just_exited_multi_gui = true;
	}
	
	mc_gui = nullptr;
	
	multi_targets.clear();
					
	if (wedge::is_onscreen_controller_enabled()) {
		wedge::set_onscreen_controller_b3_enabled(false, -1);
	}
}

bool Battle_Game::get_osc_enabled()
{
	return osc_enabled;
}

void Battle_Game::start_turn(wedge::Battle_Entity *entity)
{
	if (simple_turn_display) {
		return;
	}

	for (auto it = started_turns.begin(); it != started_turns.end(); it++) {
		auto p = *it;
		if (p.first == entity) {
			started_turns.erase(it);
			break;
		}
	}

	gfx::Sprite *sprite = new gfx::Sprite("turn_arrow");
	sprite->set_animation("only", ::turn_sprite_ended, sprite);
	std::pair<wedge::Battle_Entity *, gfx::Sprite *> p;
	p.first = entity;
	p.second = sprite;
	started_turns.push_back(p);
}

void Battle_Game::turn_sprite_ended(gfx::Sprite *sprite)
{
	started_turns_to_delete.push_back(sprite);
}

bool Battle_Game::in_water()
{
	return water;
}

float Battle_Game::get_water_height()
{
	Uint32 now = GET_TICKS();
	int t = now % 1000;
	if (t < 500) {
		return 4.0f;
	}
	else {
		return 3.0f;
	}
}
