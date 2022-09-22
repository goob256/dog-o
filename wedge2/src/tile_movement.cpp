#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/input.h"
#include "wedge2/map_entity.h"
#include "wedge2/systems.h"
#include "wedge2/tile_movement.h"

using namespace wedge;

namespace wedge {

Tile_Movement_Step::Tile_Movement_Step(Map_Entity *entity, Direction direction, Task *task) :
	Step(task),
	entity(entity),
	moving(false),
	next_direction(DIR_NONE),
	waiting_for_next_direction(false),
	_hit_wall(false),
	movement_delay(0),
	do_try_tile(false),
	do_next_path(false)
{
	go(direction);
}

Tile_Movement_Step::~Tile_Movement_Step()
{
}

void Tile_Movement_Step::go(Direction direction)
{
	// Special case for slopes
	auto e_off = entity->get_offset();
	if (e_off.x != 0.0f && e_off.y != 0.0f) {
		return;
	}

	moving = true;

	start_tile = entity->get_position();

	Area *area = entity->get_area();

	gfx::Tilemap *tilemap = area->get_tilemap();

	util::Point<int> inc;
	util::Point<float> new_offset;
	util::Point<float> new_increment;

	switch (direction) {
		case DIR_N:
			inc = util::Point<int>(0, -1);
			new_offset = util::Point<float>(0.0f, 1.0f);
			new_increment = util::Point<float>(0.0f, -1.0f);
			break;
		case DIR_E:
			inc = util::Point<int>(1, 0);
			new_offset = util::Point<float>(-1.0f, 0.0f);
			new_increment = util::Point<float>(1.0f, 0.0f);
			break;
		case DIR_S:
			inc = util::Point<int>(0, 1);
			new_offset = util::Point<float>(0.0f, -1.0f);
			new_increment = util::Point<float>(0.0f, 1.0f);
			break;
		case DIR_W:
			inc = util::Point<int>(-1, 0);
			new_offset = util::Point<float>(1.0f, 0.0f);
			new_increment = util::Point<float>(-1.0f, 0.0f);
			break;
		default:
			moving = false;
			break;
	}

	if (moving == false) {
		gfx::Sprite *sprite = entity->get_sprite();
		std::string anim = sprite ? sprite->get_animation() : "";
		if (sprite && (anim.substr(0, 4) == "walk" || anim.substr(0, 5) == "stand")) {
			entity->set_direction(entity->get_direction(), true, false);
		}
		entity->set_moving(false);
	}
	else {
		bool entity_is_solid = entity->is_solid();
		std::vector<Map_Entity *> players = AREA->get_players();
		std::vector<Map_Entity *>::iterator it = players.begin();
		it++;
		bool is_party_member = std::find(it, players.end(), entity) != players.end();
		bool in_bounds = tilemap->get_size().is_in_bounds(start_tile + inc);
		Map_Entity *entity_on_tile = area->entity_on_tile(start_tile + inc);
		bool both_party_members = entity_on_tile == NULL ? false : std::find(players.begin(), players.end(), entity) != players.end() && std::find(players.begin(), players.end(), entity_on_tile) != players.end();
		if ((entity_is_solid && tilemap->is_solid(-1, start_tile + inc)) || (is_party_member && in_bounds == false) || (both_party_members == false && (entity_on_tile != NULL && entity_on_tile->is_solid()))) {
			_hit_wall = true;
			// Can't set moving = false here or else go() can be called before run checks hit_wall below
			//moving = false;
			entity->set_direction(direction, true, false);
			entity->set_moving(false);
		}
		else {
			entity->set_position(start_tile + inc);
			entity->set_offset(new_offset);
			increment = new_increment;

			area->start_water_ripple(start_tile);

			gfx::Sprite *sprite = entity->get_sprite();
			bool set_dir;
			if (sprite && sprite->get_animation() == std::string("walk_") + direction_to_string(direction)) {
				set_dir = false;
			}
			else {
				set_dir = true;
			}
			if (set_dir) {
				entity->set_direction(direction, true, true);
			}
			if (entity->is_moving() == false) {
				entity->set_moving(true);
			}

			if (INSTANCE->party_following_player) {
				std::vector<Map_Entity *> players = AREA->get_players();
				int index = -1;
				for (size_t i = 0; i < players.size()-1; i++) {
					if (players[i] == entity) {
						index = (int)i;
						break;
					}
				}
				if (index >= 0) {
					Map_Entity *follower = players[index+1];
					if (follower && follower->get_position() != start_tile) {
						// Slopes need a special case
						bool do_try = false;
						util::Point<int> to_try;
						auto follower_pos = follower->get_position();
						// If we're on a slope
						if (follower_pos.x != start_tile.x && follower_pos.y != start_tile.y) {
							to_try.x = start_tile.x;
							to_try.y = follower_pos.y;
							do_try = true;
						}
						Map_Entity_Input_Step *meis = follower->get_input_step();
						if (do_try) {
							if (meis->get_movement_step()->is_moving()) {
								meis->get_movement_step()->set_try_tile(to_try);
								//meis->get_movement_step()->delay_movement(index+1);
							}
							else {
								auto hooks = area->get_hooks();
								if (hooks) {
									hooks->try_tile(follower, to_try);
								}
							}
						}
						else {
							if (meis) {
								if (meis->get_movement_step()->is_moving() && meis->get_movement_step()->increment.x != 0.0f && meis->get_movement_step()->increment.y != 0.0f) {
									meis->get_movement_step()->set_next_path(start_tile);
								}
								else if (meis->is_following_path()) {
									meis->add_to_path(start_tile);
								}
								else {
									auto offset = follower->get_offset();
									if (offset.x != 0.0f || offset.y != 0.0f) {
										// Was going on slope, need to zero offset
										follower->set_offset({0.0f, 0.0f});
									}
									meis->set_path(start_tile);
									// If the lead player (the one 'follower' is following) is pathfinding and thus their
									// run() winds up in the done_signal (below) which keeps them moving along the path,
									// it will wind up here setting a path for their follower. Since players' are run()
									// first to last, that means the follower will then immediately run() after the player
									// giving them 1 extra run which means their movement will finish before the player,
									// resulting in them stopping which creates a jittery walk cycle. The line below
									// delays the follower by index+1 frames (for the first follower this would be 1, the
									// second 2, ...) which re-syncs movement between everyone.
									meis->get_movement_step()->delay_movement(index+1);
								}
							}
						}
					}
				}
			}
		}
	}
}

bool Tile_Movement_Step::run()
{
	if (movement_delay > 0) {
		movement_delay--;
		return true;
	}

	if (AREA->get_gameover() && entity == AREA->get_player(0)) {
		if (entity->get_do_tilt()) {
			entity->set_angle(0.0f);
		}
		return true;
	}

	if (_hit_wall) {
		moving = false;
		send_done_signal();
		_hit_wall = false;
		if (entity->get_do_tilt()) {
			entity->set_angle(0.0f);
		}
		return true;
	}

	if (moving == false) {
		return true;
	}

	util::Point<float> offset = entity->get_offset();

	float speed = entity->get_speed();
	util::Point<float> inc(speed * increment.x, speed * increment.y);
	offset += inc; // use inc here

	// use increment everywhere else
	if (increment.x < 0.0f) {
		if (offset.x <= 0.0f) {
			moving = false;
		}
	}
	else if (increment.x > 0.0f) {
		if (offset.x >= 0.0f) {
			moving = false;
		}
	}

	if (increment.y < 0.0f) {
		if (offset.y <= 0.0f) {
			moving = false;
		}
	}
	else if (increment.y > 0.0f) {
		if (offset.y >= 0.0f) {
			moving = false;
		}
	}

	if (moving == false) {
		offset = util::Point<float>(0.0f, 0.0f);
		if (entity->get_do_tilt()) {
			entity->set_angle(0.0f);
		}
	}
	else {
		if (entity->get_do_tilt()) {
			gfx::Sprite *sprite = entity->get_sprite();
			if (sprite) {
				std::vector<Uint32> v = sprite->get_frame_times();
				Uint32 total = 0;
				for (size_t i = 0; i < v.size(); i++) {
					total += v[i];
				}
				Uint32 elapsed = sprite->get_elapsed();
				Uint32 t = elapsed % total;
				float p = t / (float)total;
				float dest_angle;
				int tilt_degrees = entity->get_tilt_degrees();
				if (p < 0.5f) {
					p /= 0.5f;
					dest_angle = tilt_degrees * M_PI / 360.0f;
				}
				else {
					p -= 0.5f;
					p /= 0.5f;
					dest_angle = -tilt_degrees * M_PI / 360.0f;
				}
				float a;
				if (p < 0.5f) {
					p /= 0.5f;
					a = p * dest_angle;
				}
				else {
					p -= 0.5f;
					p /= 0.5f;
					a = (1.0f - p) * dest_angle;
				}
				Direction dir = entity->get_direction();
				if (dir == wedge::DIR_E || dir == wedge::DIR_N) {
					a = -a;
				}
				entity->set_angle(a);
			}
		}
	}
	
	entity->set_offset(offset);

	if (moving == false) {
		waiting_for_next_direction = true;
		send_done_signal();
		waiting_for_next_direction = false;
		if (do_next_path) {
			do_next_path = false;
			entity->get_input_step()->set_path(next_path);
		}
		else if (do_try_tile) {
			do_try_tile = false;
			Area *area = entity->get_area();
			auto hooks = area->get_hooks();
			if (hooks) {
				hooks->try_tile(entity, tile_to_try);
				//return true;
			}
		}
		else if (next_direction != DIR_NONE) {
			go(next_direction);
			next_direction = DIR_NONE;
		}
		else {
			Map_Entity_Input_Step *meis = entity->get_input_step();
			if (meis != NULL && meis->party_all_dead() == false && meis->is_following_path() == false) { // set_path was not called during send_done_signal above?
				entity->set_direction(entity->get_direction(), true, false);
				entity->set_moving(false);
			}

			moving = false;
		}
	}

	return true;
}

void Tile_Movement_Step::set_next_direction(Direction direction)
{
	if (moving) {
		return;
	}

	if (waiting_for_next_direction) {
		next_direction = direction;
		moving = true;
	}
	else {
		go(direction);
	}
}

bool Tile_Movement_Step::is_moving()
{
	return moving == true;
}

void Tile_Movement_Step::set_moving(bool moving)
{
	this->moving = moving;
}

void Tile_Movement_Step::set_increment(util::Point<float> increment)
{
	this->increment = increment;
}

bool Tile_Movement_Step::hit_wall()
{
	return _hit_wall;
}

void Tile_Movement_Step::delay_movement(int frames)
{
	movement_delay += frames;
}

bool Tile_Movement_Step::is_waiting_for_next_direction()
{
	return waiting_for_next_direction;
}

void Tile_Movement_Step::set_try_tile(util::Point<int> tile)
{
	tile_to_try = tile;
	do_try_tile = true;
}

void Tile_Movement_Step::set_next_path(util::Point<int> tile)
{
	next_path = tile;
	do_next_path = true;
}

}
