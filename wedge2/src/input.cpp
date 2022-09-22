#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/battle_game.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/input.h"
#include "wedge2/map_entity.h"
#include "wedge2/pause_task.h"
#include "wedge2/tile_movement.h"
		
using namespace wedge;

namespace wedge {

Map_Entity_Input_Step::Map_Entity_Input_Step(Map_Entity *entity, Task *task) :
	Step(task),
	entity(entity),
	l(false),
	r(false),
	u(false),
	d(false),
	l_t(0),
	r_t(0),
	u_t(0),
	d_t(0),
	stash_l(false),
	stash_r(false),
	stash_u(false),
	stash_d(false),
	following_path(false),
	stop_at_next_tile(false),
	joy_axis0(0.0f),
	joy_axis1(0.0f),
	pause_at_next_tile(false),
	can_cancel_path(true),
	presses_paused(false),
	locked(false),
	mini_pause_at_next_tile(false),
	no_battles_next_step(false)
{
	create_movement_step();
}

Map_Entity_Input_Step::~Map_Entity_Input_Step()
{
}

void Map_Entity_Input_Step::handle_joy(TGUI_Event *event, int x, int y, int old_x, int old_y)
{
	std::vector<TGUI_Event> events;
	TGUI_Event e;

	if (x != old_x) {
		if (old_x != 0) {
			e.type = TGUI_KEY_UP;
			e.keyboard.code = old_x < 0 ? GLOBALS->key_l : GLOBALS->key_r;
			e.keyboard.is_repeat = false;
			e.keyboard.simulated = true;
			events.push_back(e);
		}
		if (x != 0) {
			e.type = TGUI_KEY_DOWN;
			e.keyboard.code = x < 0 ? GLOBALS->key_l : GLOBALS->key_r;
			e.keyboard.is_repeat = false;
			e.keyboard.simulated = true;
			events.push_back(e);
		}

	}
	if (y != old_y) {
		if (old_y != 0) {
			e.type = TGUI_KEY_UP;
			e.keyboard.code = old_y < 0 ? GLOBALS->key_u : GLOBALS->key_d;
			e.keyboard.is_repeat = false;
			e.keyboard.simulated = true;
			events.push_back(e);
		}
		if (y != 0) {
			e.type = TGUI_KEY_DOWN;
			e.keyboard.code = y < 0 ? GLOBALS->key_u : GLOBALS->key_d;
			e.keyboard.is_repeat = false;
			e.keyboard.simulated = true;
			events.push_back(e);
		}
	}

	if (events.size() > 0) {
		event->type = events[0].type;
		event->keyboard.code = events[0].keyboard.code;
		event->keyboard.is_repeat = events[0].keyboard.is_repeat;

		for (size_t i = 1; i < events.size(); i++) {
			shim::push_event(events[i]);
		}
	}
}

static bool dummy(Map_Entity *entity, Direction dir, bool a, bool b)
{
	// maybe save direction and if activate_with (below) returns false, set direction back to original
	entity->set_direction(dir, a, b);
	return AREA->get_current_area()->activate_with(entity);
}

void Map_Entity_Input_Step::manual_handle_event(TGUI_Event *event)
{
	if (party_all_dead() || AREA->get_gameover()) {
		return; // dead from poison
	}

	/*
	if (following_path && can_cancel_path == false) {
		return;
	}
	*/

	Area *area = entity->get_area();
	std::vector<Map_Entity *> players = AREA->get_players();

	Direction direction = DIR_NONE;

	input::convert_focus_to_original(event);

	// NOTE: Don't check for following path or anything like that for joysticks, when they're converted to keys the checks will take place
	if (event->type == TGUI_JOY_AXIS && (event->joystick.axis == 0 || event->joystick.axis == 1) && event->joystick.is_repeat == false) {
		int x, y;
		GLOBALS->get_joy_xy(event, joy_axis0, joy_axis1, &x, &y);
		int old_x = joy_axis0;
		int old_y = joy_axis1;
		if (x != 0 && y != 0) {
			if (old_x != 0) {
				joy_axis0 = x = 0;
				joy_axis1 = y;
			}
			else if (old_y != 0) {
				joy_axis0 = x;
				joy_axis1 = y = 0;
			}
			else {
				x = y = 0;
			}
		}
		else {
			joy_axis0 = x;
			joy_axis1 = y;
		}
		handle_joy(event, x, y, old_x, old_y);
	}
	else if (event->type == TGUI_JOY_DOWN && event->joystick.is_repeat == false) {
		bool repeat = event->joystick.is_repeat;
		if (event->joystick.button == GLOBALS->joy_action) {
			event->type = TGUI_KEY_DOWN;
			event->keyboard.code = /*GLOBALS->key_b1*/GLOBALS->key_action;
		}
		else if (event->joystick.button == TGUI_B_L) {
			event->type = TGUI_KEY_DOWN;
			event->keyboard.code = GLOBALS->key_l;
		}
		else if (event->joystick.button == TGUI_B_R) {
			event->type = TGUI_KEY_DOWN;
			event->keyboard.code = GLOBALS->key_r;
		}
		else if (event->joystick.button == TGUI_B_U) {
			event->type = TGUI_KEY_DOWN;
			event->keyboard.code = GLOBALS->key_u;
		}
		else if (event->joystick.button == TGUI_B_D) {
			event->type = TGUI_KEY_DOWN;
			event->keyboard.code = GLOBALS->key_d;
		}
		if (event->type != TGUI_JOY_DOWN) {
			event->keyboard.is_repeat = repeat;
			event->keyboard.simulated = true;
		}
	}
	else if (event->type == TGUI_JOY_UP) {
		if (event->joystick.button == TGUI_B_L) {
			event->type = TGUI_KEY_UP;
			event->keyboard.code = GLOBALS->key_l;
		}
		else if (event->joystick.button == TGUI_B_R) {
			event->type = TGUI_KEY_UP;
			event->keyboard.code = GLOBALS->key_r;
		}
		else if (event->joystick.button == TGUI_B_U) {
			event->type = TGUI_KEY_UP;
			event->keyboard.code = GLOBALS->key_u;
		}
		else if (event->joystick.button == TGUI_B_D) {
			event->type = TGUI_KEY_UP;
			event->keyboard.code = GLOBALS->key_d;
			event->keyboard.simulated = true;
		}
	}

	if (event->type == TGUI_KEY_DOWN && presses_paused == false && event->keyboard.is_repeat == false && !(following_path && can_cancel_path == false)) {
		if (following_path) {
			end_movement(true);
		}
		else if (following_path == false) {
			Direction dir = DIR_NONE;;
			if (event->keyboard.code == GLOBALS->key_l) {
				dir = DIR_W;
				l = true;
				l_t = GET_TICKS();
				stash_l = true;
			}
			else if (event->keyboard.code == GLOBALS->key_r) {
				dir = DIR_E;
				r = true;
				r_t = GET_TICKS();
				stash_r = true;
			}
			else if (event->keyboard.code == GLOBALS->key_u) {
				dir = DIR_N;
				u = true;
				u_t = GET_TICKS();
				stash_u = true;
			}
			else if (event->keyboard.code == GLOBALS->key_d) {
				dir = DIR_S;
				d = true;
				d_t = GET_TICKS();
				stash_d = true;
			}
			if (movement_step->is_moving() == false) {
				if (dir != DIR_NONE) {
					direction = dir;
					entity->set_direction(dir, true, false);
				}
				else if (event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) {
					area->activate_with(entity);
				}
			}
		}
	}
	else if (event->type == TGUI_KEY_DOWN && event->keyboard.is_repeat == false) { // no presses_paused check here!
		if (event->keyboard.code == GLOBALS->key_l) {
			stash_l = true;
		}
		else if (event->keyboard.code == GLOBALS->key_r) {
			stash_r = true;
		}
		else if (event->keyboard.code == GLOBALS->key_u) {
			stash_u = true;
		}
		else if (event->keyboard.code == GLOBALS->key_d) {
			stash_d = true;
		}
	}
	else if (event->type == TGUI_KEY_UP && event->keyboard.is_repeat == false && following_path == false) {
		if (event->keyboard.code == GLOBALS->key_l) {
			l = false;
			stash_l = false;
		}
		else if (event->keyboard.code == GLOBALS->key_r) {
			r = false;
			stash_r = false;
		}
		else if (event->keyboard.code == GLOBALS->key_u) {
			u = false;
			stash_u = false;
		}
		else if (event->keyboard.code == GLOBALS->key_d) {
			d = false;
			stash_d = false;
		}
		if (presses_paused == false && movement_step->is_moving() == false) {
			direction = latest();
		}
	}
	else if (event->type == TGUI_KEY_UP && event->keyboard.is_repeat == false && following_path == true) {
		if (event->keyboard.code == GLOBALS->key_l) {
			stash_l = false;
		}
		else if (event->keyboard.code == GLOBALS->key_r) {
			stash_r = false;
		}
		else if (event->keyboard.code == GLOBALS->key_u) {
			stash_u = false;
		}
		else if (event->keyboard.code == GLOBALS->key_d) {
			stash_d = false;
		}
	}
	else if (GLOBALS->onscreen_controller_was_enabled == false && event->type == TGUI_MOUSE_DOWN && presses_paused == false && !(following_path && can_cancel_path == false) && event->mouse.x >= 0.0f && event->mouse.y >= 0.0f && event->mouse.x < shim::screen_size.w && event->mouse.y < shim::screen_size.h) {
		util::Point<int> entity_pos = entity->get_position();
		util::Point<float> entity_offset = entity->get_offset();
		util::Size<int> entity_size = entity->get_size();
		util::Point<float> sz(entity_size.w / 2.0f, 1.0f - entity_size.h / 2.0f);
		add_tiles(entity_pos, entity_offset, sz);
		util::Point<float> map_offset = area->get_centred_offset(entity_pos, entity_offset, true);
		float x = event->mouse.x - map_offset.x;
		float y = event->mouse.y - map_offset.y;
		x /= shim::tile_size;
		y /= shim::tile_size;
		gfx::Tilemap *tilemap = area->get_tilemap();
		util::Size<int> tilemap_size = tilemap->get_size();
		if ((event->mouse.x >= shim::tile_size || event->mouse.y >= shim::tile_size) && x >= 0.0f && y >= 0.0f && x < tilemap_size.w*shim::tile_size && y < tilemap_size.h*shim::tile_size) {
			util::Point<int> entity_pos = entity->get_position(); // must get it again, changed above with add_tiles
			util::Point<float> clicked(x, y);
			Map_Entity *entity_on_tile;
			util::Point<int> diff = clicked; // must be cast to int first
			diff -= entity_pos;
			std::vector<Map_Entity *>::iterator it = players.begin();
			it++;
			Direction click_direction = direction_from_offset(diff);
			std::vector<util::A_Star::Way_Point> waypoints = area->get_way_points(clicked);
			bool has_wp = false;
			for (auto wp : waypoints) {
				for (auto pt : wp.to) {
					if (pt == clicked) {
						has_wp = true;
						break;
					}
				}
				for (auto pair : wp.click_swaps) {
					if (pair.second == clicked) {
						has_wp = true;
						break;
					}
				}
				if (has_wp) {
					break;
				}
			}
			if (movement_step->is_moving() == false && ((diff.x != 0 && diff.y == 0) || (diff.x == 0 && diff.y != 0)) && ((entity_on_tile = area->entity_on_tile(clicked, players)) != NULL || (all_solid(tilemap, entity_pos, clicked) && has_wp == false)) && (std::find(players.begin(), players.end(), entity) != players.end() && (INSTANCE->party_following_player == false || entity_on_tile != NULL || area->entity_on_tile(clicked, std::vector<Map_Entity *>(1, entity_on_tile)) == NULL)) && dummy(entity, click_direction, true, false)) {
				util::Point<int> tile_pos = diff;
				// normalise
				if (tile_pos.x != 0) {
					tile_pos.x /= abs(tile_pos.x);
				}
				if (tile_pos.y != 0) {
					tile_pos.y /= abs(tile_pos.y);
				}
				tile_pos += entity_pos;
				area->try_tile(entity, tile_pos);
			}
			else {
				bool was_following_path = following_path;
				if (was_following_path) {
					std::vector<Map_Entity *> ignore;
					for (size_t i = 0; i < INSTANCE->stats.size(); i++) {
						ignore.push_back(AREA->get_player((int)i));
					}
					util::A_Star *a_star = new util::A_Star(tilemap, area->get_entity_solids(ignore));
					path = a_star->find_path(entity_pos, clicked);
					delete a_star;
				}
				else {
					set_path(clicked, true, true);
				}
			}
		}
		else {
			path.clear();
		}
	}

	if (direction != DIR_NONE) {
		start_moving(direction);
	}

	// convert back to original
	input::Focus_Event *focus;
	if ((focus = dynamic_cast<input::Focus_Event *>(event)) != NULL) {
		input::convert_to_focus_event(event, focus);
	}
}

void Map_Entity_Input_Step::done_signal(Step *step)
{
	Area *area = entity->get_area();
	bool done = false;
	bool zero_party_offset = true;
	bool skip_battles = no_battles_next_step || INSTANCE->party_following_player == false;
	no_battles_next_step = false;

	if (movement_step->hit_wall()) {
		end_movement(false, false);
		return;
	}

	auto hooks = area->get_hooks();
	if (hooks && hooks->get_done_started_and_moved() == false) {
		// No need for ::pause_presses here, wouldn't work properly anyway
		presses_paused = false;
		if (area->started_and_moved()) {
			// same as for on_tile
			done = true;
			stash_l = l;
			stash_r = r;
			stash_u = u;
			stash_d = d;
			if (area->get_next_area_name() != "") { // break early if changing areas
				return;
			}
			if (pause_at_next_tile) {
				pause_at_next_tile = false;
				mini_pause_at_next_tile = true;
			}
		}
		else {
			following_path = false;
			path.clear();
			send_done_signal();

			if (stash_l) {
				l = true;
				l_t = GET_TICKS();
			}
			else if (stash_r) {
				r = true;
				r_t = GET_TICKS();
			}
			if (stash_u) {
				u = true;
				u_t = GET_TICKS();
			}
			else if (stash_d) {
				d = true;
				d_t = GET_TICKS();
			}
		}
		hooks->set_done_started_and_moved(true);
	}

	if (party_all_dead() == false && area->on_tile(entity)) {
		skip_battles = true; // skip battles when on_tile returns true
		done = true;
		stash_l = l;
		stash_r = r;
		stash_u = u;
		stash_d = d;
		if (area->get_next_area_name() != "") { // break early if changing areas
			return;
		}
		if (pause_at_next_tile) {
			pause_at_next_tile = false;
			mini_pause_at_next_tile = true;
		}
	}

	if (entity == AREA->get_player(0)) {
		if (!(INSTANCE->stats.size() > 1 && INSTANCE->party_following_player == false)) {
			INSTANCE->step_count++;
		}
		if (INSTANCE->step_count % 10 == 0) {
			bool poisoned = false;
			for (size_t i = 0; i < MAX_PARTY; i++) {
				if (INSTANCE->stats[i].base.status == STATUS_POISONED) {
					poisoned = true;
					int damage = MAX(1, INSTANCE->stats[i].base.fixed.max_hp * 0.01);
					INSTANCE->stats[i].base.hp = MAX(0, INSTANCE->stats[i].base.hp - damage);
					if (INSTANCE->stats[i].base.hp <= 0) {
						INSTANCE->stats[i].base.status = STATUS_OK;
					}
				}
			}
			if (poisoned) {
				globals->poison->play(false);
				rumble(1.0f, 500);
			}
			if (party_all_dead()) {
				stop_at_next_tile = true;
				audio::stop_music();
				GLOBALS->gameover->play(shim::music_volume, true);
				gfx::add_notification(GLOBALS->game_t->translate(11)/* Originally: You died from poison! */);
				AREA->set_gameover(true);
			}
		}
		bool do_battle = false;
		if (INSTANCE->step_count >= globals->max_battle_steps) {
			do_battle = true;
		}
		else if (INSTANCE->step_count >= globals->min_battle_steps) {
			int diff = globals->max_battle_steps - globals->min_battle_steps;
			if (util::rand(0, diff) == 0) {
				do_battle = true;
			}
		}
		if (skip_battles == false && party_all_dead() == false && do_battle && util::bool_arg(false, shim::argc, shim::argv, "no-battles") == false) {
			Battle_Game *battle_game = area->get_random_battle();
			if (battle_game) {
				battle_game->start_transition_in();
				done = true;
				zero_party_offset = false;
			}
		}
	}

	if (movement_step->is_moving()) { // when entering this function, moving == false, so if it's true now a path was set in on_tile
		stop_at_next_tile = false;
		pause_at_next_tile = false;
		mini_pause_at_next_tile = false;
		return;
	}
	else if (stop_at_next_tile) {
		stop_at_next_tile = false;
		done = true;

		if (party_all_dead() == false) {
			if (pause_at_next_tile) {
				pause_at_next_tile = false;
				AREA->start_menu();
			}
			else if (mini_pause_at_next_tile) {
				mini_pause_at_next_tile = false;
				mini_pause();
			}
		}
	}

	bool was_following_path = false;
	
	if (done == false) {
		was_following_path = path.size() > 0;
		util::Point<int> entity_pos = entity->get_position();
		Direction direction;
		// Weird stuff with waypoints can cause you to reach the goal early
		if (was_following_path && path.back().position == entity_pos) {
			direction = DIR_NONE;
			following_path = false;
			path.clear();
			send_done_signal();
			done = true;
		}
		else if (was_following_path) {
			direction = path_next(entity_pos);
		}
		else {
			direction = latest();
		}
		if (direction != DIR_NONE) {
			util::Point<int> tile_pos = add_direction(entity->get_position(), direction, 1);
			Map_Entity *entity_on_tile = area->entity_on_tile(tile_pos);
			std::vector<Map_Entity *> players = AREA->get_players();
			std::vector<Map_Entity *>::iterator it = players.begin();
			it++;
			if (!((entity_on_tile == NULL || entity_on_tile->is_solid() == false) || (std::find(players.begin(), players.end(), entity) != players.end() && std::find(players.begin(), players.end(), entity_on_tile) != players.end()))) {
				entity->set_direction(direction, true, false);
				if (following_path) {
					done = true;
				}
			}
			else {
				if (entity->get_area()->try_tile(entity, tile_pos)) {
					done = true;
				}
				else {
					movement_step->set_next_direction(direction);
				}
			}
		}
		else if (following_path) {
			done = true;
		}
	}

	if (done) {
		end_movement(false, true, zero_party_offset);
	}

	if (was_following_path) {
		l = r = u = d = false;
	}
}

Direction Map_Entity_Input_Step::path_next(util::Point<int> entity_pos)
{
	if (path.size() == 0) {
		return DIR_NONE;
	}

	util::A_Star::Node front = path.front();
	Direction direction;
	
	if (front.position.x < entity_pos.x) {
		direction = DIR_W;
	}
	else if (front.position.x > entity_pos.x) {
		direction = DIR_E;
	}
	else if (front.position.y < entity_pos.y) {
		direction = DIR_N;
	}
	else if (front.position.y > entity_pos.y) {
		direction = DIR_S;
	}
	else {
		path.clear();
		direction = DIR_NONE;
	}

	if (path.size() != 0) { // could be clear'ed above
		path.pop_front();
	}

	return direction;
}

bool Map_Entity_Input_Step::is_moving()
{
	return movement_step->is_moving();
}

bool Map_Entity_Input_Step::set_path(util::Point<int> goal, bool can_cancel, bool check_solids, bool allow_out_of_bounds)
{
	std::vector<Map_Entity *> players = AREA->get_players();
	bool is_player = false;
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i] == entity) {
			is_player = true;
			break;
		}
	}

	if (is_player && party_all_dead()) {
		return false;
	}

	if (following_path) { // always, not just when can_cancel_path is true
		return false;
	}

	// FIXME: is this OK? (why not?)
	if (movement_step->is_moving()) {
		return false;
	}

	Area *area = entity->get_area();
	gfx::Tilemap *tilemap = area->get_tilemap();
	std::vector<Map_Entity *> ignore;
	if (std::find(players.begin(), players.end(), entity) != players.end()) {
		for (size_t i = 0; i < INSTANCE->stats.size(); i++) {
			ignore.push_back(AREA->get_player((int)i));
		}
	}
	util::Point<float> entity_pos = entity->get_position();
	util::A_Star *a_star = new util::A_Star(tilemap, area->get_entity_solids(ignore));
	path = a_star->find_path(entity_pos, goal, check_solids, check_solids, allow_out_of_bounds);
	delete a_star;

	following_path = path.size() > 0;

	if (following_path) {
		l = r = u = d = false;
		Direction direction = path_next(entity_pos);
		if (direction != DIR_NONE) {
			start_moving(direction);
		}
		else {
			following_path = false;
		}
	}

	can_cancel_path = can_cancel;

	return following_path;
}

