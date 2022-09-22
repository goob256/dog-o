#include <wedge2/battle_enemy.h>
#include <wedge2/delay.h>
#include <wedge2/general.h>
#include <wedge2/generic_callback.h>
#include <wedge2/generic_immediate_callback.h>
#include <wedge2/play_animation.h>
#include <wedge2/play_sound.h>
#include <wedge2/rumble.h>
#include <wedge2/special_number.h>
#include <wedge2/stats.h>

#include "achievements.h"
#include "battle_game.h"
#include "battle_player.h"
#include "battles.h"
#include "coin.h"
#include "enemies.h"
#include "fire.h"
#include "general.h"
#include "globals.h"
#include "hit.h"
#include "inventory.h"
#include "stats.h"

static void attack_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	Battle_Enemy *e = static_cast<Battle_Enemy *>(data);
	e->get_sprite()->set_animation("idle");
	e->set_attack_done(true);
}

static void hit_callback(void *data)
{
	auto e = static_cast<Battle_Enemy *>(data);
	gfx::Sprite *sprite = e->get_sprite();
	if (sprite->get_animation() == "hit") {
		sprite->set_animation(sprite->get_previous_animation());
	}
}

Battle_Enemy::Battle_Enemy(std::string name, int min_delay, int max_delay) :
	wedge::Battle_Enemy(name),
	turn_started(false),
	attack_done(false),
	min_delay(min_delay),
	max_delay(max_delay)
{
	if (static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
		next_move = GET_TICKS() + min_delay;
		inverse_x = true;
	}
	else {
		next_move = GET_TICKS() + util::rand(min_delay, max_delay);
	}
	attack_anims.push_back("attack");
	attack_sounds.push_back(DOGO_GLOBALS->enemy_attack);
	attack_mults.push_back(1.0f);

	float m = enemy_speed_mul();
	min_delay *= m;
	max_delay *= m;
}

Battle_Enemy::~Battle_Enemy()
{
	if (stats->hp <= 0) {
		DOGO_INSTANCE->enemies_killed++;
		if (DOGO_INSTANCE->enemies_killed >= 100) {
			util::achieve((void *)ACH_WAYDOWN);
		}
	}
}

void Battle_Enemy::set_attack_done(bool done)
{
	attack_done = done;
}

int Battle_Enemy::start_attack(wedge::Battle_Game::Turn *turn)
{
	attack_done = false;
	attack_num = util::rand(0, int(attack_anims.size()-1));
	if (attack_num < (int)attack_sounds.size()) {
		if (attack_sounds[attack_num] != nullptr) {
			attack_sounds[attack_num]->play(false);
		}
	}
	else if (attack_sounds.size() > 0) {
		attack_sounds[0]->play(false);
	}
	sprite->set_animation(attack_anims[attack_num], attack_callback, this);
	if (attack_num < (int)attack_names.size() && attack_names[attack_num] != "") {
		auto b = dynamic_cast<Battle_Game *>(BATTLE);
		b->add_notification(Battle_Game::LEFT, attack_names[attack_num]);
	}

	return attack_num;
}

bool Battle_Enemy::turn_attack(wedge::Battle_Game::Turn *turn)
{
	if (attack_done) {
		auto p = turn->targets[0];
		float mult;
		if ((int)attack_mults.size() > attack_num) {
			mult = attack_mults[attack_num];
		}
		else if (attack_mults.size() > 0) {
			mult = attack_mults[0];
		}
		else {
			mult = 1.0f;
		}
		int damage = MAX(1, stats->fixed.attack*mult-p->get_stats()->fixed.defence);
		add_special_number(this, p, damage, true, true);
		turn_started = false;
		next_move = GET_TICKS() + util::rand(min_delay, max_delay);
		return false;
	}
	return true;
}

void Battle_Enemy::start_dialogue(wedge::Battle_Game::Turn *turn)
{
	static_cast<Battle_Game *>(BATTLE)->close_guis();
	Battle_Game *b = static_cast<Battle_Game *>(BATTLE);
	b->set_window_shown(false);
	GLOBALS->do_dialogue(turn->dialogue.tag, turn->dialogue.text, turn->dialogue.type, turn->dialogue.position, turn->dialogue.monitor);
}

bool Battle_Enemy::turn_dialogue(wedge::Battle_Game::Turn *turn)
{
	if (GLOBALS->dialogue_active(BATTLE) == false) {
		Battle_Game *b = static_cast<Battle_Game *>(BATTLE);
		b->set_window_shown(true);
		turn_started = false;
		return false;
	}
	return true;
}

bool Battle_Enemy::take_hit(wedge::Battle_Entity *actor, int damage)
{
	if (damage > 0) {
		sprite->set_animation("hit", hit_callback, this);
	}
	return wedge::Battle_Enemy::take_hit(actor, damage);
}

util::Point<int> Battle_Enemy::get_turn_order_display_offset()
{
	return util::Point<int>(0, 0);
}

//--

Enemy_Puddle::Enemy_Puddle() :
	Battle_Enemy(GLOBALS->game_t->translate(13)/* Originally: Puddle */, 3000, 6000)
{
}

Enemy_Puddle::~Enemy_Puddle()
{
}

bool Enemy_Puddle::start()
{
	gold = 20;
	sprite = new gfx::Sprite("puddle");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 50;
	stats->fixed.attack = 28 * enemy_attack_mul();
	stats->fixed.defence = 10 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 20);

	attack_names.push_back(GLOBALS->game_t->translate(182)/* Originally: Soak */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Puddle::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Puddle::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

//--

static void end_sensei1_battle(void *data)
{
	BATTLE->set_done(true);
}

static void end_hadouken(void *data)
{
	auto s = static_cast<Enemy_Sensei1  *>(data);
	s->end_hadouken();
}

static void set_idle(void *data)
{
	auto s = static_cast<Enemy_Sensei1 *>(data);
	s->get_sprite()->set_animation("idle");
	auto b = dynamic_cast<Battle_Game *>(BATTLE);
	b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(226)/* Originally: Fireball */);
	DOGO_GLOBALS->hadouken->play(false);
}

static void adjust_next_damage_reminder(void *data)
{
	auto s = static_cast<Enemy_Sensei1 *>(data);
	s->adjust_next_damage_reminder();
}

static void adjust_next_item_reminder(void *data)
{
	auto s = static_cast<Enemy_Sensei1 *>(data);
	s->adjust_next_item_reminder();
}

static void adjust_next_both_reminder(void *data)
{
	auto s = static_cast<Enemy_Sensei1 *>(data);
	s->adjust_next_both_reminder();
}

Enemy_Sensei1::Enemy_Sensei1() :
	Battle_Enemy(GLOBALS->game_t->translate(193)/* Originally: Sensei */, 1500, 3000),
	did_damage(false),
	doing_hadouken(false),
	hadouken_done(false),
	done_attacks(false),
	p0_hit(0),
	p1_hit(0),
	did_both(false),
	fireball_done(false)
{
	click_zone_inc = 5;

	attack_anims.clear();
	attack_anims.push_back("punch");
	attack_anims.push_back("kick");
	//attack_names.clear(); - not needed
	attack_names.push_back(GLOBALS->game_t->translate(112)/* Originally: Punch */);
	attack_names.push_back(GLOBALS->game_t->translate(110)/* Originally: Kick */);
	
	attack_sounds.clear();
	attack_sounds.push_back(DOGO_GLOBALS->laugh);

	set_button_strings();

	std::string text = GLOBALS->game_t->translate(213)/* Originally: In battle, you must be quick. The enemy will not wait around and you must not either.^ */;

	if (easy_combos) {
		text += GLOBALS->game_t->translate(405)/* Originally: To attempt a move on an enemy, select COMBO, then pick a move from the list.^ */;
	}
	else {
		text += util::string_printf(GLOBALS->game_t->translate(404)/* To attempt a move on an enemy, select COMBO.^Pleasant, use "%s" to punch. Tom, you use "%s" to kick.^ */.c_str(), pleasant_button.c_str(), tom_button.c_str());
	}

	text += util::string_printf(GLOBALS->game_t->translate(406)/* Originally: If you're fast, you can increase your MULTIPLIER. */.c_str(), pleasant_button.c_str(), tom_button.c_str());

	if (switch_touch) {
		auto bg = static_cast<Battle_Game *>(BATTLE);
		bool dc = bg->is_detecting_combo();
		if ((dc == false && wedge::is_onscreen_controller_enabled()) || (dc == true && bg->get_osc_enabled())) {
			text += util::string_printf(GLOBALS->game_t->translate(214)/* Originally: ^You can change who will go next using "%s". */.c_str(), get_key_name(TGUIK_RIGHT).c_str());
		}
		else {
			text += GLOBALS->game_t->translate(218)/* Originally: ^You can change who will go next by tapping your party on the right! */;
		}
	}
	else {
		text += util::string_printf(GLOBALS->game_t->translate(214)/* Originally: ^You can change who will go next using "%s". */.c_str(), switch_button.c_str());
	}

	NEW_SYSTEM_AND_TASK(BATTLE)
	auto cb = new wedge::Generic_Callback_Step(::adjust_next_damage_reminder, this, new_task);
	ADD_STEP(cb)
	ADD_TASK(new_task)
	FINISH_SYSTEM(BATTLE)

	BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, text, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, cb);

	next_damage_reminder = GET_TICKS() + 30*1000;
	next_both_reminder = 0;
	next_caramel_before_hadouken_reminder = 0;
	
	draw_shadow = true;
	shadow_size = {12, 6};
	auto_shadow_pos = true;
}

Enemy_Sensei1::~Enemy_Sensei1()
{
}

bool Enemy_Sensei1::start()
{
	gold = 20;
	sprite = new gfx::Sprite("sensei");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = START_HP;
	stats->fixed.attack = 30 * enemy_attack_mul();
	stats->fixed.defence = 10 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Sensei1::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}
	
	// In case anyone plans to attack for 1 billion HP
	if (stats->hp < START_HP - 200) {
		stats->hp = START_HP - 200;
	}

	if (doing_hadouken) {
		if (GET_TICKS() > hadouken_dialogue_appear_time && GLOBALS->dialogue_active(BATTLE) == false) {
			doing_hadouken = false;
			next_item_reminder = GET_TICKS() + 30*1000;

			auto players = BATTLE->get_players();
			
			gfx::Image *img = sprite->get_current_image();

			NEW_SYSTEM_AND_TASK(BATTLE)
			if (players[0]->get_stats()->hp > 0) {
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->grunt, false, false, new_task))
				ADD_STEP(new wedge::Play_Animation_Step(sprite, "hadouken", new_task))
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_idle, this, new_task))
				auto f = new Fire_Step({0, 0}, players[0], shim::palette[8], shim::palette[28], shim::palette[22], new_task);
				ADD_STEP(f)
				util::Point<float> start_pos = get_position() + util::Point<float>(img->size.w, img->size.h/2.0f);
				util::Point<float> end_pos = static_cast<Battle_Player *>(players[0])->get_draw_pos() + util::Point<float>(8.0f, 8.0f);
				f->set_start_pos(start_pos);
				f->set_end_pos(end_pos);
				f->set_start_angle((end_pos-start_pos).angle());
				f->set_total_particles();
			}
			if (players[1]->get_stats()->hp > 0) {
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->grunt, false, false, new_task))
				ADD_STEP(new wedge::Play_Animation_Step(sprite, "hadouken", new_task))
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_idle, this, new_task))
				auto f = new Fire_Step({0, 0}, players[1], shim::palette[8], shim::palette[28], shim::palette[22], new_task);
				ADD_STEP(f)
				util::Point<float> start_pos = get_position() + util::Point<float>(img->size.w, img->size.h/2.0f);
				util::Point<float> end_pos = static_cast<Battle_Player *>(players[1])->get_draw_pos() + util::Point<float>(8.0f, 8.0f);
				f->set_start_pos(start_pos);
				f->set_end_pos(end_pos);
				f->set_start_angle((end_pos-start_pos).angle());
				f->set_total_particles();
			}
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(::end_hadouken, this, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(BATTLE)

			auto t = new wedge::Battle_Game::Turn;
			t->actor = this;
			t->turn_type = wedge::Battle_Game::ATTACK;
			t->turn_name = "fireball";
			t->targets.push_back(BATTLE->get_random_player());
			t->started = false;

			return t;
		}

		return nullptr;
	}
	else if (did_damage == false) {
		if (stats->hp <= START_HP - 200) {
			did_damage = true;
			return nullptr;
		}
		else if (GLOBALS->dialogue_active(BATTLE)) {
			next_damage_reminder = GET_TICKS() + 30*1000;
			return nullptr;
		}
		else if (GET_TICKS() >= next_damage_reminder) {
			auto players = BATTLE->get_players();
			int count = 0;
			if (players[0]->get_stats()->hp <= 0) {
				count++;
			}
			if (players[1]->get_stats()->hp <= 0) {
				count++;
			}
			
			std::string text;

			if (count > 0) {
				add_caramels();
				text = GLOBALS->game_t->translate(229)/* You can use sweets to revive someone who is unconscious. Try it now. */;
			}
			else {
				set_button_strings();
				if (easy_combos) {
					text = GLOBALS->game_t->translate(442)/* Originally: Choose COMBO then select a move from the list. */;
				}
				else {
					text = util::string_printf(GLOBALS->game_t->translate(219)/* Originally: Pleasant, use "%s" to punch. Tom, you use "%s" to kick. */.c_str(), pleasant_button.c_str(), tom_button.c_str());
				}
				if (switch_touch) {
					auto bg = static_cast<Battle_Game *>(BATTLE);
					bool dc = bg->is_detecting_combo();
					if ((dc == false && wedge::is_onscreen_controller_enabled()) || (dc == true && bg->get_osc_enabled())) {
						text += util::string_printf(GLOBALS->game_t->translate(214)/* Originally: ^You can change who will go next using "%s". */.c_str(), get_key_name(TGUIK_RIGHT).c_str());
					}
					else {
						text += GLOBALS->game_t->translate(218)/* Originally: ^You can change who will go next by tapping your party on the right! */;
					}
				}
				else {
					text += util::string_printf(GLOBALS->game_t->translate(214)/* Originally: ^You can change who will go next using "%s". */.c_str(), switch_button.c_str());
				}
			}

			NEW_SYSTEM_AND_TASK(BATTLE)
			auto cb = new wedge::Generic_Callback_Step(::adjust_next_damage_reminder, this, new_task);
			ADD_STEP(cb)
			ADD_TASK(new_task)
			FINISH_SYSTEM(BATTLE)

			BATTLE->drop_inactive_turns();

			BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, text, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, cb);
			next_damage_reminder = GET_TICKS() + 30*1000;
			
			return nullptr;
		}
	}
	else if (hadouken_done) {
		auto players = BATTLE->get_players();
		bool all_above_one = true;
		int num_above_one = 0;
		for (auto p : players) {
			if (p->get_stats()->hp <= 1) {
				all_above_one = false;
			}
			else {
				num_above_one++;
			}
		}
		if (all_above_one) {
			NEW_SYSTEM_AND_TASK(BATTLE)
			auto cb = new wedge::Generic_Callback_Step(end_sensei1_battle, nullptr, new_task);
			ADD_STEP(cb)
			ADD_TASK(new_task)
			FINISH_SYSTEM(BATTLE)

			std::string text;
			std::string button;
		
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

			auto bg = static_cast<Battle_Game *>(BATTLE);
			bool dc = bg->is_detecting_combo();

			std::string open_quote = GLOBALS->game_t->translate(402)/* Originally: " */;
			std::string close_quote = GLOBALS->game_t->translate(403)/* Originally: " */;

#if defined ANDROID || (defined IOS && !defined TVOS)
			if (input::system_has_keyboard() == false) {
#else
			if ((shim::force_tablet && easy_combos == false) || ((dc == false && wedge::is_onscreen_controller_enabled()) || (dc == true && bg->get_osc_enabled()))) {
#endif
				button = GLOBALS->game_t->translate(368)/* the back arrow */;
				open_quote = "";
				close_quote = "";
			}
			else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
			if (shim::steam_init_failed == false) {
				EControllerActionOrigin *origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];
				ControllerHandle_t handle = input::get_controller_handle();

				int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->ingame_action_set, DOGO_GLOBALS->ingame_menu, origins);
				if (count > 0) {
					button = input::get_joystick_button_colour_code(origins[0]) + input::get_joystick_button_name(origins[0]) + black;
				}
				else {
					button = get_key_name(GLOBALS->key_menu);
				}
			}
			else
#endif
				{
					button = input::get_joystick_button_colour_code(GLOBALS->joy_menu) + input::get_joystick_button_name(GLOBALS->joy_menu) + black;
				}
			}
			else {
				button = get_key_name(GLOBALS->key_menu);
			}

			if (easy_combos) {
				text = GLOBALS->game_t->translate(441)/* Originally: Very good! You know enough for now.^Take these White Belts. With each belt, you'll learn new skills.^You must equip a belt to use its skills.^ */;
			}
			else {
				text = GLOBALS->game_t->translate(216)/* Originally: Very good! You know enough for now.^Take these White Belts. With each belt, you'll learn new skills.^ */;
			}
			text += GLOBALS->game_t->translate(518)/* Open the menu with */;
			text += " ";
			text += open_quote;
			text += button;
			text += close_quote;
			text += " ";
			if (easy_combos) {
				text += GLOBALS->game_t->translate(521)/* Originally: to equip your new belts.^Advanced skills cost SP to use. You can replenish SP by eating vegetables.^Your SP will also replenish slowly over time. */;
			}
			else {
				text += GLOBALS->game_t->translate(519)/* and equip the belts, then check out your new skills in the PRACTICE menu.^Advanced skills cost SP to use. You can replenish SP by eating vegetables.^Your SP will also replenish slowly over time. */;
			}
			BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, text, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, cb);
			next_item_reminder = GET_TICKS() + 30*1000;
			return nullptr;
		}
		else if (GLOBALS->dialogue_active(BATTLE)) {
			next_item_reminder = GET_TICKS() + 30*1000;
			return nullptr;
		}
		else if (GET_TICKS() >= next_item_reminder) {
			add_apples();
			add_caramels();
			next_item_reminder = GET_TICKS() + 30*1000;
			NEW_SYSTEM_AND_TASK(BATTLE)
			auto cb = new wedge::Generic_Callback_Step(::adjust_next_item_reminder, this, new_task);
			ADD_STEP(cb)
			ADD_TASK(new_task)
			FINISH_SYSTEM(BATTLE)
			BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(217)/* Originally: Pay attention! I could knock you out with one hit! A piece of fruit will replenish your HP. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, cb);
			return nullptr;
		}
	}
	else if (did_both == false) {
		if (p0_hit >= 50 && p1_hit >= 50) {
			auto players = BATTLE->get_players();
			int count = 0;
			if (players[0]->get_stats()->hp <= 0) {
				count++;
			}
			if (players[1]->get_stats()->hp <= 0) {
				count++;
			}
			
			std::string text;

			if (count > 0) {
				if (GLOBALS->dialogue_active(BATTLE)) {
					next_caramel_before_hadouken_reminder = GET_TICKS() + 30*1000;
					return nullptr;
				}
				else if (GET_TICKS() >= next_caramel_before_hadouken_reminder) {
					add_caramels();
					std::string text = GLOBALS->game_t->translate(229)/* You can use sweets to revive someone who is unconscious. Try it now. */;

					NEW_SYSTEM_AND_TASK(BATTLE)
					auto cb = new wedge::Generic_Callback_Step(::adjust_next_both_reminder, this, new_task);
					ADD_STEP(cb)
					ADD_TASK(new_task)
					FINISH_SYSTEM(BATTLE)

					BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, text, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, cb);
					next_caramel_before_hadouken_reminder = GET_TICKS() + 30*1000;
					return nullptr;
				}
				else {
					return nullptr;
				}
			}
			else {
				// This fixes an issue where there are multiple turns in queue, causing the dialogue (3 lines down) to be delayed longer than the appear_time (bottom of block)
				BATTLE->drop_inactive_turns();

				std::string text = GLOBALS->game_t->translate(215)/* Originally: Very good, but how will you counter this?! */;
				BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, text, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, nullptr);
				doing_hadouken = true;

				done_attacks = true;

				did_both = true;

				hadouken_dialogue_appear_time = GET_TICKS() + 100; // kind of a hack, to allow the dialogue above to appear before the fireballs are thrown. The fireballs then get thrown after that dialogue goes away.
			}
		}
		else if (GLOBALS->dialogue_active(BATTLE)) {
			next_both_reminder = GET_TICKS() + 30*1000;
			return nullptr;
		}
		else if (GET_TICKS() >= next_both_reminder) {
			auto players = BATTLE->get_players();
			int count = 0;
			if (players[0]->get_stats()->hp <= 0) {
				count++;
			}
			if (players[1]->get_stats()->hp <= 0) {
				count++;
			}
			
			std::string text;

			if (count > 0) {
				add_caramels();
				text = GLOBALS->game_t->translate(229)/* You can use sweets to revive someone who is unconscious. Try it now. */;
			}
			else {
				set_button_strings();
				if (switch_touch) {
					auto bg = static_cast<Battle_Game *>(BATTLE);
					bool dc = bg->is_detecting_combo();
					if ((dc == false && wedge::is_onscreen_controller_enabled()) || (dc == true && bg->get_osc_enabled())) {
						text = util::string_printf(GLOBALS->game_t->translate(227)/* Originally: Switch it up with "%s"! You both need to learn!^ */.c_str(), get_key_name(TGUIK_RIGHT).c_str());
					}
					else {
						text = GLOBALS->game_t->translate(228)/* Originally: Switch it up by tapping your party on the right! You both need to learn!^ */;
					}
				}
				else {
					text = util::string_printf(GLOBALS->game_t->translate(227)/* Originally: Switch it up with "%s"! You both need to learn!^ */.c_str(), switch_button.c_str());
				}
				if (easy_combos == false) {
					text += util::string_printf(GLOBALS->game_t->translate(219)/* Originally: Pleasant, use "%s" to punch. Tom, you use "%s" to kick. */.c_str(), pleasant_button.c_str(), tom_button.c_str());
				}
			}

			NEW_SYSTEM_AND_TASK(BATTLE)
			auto cb = new wedge::Generic_Callback_Step(::adjust_next_both_reminder, this, new_task);
			ADD_STEP(cb)
			ADD_TASK(new_task)
			FINISH_SYSTEM(BATTLE)

			BATTLE->do_dialogue(this, GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, text, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, cb);
			next_both_reminder = GET_TICKS() + 30*1000;
			return nullptr;
		}
	}

	if (done_attacks == false) {
		auto t = new wedge::Battle_Game::Turn;
		t->actor = this;
		t->turn_type = wedge::Battle_Game::ATTACK;
		t->turn_name = "attack";
		t->targets.push_back(BATTLE->get_random_player());
		t->started = false;
		
		next_move = GET_TICKS() + util::rand(min_delay, max_delay);

		return t;
	}
	else {
		return nullptr;
	}
}

