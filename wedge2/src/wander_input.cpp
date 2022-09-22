#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/input.h"
#include "wedge2/map_entity.h"
#include "wedge2/wander_input.h"

using namespace wedge;

namespace wedge {

Wander_Input_Step::Wander_Input_Step(Map_Entity *entity, bool can_wander_l, bool can_wander_r, bool can_wander_u, bool can_wander_d, util::Point<int> home_position, int max_dist_from_home, Task *task) :
	Map_Entity_Input_Step(entity, task),
	last_move_direction(-1),
	ticks(0),
	home_position(home_position),
	max_dist_from_home(max_dist_from_home),
	waiting_for_movement_end(false)
{
	can_wander[0] = can_wander_l;
	can_wander[1] = can_wander_r;
	can_wander[2] = can_wander_u;
	can_wander[3] = can_wander_d;

	num_directions = 0;

	for (int i = 0; i < 4; i++) {
		if (can_wander[i] == true) {
			num_directions++;
		}
	}

	ticks = util::rand(0, DELAY/(1000.0f/shim::logic_rate));
}

Wander_Input_Step::~Wander_Input_Step()
{
}

bool Wander_Input_Step::run()
{
	if (waiting_for_movement_end == false) {
		ticks++;
		int elapsed = ticks * (1000.0f/shim::logic_rate);
		if (elapsed >= DELAY) {
			util::Point<int> position = entity->get_position();
			util::Point<int> diff = position - home_position;
			util::Point<int> movement_offset;
			if (diff.x <= -max_dist_from_home) {
				last_move_direction = 1;
			}
			else if (diff.x >= max_dist_from_home) {
				last_move_direction = 0;
			}
			else if (diff.y <= -max_dist_from_home) {
				last_move_direction = 3;
			}
			else if (diff.y >= max_dist_from_home) {
				last_move_direction = 2;
			}
			else {
				if (last_move_direction == -1) {
					int dir = util::rand(0, num_directions-1);
					int found = 0;
					for (int i = 0; i < 4; i++) {
						if (can_wander[i]) {
							if (found == dir) {
								last_move_direction = i;
								break;
							}
							found++;
						}
					}
				}
				else {
					float prob_same = 1.0f / num_directions * 1.5f;
					float prob_other = (1.0f - prob_same) / (num_directions - 1);
					for (int i = 0; i < 4; i++) {
						if (i != last_move_direction && can_wander[i]) {
							if (util::rand(0, 1000) <= prob_other*1000) {
								// if it never gets here, last_move_direction doesn't change, that's what we want
								last_move_direction = i;
								break;
							}
						}
					}
				}
			}
			switch (last_move_direction) {
				case 0:
					movement_offset = util::Point<int>(-1, 0);
					break;
				case 1:
					movement_offset = util::Point<int>(1, 0);
					break;
				case 2:
					movement_offset = util::Point<int>(0, -1);
					break;
				case 3:
					movement_offset = util::Point<int>(0, 1);
					break;
			}
			util::Point<int> goal = position + movement_offset;
			wedge::Area *area = AREA->get_current_area();
			wedge::Area_Hooks *hooks = area->get_hooks();
			if (hooks->get_fade_zone(goal) != nullptr || hooks->get_scroll_zones(goal).size() > 0) {
				// Cancel out the above addition, and move one in the opposite direction
				movement_offset.x *= -2;
				movement_offset.y *= -2;
				goal += movement_offset;
			}
			if (set_path(goal, false)) {
				waiting_for_movement_end = true;
			}
		}
	}
	else {
		if (path.size() == 0) {
			waiting_for_movement_end = false;
			ticks = 0;
		}
	}

	return true;
}

}
