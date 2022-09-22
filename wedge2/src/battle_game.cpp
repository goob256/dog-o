#include "wedge2/area_game.h"
#include "wedge2/battle_end.h"
#include "wedge2/battle_game.h"
#include "wedge2/battle_enemy.h"
#include "wedge2/battle_entity.h"
#include "wedge2/battle_player.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/omnipresent.h"

using namespace wedge;

namespace wedge {

Battle_Game::Battle_Game(std::string bg, int bg_delay) :
	bg_delay(bg_delay),
	gold(0),
	experience(0),
	boss_battle(false),
	done(false),
	gameover(false),
	ran_dialogs(0),
	did_dialogs(false),
	started_dialogs(0),
	boss_music_name("music/boss.mml"),
	can_die(true),
	attack(0),
	mp(0),
	started_up(false)
{
	for (int i = 1; i <= 1000; i++) {
		gfx::Image *img;
		try {
			img = new gfx::Image("battle_bgs/" + bg + util::itos(i) + ".tga");
		}
		catch (util::Error &e) {
			break;
		}
		backgrounds.push_back(img);
	}

	startup_time = GET_TICKS() + 2250;

	won_object.type = OBJECT_NONE;
}

Battle_Game::~Battle_Game()
{
	for (auto t : active_turns) {
		delete t;
	}
	for (auto t : turns) {
		delete t;
	}

	for (size_t i = 0; i < entities.size(); i++) {
		delete entities[i];
	}

	for (size_t i = 0; i < backgrounds.size(); i++) {
		delete backgrounds[i];
	}

	if (music_backup != nullptr) {
		if (AREA == NULL) { // was a gameover
			delete music_backup;
		}
		else {
			delete shim::music;
			shim::music = music_backup;
			shim::music->play(shim::music_volume, true);
		}
	}
}

bool Battle_Game::start()
{
	INSTANCE->step_count = 0; // reset step count

	std::string music_name;
	if (boss_battle) {
		music_name = boss_music_name;
	}
	else {
		music_name = "music/battle.mml";
	}
	if (shim::music->get_name() == music_name) {
		music_backup = nullptr;
	}
	else {
		music_backup = shim::music;
		music_backup->pause();
		shim::music = new audio::MML(music_name);
		shim::music->play(shim::music_volume, true);
	}

	for (size_t i = 0; i < MAX_PARTY; i++) {
		Battle_Player *player = create_player((int)i);
		player->start();
		entities.push_back(player);
	}
	
	NEW_SYSTEM_AND_TASK(this)
	battle_end_step = new Battle_End_Step(new_task);
	ADD_STEP(battle_end_step)
	ADD_TASK(new_task)
	FINISH_SYSTEM(this)
	
	return true;
}

void Battle_Game::handle_event(TGUI_Event *event)
{
	Game::handle_event(event);
}

bool Battle_Game::run()
{
	if (done || GET_TICKS() < startup_time) {
		if (done) {
			std::vector<Battle_Entity *> players = get_players();
			for (size_t i = 0; i < players.size(); i++) {
				players[i]->run();
			}

			int ndialogs = 0;
			if (gold != 0) {
				ndialogs++;
			}
			if (won_object.type != OBJECT_NONE) {
				ndialogs++;
			}
			if (attack != 0 || mp != 0) {
				ndialogs++;
			}
			if (ran_dialogs >= ndialogs && did_dialogs == false) {
				did_dialogs = true;
				start_transition_out();
			}
			else if (ran_dialogs < ndialogs) {
				if (gold != 0 && started_dialogs == 0 && started_dialogs == ran_dialogs) {
					started_dialogs++;
					if (gold < 0) {
						std::string text = util::string_printf(GLOBALS->game_t->translate(176)/* Originally: Dropped %d gold! */.c_str(), -gold);
						GLOBALS->do_dialogue("", text, DIALOGUE_MESSAGE, DIALOGUE_BOTTOM, battle_end_step);
					}
					else {
						std::string text = util::string_printf(GLOBALS->game_t->translate(177)/* Originally: Received %d gold! */.c_str(), gold);
						GLOBALS->do_dialogue("", text, DIALOGUE_MESSAGE, DIALOGUE_BOTTOM, battle_end_step);
					}
					INSTANCE->add_gold(gold);
				}
				else if ((attack != 0 || mp != 0) && started_dialogs == ran_dialogs && started_dialogs == ndialogs-1) {
					started_dialogs++;
					std::string text;
					// Stats get copied from battle to INSTANCE
					auto v = get_players();
					if (mp != 0 && attack != 0) {
						text = util::string_printf(GLOBALS->game_t->translate(279)/* Originally: LEVEL UP! Gained %d ATK and %d Max SP! */.c_str(), attack, mp);
						for (auto p : v) {
							auto s = p->get_stats();
							s->fixed.attack += attack;
							s->fixed.max_mp += mp;
						}
					}
					else if (mp != 0) {
						text = util::string_printf(GLOBALS->game_t->translate(280)/* Originally: LEVEL UP! Gained %d Max SP! */.c_str(), mp);
						for (auto p : v) {
							auto s = p->get_stats();
							s->fixed.max_mp += mp;
						}
					}
					else {
						text = util::string_printf(GLOBALS->game_t->translate(276)/* Originally: LEVEL UP! Gained %d ATK! */.c_str(), attack);
						for (auto p : v) {
							auto s = p->get_stats();
							s->fixed.attack += attack;
						}
					}
					GLOBALS->do_dialogue("", text, DIALOGUE_MESSAGE, DIALOGUE_BOTTOM, battle_end_step);
					GLOBALS->levelup->play(false);
				}
				else if ((ran_dialogs > 0 || gold == 0) && started_dialogs == ran_dialogs) {
					started_dialogs++;
					if (won_object.type != OBJECT_NONE) {
						std::string message;
						int can_add = INSTANCE->inventory.add(won_object);
						if (can_add != won_object.quantity) {
							if (can_add > 0) {
								won_object.quantity = can_add;
								INSTANCE->inventory.add(won_object);
							}
							message = GLOBALS->game_t->translate(178)/* Originally: Inventory full! */;
						}
						else {
							if (won_object.quantity > 1) {
								if (GLOBALS->language == "Spanish") {
									message = util::string_printf(GLOBALS->game_t->translate(5)/* Originally: Received %d %s! */.c_str(), won_object.name.c_str(), won_object.quantity);
								}
								else {
									message = util::string_printf(GLOBALS->game_t->translate(5)/* Originally: Received %d %s! */.c_str(), won_object.quantity, won_object.name.c_str());
								}
							}
							else {
								message = util::string_printf(GLOBALS->game_t->translate(6)/* Originally: Received %s! */.c_str(), won_object.name.c_str());
							}
						}
						globals->do_dialogue("", message, DIALOGUE_MESSAGE, DIALOGUE_BOTTOM, battle_end_step);
					}
				}
			}
		}
		Game::run();
		return true;
	}

	if (started_up == false) {
		started_up = true;
		startup();
	}

	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->run();
	}