bool Enemy_Sensei1::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn->turn_name != "fireball") {
				start_attack(turn);
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn->turn_name == "fireball") {
				if (fireball_done == true) {
					turn_started = false;
					return false;
				}
			}
			else {
				if (turn_attack(turn) == false) {
					return false;
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Sensei1::set_button_strings()
{
	switch_touch = false;

	pleasant_button = "";
	tom_button = "";
	switch_button = "";

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

#ifdef STEAMWORKS
	EControllerActionOrigin *origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];
	ControllerHandle_t handle = input::get_controller_handle();
#endif
	auto bg = static_cast<Battle_Game *>(BATTLE);
	bool dc = bg->is_detecting_combo();

#if defined ANDROID || (defined IOS && !defined TVOS)
	if (input::system_has_keyboard() == false) {
#else
	if ((shim::force_tablet && easy_combos == false) || ((dc == false && wedge::is_onscreen_controller_enabled()) || (dc == true && bg->get_osc_enabled()))) {
#endif
#ifdef STEAMWORKS
		pleasant_button = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_X) + input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_X) + black;
		tom_button = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_A) + input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_A) + black;
#else
		pleasant_button = input::get_joystick_button_colour_code(TGUI_B_X) + input::get_joystick_button_name(TGUI_B_X) + black;
		tom_button = input::get_joystick_button_colour_code(TGUI_B_A) + input::get_joystick_button_name(TGUI_B_A) + black;
#endif
	}
	else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
		if (shim::steam_init_failed == false) {
			int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_punch, origins);
			if (count > 0) {
				pleasant_button = input::get_joystick_button_colour_code(origins[0]) + input::get_joystick_button_name(origins[0]) + black;
			}
			else {
				pleasant_button = get_key_name(GLOBALS->key_b3);
			}
			count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_kick, origins);
			if (count > 0) {
				tom_button = input::get_joystick_button_colour_code(origins[0]) + input::get_joystick_button_name(origins[0]) + black;
			}
			else {
				tom_button = get_key_name(GLOBALS->key_b1);
			}
		}
		else
#endif
		{
			pleasant_button = input::get_joystick_button_colour_code(GLOBALS->joy_b3) + input::get_joystick_button_name(GLOBALS->joy_b3) + black;
			tom_button = input::get_joystick_button_colour_code(GLOBALS->joy_b1) + input::get_joystick_button_name(GLOBALS->joy_b1) + black;
		}
	}
	else {
		pleasant_button = get_key_name(GLOBALS->key_b3);
		tom_button = get_key_name(GLOBALS->key_b1);
	}

	if (input::is_joystick_connected() == false && input::system_has_keyboard() == false) {
		switch_touch = true;
	}
#ifdef STEAMWORKS
	else if (shim::steam_init_failed == false) {
		if (input::is_joystick_connected()) {
			int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->menu_action_set, DOGO_GLOBALS->menu_switch, origins);
			if (count > 0) {
				std::string colour = input::get_joystick_button_colour_code(origins[0]);
				std::string name = input::get_joystick_button_name(origins[0]);

				switch_button = colour + name + black;
			}
			else {
				switch_button = get_key_name(GLOBALS->key_switch);
			}
		}
		else {
			switch_button = get_key_name(GLOBALS->key_switch);
		}
	}
#endif
	else if (input::is_joystick_connected()) {
		switch_button = input::get_joystick_button_colour_code(GLOBALS->joy_switch) + input::get_joystick_button_name(GLOBALS->joy_switch) + black;
	}
	else
	{
		switch_button = get_key_name(GLOBALS->key_switch);
	}

#ifdef STEAMWORKS
	delete[] origins;
#endif
}

void Enemy_Sensei1::end_hadouken()
{
	hadouken_done = true;
	next_move = GET_TICKS() + util::rand(min_delay, max_delay);
	add_apples();
	fireball_done = true;
}

void Enemy_Sensei1::adjust_next_damage_reminder()
{
	next_damage_reminder = GET_TICKS() + 30*1000;
	next_move = GET_TICKS() + util::rand(min_delay, max_delay);
}

void Enemy_Sensei1::adjust_next_item_reminder()
{
	next_item_reminder = GET_TICKS() + 30*1000;
	next_move = GET_TICKS() + util::rand(min_delay, max_delay);
}
	
void Enemy_Sensei1::adjust_next_both_reminder()
{
	next_both_reminder = GET_TICKS() + 30*1000;
	next_move = GET_TICKS() + util::rand(min_delay, max_delay);
}
	
int Enemy_Sensei1::get_max_turns()
{
	return 1;
}

bool Enemy_Sensei1::take_hit(wedge::Battle_Entity *actor, int damage)
{
	auto p = BATTLE->get_players();
	if (actor == p[0]) {
		p0_hit += damage;
	}
	else {
		p1_hit += damage;
	}
	auto ret = Battle_Enemy::take_hit(actor, damage);
	/*
	if (stats->hp <= START_HP - 200) {
		BATTLE->drop_inactive_turns(); // stops battle usage help ("Choose COMBO then...") directly before "how will you handle this??"
	}
	*/
	return ret;
}

void Enemy_Sensei1::add_apples()
{
	auto players = BATTLE->get_players();
	int count = 0;
	if (players[0]->get_stats()->hp <= 1) {
		count++;
	}
	if (players[1]->get_stats()->hp <= 1) {
		count++;
	}

	auto obj = OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1);
	int index = INSTANCE->inventory.find(obj);
	auto all = INSTANCE->inventory.get_all();
	int add = 0;
	if (index < 0) {
		add = 2;
	}
	else {
		if (all[index].quantity == 1) {
			add = 1;
		}
	}
	add = MIN(add, count);
	if (add > 0) {
		auto obj = OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, add);
		INSTANCE->inventory.add(obj);
	}
}

void Enemy_Sensei1::add_caramels()
{
	auto players = BATTLE->get_players();
	auto all = INSTANCE->inventory.get_all();
	int count = 0;
	if (players[0]->get_stats()->hp <= 0) {
		count++;
	}
	if (players[1]->get_stats()->hp <= 0) {
		count++;
	}
	auto obj = OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1);
	int index = INSTANCE->inventory.find(obj);
	if (index < 0 && count > 0) {
		auto obj = OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, count);
		INSTANCE->inventory.add(obj);
	}
	else if (index >= 0 && all[index].quantity < count) {
		auto obj = OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, count-all[index].quantity);
		INSTANCE->inventory.add(obj);
	}
}

//--

void Enemy_Wrath::static_start()
{
	doing_drain = nullptr;
}

static void wrath_cast(void *data)
{
	Enemy_Wrath *e = static_cast<Enemy_Wrath *>(data);
	e->get_sprite()->set_animation("idle");
}

Battle_Enemy *Enemy_Wrath::doing_drain = nullptr;

Enemy_Wrath::Enemy_Wrath() :
	Battle_Enemy(GLOBALS->game_t->translate(247)/* Originally: Wrath */, 3000, 6000),
	draining(false)
{
	draw_shadow = true;
	shadow_pos = {30, 32};
	shadow_size = {42, 16};
	attack_sounds.clear();
	attack_sounds.push_back(DOGO_GLOBALS->melee);
}

Enemy_Wrath::~Enemy_Wrath()
{
	if (this == doing_drain) {
		doing_drain = nullptr;
	}
}