void Map_Entity_Input_Step::start_moving(Direction direction)
{
	util::Point<int> entity_pos = entity->get_position();

	Area *area = entity->get_area();
	if (area) {
		util::Point<int> tile_pos = add_direction(entity_pos, direction, 1);
		if (area->try_tile(entity, tile_pos)) {
			end_movement(false);
			return;
		}
	}

	movement_step->set_next_direction(direction);
}

void Map_Entity_Input_Step::end_movement()
{
	end_movement(true);
}

void Map_Entity_Input_Step::end_movement(bool at_next_tile, bool reset_directions, bool zero_party_offset)
{
	if (at_next_tile == true) {
		if (movement_step->is_moving() == false) {
			return;
		}
		stop_at_next_tile = true;
		return;
	}

	stop_at_next_tile = false;

	gfx::Sprite *sprite = entity->get_sprite();
	sprite->stop();
	std::string current_anim = sprite->get_animation();
	if (party_all_dead()) {
		if (locked == false) {
			locked = true;
			std::vector<Map_Entity *> players = AREA->get_players();
			for (size_t i = 0; i < players.size(); i++) {
				Map_Entity_Input_Step *meis = players[i]->get_input_step();
				if (meis != this) {
					meis->end_movement(false);
				}
			}
			locked = false;
		}
		entity->get_sprite()->set_animation("dead");
	}
	else if (current_anim == "walk_n") {
		sprite->set_animation("stand_n");
	}
	else if (current_anim == "walk_e" || current_anim == "jump_e") {
		sprite->set_animation("stand_e");
	}
	else if (current_anim == "walk_s") {
		sprite->set_animation("stand_s");
	}
	else if (current_anim == "walk_w" || current_anim == "jump_w") {
		sprite->set_animation("stand_w");
	}

	bool was_following_path = following_path;
	if (following_path) {
		following_path = false;
		path.clear();
		send_done_signal();
	}

	if (reset_directions) {
		l = r = u = d = false;
		joy_axis_up();
	}

	if (zero_party_offset) {
		// If player stops, everyone stops (this avoids subtle bugs where new Systems are created with A_Star_Step and set_path fails because player NPCs are still moving)
		std::vector<Map_Entity *> players = AREA->get_players();
		if (entity == players[0] && (was_following_path == false || can_cancel_path)) {
			for (size_t i = 1; i < players.size(); i++) {
				if (players[i]->get_input_step()->path.size() == 0) {
					players[i]->get_input_step()->end_movement(false);
					players[i]->set_offset(util::Point<float>(0.0f, 0.0f));
				}
			}
		}
	}
}