	std::vector<Battle_Entity *> players = get_players();
	bool all_players_dead = true;
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i]->is_dead() == false) {
			all_players_dead = false;
			break;
		}
	}

	if (all_players_dead) {
		if (can_die == false) {
			won_object.type = OBJECT_NONE;
			set_done(true);
		}
		else {
			if (gameover == false) {
				OMNIPRESENT->start_fade(GLOBALS->gameover_fade_colour, GLOBALS->gameover_timeout-GLOBALS->gameover_fade_time, GLOBALS->gameover_fade_time);
				game_end_time = GET_TICKS() + GLOBALS->gameover_timeout;
				gameover = true;
				if (music_backup == nullptr) {
					music_backup = shim::music;
				}
				else {
					delete shim::music;
				}
				shim::music = new audio::MML("music/gameover.mml");
				shim::music->play(shim::music_volume, true);
				gfx::add_notification(GLOBALS->game_t->translate(12)/* Originally: You died in battle! */);
			}
			else if (GET_TICKS() >= game_end_time) {
				done = true;
				for (auto t : turns) {
					delete t;
				}
				for (auto t : active_turns) {
					delete t;
				}
				delete AREA;
				AREA = NULL;
				return false;
			}
		}
	}


	bool all_enemies_dead = true;
	bool all_enemies_hp_0 = true;
	auto enemy_it = entities.begin();
	while (enemy_it != entities.end()) {
		Battle_Entity *e = *enemy_it;
		if (e->get_type() == Battle_Entity::ENEMY) {
			if (e->get_stats()->hp > 0) {
				all_enemies_hp_0 = false;
			}
			if (e->is_dead()) {
				enemy_it = entities.erase(enemy_it);
				delete e;
			}
			else {
				all_enemies_dead = false;
				enemy_it++;
			}
		}
		else {
			enemy_it++;
		}
	}

	bool running = false;
	for (auto p : get_players()) {
		if (static_cast<Battle_Player *>(p)->is_running()) {
			running = true;
			break;
		}
	}

	if (!(all_enemies_hp_0 || all_players_dead || running)) {
		// this dialogue check is for dialogue at the very start of battle before any turns
 		if (!((turns.size() > 0 && turns[0]->turn_type == DIALOGUE) || (active_turns.size() > 0 && active_turns[0]->turn_type == DIALOGUE))) {
			for (auto e : entities) {
				if (e->get_stats()->hp <= 0) {
					continue;
				}
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
				int max = e->get_max_turns();
				if (e->is_fast()) {
					max = 5;
				}
				bool cant_go = false;
				if (e->get_sprite()->get_animation() == "attack_defend") {
					cant_go = true;
				}
				if (turn_count < max && cant_go == false) {
					Turn *t = e->get_turn();
					if (t) {
						if (t->targets.size() > 0 && dynamic_cast<Battle_Player *>(t->targets[0])) {
							t->targetting_enemies = false;
						}
						else {
							t->targetting_enemies = true;
						}
						turns.push_back(t);
						Uint32 now = GET_TICKS();
						if ((e->is_fast() && now >= e->get_fast_end()) || (e->is_fast() && e->get_buff() != Battle_Entity::FAST)) {
							e->set_fast(false);
						}
					}
				}
			}
		}

		auto it = turns.begin();
		while (it != turns.end()) {
			Turn *t = *it;
			if (std::find(entities.begin(), entities.end(), t->actor) == entities.end() || t->actor->get_stats()->hp <= 0) {
				it = turns.erase(it);
				delete t;
			}
		  	else if (can_start_turn(t)) {
				active_turns.push_back(t);
				it = turns.erase(it);
				start_turn(t->actor);
			}
			else {
				break;
			}
		}
	}
	else {
		for (auto t : turns) {
			delete t;
		}
		turns.clear();
	}

	auto it = active_turns.begin();
	while (it != active_turns.end()) {
		Turn *t = *it;
		if (t->started == false) {
			if (fixup_targets(t->actor, t->targets, t->targetting_enemies, true)) {
				if (t->turn_type == ITEM && t->targets.size() == 0) {
					int i = atoi(t->turn_name.c_str());
					auto inv = INSTANCE->inventory.get_all();
					inv[i].used--;
					inv[i].used = MAX(0, inv[i].used);
				}

				it = active_turns.erase(it);
				if (t->targets.size() == 0) {
					delete t;
				}
				else {
					turns.push_back(t);
				}
				continue;
			}
			else {
				t->started = true;
			}
		}
		if (t->actor->take_turn(t) == false) {
			if (done) {
				// can get here when running away, which calls set_done which erases all turns
				Game::run();
				return true;
			}
			it = active_turns.erase(it);
			delete t;
		}
		else {
			it++;
		}
	}

	if (add_enemies.size() > 0) {
		all_enemies_dead = false;
		// Skip players and add enemies in correct order (enemies are pushed after players)
		for (auto e : add_enemies) {
			bool added = false;
			for (auto it = entities.begin(); it != entities.end(); it++) {
				auto ent = *it;
				if (dynamic_cast<Battle_Player *>(ent)) {
					continue;
				}
				auto e2 = static_cast<Battle_Enemy *>(ent);
				if (e->get_position().x > e2->get_position().x) {
					entities.insert(it, e);
					added = true;
					break;
				}
			}
			if (added == false) {
				entities.push_back(e);
			}
		}
		add_enemies.clear();
	}

	if (all_enemies_dead && active_turns.size() == 0) {
		done = true;

		gfx::cancel_all_notifications();

		for (auto t : turns) {
			delete t;
		}
		for (auto t : active_turns) {
			delete t;
		}
		auto v = get_players();
		for (auto p : v) {
			if (p->get_stats()->hp > 0) {
				p->get_sprite()->set_animation("victory");
			}
		}
		if (music_backup == nullptr) {
			music_backup = shim::music;
		}
		else {
			delete shim::music;
		}
		shim::music = new audio::MML("music/victory.mml");
		shim::music->play(shim::music_volume, true);
		
		won_object = get_found_object();

		return true;
	}

	Game::run();

	return true; // the transition out handles destruction of the battle game in normal circumstances
}