bool Enemy_Wrath::start()
{
	gold = 28;
	sprite = new gfx::Sprite("wrath");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 75;
	stats->fixed.attack = 32 * enemy_attack_mul();
	stats->fixed.defence = 10 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 30);

	attack_names.push_back(GLOBALS->game_t->translate(248)/* Originally: Claw */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Wrath::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;

	if (doing_drain == nullptr && util::rand(0, 2) == 0) {
		t->turn_name = "drain";
		doing_drain = this;
	}
	else {
		t->turn_name = "attack";
	}

	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Wrath::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn->turn_name == "drain") {
				draining = true;
				drain_start = GET_TICKS();
				sprite->set_animation("cast", wrath_cast, this);
				laughed = false;
				time_until_laugh = 0;
				gfx::Sprite::Animation *anim = sprite->get_animation("cast");
				for (int i = 0; i < 3; i++) {
					time_until_laugh += anim->delays[i];
				}
				drain_target = static_cast<Battle_Player *>(turn->targets[0]);
			}
			else {
				start_attack(turn);
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn->turn_name == "drain") {
				Uint32 now = GET_TICKS();
				if (laughed == false && int(now-drain_start) >= time_until_laugh) {
					laughed = true;
					DOGO_GLOBALS->evil_laugh->play(false);
					soul_suck_start = now;
				}
				else if (laughed) {
					if (now >= soul_suck_start+2000) {
						draining = false;
						doing_drain = nullptr;
						int damage = drain_target->get_stats()->fixed.max_hp / 6;
						damage *= enemy_attack_mul();
						add_special_number(this, drain_target, damage, true, true);
						turn_started = false;
						next_move = GET_TICKS() + util::rand(min_delay, max_delay);
						return false;
					}
				}
			}
			else {
				if (turn_attack(turn) == false) {
					return false;
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Wrath::draw_fore()
{
	Battle_Enemy::draw_fore();

	if (draining) {
		Uint32 now = GET_TICKS();
		if (laughed) {
			float p = (now-soul_suck_start) / 2000.0f;
			if (p > 1.0f) {
				p = 1.0f;
			}
			gfx::Sprite::Animation *anim = drain_target->get_sprite()->get_animation("hit");
			gfx::Image *img = anim->images[0];
			util::Point<float> player_pos = drain_target->get_draw_pos() + util::Point<float>(shim::tile_size/2.0f, shim::tile_size/2.0f);
			util::Point<float> my_pos = get_position();
			util::Point<int> topleft, bottomright;
			gfx::Sprite::Animation *my_anim = sprite->get_animation("cast");
			my_anim->images[3]->get_bounds(topleft, bottomright);
			my_pos += topleft + util::Point<float>((bottomright.x-topleft.x)/2.0f, (bottomright.y-topleft.y)/2.0f);
			my_pos = (my_pos+player_pos)/2.0f;
			util::Point<float> pos = player_pos + (my_pos-player_pos)*p;
			float alpha;
			if (p < 0.5f) {
				alpha = p / 0.5f;
			}
			else {
				alpha = 1.0f - ((p-0.5f) / 0.5f);
			}
			gfx::Shader *tmp = shim::current_shader;
			shim::current_shader = DOGO_GLOBALS->alpha_shader;
			shim::current_shader->use();
			shim::current_shader->set_float("alpha", alpha);
			gfx::update_projection();
			int flags;
			if (inverse_x) {
				pos.x = shim::screen_size.w - pos.x;
				flags = gfx::Image::FLIP_H;
			}
			else {
				flags = 0;
			}
			img->draw_tinted(shim::black, pos-img->size/2.0f, flags);
			shim::current_shader = tmp;
			shim::current_shader->use();
			gfx::update_projection();
		}
		float p;
		if (laughed == false) {
			p = (now-drain_start) / (float)time_until_laugh;
		}
		else if (now-soul_suck_start < 250) {
			p = 1.0f;
		}
		else if (int(now-soul_suck_start) < 250+time_until_laugh) {
			p = ((now-soul_suck_start)-250) / (float)time_until_laugh;
			p = 1.0f - p;
		}
		else {
			return;
		}
		p = p * 0.5f;
		SDL_Colour black = shim::black;
		black.r *= p;
		black.g *= p;
		black.b *= p;
		black.a *= p;
		gfx::draw_filled_rectangle(black, {0, 0}, shim::screen_size);
	}
}

//--

Enemy_Coinman::Enemy_Coinman() :
	Battle_Enemy(GLOBALS->game_t->translate(250)/* Originally: Coinman */, 3000, 6000)
{
	draw_shadow = true;
	auto_shadow_pos = true;
	shadow_size = {30, 16};
	coin_thud = new audio::Sample("coin_thud.flac");
	coin_jump = new audio::Sample("coin_jump.flac");
}

Enemy_Coinman::~Enemy_Coinman()
{
	delete coin_thud;
	delete coin_jump;
}

bool Enemy_Coinman::start()
{
	gold = 55;
	sprite = new gfx::Sprite("coinman");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 70;
	stats->fixed.attack = 30 * enemy_attack_mul();
	stats->fixed.defence = 12 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	attack_names.push_back(GLOBALS->game_t->translate(249)/* Originally: Bounce */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Coinman::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	int n = util::rand(1, 2);
	for (int i = 0; i < n; i++) {
		wedge::Battle_Entity *e = BATTLE->get_random_player();
		t->targets.push_back(e);
	}
	if (t->targets.size() == 2 && t->targets[0] == t->targets[1]) {
		t->targets.erase(t->targets.begin());
	}
	t->started = false;

	return t;
}

bool Enemy_Coinman::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			hit_ground = false;
			coin_jump->play(false);
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			Uint32 now = GET_TICKS();
			if (hit_ground == false) {
				int nf = sprite->get_num_frames();
				int cf = sprite->get_current_frame();
				if  (cf == nf-1) {
					hit_ground = true;
					hit_ground_time = now;
					for (auto p : turn->targets) {
						p->get_sprite()->set_animation("hit");
					}
					coin_thud->play(false);
				}
			}
			else if (now-hit_ground_time > BOUNCE_TIME) {
				for (auto p : turn->targets) {
					static_cast<Battle_Player *>(p)->set_run_offset({0.0f, 0.0f});
					p->get_sprite()->set_animation(p->get_sprite()->get_previous_animation());
					int damage = MAX(1, stats->fixed.attack-p->get_stats()->fixed.defence);
					add_special_number(this, p, damage, true, true);
				}
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
			else {
				Uint32 elapsed = now - hit_ground_time;
				float p = elapsed / (float)BOUNCE_TIME;
				if (p > 1.0f) {
					p = 1.0f;
				}
				const float max_h = 10.0f;
				float h = sin(p*M_PI) * max_h;
				for (auto p : turn->targets) {
					static_cast<Battle_Player *>(p)->set_run_offset({0.0f, -h});
				}
			}
			return true;
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

bool Enemy_Coinman::take_hit(wedge::Battle_Entity *actor, int damage)
{
	if (damage > 0) {
		util::Point<float> centre = position;
		util::Point<int> topleft, bottomright;
		sprite->get_bounds(topleft, bottomright);
		centre += topleft;
		centre += util::Point<int>((bottomright.x-topleft.x)/2, (bottomright.y-topleft.y)/2);
		NEW_SYSTEM_AND_TASK(BATTLE)
		for (int i = 0; i < 15; i++) {
			if (i != 0) {
				ANOTHER_TASK
			}
			const int dist = 5;
			int x = util::rand(0, dist*2);
			x -= dist;
			int y = util::rand(0, dist*2);
			y -= dist;
			ADD_STEP(new Coin_Step(centre+util::Point<int>(x, y), new_task));
			ADD_TASK(new_task)
		}
		FINISH_SYSTEM(BATTLE)
	}
	return Battle_Enemy::take_hit(actor, damage);
}

//--

int Enemy_Unicorn::count = 0;
gfx::Image *Enemy_Unicorn::rainbow;
const float Enemy_Unicorn::MIN_TRI_SZ = 1.0f;
const float Enemy_Unicorn::MAX_TRI_SZ = 3.0f;

void Enemy_Unicorn::static_start()
{
	count = 0;
}

Enemy_Unicorn::Enemy_Unicorn() :
	Battle_Enemy(GLOBALS->game_t->translate(251)/* Originally: Unicorn */, 3000, 6000),
	doing_rainbow(false)
{
	draw_shadow = true;
	auto_shadow_pos = true;
	shadow_size = {30, 16};

	if (count == 0) {
		rainbow = new gfx::Image("misc/rainbow.tga");
	}
	count++;
	
	sfx = new audio::MML("sfx/rainbow.mml");
	attack_sounds.clear();
	attack_sounds.push_back(sfx);
}

Enemy_Unicorn::~Enemy_Unicorn()
{
	count--;
	if (count == 0) {
		delete rainbow;
	}
	
	delete sfx;
}

bool Enemy_Unicorn::start()
{
	gold = 28;
	sprite = new gfx::Sprite("unicorn");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 60;
	stats->fixed.attack = 30 * enemy_attack_mul();
	stats->fixed.defence = 12 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 30);
	
	attack_names.push_back(GLOBALS->game_t->translate(252)/* Originally: Rainbow */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Unicorn::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	wedge::Battle_Entity *e = BATTLE->get_random_player();
	t->targets.push_back(e);
	t->started = false;

	return t;
}

bool Enemy_Unicorn::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			rainbow_start = GET_TICKS();
			doing_rainbow = true;
			rainbow_target = turn->targets[0];
			init_tris(tris1);
			init_tris(tris2);
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			update_tris(tris1);
			update_tris(tris2);
			if (turn_attack(turn) == false) {
				doing_rainbow = false;
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Unicorn::draw_fore()
{
	Battle_Enemy::draw_fore();

	if (doing_rainbow) {
		util::Point<int> horn_tip = {34, 8};

		util::Point<float> p1 = position + horn_tip;
		Battle_Player *player1 = dynamic_cast<Battle_Player *>(rainbow_target);
		util::Point<float> p2 = player1->get_draw_pos() + util::Point<int>(shim::tile_size/2, shim::tile_size);

		/*
		if (inverse_x) {
			p1.x = shim::screen_size.w - p1.x;
			p2.x = shim::screen_size.w - p2.x;
		}
		*/

		Uint32 now = GET_TICKS();
		Uint32 elapsed = now - rainbow_start;

		float p = elapsed / (float)RAINBOW_TIME;

		float alpha = p / 0.5f;
		if (alpha > 1.0f) {
			alpha = 1.0f;
		}

		if (p > 1.1667f) {
			float p2 = p - 1.1667f;
			if (p2 > 0.1667f) {
				p2 = 0.1667f;
			}
			alpha = 1.0f - (p2 / 0.1667f);
		}

		if (p > 1.0f) {
			p = 1.0f;
		}
		gfx::enable_depth_test(true);
		gfx::enable_depth_write(true);
		gfx::clear_depth_buffer(1.0f);
		gfx::set_depth_mode(gfx::COMPARE_NOTEQUAL);

		rainbow->start_batch();

		float dx = std::abs(p2.x-p1.x);
		float dy = std::abs(p2.y-p1.y);
		float radius = (dx+dy/2.0f)/2.0f; // guess

		// The loops guesses these
		util::Point<float> centre;
		float start_a;
		float end_a;
		float diff_y = 0.0f;

		while (true) {
			float a = atan(dy/radius);
			bool done = false;
			if (p1.y > p2.y) {
				centre = {p1.x+radius, MAX(p1.y, p2.y)};
				start_a = float(M_PI);
				end_a = M_PI*2 - a;
				float x = cos(end_a) * radius + centre.x;
				float y = sin(end_a) * radius + centre.y;
				float l = std::abs(x-p2.x);
				if (l < 1.0f) {
					done = true;
					diff_y = y - p2.y;
					end_a -= (diff_y/radius/2.0f) * (M_PI/2.0f);
				}
				else {
					if (x > p2.x) {
						radius -= 0.5f;
					}
					else {
						radius += 0.5f;
					}
				}
			}
			else {
				centre = {p2.x-radius, MAX(p1.y, p2.y)};
				start_a = M_PI + a;
				end_a = float(M_PI*2);
				float x = cos(start_a) * radius + centre.x;
				float y = sin(start_a) * radius + centre.y;
				float l = std::abs(x-p1.x);
				if (l < 1.0f) {
					done = true;
					diff_y = y - p1.y;
					start_a += (diff_y/radius/2.0f) * (M_PI/2.0f);
				}
				else {
					if (x > p1.x) {
						radius += 0.5f;
					}
					else {
						radius -= 0.5f;
					}
				}
			}
			if (done) {
				break;
			}
		}
		const int nl = 150;
		for (int i = 0; i < nl*p; i++) {
			util::Point<float> pos = centre;
			float pct2 = i / (float)(nl-1);
			float a = start_a + pct2 * (end_a-start_a);
			float d_max = diff_y / radius;
			float m;
			if (p1.y < p2.y) {
				if (a < M_PI*1.5f) { // left half of rainbow
					float m1 = 1.0f + d_max;
					float m2 = 1.0f + (d_max/2.0f);
					float f = a - M_PI;
					float p3 = f / (M_PI/2.0f);
					m = m1 + (m2-m1) * p3;
				}
				else { // right half
					float m1 = 1.0f + (d_max/2.0f);
					float m2 = 1.0f;
					float f = a - M_PI*1.5f;
					float p3 = f / (M_PI/2.0f);
					m = m1 + (m2-m1) * p3;
				}
			}
			else {
				if (a < M_PI*1.5f) {
					float m1 = 1.0f;
					float m2 = 1.0f + (d_max/2.0f);
					float f = a - M_PI;
					float p3 = f / (M_PI/2.0f);
					m = m1 + (m2-m1) * p3;
				}
				else {
					float m1 = 1.0f + (d_max/2.0f);
					float m2 = 1.0f + d_max;
					float f = a - M_PI*1.5f;
					float p3 = f / (M_PI/2.0f);
					m = m1 + (m2-m1) * p3;
				}
			}
			pos.x += cos(a) * radius;
			pos.y += sin(a) * radius * m;
			float a2 = a - M_PI/2.0f;
			if (inverse_x) {
				pos.x = shim::screen_size.w - pos.x;
				a2 = -a2;
			}
			rainbow->draw_tinted_rotated(make_translucent(shim::white, alpha*0.75f), {rainbow->size.w/2.0f, rainbow->size.h/2.0f}, pos, a2);
		}

		rainbow->end_batch();

		gfx::enable_depth_test(false);
		gfx::enable_depth_write(false);
		gfx::set_depth_mode(gfx::COMPARE_LESSEQUAL);

		util::Point<float> start_p = centre;
		util::Point<float> end_p = centre;

		start_p.x += cos(start_a) * radius;
		start_p.y += sin(start_a) * radius;
		float end = start_a + p * (end_a-start_a);
		end_p.x += cos(end) * radius;
		end_p.y += sin(end) * radius;

		draw_tris(tris1, start_p, start_a, alpha);
		draw_tris(tris2, end_p, end, alpha);
	}
}
	
void Enemy_Unicorn::init_tris(std::vector<Tri> &v)
{
	v.clear();

	int ci = 0;

	util::srand(0);

	for (int i = 0; i < 12; i++) {
		Tri t;
		t.ox = int(util::rand(0, 8)) - 4;
		t.oy = int(util::rand(0, 18)) - 9;
		t.sz = MIN_TRI_SZ + (util::rand(0, 1000) / 1000.0f * (MAX_TRI_SZ - MIN_TRI_SZ));
		t.sz_dir = util::rand(0, 1) == 0 ? -1 : 1;
		t.a = util::rand(0, 1000) / 1000.0f * M_PI * 2;
		t.a_dir = util::rand(0, 1) == 0 ? -1 : 1;
		t.colour = shim::white;
		v.push_back(t);
		ci++;
	}

	util::srand((Uint32)time(NULL));
}

void Enemy_Unicorn::update_tris(std::vector<Tri> &v)
{
	const float dsz = 0.05f;
	const float da = 0.1f;

	for (auto &t : v) {
		t.sz += t.sz_dir * dsz;
		if (t.sz_dir < 0 && t.sz <= MIN_TRI_SZ) {
			t.sz_dir = -t.sz_dir;
			t.sz = MIN_TRI_SZ;
		}
		else if (t.sz_dir > 0 && t.sz >= MAX_TRI_SZ) {
			t.sz_dir = -t.sz_dir;
			t.sz = MAX_TRI_SZ;
		}
		t.a += da * t.a_dir;
	}
}
	
void Enemy_Unicorn::draw_tris(std::vector<Tri> &v, util::Point<float> pos, float angle, float alpha)
{
	gfx::enable_depth_test(true);
	gfx::enable_depth_write(true);
	gfx::set_depth_mode(gfx::COMPARE_NOTEQUAL);

	for (auto t : v) {
		gfx::clear_depth_buffer(1.0f);

		util::Point<float> centre = pos;

		float oa = atan2(t.oy, t.ox);
		oa += angle + M_PI/2.0f;

		util::Point<float> p1 = {0.0f, 0.0f};
		util::Point<float> p2 = {t.ox, t.oy};
		float len = (p2-p1).length();

		centre.x += cos(oa) * len;
		centre.y += sin(oa) * len;

		float a1 = t.a;
		float a2 = a1 + (M_PI*2.0f) / 3.0f;
		float a3 = a2 + (M_PI*2.0f) / 3.0f;

		util::Point<float> a = centre;
		util::Point<float> b = centre;
		util::Point<float> c = centre;

		a.x += cos(a1) * t.sz;
		a.y += sin(a1) * t.sz;
		b.x += cos(a2) * t.sz;
		b.y += sin(a2) * t.sz;
		c.x += cos(a3) * t.sz;
		c.y += sin(a3) * t.sz;

		gfx::draw_primitives_start();

		if (inverse_x) {
			a.x = shim::screen_size.w - a.x;
			b.x = shim::screen_size.w - b.x;
			c.x = shim::screen_size.w - c.x;

			// Correct winding so not invisible
			util::Point<float> tmp;
			tmp = b;
			b = c;
			c = tmp;
		}

		gfx::draw_filled_triangle(make_translucent(t.colour, alpha), a, b, c);

		// Make it a star

		a1 = a1 + M_PI;
		a2 = a1 + (M_PI*2.0f) / 3.0f;
		a3 = a2 + (M_PI*2.0f) / 3.0f;

		a = centre;
		b = centre;
		c = centre;

		a.x += cos(a1) * t.sz;
		a.y += sin(a1) * t.sz;
		b.x += cos(a2) * t.sz;
		b.y += sin(a2) * t.sz;
		c.x += cos(a3) * t.sz;
		c.y += sin(a3) * t.sz;

		if (inverse_x) {
			a.x = shim::screen_size.w - a.x;
			b.x = shim::screen_size.w - b.x;
			c.x = shim::screen_size.w - c.x;

			// Correct winding so not invisible
			util::Point<float> tmp;
			tmp = b;
			b = c;
			c = tmp;
		}

		gfx::draw_filled_triangle(make_translucent(t.colour, alpha), a, b, c);

		gfx::draw_primitives_end();
	}

	gfx::enable_depth_test(false);
	gfx::enable_depth_write(false);
	gfx::set_depth_mode(gfx::COMPARE_LESSEQUAL);
}

//--

Enemy_Tentacle::Enemy_Tentacle() :
	Battle_Enemy(GLOBALS->game_t->translate(267)/* Originally: Tentacle */, 3000, 6000)
{
}

Enemy_Tentacle::~Enemy_Tentacle()
{
}

bool Enemy_Tentacle::start()
{
	gold = 20;
	sprite = new gfx::Sprite("tentacle");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 55;
	stats->fixed.attack = 28 * enemy_attack_mul();
	stats->fixed.defence = 10 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	attack_names.push_back(GLOBALS->game_t->translate(268)/* Originally: Poke */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Tentacle::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Tentacle::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

bool Enemy_Tentacle::take_hit(wedge::Battle_Entity *actor, int damage)
{
	bool ret = Battle_Enemy::take_hit(actor, damage);

	if (stats->hp <= 0) {
		dynamic_cast<Battle_Octo *>(BATTLE)->delete_tentacle(this);
	}

	return ret;
}

//--

Enemy_Octo::Enemy_Octo() :
	Battle_Enemy(GLOBALS->game_t->translate(258)/* Originally: Octo */, 3000, 6000)
{
}

Enemy_Octo::~Enemy_Octo()
{
}

bool Enemy_Octo::start()
{
	gold = 0;
	sprite = new gfx::Sprite("octo");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 999999999;
	stats->fixed.attack = 28 * enemy_attack_mul();
	stats->fixed.defence = 10 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Octo::get_turn()
{
	return nullptr;
}

bool Enemy_Octo::take_turn(wedge::Battle_Game::Turn *turn)
{
	return false;
}

//--

static void hop_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	Enemy_Flame_Frog *ff = static_cast<Enemy_Flame_Frog *>(data);
	ff->get_sprite()->set_animation("idle");
	ff->set_hop_done(true);
}

int Enemy_Flame_Frog::count = 0;
SDL_Colour Enemy_Flame_Frog::palette[256];
int Enemy_Flame_Frog::palette_size;
gfx::Sprite *Enemy_Flame_Frog::particle;

void Enemy_Flame_Frog::static_start()
{
	count = 0;
}

Enemy_Flame_Frog::Enemy_Flame_Frog() :
	Battle_Enemy(GLOBALS->game_t->translate(354)/* Originally: Flame Frog */, 2750, 5500),
	hop_done(false)
{
	if (count == 0) {
		palette_size = gfx::load_palette("flame.gpl", palette, 256);
		particle = new gfx::Sprite("flame_frog_particle");
	}
	count++;

	sfx_attack = new audio::MML("sfx/flame_frog_croak.mml");
	attack_sounds.clear();
	attack_sounds.push_back(sfx_attack);
	
	sfx_hop = new audio::MML("sfx/flame_frog_hop.mml");

	draw_shadow = true;
	shadow_size = {30, 15};
	auto_shadow_pos = true;
}

Enemy_Flame_Frog::~Enemy_Flame_Frog()
{
	count--;
	if (count == 0) {
		delete particle;
	}
	
	delete sfx_attack;
	delete sfx_hop;
}

bool Enemy_Flame_Frog::start()
{
	gold = 40;
	sprite = new gfx::Sprite("flame_frog");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 120;
	stats->fixed.attack = 40 * enemy_attack_mul();
	stats->fixed.defence = 30 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	attack_anims.clear();
	attack_anims.push_back("croak");
	attack_names.push_back(GLOBALS->game_t->translate(355)/* Originally: Croak */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Flame_Frog::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto enemies = BATTLE->get_enemies();

	std::vector<Enemy_Flame_Frog *> v;

	for (auto e : enemies) {
		auto ff = dynamic_cast<Enemy_Flame_Frog *>(e);
		if (ff) {
			v.push_back(ff);
		}
	}

	bool do_attack;
	
	if (util::rand(0, 2) != 0) {
		do_attack = true;
	}
	else {
		// If only 1 left, don't heal as fast
		if (v.size() == 1 && util::rand(0, 1) == 0) {
			do_attack = true;
		}
		else {
			do_attack = false;
		}
	}

	auto t = new wedge::Battle_Game::Turn;
	t->actor = this;

	if (v.size() == 0/* impossible? */ || do_attack) {
		t->turn_type = wedge::Battle_Game::ATTACK;
		t->turn_name = "croak";
		t->targets.push_back(BATTLE->get_random_player());
	}
	else {
		t->turn_type = wedge::Battle_Game::ITEM;
		t->turn_name = "hop";
		t->targets.push_back(v[util::rand(0, int(v.size()-1))]);
	}

	t->started = false;

	return t;
}

bool Enemy_Flame_Frog::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
			done_regular_attack = false;
			croak_start_time = GET_TICKS();
			added_particles = 0;
			started_adding_particles = false;
			delay_before_flame = 0;
			gfx::Sprite::Animation *anim = sprite->get_animation("croak");
			for (int i = 0; i < FLAME_FRAME; i++) {
				delay_before_flame += anim->delays[i];
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::ITEM) {
			hop_done = false;
			sfx_hop->play(false);
			sprite->set_animation("hop", hop_callback, this);
			static_cast<Battle_Game *>(BATTLE)->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(356)/* Originally: Hop */);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (done_regular_attack == false) {
				done_regular_attack = turn_attack(turn) == false;
			}

			if (done_regular_attack && (started_adding_particles == false || particles.size() > 0)) {
				turn_started = true;
			}

			Uint32 now = GET_TICKS();
			Uint32 elapsed = now - croak_start_time;
	
			if (started_adding_particles == false && int(elapsed) >= delay_before_flame) {
				started_adding_particles = true;
			}

			gfx::Sprite::Animation *anim = sprite->get_animation("croak");

			int flame_duration = anim->delays[FLAME_FRAME];

			int particles_to_add = 0;

			int elapsed_flame = int(elapsed) - delay_before_flame;
			elapsed_flame = MIN(flame_duration, elapsed_flame);

			//int add_period = flame_duration - MAX_PARTICLE_LIFETIME;
			int add_period = flame_duration;

			if (elapsed_flame >= add_period) {
				particles_to_add = NUM_PARTICLES - added_particles;
			}
			else {
				float p = elapsed_flame / float(add_period);
				int should_be_added = p * NUM_PARTICLES;
				particles_to_add = should_be_added - added_particles;
				if (particles_to_add < 0) {
					particles_to_add = 0;
				}
				if (added_particles + particles_to_add > NUM_PARTICLES) {
					particles_to_add = NUM_PARTICLES - added_particles;
				}
			}

			for (int i = 0; i < particles_to_add; i++) {
				add_particle();
			}

			for (auto it = particles.begin(); it != particles.end();) {
				Particle &p = *it;
				if (GET_TICKS() >= p.start_time+p.lifetime) {
					it = particles.erase(it);
				}
				else {
					p.pos += p.velocity;
					it++;
				}
			}

			if (done_regular_attack && started_adding_particles && particles.size() == 0) {
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::ITEM) {
			if (hop_done) {
				DOGO_GLOBALS->item_sfx[ITEM_APPLE]->play(false);
				Battle_Enemy *enemy = dynamic_cast<Battle_Enemy *>(turn->targets[0]);
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				int amount = stats->fixed.max_hp * 0.25f;
				SDL_Colour colour;
				SDL_Colour shadow_colour;
				std::string text;
				get_use_item_info(amount, ITEM_APPLE, colour, shadow_colour, text);
				auto pos = enemy->get_position();
				gfx::Sprite *sprite = enemy->get_sprite();
				util::Point<int> topleft, bottomright;
				sprite->get_bounds(topleft, bottomright);
				pos += topleft;
				pos.x += (bottomright.x-topleft.x) + 5;
				pos.y -= shim::tile_size/4;
				if (inverse_x) {
					pos.x = shim::screen_size.w - pos.x - shim::font->get_text_width(text);
				}
				NEW_SYSTEM_AND_TASK(BATTLE)
				wedge::Special_Number_Step *step = new wedge::Special_Number_Step(colour, shadow_colour, text, pos, wedge::Special_Number_Step::RISE, new_task, false);
				ADD_STEP(step)
				ADD_TASK(new_task)
				FINISH_SYSTEM(BATTLE)
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
			return true;
		}
	}
	return true;
}

void Enemy_Flame_Frog::set_hop_done(bool done)
{
	hop_done = done;
}

void Enemy_Flame_Frog::add_particle()
{
	Particle p;
	p.start_time = GET_TICKS();
	p.lifetime = MIN_PARTICLE_LIFETIME + util::rand(0, MAX_PARTICLE_LIFETIME-MIN_PARTICLE_LIFETIME);
	p.pos = get_position() + util::Point<int>(35, 9);
	if (inverse_x) {
		p.pos.x = shim::screen_size.w - p.pos.x;
	}
	float min_x_vel = 0.4f;
	float max_x_vel = 0.667f;
	float min_y_vel = -0.05f;
	float max_y_vel = 0.25f;
	p.velocity.x = min_x_vel + float(util::rand(0, 1000))/1000.0f * (max_x_vel - min_x_vel);
	if (inverse_x) {
		p.velocity.x = -p.velocity.x;
	}
	p.velocity.y = min_y_vel + float(util::rand(0, 1000))/1000.0f * (max_y_vel - min_y_vel);
	p.velocity.y = -p.velocity.y;
	particles.push_back(p);
	added_particles++;
}

void Enemy_Flame_Frog::draw_fore()
{
	Battle_Enemy::draw_fore();

	Uint32 now = GET_TICKS();

	particle->get_parent_image()->start_batch();

	for (auto &p : particles) {
		Uint32 elapsed = now - p.start_time;
		float f = elapsed / (float)p.lifetime;
		f = MIN(0.999f, f);
		int palette_index = f * palette_size;
		int num_frames = particle->get_num_frames();
		int frame = f * num_frames;
		gfx::Image *img = particle->get_image(frame);
		float alpha;
		if (f < 0.75f) {
			alpha = 1.0f;
		}
		else {
			alpha = 1.0f - ((f - 0.8f) / 0.2f);
		}
		alpha = MIN(1.0f, MAX(0.0f, alpha));
		SDL_Colour c = palette[palette_index];
		c.r *= alpha;
		c.g *= alpha;
		c.b *= alpha;
		c.a *= alpha;
		img->draw_tinted(c, util::Point<float>(p.pos.x-img->size.w/2.0f, p.pos.y-img->size.h/2.0f), inverse_x ? gfx::Image::FLIP_H : 0);
	}
	
	particle->get_parent_image()->end_batch();
}

//--

int Enemy_Bot::count = 0;
audio::Sound *Enemy_Bot::bot;

void Enemy_Bot::static_start()
{
	count = 0;
}

Enemy_Bot::Enemy_Bot() :
	Battle_Enemy(GLOBALS->game_t->translate(357)/* Originally: Bot */, 2750, 5500)
{
	if (count == 0) {
		bot = new audio::MML("sfx/bot.mml");
		bot->play(true);
	}
	count++;

	machine_gun = new audio::MML("sfx/machine_gun.mml");
	attack_sounds.clear();
	attack_sounds.push_back(machine_gun);

	draw_shadow = true;
	shadow_pos = {27, 43};
	shadow_size = {21, 11};
}

Enemy_Bot::~Enemy_Bot()
{
	count--;
	if (count == 0) {
		delete bot;
	}
	
	delete machine_gun;
}

bool Enemy_Bot::start()
{
	gold = 35;
	sprite = new gfx::Sprite("bot");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 90;
	stats->fixed.attack = 36 * enemy_attack_mul();
	stats->fixed.defence = 28 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	attack_names.push_back(GLOBALS->game_t->translate(358)/* Originally: Machine Gun */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Bot::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Bot::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

bool Enemy_Bot::take_hit(wedge::Battle_Entity *actor, int damage)
{
	bool ret = Battle_Enemy::take_hit(actor, damage);
	bool all_dead = true;
	auto enemies = BATTLE->get_enemies();
	for (auto e : enemies) {
		if (dynamic_cast<Enemy_Bot *>(e)) {
			if (e->get_stats()->hp > 0) {
				all_dead = false;
				break;
			}
		}
	}
	if (all_dead) {
		bot->stop();
	}
	return ret;
}

//--

int Enemy_Goblin::count = 0;
gfx::Sprite *Enemy_Goblin::bomb;
SDL_Colour Enemy_Goblin::palette[256];
int Enemy_Goblin::palette_size;
gfx::Image *Enemy_Goblin::particles[3];

void Enemy_Goblin::static_start()
{
	count = 0;
}

Enemy_Goblin::Enemy_Goblin() :
	Battle_Enemy(GLOBALS->game_t->translate(359)/* Originally: Goblin */, 2750, 5500),
	doing_bomb(false)
{
	if (count == 0) {
		bomb = new gfx::Sprite("bomb");
		palette_size = gfx::load_palette("explosion.gpl", palette, 256);
		particles[0] = new gfx::Image("misc/explosion1.tga");
		particles[1] = new gfx::Image("misc/explosion2.tga");
		particles[2] = new gfx::Image("misc/explosion3.tga");
	}
	count++;
	
	explosion = new audio::MML("sfx/explosion.mml");
	
	draw_shadow = true;
	shadow_size = {12, 6};
	auto_shadow_pos = true;
}

Enemy_Goblin::~Enemy_Goblin()
{
	count--;
	if (count == 0) {
		delete bomb;
		delete particles[0];
		delete particles[1];
		delete particles[2];
	}

	delete explosion;
}

bool Enemy_Goblin::start()
{
	gold = 32;
	sprite = new gfx::Sprite("goblin");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 80;
	stats->fixed.attack = 34 * enemy_attack_mul();
	stats->fixed.defence = 28 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);

	attack_names.push_back(GLOBALS->game_t->translate(360)/* Originally: Stab */);
	attack_names.push_back(GLOBALS->game_t->translate(361)/* Originally: Bomb */);
	attack_anims.push_back("throw");
	attack_mults.clear();
	attack_mults.push_back(0.9f);
	attack_mults.push_back(1.1f);

	attack_sounds.clear();
	attack_sounds.push_back(DOGO_GLOBALS->melee2);
	attack_sounds.push_back(nullptr);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Goblin::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->targets.push_back(BATTLE->get_random_player());
	t->turn_name = "attack";
	t->started = false;

	return t;
}

bool Enemy_Goblin::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			int type = start_attack(turn);
			if (type == 1) {
				bomb_start = GET_TICKS();
				doing_bomb = true;
				bomb_target = turn->targets[0];
				thrown = false;
				util::Point<float> pos = calc_arch(0.5f);
				float dy = fabsf(pos.y - (position.y+sprite->get_current_image()->size.h));
				util::Point<int> topleft, bottomright;
				sprite->get_bounds(topleft, bottomright);
				float dx = fabsf(pos.x - (position.x+topleft.x+(bottomright.x-topleft.x)/2.0f));
				float d = sqrtf(dy*dy + dx*dx);
				float lo = 75.0f;
				float hi = 175.0f;
				if (d < lo) {
					d = lo;
				}
				if (d > hi) {
					d = hi;
				}
				float p = (d-lo)/(hi-lo);
				arch_time = MIN_ARCH_TIME + p * (MAX_ARCH_TIME-MIN_ARCH_TIME);
				played_explosion = false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (doing_bomb) {
				if (turn_attack(turn) == true) {
					Uint32 now = GET_TICKS();
					Uint32 elapsed = now - bomb_start;
					if (elapsed >= THROW_TIME+arch_time+EXPLOSION_TIME) {
						sprite->set_animation("idle"); // this triggers the callback, so next frame the attack will end
					}
				}
				else {
					doing_bomb = false;
					return false;
				}
			}
			else {
				if (turn_attack(turn) == false) {
					return false;
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Goblin::draw_fore()
{
	Battle_Enemy::draw_fore();

	if (doing_bomb) {
		Uint32 now = GET_TICKS();
		Uint32 elapsed = now - bomb_start;

		if ((int)elapsed < THROW_TIME || sprite->get_current_frame() < 6) {
			// This would be the throw anim starting before released
			return;
		}

		if (thrown == false) {
			DOGO_GLOBALS->throw_->play(false);
		}

		thrown = true;

		elapsed -= THROW_TIME;

		if (elapsed < arch_time) {
			float p = elapsed / (float)arch_time;

			if (p > 1.0f) {
				p = 1.0f;
			}

			util::Point<float> pos = calc_arch(p);

			auto img = bomb->get_current_image();
			
			if (inverse_x) {
				pos.x = shim::screen_size.w - pos.x;
			}

			img->draw(pos-util::Point<int>(img->size.w/2, img->size.h/2));
		}
		else {
			if (played_explosion == false) {
				explosion->play(false);
				played_explosion = true;
			}

			elapsed -= arch_time;

			float p = elapsed / (float)EXPLOSION_TIME;

			if (p > 1.0f) {
				p = 1.0f;
			}

			float ncircs = 10;

			float p2;

			if (p < 0.15f) {
				p2 = p / 0.15f;
			}
			else if (p >= 0.85f) {
				p2 = 1.0f - ((p - 0.85f) / 0.15f);
			}
			else {
				p2 = 1.0f;
			}

			ncircs *= p2;

			const float min_circ_r = 2;
			const float max_circ_r = 8;

			Battle_Player *player1 = dynamic_cast<Battle_Player *>(bomb_target);
			util::Point<float> pos = player1->get_draw_pos() + util::Point<int>(shim::tile_size/2, shim::tile_size-3);

			int s = p * 15;

			util::srand(s);

			for (int i = 0; i < ncircs; i++) {
				int pal_index = util::rand(0, palette_size-1);
				float r = min_circ_r + (util::rand(0, 1000)/1000.0f * (max_circ_r - min_circ_r));
				r *= p2;
				if (r < 1.25f / shim::scale) {
					continue;
				}
				float a = util::rand(0, 1000)/1000.0f * M_PI;
				float l = util::rand(0, 1000)/1000.0f * 10.0f;
				util::Point<float> draw_pos(pos.x + cos(a) * l, pos.y - sin(a) * l);
				//float angle = util::rand(0, 1000)/1000.0f * M_PI*2.0f;
				//Uint32 t = GET_TICKS() % 250;
				//angle += (t / 250.0f * M_PI * 2.0f) * (util::rand(0, 1000.0f)/1000.0f * 1.0f);
				int part = util::rand(0, 2);

				if (inverse_x) {
					draw_pos.x = shim::screen_size.w - draw_pos.x;
				}

				particles[part]->draw_tinted_rotated_scaled(make_translucent(palette[pal_index], util::rand(0, 1) == 0 ? 0.75f : 1.0f), util::Point<float>(particles[part]->size.w/2.0f, particles[part]->size.h/2.0f), draw_pos, /*angle*/0.0f, (r*2.0f)/particles[part]->size.w, (util::rand(0, 1) == 0 ? gfx::Image::FLIP_V : 0) | (util::rand(0, 1) == 0 ? gfx::Image::FLIP_H : 0));
				//gfx::draw_filled_circle(make_translucent(palette[pal_index], util::rand(0, 1) == 0 ? 1.0f : 0.75f), draw_pos, r, util::rand(5, 8));
			}

			util::srand((Uint32)time(NULL));
		}
	}
}

util::Point<float> Enemy_Goblin::calc_arch(float p)
{
	util::Point<int> hand = {17, 6};

	util::Point<float> p1 = position + hand;
	Battle_Player *player1 = dynamic_cast<Battle_Player *>(bomb_target);
	util::Point<float> p2 = player1->get_draw_pos() + util::Point<int>(shim::tile_size/2, shim::tile_size-3);

	float dx = std::abs(p2.x-p1.x);
	float dy = std::abs(p2.y-p1.y);
	float radius = (dx+dy/2.0f)/2.0f; // guess

	// The loops guesses these
	util::Point<float> centre;
	float start_a;
	float end_a;
	float diff_y = 0.0f;

	while (true) {
		float a = atan(dy/radius);
		bool done = false;
		if (p1.y > p2.y) {
			centre = {p1.x+radius, MAX(p1.y, p2.y)};
			start_a = float(M_PI);
			end_a = M_PI*2 - a;
			float x = cos(end_a) * radius + centre.x;
			float y = sin(end_a) * radius + centre.y;
			float l = std::abs(x-p2.x);
			if (l < 1.0f) {
				done = true;
				diff_y = y - p2.y;
				end_a -= (diff_y/radius/2.0f) * (M_PI/2.0f);
			}
			else {
				if (x > p2.x) {
					radius -= 0.5f;
				}
				else {
					radius += 0.5f;
				}
			}
		}
		else {
			centre = {p2.x-radius, MAX(p1.y, p2.y)};
			start_a = M_PI + a;
			end_a = float(M_PI*2);
			float x = cos(start_a) * radius + centre.x;
			float y = sin(start_a) * radius + centre.y;
			float l = std::abs(x-p1.x);
			if (l < 1.0f) {
				done = true;
				diff_y = y - p1.y;
				start_a += (diff_y/radius/2.0f) * (M_PI/2.0f);
			}
			else {
				if (x > p1.x) {
					radius += 0.5f;
				}
				else {
					radius -= 0.5f;
				}
			}
		}
		if (done) {
			break;
		}
	}

	util::Point<float> pos = centre;
	float a = start_a + p * (end_a-start_a);
	float d_max = diff_y / radius;
	float m;
	if (p1.y < p2.y) {
		if (a < M_PI*1.5f) { // left half of arch
			/*
			float m1 = 1.0f + d_max;
			float m2 = 1.0f + (d_max/2.0f);
			*/
			float m1 = 1.0f + d_max;
			float m2 = 1.0f + d_max;
			float f = a - M_PI;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
		else { // right half
			/*
			float m1 = 1.0f + (d_max/2.0f);
			float m2 = 1.0f;
			*/
			float m1 = 1.0f;
			float m2 = 1.0f;
			float f = a - M_PI*1.5f;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
	}
	else {
		if (a < M_PI*1.5f) {
			/*
			float m1 = 1.0f;
			float m2 = 1.0f + (d_max/2.0f);
			*/
			float m1 = 1.0f;
			float m2 = 1.0f;
			float f = a - M_PI;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
		else {
			/*
			float m1 = 1.0f + (d_max/2.0f);
			float m2 = 1.0f + d_max;
			*/
			float m1 = 1.0f + d_max;
			float m2 = 1.0f + d_max;
			float f = a - M_PI*1.5f;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
	}

	// FIXME?
	m = 1.0f;

	pos.x += cos(a) * radius;
	pos.y += sin(a) * radius * m;

	return pos;
}

//--

int Enemy_Red_Wasp::count = 0;
audio::Sound *Enemy_Red_Wasp::red_wasp;

void Enemy_Red_Wasp::static_start()
{
	count = 0;
}

Enemy_Red_Wasp::Enemy_Red_Wasp() :
	Battle_Enemy(GLOBALS->game_t->translate(365)/* Originally: Red_Wasp */, 2750, 5500)
{
	if (count == 0) {
		red_wasp = new audio::MML("sfx/red_wasp.mml");
		red_wasp->play(0.2f, true);
	}
	count++;

	draw_shadow = true;
	shadow_size = {26, 13};
	auto_shadow_pos = true;
}

Enemy_Red_Wasp::~Enemy_Red_Wasp()
{
	count--;
	if (count == 0) {
		delete red_wasp;
	}
}

bool Enemy_Red_Wasp::start()
{
	gold = 40;

	sprite = new gfx::Sprite("red_wasp");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 100;
	stats->fixed.attack = 35 * enemy_attack_mul();
	stats->fixed.defence = 27 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 30);

	attack_names.push_back(GLOBALS->game_t->translate(367)/* Originally: Sting */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Red_Wasp::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Red_Wasp::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

//--

static void chilly_spit2(void *data)
{
	auto chilly = static_cast<Enemy_Chilly *>(data);
	auto sprite = chilly->get_sprite();
	sprite->set_animation("idle");
}

static void chilly_spit1(void *data)
{
	auto chilly = static_cast<Enemy_Chilly *>(data);
	auto sprite = chilly->get_sprite();
	sprite->set_animation("turn_e_s", chilly_spit2, chilly);
}

static void chilly_spit0(void *data)
{
	auto chilly = static_cast<Enemy_Chilly *>(data);
	auto sprite = chilly->get_sprite();
	sprite->set_animation("spit", chilly_spit1, chilly);
	chilly->play_spit_sound();
}

static void chilly_impact(void *data)
{
	auto chilly = static_cast<Enemy_Chilly *>(data);
	chilly->set_impact_done();
}

static void chilly_rage(void *data)
{
	auto chilly = static_cast<Enemy_Chilly *>(data);
	auto sprite = chilly->get_sprite();
	sprite->set_animation("idle");
}

static void chilly_dead(void *data)
{
	auto chilly = static_cast<Enemy_Chilly *>(data);
	auto sprite = chilly->get_sprite();
	sprite->set_animation("dead");
}

static void chilly_shake(void *data)
{
	gfx::screen_shake(0.75f, 350);
}

static void chilly_rage_rumble(void *data)
{
	gfx::screen_shake(1.0f, 500);

	NEW_SYSTEM_AND_TASK(BATTLE)
	ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(BATTLE)
}

static void chilly_rage_battle()
{
	NEW_SYSTEM_AND_TASK(BATTLE)
	ADD_STEP(new wedge::Delay_Step(550, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_rumble, nullptr, new_task))
	ADD_STEP(new wedge::Delay_Step(550, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_rumble, nullptr, new_task))
	ADD_STEP(new wedge::Delay_Step(550, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_rumble, nullptr, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(BATTLE)
}

Enemy_Chilly::Enemy_Chilly() :
	Battle_Enemy(GLOBALS->game_t->translate(419)/* Originally: Chilly */, 2750, 5500)
{
	draw_shadow = true;
	shadow_size = {48, 24};
	shadow_pos = {43, 76};

	chilly_thud = new audio::Sample("chilly_thud.flac");
	chilly_spit = new audio::Sample("chilly_spit.flac");
	chilly_groan = new audio::Sample("chilly_groan.flac");
	icicle_impact = new audio::MML("sfx/icicle_impact.mml");
	icicle = new gfx::Sprite("icicle");
	goblin_img = new gfx::Image("misc/goblin.tga");

	goblin0_pos = util::Point<int>(shim::tile_size*6, shim::tile_size*1.75f);
	goblin1_pos = util::Point<int>(shim::tile_size*6.25, shim::tile_size*3.25f);
}

Enemy_Chilly::~Enemy_Chilly()
{
	delete chilly_thud;
	delete chilly_spit;
	delete chilly_groan;
	delete icicle_impact;
	delete icicle;
	delete goblin_img;
}

bool Enemy_Chilly::start()
{
	gold = 130;

	sprite = new gfx::Sprite("chilly");

	spit_len = 0;
	sprite->set_animation("turn_e");
	spit_len += sprite->get_length();
	sprite->set_animation("spit");
	spit_len += sprite->get_length();
	sprite->set_animation("turn_e_s");
	spit_len += sprite->get_length();

	sprite->set_animation("idle");

	icicle->set_animation("impact");
	impact_len = icicle->get_length();

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 650;
	stats->fixed.attack = 42 * enemy_attack_mul();
	stats->fixed.defence = 32 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 25);
	
	attack_names.push_back(GLOBALS->game_t->translate(249)/* Originally: Bounce */);

	attack_sounds.clear();
	attack_sounds.push_back(nullptr);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Chilly::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	if (stats->hp >= 1000000) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;

	auto enemies = BATTLE->get_enemies();

	if (stats->hp < 1000000 && enemies.size() < 3) {
		move = SPIT;
		if (enemies.size() == 1) {
			spit_num = 0;
		}
		else {
			Enemy_Goblin *goblin = nullptr;
			for (auto e : enemies) {
				Enemy_Goblin *g = dynamic_cast<Enemy_Goblin *>(e);
				if (g != nullptr) {
					goblin = g;
					break;
				}
			}
			if (goblin->get_position().y == goblin0_pos.y) {
				spit_num = 1;
			}
			else {
				spit_num = 0;
			}
		}
		t->turn_name = "spit";
		t->targets.push_back(this); // just need something, because below it checks this and does nothing if it's empty
	}
	else {
		if (util::rand(0, 1) == 0) {
			move = BOUNCE;
			t->turn_name = "attack";
			t->targets.push_back(BATTLE->get_random_player());
		}
		else {
			move = RAGE;
			t->turn_name = "rage";
			auto v = BATTLE->get_players();
			for (auto p : v) {
				if (p->get_stats()->hp > 0) {
					if (util::rand(0, 1) == 0) {
						t->targets.push_back(p);
					}
				}
			}
			icicle_targets = t->targets;
		}
	}

	return t;
}

bool Enemy_Chilly::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (move == SPIT) {
			spit_start = GET_TICKS();
			sprite->set_animation("turn_e", chilly_spit0, this);
			goblin_added = false;
			auto b = dynamic_cast<Battle_Game *>(BATTLE);
			b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(420)/* Originally: Huck */);
		}
		else if (move == RAGE) {
			impact_done = false;
			icicle->set_animation("falling");
			sprite->set_animation("rage", chilly_rage, this);
			DOGO_GLOBALS->jump->play(false);
			chilly_thud->play(false);
			chilly_rage_battle();
			rage_start = GET_TICKS();
			drop_sound_done = false;
			smash_sound_played = false;

			// Remove any dead players from icicle_targets
			for (auto it = icicle_targets.begin(); it != icicle_targets.end();) {
				auto e = *it;
				if (e->get_stats()->hp <= 0) {
					it = icicle_targets.erase(it);
				}
				else {
					it++;
				}
			}
			
			auto b = dynamic_cast<Battle_Game *>(BATTLE);
			b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(421)/* Originally: Rage */);
		}
		else if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			hit_ground = false;
			DOGO_GLOBALS->jump->play(false);

			NEW_SYSTEM_AND_TASK(BATTLE)
			ADD_STEP(new wedge::Delay_Step(450, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_shake, nullptr, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(BATTLE)

			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (move == SPIT) {
			Uint32 now = GET_TICKS();
			int elapsed = now - spit_start;

			if (goblin_added == false) {
				if (elapsed >= SPIT_START + SPIT_TIME) {
					goblin_added = true;
					
					util::Point<int> pos;
					if (spit_num == 0) {
						pos = goblin0_pos;
					}
					else {
						pos = goblin1_pos;
					}

					wedge::Battle_Enemy *goblin = new Enemy_Goblin();
					goblin->start();
					goblin->set_position(pos);

					static_cast<Battle_Game *>(BATTLE)->add_gold(goblin->get_gold());

					BATTLE->add_enemy(goblin);
				}
			}

			if (elapsed >= spit_len) {
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);

				return false;
			}
		}
		else if (move == RAGE) {
			Uint32 now = GET_TICKS();

			if (impact_done && now >= impact_done_time + impact_len + IMPACT_FADE) {
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);

				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			Uint32 now = GET_TICKS();
			if (hit_ground == false) {
				//int nf = sprite->get_num_frames();
				int cf = sprite->get_current_frame();
				if  (cf == 3) {
					hit_ground = true;
					hit_ground_time = now;
					for (auto p : turn->targets) {
						p->get_sprite()->set_animation("hit");
					}
					DOGO_GLOBALS->thud->play(false);
				}
			}
			else if (int(now-hit_ground_time) > BOUNCE_TIME) {
				for (auto p : turn->targets) {
					static_cast<Battle_Player *>(p)->set_run_offset({0.0f, 0.0f});
					p->get_sprite()->set_animation(p->get_sprite()->get_previous_animation());
					int damage = MAX(1, stats->fixed.attack*1.1f-p->get_stats()->fixed.defence);
					add_special_number(this, p, damage, true, true);
				}
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
			else {
				Uint32 elapsed = now - hit_ground_time;
				float p = elapsed / (float)BOUNCE_TIME;
				if (p > 1.0f) {
					p = 1.0f;
				}
				const float max_h = 10.0f;
				float h = sin(p*M_PI) * max_h;
				for (auto p : turn->targets) {
					static_cast<Battle_Player *>(p)->set_run_offset({0.0f, -h});
				}
			}
			return true;
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Chilly::draw_fore()
{
	Uint32 now = GET_TICKS();

	if (turn_started && move == SPIT && goblin_added == false) {
		int elapsed = now - spit_start;

		if (elapsed >= SPIT_START) {
			elapsed -= SPIT_START;
			float p = elapsed / (float)SPIT_TIME;
			if (p > 1.0f) {
				p = 1.0f;
			}
			util::Point<int> end_pos;
			if (spit_num == 0) {
				end_pos = goblin0_pos;
			}
			else {
				end_pos = goblin1_pos;
			}
			end_pos += util::Point<int>(shim::tile_size/2, shim::tile_size/2); // centre
			util::Point<int> start_pos = get_position() + util::Point<int>(70, 48);

			util::Point<float> pos = start_pos + util::Point<float>((end_pos.x-start_pos.x)*p, (end_pos.y-start_pos.y)*p);

			const int spins = 1;

			goblin_img->draw_rotated(util::Point<float>(goblin_img->size.w/2.0f, goblin_img->size.h/2.0f), pos, p * M_PI*2 * spins);
		}
	}
	else if (turn_started && move == RAGE && now >= rage_start + ICICLE_DELAY) {
		if (drop_sound_done == false) {
			drop_sound_done = true;
			for (size_t i = 0; i < icicle_targets.size(); i++) {
				DOGO_GLOBALS->fall->play(false);
			}
		}

		int elapsed = now - rage_start - ICICLE_DELAY;

		if (elapsed < ICICLE_FALL) {
			float p = elapsed / (float)ICICLE_FALL;

			for (auto e : icicle_targets) {
				Battle_Player *player = static_cast<Battle_Player *>(e);
				util::Point<int> pos = player->get_draw_pos();

				const int max_h = 160;

				float h = max_h * (1.0f - p);

				gfx::Image *img = icicle->get_current_image();
				int w = img->size.w;

				int x = pos.x - (w - shim::tile_size);

				img->draw(util::Point<float>(x, (pos.y + shim::tile_size) - img->size.h - h));
			}
		}
		else {
			if (smash_sound_played == false) {
				icicle->set_animation("impact", chilly_impact, this);
				for (auto p : icicle_targets) {
					int damage = MAX(1, stats->fixed.attack-p->get_stats()->fixed.defence);
					add_special_number(this, p, damage, true, true);
				}
			}
			
			for (auto e : icicle_targets) {
				if (smash_sound_played == false) {
					icicle_impact->play(false);
				}

				Battle_Player *player = static_cast<Battle_Player *>(e);
				util::Point<int> pos = player->get_draw_pos();

				gfx::Image *img;
				SDL_Colour tint;

				if (impact_done == false) {
					img = icicle->get_current_image();
					tint = shim::white;
				}
				else {
					img = icicle->get_image(icicle->get_num_frames()-1);
					tint = shim::white;
					int elapsed = now - impact_done_time;
					float p = elapsed / (float)IMPACT_FADE;
					if (p > 1.0f) {
						p = 1.0f;
					}
					p = 1.0f - p;
					tint.r *= p;
					tint.g *= p;
					tint.b *= p;
					tint.a *= p;
				}
				
				int w = img->size.w;

				int x = pos.x - (w - shim::tile_size);

				img->draw_tinted(tint, util::Point<float>(x, pos.y + shim::tile_size - img->size.h));
			}
			
			smash_sound_played = true;
		}
	}
}

void Enemy_Chilly::play_spit_sound()
{
	// Note: needs a delay
	NEW_SYSTEM_AND_TASK(BATTLE)
	ADD_STEP(new wedge::Delay_Step(250, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(chilly_spit, false, false, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(BATTLE)
}

bool Enemy_Chilly::take_hit(wedge::Battle_Entity *actor, int damage)
{
	if (stats->hp - damage <= 0) {
		stats->hp = 1000000;
		sprite->set_animation("dead_anim", chilly_dead, this);
		BATTLE->drop_entity_turns(this);
		chilly_groan->play(false);
	}
	else if (stats->hp < 1000000) {
		return Battle_Enemy::take_hit(actor, damage);
	}

	return false;
}

void Enemy_Chilly::set_impact_done()
{
	impact_done = true;
	impact_done_time = GET_TICKS();
}

//--

std::map< gfx::Sprite *, Enemy_Snapper::ripple_info > Enemy_Snapper::water_ripples;

void Enemy_Snapper::static_start()
{
	water_ripples.clear();
}

void Enemy_Snapper::end_water_ripple(void *data)
{
	gfx::Sprite *sprite = static_cast<gfx::Sprite *>(data);
	auto it = water_ripples.find(sprite);
	if (it != water_ripples.end()) {
		delete sprite;
		water_ripples.erase(it);
	}
}

Enemy_Snapper::Enemy_Snapper() :
	Battle_Enemy(GLOBALS->game_t->translate(508)/* Originally: Snapper */, 2500, 5000),
	last_sprite_centre(-1),
	last_sprite_bottom(-1),
	played_chomp(false),
	played_splash2(false)
{
	chomp = new audio::Sample("chomp.flac");
	splash_short = new audio::MML("sfx/splash_short.mml");
	
	attack_sounds.clear();
	attack_sounds.push_back(splash_short);
}

Enemy_Snapper::~Enemy_Snapper()
{
	for (auto p : water_ripples) {
		delete p.first;
	}
	water_ripples.clear();

	delete chomp;
	delete splash_short;
}

bool Enemy_Snapper::start()
{
	gold = 32;

	sprite = new gfx::Sprite("snapper");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 95;
	stats->fixed.attack = 32 * enemy_attack_mul();
	stats->fixed.defence = 32 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	attack_names.push_back(GLOBALS->game_t->translate(510)/* Originally: Chomp */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Snapper::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Snapper::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (played_chomp == false && sprite->get_current_frame() == 5) {
				played_chomp = true;
				chomp->play(false);
			}
			if (played_splash2 == false && sprite->get_current_frame() == 13) {
				played_splash2 = true;
				splash_short->play(false);
			}
			if (turn_attack(turn) == false) {
				played_chomp = false;
				played_splash2 = false;
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Snapper::draw()
{
	util::Point<float> pos = position;
	auto current_image = sprite->get_current_image();
	auto b = static_cast<Battle_Game *>(BATTLE);
	bool in_water = b->in_water();
	
	if (in_water) {
		float wh = b->get_water_height();

		for (auto p : water_ripples) {
			if (p.second.snapper == this) {
				gfx::Image *img = p.first->get_current_image();
				util::Point<int> pos = p.second.pos;
				if (inverse_x) {
					pos.x = shim::screen_size.w-pos.x;
				}
				img->draw({ float(pos.x-img->size.w/2.0f), float(pos.y-img->size.h/2.0f-wh) });
			}
		}

		if (shim::opengl) {
			glEnable_ptr(GL_SCISSOR_TEST);
			int h = (pos.y+current_image->size.h-wh)*shim::scale+shim::screen_offset.y;
			glScissor_ptr(0, shim::real_screen_size.h-h, shim::screen_size.w*shim::scale+shim::screen_offset.x, h);
			PRINT_GL_ERROR("glScissor");
		}
#ifdef _WIN32
		else {
			shim::d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			RECT scissor = { 0, 0, (LONG)(shim::screen_size.w*shim::scale+shim::screen_offset.x), (LONG)((pos.y+current_image->size.h-wh)*shim::scale+shim::screen_offset.y) };
			shim::d3d_device->SetScissorRect(&scissor);
		}
#endif
	}

	Battle_Enemy::draw();
	
	if (in_water) {
		gfx::unset_scissor();
	}
}

void Enemy_Snapper::run()
{
	if (static_cast<Battle_Game *>(BATTLE)->in_water()) {
		gfx::Image *current_image = sprite->get_current_image();
		util::Point<int> topleft, bottomright;
		current_image->get_bounds(topleft, bottomright);
		util::Point<float> pos = position;
		int centre_x = pos.x + topleft.x + (bottomright.x-topleft.x)/2;
		int bottom_y = pos.y + bottomright.y;
		int abs_bottom = pos.y + current_image->size.h;
		auto b = static_cast<Battle_Game *>(BATTLE);
		float wh = b->get_water_height();
		bool was_below = last_sprite_bottom == -1 ? true : last_sprite_bottom >= (abs_bottom-wh);
		bool is_below = bottom_y >= (abs_bottom-wh);
		bool moved_x = last_sprite_centre == -1 ? false : std::abs(centre_x-last_sprite_centre) >= 3;
		bool do_ripple = (moved_x && was_below && is_below) || (was_below != is_below);
		if (last_sprite_centre == -1 || moved_x) {
			last_sprite_centre = centre_x;
		}
		if (last_sprite_bottom == -1 || was_below != is_below) {
			last_sprite_bottom = bottom_y;
		}
		if (do_ripple) {
			for (auto it = water_ripples.begin(); it != water_ripples.end(); it++) {
				auto p = *it;
				if (p.second.snapper == this) {
					delete p.first;
					water_ripples.erase(it);
					break;
				}
			}

			gfx::Sprite *wr = new gfx::Sprite("water_ripple2");
			ripple_info info;
			info.pos = util::Point<int>{ last_sprite_centre, int(pos.y + current_image->size.h) };
			info.snapper = this;
			wr->set_animation("only", end_water_ripple, wr);
			water_ripples[wr] = info;
		}
	}
}

util::Point<int> Enemy_Snapper::get_turn_order_display_offset()
{
	return util::Point<int>(15, -5);
}

//--

Enemy_Bone1::Enemy_Bone1() :
	Battle_Enemy(GLOBALS->game_t->translate(509)/* Originally: Bone */, 2500, 5000)
{
	sound = new audio::MML("sfx/bone1.mml");
	
	attack_sounds.clear();
	attack_sounds.push_back(sound);
}

Enemy_Bone1::~Enemy_Bone1()
{
	delete sound;
}

bool Enemy_Bone1::start()
{
	gold = 52;

	sprite = new gfx::Sprite("bone1");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 150;
	stats->fixed.attack = 38 * enemy_attack_mul();
	stats->fixed.defence = 35 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	attack_names.push_back(GLOBALS->game_t->translate(511)/* Originally: Club */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Bone1::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Bone1::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

//--

Enemy_Bone2::Enemy_Bone2() :
	Battle_Enemy(GLOBALS->game_t->translate(509)/* Originally: Bone */, 2500, 5000)
{
	sound = new audio::MML("sfx/bone2.mml");
	
	attack_sounds.clear();
	attack_sounds.push_back(sound);
}

Enemy_Bone2::~Enemy_Bone2()
{
	delete sound;
}

bool Enemy_Bone2::start()
{
	gold = 52;

	sprite = new gfx::Sprite("bone2");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 150;
	stats->fixed.attack = 38 * enemy_attack_mul();
	stats->fixed.defence = 35 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	attack_names.push_back(GLOBALS->game_t->translate(512)/* Originally: Clobber */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Bone2::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Bone2::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

//--

int Enemy_Aura::count;
gfx::Image *Enemy_Aura::work1;
gfx::Image *Enemy_Aura::work2;
gfx::Sprite *Enemy_Aura::main_auraball;
audio::Sound *Enemy_Aura::loop_sfx;

void Enemy_Aura::static_start()
{
	count = 0;
	work1 = nullptr;
	work2 = nullptr;
	main_auraball = nullptr;
	loop_sfx = nullptr;
}

void Enemy_Aura::end_spawn_anim(void *data)
{
	auto aura = static_cast<Enemy_Aura *>(data);
	aura->get_sprite()->set_animation("idle");
}

Enemy_Aura::Enemy_Aura() :
	Battle_Enemy(GLOBALS->game_t->translate(13)/* Originally: Aura */, 3000, 6000),
	spawning(false)
{
	if (count == 0) {
		loop_sfx = new audio::MML("sfx/aura.mml");
		loop_sfx->play(true);
	}
	found_device();

	draw_shadow = true;
	shadow_size = {40, 20};
	auto_shadow_pos = true;
	shadow_offset.y = static_cast<Battle_Game *>(BATTLE)->in_water() ? 2 : 6;

	auraball = new gfx::Sprite("auraball");

	auraball_appear = new audio::Sample("auraball_appear.flac");
	auraball_hit = new audio::MML("sfx/auraball_hit.mml");

	init_glow(main_g, RING_MIN, RING_MAX, RING_TIME, HEX_MIN, HEX_MAX, HEX_TIME);
}

Enemy_Aura::~Enemy_Aura()
{
	lost_device();
	if (count == 0) {
		delete loop_sfx;
	}

	delete auraball;

	delete auraball_appear;
	delete auraball_hit;
}

void Enemy_Aura::init_glow(glow &g, int RING_MIN, int RING_MAX, int RING_TIME, int HEX_MIN, int HEX_MAX, int HEX_TIME)
{
	g.glow_alpha = float(util::rand(0, 1000)) / 1000.0f;
	g.glow_dir = util::rand(0, 1) == 0 ? -1 : 1;
	g.hex_angle = float(M_PI);
	g.ring_dir = util::rand(0, 1) == 0 ? -1 : 1;
	if (g.ring_dir == 1) {
		g.ring_prev = RING_MIN;
	}
	else {
		g.ring_prev = RING_MAX;
	}
	g.hex_dir = util::rand(0, 1) == 0 ? -1 : 1;
	if (g.hex_dir == 1) {
		g.hex_prev = HEX_MIN;
	}
	else {
		g.hex_prev = HEX_MAX;
	}
	g.ring_interp = new math::I_Hermite();
	g.ring_interp->start(0, 0, 1.0f, 1.0f, RING_TIME);
	g.hex_interp = new math::I_Hermite();
	g.hex_interp->start(0, 0, 1.0f, 1.0f, HEX_TIME);
	if (g.ring_dir == 1) {
		g.ring_dest = util::rand((int)g.ring_prev, RING_MAX);
	}
	else {
		g.ring_dest = util::rand(RING_MIN, (int)g.ring_prev);
	}
	if (g.hex_dir == 1) {
		g.hex_dest = util::rand((int)g.hex_prev, HEX_MAX);
	}
	else {
		g.hex_dest = util::rand(HEX_MIN, (int)g.hex_prev);
	}
}

void Enemy_Aura::update_glow(glow &g, int RING_MIN, int RING_MAX, int RING_TIME, int HEX_MIN, int HEX_MAX, int HEX_TIME)
{
	bool done;
	done = g.ring_interp->interpolate(1) == false;
	float ring_val = g.ring_interp->get_value();
	if (g.ring_dir == 1) {
		g.ring_sz = g.ring_prev + (g.ring_dest-g.ring_prev) * ring_val;
	}
	else {
		g.ring_sz = g.ring_prev - (g.ring_prev-g.ring_dest) * ring_val;
	}
	g.ring_thickness = (g.ring_sz-RING_MIN) / (float)(RING_MAX-RING_MIN) * 4.0f;
	if (done) {
		g.ring_interp->start(0, 0, 1.0f, 1.0f, RING_TIME);
		g.ring_dir = -g.ring_dir;
		g.ring_prev = g.ring_dest;
		if (g.ring_dir == 1) {
			g.ring_dest = util::rand((int)g.ring_prev, RING_MAX);
		}
		else {
			g.ring_dest = util::rand(RING_MIN, (int)g.ring_prev);
		}
	}
	done = g.hex_interp->interpolate(1) == false;
	float hex_val = g.hex_interp->get_value();
	if (g.hex_dir == 1) {
		g.hex_sz = g.hex_prev + (g.hex_dest-g.hex_prev) * hex_val;
	}
	else {
		g.hex_sz = g.hex_prev - (g.hex_prev-g.hex_dest) * hex_val;
	}
	if (done) {
		g.hex_interp->start(0, 0, 1.0f, 1.0f, HEX_TIME);
		g.hex_dir = -g.hex_dir;
		g.hex_prev = g.hex_dest;
		if (g.hex_dir == 1) {
			g.hex_dest = util::rand((int)g.hex_prev, HEX_MAX);
		}
		else {
			g.hex_dest = util::rand(HEX_MIN, (int)g.hex_prev);
		}
	}
	g.hex_angle += 0.015f;
	g.glow_alpha += g.glow_dir == 1 ? 0.01f : -0.01f;
	if (g.glow_dir == 1) {
		if (g.glow_alpha > 1.0f) {
			g.glow_alpha = 1.0f - (g.glow_alpha - 1.0f);
			g.glow_dir = -g.glow_dir;
		}
	}
	else {
		if (g.glow_alpha < 0.0f) {
			g.glow_alpha = -g.glow_alpha;
			g.glow_dir = -g.glow_dir;
		}
	}
}

void Enemy_Aura::draw_glow(float alpha, glow &g, util::Point<float> pos)
{
	glm::mat4 save_mv, save_p;
	gfx::get_matrices(save_mv, save_p);

	gfx::set_target_image(work1);
	gfx::clear(shim::transparent);
	gfx::draw_circle(shim::palette[8], util::Point<int>(work1->size.w/2, work1->size.h/2), g.ring_sz, g.ring_thickness);

	gfx::set_target_image(work2);
	gfx::clear(shim::transparent);
	work1->draw_tinted(make_translucent(shim::white, g.glow_alpha/4.0f), {0, 0});

	gfx::draw_filled_circle(make_translucent(shim::palette[8], (1.0f-g.glow_alpha)/4.0f), util::Point<int>(work2->size.w/2, work2->size.h/2), g.hex_sz, 6, g.hex_angle);

	gfx::set_target_backbuffer();

	gfx::set_matrices(save_mv, save_p);
	gfx::update_projection();

	work2->draw_tinted(make_translucent(shim::white, alpha), pos-util::Point<int>(work2->size.w/2, work2->size.h/2));
}

void Enemy_Aura::found_device()
{
	if (count == 0) {
		work1 = new gfx::Image(util::Size<int>(HEX_MAX*4, HEX_MAX*4));
		work2 = new gfx::Image(util::Size<int>(HEX_MAX*4, HEX_MAX*4));
		main_auraball = new gfx::Sprite("auraball");
	}
	count++;
}

void Enemy_Aura::lost_device()
{
	count--;
	if (count == 0) {
		delete work1;
		work1 = nullptr;
		delete work2;
		work2 = nullptr;
		delete main_auraball;
		main_auraball = nullptr;
	}
}

bool Enemy_Aura::start()
{
	gold = 52;

	sprite = new gfx::Sprite("aura");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 180;
	stats->fixed.attack = 40 * enemy_attack_mul();
	stats->fixed.defence = 36 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Aura::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	int n = util::rand(1, 3);
	for (int i = 0; i < n; i++) {
		wedge::Battle_Entity *e = BATTLE->get_random_player();
		t->targets.push_back(e);
	}
	std::sort(t->targets.begin(), t->targets.end());
	auto last = std::unique(t->targets.begin(), t->targets.end());
	t->targets.erase(last, t->targets.end());
	t->started = false;

	return t;
}

bool Enemy_Aura::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			attack_start = GET_TICKS();
			auraball->set_animation("die");
			spawn_full_len = SPAWN_PRE_TIME + SPAWN_TIME + SPAWN_MOVE_TIME + auraball->get_length();
			auraball->set_animation("normal");
			spawning = true;
			sprite->set_animation("spawn", end_spawn_anim, this);
			spawn_turn = turn;
			did_die = false;
			for (size_t i = 0; i < turn->targets.size(); i++) {
				glow g;
				float p = auraball->get_current_image()->size.w / (float)sprite->get_current_image()->size.w;
				init_glow(g, RING_MIN*p, RING_MAX*p, RING_TIME*p, HEX_MIN*p, HEX_MAX*p, HEX_TIME*p);
				ball_g.push_back(g);
			}
			auto b = dynamic_cast<Battle_Game *>(BATTLE);
			b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(513)/* Originally: Spawn */);
			appear_played = false;
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			Uint32 now = GET_TICKS();
			int elapsed = now - attack_start;
			if (elapsed >= spawn_full_len) {
				spawning = false;
				ball_g.clear();
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Aura::draw_back()
{
	Battle_Enemy::draw_back();

	update_glow(main_g, RING_MIN, RING_MAX, RING_TIME, HEX_MIN, HEX_MAX, HEX_TIME);

	gfx::Image *img = sprite->get_current_image();
	util::Point<int> pos = position;
	if (inverse_x) {
		pos.x = shim::screen_size.w - pos.x - img->size.w;
	}
	float alpha = stats->hp <= 0 ? 1.0f - ((GET_TICKS()-dead_start)/(float)die_time) : 1.0f;
	if (alpha < 0.0f) {
		alpha = 0.0f;
	}
	draw_glow(alpha, main_g, pos+util::Point<int>(img->size.w/2, img->size.h/2));
}

void Enemy_Aura::draw()
{
	Battle_Enemy::draw();
	
	Uint32 now = GET_TICKS();

	int elapsed = now - attack_start;

	if (spawning && elapsed >= SPAWN_PRE_TIME) {
		elapsed -= SPAWN_PRE_TIME;

		if (appear_played == false) {
			appear_played = true;
			auraball_appear->play(false);
		}

		for (size_t i = 0; i < ball_g.size(); i++) {
			float p = auraball->get_current_image()->size.w / (float)sprite->get_current_image()->size.w;
			update_glow(ball_g[i], RING_MIN*p, RING_MAX*p, RING_TIME*p, HEX_MIN*p, HEX_MAX*p, HEX_TIME*p);
		}
		auto players = BATTLE->get_players();
		auto img = auraball->get_current_image();
		auto aura_img = sprite->get_current_image();
		bool do_did_die = false;
		for (size_t i = 0; i < spawn_turn->targets.size(); i++) {
			auto t = spawn_turn->targets[i];
			util::Point<float> start_pos(position.x+aura_img->size.w+3+img->size.w/2, 0);
			if (t == players[0]) {
				start_pos.y = position.y+5;
			}
			else if (t == players[1]) {
				start_pos.y = position.y + aura_img->size.h/2;
			}
			else {
				start_pos.y = position.y + aura_img->size.h-5;
			}
			util::Point<float> end_pos = static_cast<Battle_Player *>(t)->get_draw_pos() + util::Point<int>(-(img->size.w/2+3), 0);
			util::Point<float> pos;
			float alpha;
			float glow_alpha;
			if (elapsed < SPAWN_TIME) {
				pos = start_pos;
				alpha = (float)elapsed / SPAWN_TIME;
				glow_alpha = alpha;
			}
			else if (elapsed < SPAWN_TIME+SPAWN_MOVE_TIME) {
				float p = (elapsed-SPAWN_TIME) / (float)SPAWN_MOVE_TIME;
				pos = start_pos + (end_pos-start_pos) * p;
				alpha = 1.0f;
				glow_alpha = 1.0f;
			}
			else {
				if (did_die == false) {
					do_did_die = true;
					auraball->set_animation("die");
					auraball_hit->play(false);
					int damage = MAX(1, stats->fixed.attack-t->get_stats()->fixed.defence);
					add_special_number(this, t, damage, true, true);
				}
				pos = end_pos;
				alpha = 1.0f;
				float p = (elapsed-SPAWN_TIME-SPAWN_MOVE_TIME) / (float)(spawn_full_len-SPAWN_TIME-SPAWN_MOVE_TIME);
				if (p > 1.0f) {
					p = 1.0f;
				}
				glow_alpha = 1.0f - p;
			}
			if (inverse_x) {
				pos.x = shim::screen_size.w - pos.x;
			}
			draw_glow(glow_alpha, ball_g[i], pos);
			img = auraball->get_current_image(); // get it again because it might have changed to die
			img->draw_tinted(make_translucent(shim::white, alpha), {pos.x-img->size.w/2, pos.y-img->size.h/2});
		}
		if (do_did_die) {
			did_die = true;
		}
	}
}

//--

Enemy_Clone::Enemy_Clone() :
	Battle_Enemy(GLOBALS->game_t->translate(514)/* Originally: Clone */, 2500, 5000)
{
	global_flip_h = true;
	global_tint = shim::black;
	draw_as_player = true;

	draw_shadow = true;
	shadow_size = {12, 6};
	auto_shadow_pos = true;
}

Enemy_Clone::~Enemy_Clone()
{
}

bool Enemy_Clone::start()
{
	gold = 80;

	attack_names.clear();
	attack_anims.clear();
	attack_sounds.clear();
	attack_mults.clear();

	int r = util::rand(0, 2);
	
	if (r == 0) {
		sprite = new gfx::Sprite("pleasant");
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Combo Punch")));
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Axe Hammer")));
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Cyclone")));
	}
	else if (r == 1) {
		sprite = new gfx::Sprite("tom");
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Combo Kick")));
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Dropkick")));
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Spin Kick")));
	}
	else {
		sprite = new gfx::Sprite("wit");
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Beatdown")));
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Windmill")));
		attack_names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("The Claw")));
	}

	sprite->set_animation("idle");

	for (size_t i = 0; i < attack_names.size(); i++) {
		std::string attack_name = util::lowercase(attack_names[i]);
		for (size_t i = 0; i < attack_name.length(); i++) {
			if (attack_name[i] == ' ') {
				attack_name[i] = '_';
			}
		}
		attack_anims.push_back("attack_" + attack_name);
		attack_sounds.push_back(get_combo_sfx(attack_name));
		attack_mults.push_back(get_combo_multiplier(attack_name)/4.0f);
	}
		
	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 220;
	stats->fixed.attack = 26 * enemy_attack_mul();
	stats->fixed.defence = 32 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Clone::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Clone::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

//--

int Enemy_CutiePie::count = 0;
audio::Sound *Enemy_CutiePie::snub_sfx;
audio::Sound *Enemy_CutiePie::heart_sfx;
audio::Sound *Enemy_CutiePie::pop_sfx;

Enemy_CutiePie::Enemy_CutiePie() :
	Battle_Enemy(GLOBALS->game_t->translate(522)/* Originally: Cutie Pie */, 2500, 5000)
{
	heart = new gfx::Sprite("heart");
	heart->set_animation("pop");
	pop_time = heart->get_length();

	heart_loop = new audio::MML("sfx/heart_loop.mml");
	
	if (count == 0) {
		snub_sfx = new audio::Sample("raspberry.flac");
		heart_sfx = new audio::Sample("awww.flac");
		pop_sfx = new audio::Sample("pop.flac");
	}
	count++;

	draw_shadow = true;
	shadow_pos = {18, 24};
	shadow_size = {24, 12};
}

Enemy_CutiePie::~Enemy_CutiePie()
{
	delete heart;
	delete heart_loop;
	
	count--;
	if (count == 0) {
		delete snub_sfx;
		delete heart_sfx;
		delete pop_sfx;
	}
}

bool Enemy_CutiePie::start()
{
	gold = 65;
	sprite = new gfx::Sprite("cutie_pie");

	attack_names.push_back(GLOBALS->game_t->translate(523)/* Originally: Snub */);
	attack_sounds.clear();
	attack_sounds.push_back(snub_sfx);
		
	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 180;
	stats->fixed.attack = 40 * enemy_attack_mul();
	stats->fixed.defence = 36 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	return true;
}

wedge::Battle_Game::Turn *Enemy_CutiePie::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;
		
	t->actor = this;
	t->started = false;
	t->turn_type = wedge::Battle_Game::ATTACK;

	if (util::rand(0, 1) == 0) {
		t->turn_name = "attack";
		t->targets.push_back(BATTLE->get_random_player());
		tt = SNUB;
	}
	else {
		t->turn_name = "heart";
		t->targets.push_back(BATTLE->get_random_player());
		auto rp = BATTLE->get_random_player();
		if (t->targets[0] != rp) {
			t->targets.push_back(rp);
		}
		tt = HEART;
	}

	return t;
}

bool Enemy_CutiePie::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (tt == SNUB) {
				start_attack(turn);
			}
			else {
				attack_done = false;
				sprite->set_animation("heart", attack_callback, this);
				heart_sfx->play(false);
				heart_turn = turn;
				heart->set_animation("normal");
				popped = false;
				heart_loop->play(true);
				auto b = dynamic_cast<Battle_Game *>(BATTLE);
				b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(524)/* Originally: Heart */);
			}
			turn_start = GET_TICKS();
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (tt == SNUB) {
				if (turn_attack(turn) == false) {
					return false;
				}
			}
			else {
				if (int(GET_TICKS() - turn_start) >= HEART_TIME+pop_time) {
					heart_loop->stop();
					turn_started = false;
					next_move = GET_TICKS() + util::rand(min_delay, max_delay);
					for (auto t : turn->targets) {
						int damage = MAX(1, stats->fixed.attack*1.1f-t->get_stats()->fixed.defence);
						add_special_number(this, t, damage, true, true);
					}
					return false;
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_CutiePie::draw_fore()
{
	Battle_Enemy::draw_fore();

	if (turn_started && tt == HEART) {
		for (auto t : heart_turn->targets) {
			util::Point<float> start = position + util::Point<int>(30, 0);
			util::Point<float> end = static_cast<Battle_Player *>(t)->get_draw_pos();

			Uint32 now = GET_TICKS();
			Uint32 elapsed = now - turn_start;

			util::Point<float> pos;
			const int delay = 250;
			if ((int)elapsed >= delay && int(elapsed) < HEART_TIME+pop_time) {
				if ((int)elapsed < HEART_TIME) {
					float p = (elapsed-delay) / (float)(HEART_TIME-delay);
					pos = start + (end-start) * p;
				}
				else {
					if (popped == false) {
						popped = true;
						heart->set_animation("pop");
						pop_sfx->play(false);
					}
					pos = end;
				}
				auto img = heart->get_current_image();

				if (inverse_x) {
					pos.x = shim::screen_size.w - pos.x - img->size.w;
				}

				img->draw(pos);
			}
		}
	}
}

//--

static void thirsty_attack_callback(void *data)
{
	Enemy_Thirsty *e = static_cast<Enemy_Thirsty *>(data);
	e->get_sprite()->set_animation("idle");
}

Enemy_Thirsty::Enemy_Thirsty() :
	Battle_Enemy(GLOBALS->game_t->translate(525)/* Originally: Thirsty */, 2500, 5000)
{
	tsunami = new gfx::Sprite("tsunami");
	tsunami_sfx = new audio::MML("sfx/tsunami.mml");
	splash = new audio::MML("sfx/splash.mml");
}

Enemy_Thirsty::~Enemy_Thirsty()
{
	delete tsunami;
	delete tsunami_sfx;
	delete splash;
}

bool Enemy_Thirsty::start()
{
	gold = 105;
	sprite = new gfx::Sprite("thirsty");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 400;
	stats->fixed.attack = 42 * enemy_attack_mul();
	stats->fixed.defence = 32 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Thirsty::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;
		
	t->actor = this;
	t->started = false;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";

	auto players = BATTLE->get_players();
	for (auto p : players) {
		if (p->get_stats()->hp > 0) {
			t->targets.push_back(p);
		}
	}

	return t;
}

bool Enemy_Thirsty::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			turn_start = GET_TICKS();
			sprite->set_animation("tsunami", thirsty_attack_callback, this);
			played_sfx = false;
			splash->play(false);
			auto b = dynamic_cast<Battle_Game *>(BATTLE);
			b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(526)/* Originally: Tsunami */);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (int(GET_TICKS()-turn_start) >= TSUNAMI_TIME) {
				for (auto t : turn->targets) {
					int damage = MAX(1, stats->fixed.attack-t->get_stats()->fixed.defence);
					add_special_number(this, t, damage, true, true);
				}
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

void Enemy_Thirsty::draw_fore()
{
	Battle_Enemy::draw_fore();

	if (turn_started) {
		Uint32 now = GET_TICKS();
		Uint32 elapsed = now - turn_start;
		if ((int)elapsed >= TSUNAMI_DELAY) {
			if (played_sfx == false) {
				played_sfx = true;
				tsunami->set_animation("only");
				tsunami_sfx->play(false);
				splash->play(false);
			}

			float p = (elapsed-TSUNAMI_DELAY) / (float)TSUNAMI_TIME;
			if (p > 1.0f) {
				p = 1.0f;
			}
			const int start_x = 50;
			const int max_dist = 250;
			float x = start_x + max_dist * p;

			int flag = 0;

			auto img = tsunami->get_current_image();

			if (inverse_x) {
				flag = gfx::Image::FLIP_H;
				x = shim::screen_size.w - x - img->size.w;
			}

			// Draw it in 4 sections so it looks "3D"ish (players split the wave)

			auto b = static_cast<Battle_Game *>(BATTLE);
			auto players = b->get_players();
			int y = 0;
			const int start_y = 10;

			for (int i = 0; i < 3; i++) {
				auto player = static_cast<Battle_Player *>(players[i]);
				auto ppos = player->get_draw_pos();
				int py = ppos.y + shim::tile_size - start_y;
				int h = py - y;
				img->draw_region({0.0f, (float)y}, {img->size.w, h}, {x, float(start_y+y)}, flag);
				const float slope = 54.0f/17.0f; // based on the image
				float run = y / slope;
				float diff = inverse_x ? ((x+img->size.w/3-run) - ppos.x) : (ppos.x - (x+img->size.w/3-run));
				float p;
				if (diff < 0) {
					p = 1.0f;
				}
				else {
					p = 1.0f - (diff/(img->size.w/3));
				}
				float water_h = b->get_water_height();
				if (p >= 0.0f && shim::tile_size*p > water_h) {
					gfx::set_scissor(ppos.x, ppos.y, shim::tile_size, int(shim::tile_size*p-water_h));
					player->get_sprite()->get_current_image()->draw(ppos);
					gfx::unset_scissor();
				}
				y += h;
			}
				
			img->draw_region({0.0f, (float)y}, {img->size.w, img->size.h-y-1}, {x, 10.0f+y}, flag);
		}
	}
}

//--

static void eat_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	Enemy_Bumper *b = static_cast<Enemy_Bumper *>(data);
	b->get_sprite()->set_animation("idle");
	b->set_eat_done(true);
}

Enemy_Bumper::Enemy_Bumper() :
	Battle_Enemy(GLOBALS->game_t->translate(534)/* Originally: Bumper */, 2500, 5000),
	eat_done(false)
{
	attack_sounds.clear();
	attack_sounds.push_back(DOGO_GLOBALS->bumper_growl);
	
	eat_sfx = new audio::Sample("nomnom.flac");
}

Enemy_Bumper::~Enemy_Bumper()
{
	delete eat_sfx;
}

bool Enemy_Bumper::start()
{
	gold = 300;
	sprite = new gfx::Sprite("bumper");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 2000;
	stats->fixed.attack = 50 * enemy_attack_mul();
	stats->fixed.defence = 35 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	attack_names.push_back(GLOBALS->game_t->translate(537)/* Originally: Pound */);

	draw_shadow = true;
	shadow_size = {36, 18};
	auto_shadow_pos = true;

	return true;
}

wedge::Battle_Game::Turn *Enemy_Bumper::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	if (stats->hp >= 1000000) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	int r = util::rand(0, 1);
	if (r == 0) {
		t->turn_type = wedge::Battle_Game::ATTACK;
	}
	else {
		t->turn_type = wedge::Battle_Game::ITEM;
	}
	t->actor = this;

	if (r == 0) {
		t->turn_name = "attack";
		int n = util::rand(1, 3);
		for (int i = 0; i < n; i++) {
			wedge::Battle_Entity *e = BATTLE->get_random_player();
			bool found = false;
			for (int j = 0; j < i; j++) {
				if (e == t->targets[j]) {
					found = true;
					break;
				}
			}
			if (found == false) {
				t->targets.push_back(e);
			}
		}
	}
	else {
		t->turn_type = wedge::Battle_Game::ITEM;
		t->turn_name = "eat";
		t->targets.push_back(this);
	}

	t->started = false;

	return t;
}

bool Enemy_Bumper::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			hit_ground = false;
			hit_num = 0;
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::ITEM) {
			eat_done = false;
			eat_sound_played = false;
			eat_start = GET_TICKS();
			sprite->set_animation("eat", eat_callback, this);
			static_cast<Battle_Game *>(BATTLE)->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(538)/* Originally: Nom Nom */);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			Uint32 now = GET_TICKS();
			if (hit_ground == false) {
				int cf = sprite->get_current_frame();
				if  (cf == 1) {
					DOGO_GLOBALS->thud->play(false);
					hit_ground = true;
					hit_ground_time = now;
					turn->targets[hit_num]->get_sprite()->set_animation("hit");
					wedge::rumble(1.0f, 500);
				}
			}
			else if (now-hit_ground_time > BOUNCE_TIME) {
				auto p = turn->targets[hit_num];
				static_cast<Battle_Player *>(p)->set_run_offset({0.0f, 0.0f});
				p->get_sprite()->set_animation(p->get_sprite()->get_previous_animation());
				int damage = MAX(1, stats->fixed.attack-p->get_stats()->fixed.defence);
				add_special_number(this, p, damage, true, true);
				hit_num++;
				if (hit_num == turn->targets.size()) {
					turn_started = false;
					next_move = GET_TICKS() + util::rand(min_delay, max_delay);
					return false;
				}
				else {
					hit_ground = false;
					start_attack(turn);
				}
			}
			else {
				Uint32 elapsed = now - hit_ground_time;
				float p = elapsed / (float)BOUNCE_TIME;
				if (p > 1.0f) {
					p = 1.0f;
				}
				const float max_h = 10.0f;
				float h = sin(p*M_PI) * max_h;
				static_cast<Battle_Player *>(turn->targets[hit_num])->set_run_offset({0.0f, -h});
			}
			return true;
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::ITEM) {
			if (GET_TICKS() > eat_start+200 && eat_sound_played == false) {
				eat_sfx->play(false);
				eat_sound_played = true;
			}
			if (eat_done) {
				DOGO_GLOBALS->item_sfx[ITEM_APPLE]->play(false);
				Battle_Enemy *enemy = dynamic_cast<Battle_Enemy *>(turn->targets[0]);
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				int random_num = util::rand(0, 2);
				int amount;
				if (random_num == 0) {
					amount = 50;
				}
				else if (random_num == 1) {
					amount = 100;
				}
				else {
					amount = 150;
				}
				SDL_Colour colour;
				SDL_Colour shadow_colour;
				std::string text;
				get_use_item_info(amount, ITEM_APPLE, colour, shadow_colour, text);
				auto pos = enemy->get_position();
				gfx::Sprite *sprite = enemy->get_sprite();
				util::Point<int> topleft, bottomright;
				sprite->get_bounds(topleft, bottomright);
				pos += topleft;
				pos.x += (bottomright.x-topleft.x) + 5;
				pos.y -= shim::tile_size/4;
				if (inverse_x) {
					pos.x = shim::screen_size.w - pos.x - shim::font->get_text_width(text);
				}
				NEW_SYSTEM_AND_TASK(BATTLE)
				wedge::Special_Number_Step *step = new wedge::Special_Number_Step(colour, shadow_colour, text, pos, wedge::Special_Number_Step::RISE, new_task, false);
				ADD_STEP(step)
				ADD_TASK(new_task)
				FINISH_SYSTEM(BATTLE)
				turn_started = false;
				next_move = GET_TICKS() + util::rand(min_delay, max_delay);
				return false;
			}
			return true;
		}
	}
	return true;
}

void Enemy_Bumper::set_eat_done(bool done)
{
	eat_done = done;
}

bool Enemy_Bumper::take_hit(wedge::Battle_Entity *actor, int damage)
{
	if (stats->hp - damage <= 0) {
		stats->hp = 1000000;
		sprite->set_animation("sit_s", chilly_dead, this);
		BATTLE->drop_entity_turns(this);
		DOGO_GLOBALS->hoohoo->play(false);
	}
	else if (stats->hp < 1000000) {
		return Battle_Enemy::take_hit(actor, damage);
	}

	return false;
}

//--

int Enemy_Kiddie::count = 0;
audio::Sound *Enemy_Kiddie::pop_sfx;

Enemy_Kiddie::Enemy_Kiddie() :
	Battle_Enemy(GLOBALS->game_t->translate(535)/* Originally: Kiddie */, 2500, 5000)
{
	if (count == 0) {
		pop_sfx = new audio::Sample("pop.flac");
	}
	count++;
	
	attack_sounds.clear();
	attack_sounds.push_back(pop_sfx);
	
	draw_shadow = true;
	shadow_size = {20, 10};
	auto_shadow_pos = true;
}

Enemy_Kiddie::~Enemy_Kiddie()
{
	count--;
	if (count == 0) {
		delete pop_sfx;
	}
}

bool Enemy_Kiddie::start()
{
	gold = 150;
	sprite = new gfx::Sprite("kiddie");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 500;
	stats->fixed.attack = 45 * enemy_attack_mul();
	stats->fixed.defence = 35 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	attack_names.push_back(GLOBALS->game_t->translate(536)/* Originally: Bop */);

	return true;
}

wedge::Battle_Game::Turn *Enemy_Kiddie::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;

	t->actor = this;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->turn_name = "attack";
	t->targets.push_back(BATTLE->get_random_player());
	t->started = false;

	return t;
}

bool Enemy_Kiddie::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			start_attack(turn);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn_attack(turn) == false) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}

//--

static void bang_staff_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	Battle_Enemy *e = static_cast<Battle_Enemy *>(data);
	e->get_sprite()->set_animation("blink");
}

static void end_fireball(void *data)
{
	auto s = static_cast<Enemy_BigTuna  *>(data);
	s->end_fireball();
}

void Enemy_BigTuna::end_fireball()
{
	next_move = GET_TICKS() + util::rand(min_delay, max_delay);
	fireball_ended = true;
}

Enemy_BigTuna::Enemy_BigTuna() :
	Battle_Enemy(GLOBALS->game_t->translate(562)/* Originally: Big Tuna */, 2500, 5000)
{
	attack_anims.clear();
	attack_anims.push_back("attack2");
	attack_names.push_back(GLOBALS->game_t->translate(572)/* Originally: Butt */);

	attack_sfx = new audio::MML("sfx/bigtuna_attack.mml");
	attack2_sfx = new audio::MML("sfx/bigtuna_attack2.mml");

	attack_sounds.clear();
	attack_sounds.push_back(attack2_sfx);

	attack_number = 0;
}

Enemy_BigTuna::~Enemy_BigTuna()
{
}

bool Enemy_BigTuna::start()
{
	gold = 1000;
	sprite = new gfx::Sprite("bigtuna");

	stats = new wedge::Base_Stats();

	stats->fixed.max_hp = stats->hp = 4000;
	stats->fixed.attack = 50 * enemy_attack_mul();
	stats->fixed.defence = 35 * enemy_defence_mul();
	stats->fixed.set_extra(LUCK, 35);

	draw_shadow = true;
	shadow_size = {20, 10};
	auto_shadow_pos = true;

	return true;
}

wedge::Battle_Game::Turn *Enemy_BigTuna::get_turn()
{
	if (GET_TICKS() < next_move) {
		return nullptr;
	}

	auto t = new wedge::Battle_Game::Turn;
	t->turn_type = wedge::Battle_Game::ATTACK;
	t->actor = this;

	attack_number++;

	if (attack_number % 5 == 0) {
		t->turn_name = "attack3";
			
		auto players = BATTLE->get_players();

		for (int i = 0; i < 3; i++) {
			if (players[i]->get_stats()->hp > 0) {
				t->targets.push_back(players[i]);
			}
		}
	}
	else {
		if (util::rand(0, 1) == 0) {
			t->turn_name = "attack";
				
			auto players = BATTLE->get_players();

			for (int i = 0; i < 3; i++) {
				if (players[i]->get_stats()->hp > 0) {
					t->targets.push_back(players[i]);
				}
			}
		}
		else {
			t->turn_name = "attack2";
			wedge::Battle_Entity *e = BATTLE->get_random_player();
			t->targets.push_back(e);
		}
	}

	t->started = false;

	return t;
}

bool Enemy_BigTuna::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		if (turn->targets.size() == 0) {
			next_move = GET_TICKS() + util::rand(min_delay, max_delay);
			return false; // do nothing, we were attacking dead player or something and there are no more living players
		}
		turn_started = true;
		if (turn->turn_name == "attack2") {
			start_attack(turn);
		}
		else if (turn->turn_name == "attack3") {
			attack_done = false;
			sprite->set_animation("bang_staff", bang_staff_callback, this);
			auto b = dynamic_cast<Battle_Game *>(BATTLE);
			b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(226)/* Originally: Fireball */);
			fireball_start = GET_TICKS();
			bang_sound_done = false;
			did_fireball = false;
			fireball_ended = false;
		}
		else if (turn->turn_name == "attack") {
			attack_done = false;
			attack_sfx->play(false);
			sprite->set_animation("attack", attack_callback, this);
			auto b = dynamic_cast<Battle_Game *>(BATTLE);
			b->add_notification(Battle_Game::LEFT, GLOBALS->game_t->translate(571)/* Originally: Sweep */);
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			start_dialogue(turn);
		}
	}
	else {
		if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn->turn_name == "attack3") {
				if (bang_sound_done == false && GET_TICKS() > fireball_start+1000) {
					bang_sound_done = true;
					DOGO_GLOBALS->thud->play(false);
				}
				else if (GET_TICKS() > fireball_start+15000 && did_fireball == false) {
					did_fireball = true;

					auto players = BATTLE->get_players();
					
					gfx::Image *img = sprite->get_current_image();

					NEW_SYSTEM_AND_TASK(BATTLE)
					if (players[0]->get_stats()->hp > 0) {
						ADD_STEP(new wedge::Play_Animation_Step(sprite, "fireball", new_task))
						ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_idle, this, new_task))
						auto f = new Fire_Step({0, 0}, players[0], shim::palette[7], shim::palette[4], shim::palette[1], new_task, true);
						ADD_STEP(f)
						util::Point<float> start_pos = get_position() + util::Point<float>(42, 40);
						util::Point<float> end_pos = static_cast<Battle_Player *>(players[0])->get_draw_pos() + util::Point<float>(8.0f, 8.0f);
						f->set_start_pos(start_pos);
						f->set_end_pos(end_pos);
						f->set_start_angle((end_pos-start_pos).angle());
						f->set_total_particles();
					}
					if (players[1]->get_stats()->hp > 0) {
						ADD_STEP(new wedge::Play_Animation_Step(sprite, "fireball", new_task))
						ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_idle, this, new_task))
						auto f = new Fire_Step({0, 0}, players[1], shim::palette[7], shim::palette[4], shim::palette[1], new_task, true);
						ADD_STEP(f)
						util::Point<float> start_pos = get_position() + util::Point<float>(42, 40);
						util::Point<float> end_pos = static_cast<Battle_Player *>(players[1])->get_draw_pos() + util::Point<float>(8.0f, 8.0f);
						f->set_start_pos(start_pos);
						f->set_end_pos(end_pos);
						f->set_start_angle((end_pos-start_pos).angle());
						f->set_total_particles();
					}
					if (players[2]->get_stats()->hp > 0) {
						ADD_STEP(new wedge::Play_Animation_Step(sprite, "fireball", new_task))
						ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_idle, this, new_task))
						auto f = new Fire_Step({0, 0}, players[2], shim::palette[7], shim::palette[4], shim::palette[1], new_task, true);
						ADD_STEP(f)
						util::Point<float> start_pos = get_position() + util::Point<float>(42, 40);
						util::Point<float> end_pos = static_cast<Battle_Player *>(players[2])->get_draw_pos() + util::Point<float>(8.0f, 8.0f);
						f->set_start_pos(start_pos);
						f->set_end_pos(end_pos);
						f->set_start_angle((end_pos-start_pos).angle());
						f->set_total_particles();
					}
					ADD_STEP(new wedge::Generic_Immediate_Callback_Step(::end_fireball, this, new_task))
					ADD_TASK(new_task)
					FINISH_SYSTEM(BATTLE)
				}
				else if (fireball_ended) {
					turn_started = false;
					next_move = GET_TICKS() + util::rand(min_delay, max_delay);
					return false;
				}
			}
			else if (turn->turn_name == "attack2") {
				if (turn_attack(turn) == false) {
					return false;
				}
			}
			else {
				if (attack_done) {
					for (auto p : turn->targets) {
						float mult;
						if ((int)attack_mults.size() > attack_num) {
							mult = attack_mults[attack_num];
						}
						else if (attack_mults.size() > 0) {
							mult = attack_mults[0];
						}
						else {
							mult = 1.0f;
						}
						int damage = MAX(1, stats->fixed.attack*mult-p->get_stats()->fixed.defence);
						add_special_number(this, p, damage, true, true);
					}
					turn_started = false;
					next_move = GET_TICKS() + util::rand(min_delay, max_delay);
					return false;
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (turn_dialogue(turn) == false) {
				return false;
			}
		}
	}
	return true;
}