bool Map_Entity_Input_Step::is_following_path()
{
	return following_path;
}

void Map_Entity_Input_Step::add_to_path(util::Point<int> position)
{
	if (following_path == false) {
		return;
	}

	util::A_Star::Node node;
	node.position = position;
	path.push_back(node);
}

Tile_Movement_Step *Map_Entity_Input_Step::get_movement_step()
{
	return movement_step;
}

void Map_Entity_Input_Step::repeat_pressed()
{
	TGUI_Event e;
	e.type = TGUI_KEY_DOWN;
	e.keyboard.is_repeat = false;
	e.keyboard.simulated = true;

	if (stash_l) {
		e.keyboard.code = GLOBALS->key_l;
		manual_handle_event(&e);
	}
	if (stash_r) {
		e.keyboard.code = GLOBALS->key_r;
		manual_handle_event(&e);
	}
	if (stash_u) {
		e.keyboard.code = GLOBALS->key_u;
		manual_handle_event(&e);
	}
	if (stash_d) {
		e.keyboard.code = GLOBALS->key_d;
		manual_handle_event(&e);
	}
}

void Map_Entity_Input_Step::reset(System *new_system)
{
	l = false;
	r = false;
	u = false;
	d = false;
	following_path = false;
	path.clear();
	stop_at_next_tile = false;
	pause_at_next_tile = false;
	mini_pause_at_next_tile = false;
	can_cancel_path = true;

	remove_monitors();

	task = new Task(new_system);
	task->get_steps().push_back(this);
	new_system->get_tasks().push_back(task);

	create_movement_step();
}