std::vector<Battle_Entity *> Battle_Game::get_players()
{
	std::vector<Battle_Entity *> v;
	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_type() == Battle_Entity::PLAYER) {
			v.push_back(entities[i]);
		}
	}
	return v;
}

std::vector<Battle_Entity *> Battle_Game::get_enemies()
{
	std::vector<Battle_Entity *> v;
	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_type() == Battle_Entity::ENEMY) {
			Base_Stats *stats = entities[i]->get_stats();
			if (stats->hp > 0) {
				v.push_back(entities[i]);
			}
		}
	}
	return v;
}

std::vector<Battle_Entity *> Battle_Game::get_all_entities()
{
	return entities;
}

bool Battle_Game::is_boss_battle()
{
	return boss_battle;
}

void Battle_Game::add_entity(Battle_Entity *entity)
{
	entities.push_back(entity);
}

void Battle_Game::add_gold(int gold)
{
	gold *= 1.0f + (util::rand(1, 7) / 100.0f);
	this->gold += gold;
}

void Battle_Game::add_experience(int experience)
{
	this->experience += experience;
}

void Battle_Game::draw_fore()
{
	Game::draw_fore();
}

void Battle_Game::start_transition_in()
{
	rumble(1.0f, 1000);
}

void Battle_Game::lost_device()
{
	Game::lost_device();

	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->lost_device();
	}
}