void Map_Entity_Input_Step::pause_presses(bool pause)
{
	presses_paused = pause;
}

bool Map_Entity_Input_Step::all_solid(gfx::Tilemap *tilemap, util::Point<int> entity_pos, util::Point<int> click)
{
	util::Point<int> diff = click - entity_pos;

	if (!((diff.x != 0 && diff.y == 0) || (diff.x == 0 && diff.y != 0))) {
		return false;
	}

	util::Point<int> inc(util::sign(diff.x), util::sign(diff.y));
	util::Point<int> pos = entity_pos;

	Area *area = entity->get_area();

	while (true) {
		if (tilemap->is_solid(-1, pos) == false && area->entity_on_tile(pos) == NULL) {
			return false;
		}
		if (pos == click) {
			break;
		}
		pos += inc;
	}

	return true;
}

bool Map_Entity_Input_Step::party_all_dead()
{
	bool all_dead = true;
	for (size_t i = 0; i < INSTANCE->stats.size(); i++) {
		if (INSTANCE->stats[i].base.hp > 0) {
			all_dead = false;
			break;
		}
	}
	return all_dead;
}

void Map_Entity_Input_Step::set_stash(bool l, bool r, bool u, bool d)
{
	stash_l = l;
	stash_r = r;
	stash_u = u;
	stash_d = d;
}

bool Map_Entity_Input_Step::are_presses_paused()
{
	return presses_paused;
}

Direction Map_Entity_Input_Step::latest()
{
	Uint32 t = 0;

	if (l) {
		if (r) {
			t = MAX(t, r_t);
		}
		if (u) {
			t = MAX(t, u_t);
		}
		if (d) {
			t = MAX(t, d_t);
		}
		if (l_t >= t) {
			return DIR_W;
		}
	}
	if (r) {
		if (l) {
			t = MAX(t, l_t);
		}
		if (u) {
			t = MAX(t, u_t);
		}
		if (d) {
			t = MAX(t, d_t);
		}
		if (r_t >= t) {
			return DIR_E;
		}
	}
	if (u) {
		if (l) {
			t = MAX(t, l_t);
		}
		if (r) {
			t = MAX(t, r_t);
		}
		if (d) {
			t = MAX(t, d_t);
		}
		if (u_t >= t) {
			return DIR_N;
		}
	}
	if (d) {
		if (l) {
			t = MAX(t, l_t);
		}
		if (r) {
			t = MAX(t, r_t);
		}
		if (u) {
			t = MAX(t, u_t);
		}
		if (d_t >= t) {
			return DIR_S;
		}
	}

	return DIR_NONE;
}

void Map_Entity_Input_Step::create_movement_step()
{
	System *new_system = task->get_system();
	Task *new_task = new Task(new_system);
	movement_step = new Tile_Movement_Step(entity, DIR_NONE, new_task);
	movement_step->add_monitor(this);
	ADD_STEP(movement_step)
	ADD_TASK(new_task)
}