void Battle_Game::found_device()
{
	Game::found_device();

	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->found_device();
	}
}

void Battle_Game::resize(util::Size<int> new_size)
{
	Game::resize(new_size);

	for (size_t i = 0; i < entities.size(); i++) {
		Battle_Entity *entity = entities[i];
		entity->resize(new_size);
	}
}

void Battle_Game::startup()
{
}

void Battle_Game::set_music_backup(audio::MML *music_backup)
{
	delete this->music_backup;
	this->music_backup = music_backup;
}

util::Point<int> Battle_Game::get_offset()
{
	util::Size<int> offset = (shim::screen_size-backgrounds[0]->size)/2;
	return util::Point<int>(offset.w, offset.h);
}
	
bool Battle_Game::can_start_turn(Turn *turn)
{
	std::vector<Battle_Entity *> turn_entities;

	turn_entities.push_back(turn->actor);
	turn_entities.insert(turn_entities.end(), turn->targets.begin(), turn->targets.end());

	std::vector<Battle_Entity *> acting;
	for (auto t : active_turns) {
		acting.push_back(t->actor);
		acting.insert(acting.end(), t->targets.begin(), t->targets.end());
	}

	for (auto e : turn_entities) {
		if (std::find(acting.begin(), acting.end(), e) != acting.end()) {
			return false;
		}
	}

	// Run turns must run solo
	for (auto t : active_turns) {
		if (t->turn_type == RUN) {
			return false;
		}
	}
	if (turn->turn_type == RUN && active_turns.size() > 0) {
		return false;
	}

	return true;
}