void Map_Entity_Input_Step::joy_axis_up()
{
	if (entity->get_area()->get_next_area_name() != "" || presses_paused) {
		// On area change we don't want to do this (joysticks are held and continue on in the next area)
		return;
	}

	int old_x = joy_axis0;
	int old_y = joy_axis1;
	if (joy_axis0 != 0.0f || joy_axis1 != 0.0f) {
		TGUI_Event e;
		e.type = TGUI_UNKNOWN;
		handle_joy(&e, 0, 0, old_x, old_y);
		if (e.type != TGUI_UNKNOWN) {
			manual_handle_event(&e);
		}
		joy_axis0 = 0.0f;
		joy_axis1 = 0.0f;
	}
}

void Map_Entity_Input_Step::mini_pause()
{
	GLOBALS->mini_pause();
}

void Map_Entity_Input_Step::clear()
{
	l = false;
	r = false;
	u = false;
	d = false;
	l_t = 0;
	r_t = 0;
	u_t = 0;
	d_t = 0;
	stash_l = false;
	stash_r = false;
	stash_u = false;
	stash_d = false;
}

util::Point<int> Map_Entity_Input_Step::get_path_goal()
{
	if (following_path == false || path.size() == 0) {
		return util::Point<int>(-1, -1);
	}

	util::A_Star::Node n = path.back();
	return n.position;
}

void Map_Entity_Input_Step::set_no_battles_next_step(bool no_battles_next_step)
{
	this->no_battles_next_step = no_battles_next_step;
}

}