bool Battle_Game::fixup_targets(wedge::Battle_Entity *actor, std::vector<Battle_Entity *> &v, bool targetting_enemies, bool add_random)
{
	bool ret = false;

	auto it = v.begin();
	std::vector<Battle_Entity *> add;
	while (it != v.end()) {
		Battle_Entity *e = *it;
		bool dead = std::find(entities.begin(), entities.end(), e) == entities.end();
		bool gone = dead;
		if (dead == false) {
			auto s = e->get_stats();
			if (s->hp <= 0) {
				dead = true;
			}
		}

		bool player = dynamic_cast<Battle_Player *>(actor);
		if (gone || (dead && !(player && dynamic_cast<Battle_Player *>(e)))) {
			if (add_random) {
				std::vector<Battle_Entity *> ents;
				if (targetting_enemies) {
					ents = get_enemies();
				}
				else {
					ents = get_players();
				}
				for (auto e2 : v) {
					auto it2 = std::find(ents.begin(), ents.end(), e2);
					if (it2 != ents.end()) {
						ents.erase(it2);
					}
				}
				if (ents.size() > 0) {
					int r = (int)util::rand(0, int(ents.size()-1));
					for (int i = 0; i < (int)ents.size(); i++) {
						int index = (r+i) % ents.size();
						auto ent = ents[index];
						if (ent->get_stats()->hp < 1000000 && std::find(add.begin(), add.end(), ent) == add.end()) {
							add.push_back(ent);
							break;
						}
					}
				}
			}
			it = v.erase(it);
			ret = true;
		}
		else {
			it++;
		}
	}
			
	v.insert(v.end(), add.begin(), add.end());

	return ret;
}

Battle_Player *Battle_Game::get_random_player()
{
	auto v = get_players();
	std::vector<Battle_Entity *> living;
	for (auto e : v) {
		if (e->get_stats()->hp > 0) {
			living.push_back(e);
		}
	}
	if (living.size() == 0) {
		return nullptr;
	}
	else if (living.size() == 1) {
		return dynamic_cast<Battle_Player *>(living[0]);
	}
	return dynamic_cast<Battle_Player *>(living[util::rand(0, int(living.size()-1))]);
}

Battle_Enemy *Battle_Game::get_random_enemy()
{
	auto v = get_enemies();
	std::vector<Battle_Entity *> living;
	for (auto e : v) {
		if (e->get_stats()->hp > 0) {
			living.push_back(e);
		}
	}
	if (living.size() == 0) {
		return nullptr;
	}
	return dynamic_cast<Battle_Enemy *>(living[util::rand(0, int(living.size()-1))]);
}

void Battle_Game::set_done(bool done)
{
	this->done = done;
	if (done) {
		if (boss_battle == false) {
			// This happens when running away
			gold = util::rand(INSTANCE->get_gold() * 0.05f, INSTANCE->get_gold() * 0.075f);
			gold = -gold; // util::rand is unsigned
			won_object.type = OBJECT_NONE;
		}
		else {
			won_object = get_found_object();
		}
		for (auto t : active_turns) {
			delete t;
		}
		active_turns.clear();
		for (auto t : turns) {
			delete t;
		}
		turns.clear();
	}
}

void Battle_Game::battle_end_signal()
{
	ran_dialogs++;
}

Object Battle_Game::get_found_object()
{
	Object o;
	o.type = OBJECT_NONE;
	return o;
}

void Battle_Game::do_dialogue(Battle_Entity *actor, std::string tag, std::string text, Dialogue_Type type, Dialogue_Position position, Step *monitor)
{
	Turn *t = new Turn;
	t->actor = actor;
	t->turn_type = DIALOGUE;
	t->turn_name = "";
	t->targets = entities;
	t->started = false;
	t->dialogue.tag = tag;
	t->dialogue.text = text;
	t->dialogue.type = type;
	t->dialogue.position = position;
	t->dialogue.monitor = monitor;
	turns.push_back(t);
}

void Battle_Game::drop_inactive_turns()
{
	for (auto t : turns) {
		delete t;
	}
	turns.clear();
}

void Battle_Game::drop_entity_turns(wedge::Battle_Entity *entity)
{
	for (auto it = turns.begin(); it != turns.end();) {
		auto t = *it;
		if (t->actor == entity) {
			delete t;
			it = turns.erase(it);
		}
		else {
			it++;
		}
	}
}

void Battle_Game::add_enemy(Battle_Enemy *enemy)
{
	add_enemies.push_back(enemy);
}

bool Battle_Game::is_done()
{
	return done;
}

}
