#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/globals.h"
#include "wedge2/input.h"
#include "wedge2/omnipresent.h"
#include "wedge2/player_input.h"
#include "wedge2/tile_movement.h"

using namespace wedge;

namespace wedge {

Player_Input_Step::Player_Input_Step(Map_Entity *entity, Task *task) :
	Map_Entity_Input_Step(entity, task),
	input_paused(false)
{
}

Player_Input_Step::~Player_Input_Step()
{
}

void Player_Input_Step::handle_event(TGUI_Event *event)
{
	if (
		(event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_menu && event->keyboard.is_repeat == false) ||
		(event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_menu && event->joystick.is_repeat == false)
	) {
		bool do_pause = false;
		bool do_mini_pause = false;
		if (presses_paused == false && !(following_path && can_cancel_path == false) && MENU == nullptr) {
			//if (movement_step->is_moving() == false && GLOBALS->dialogue_active(AREA) == false) {
			if (GLOBALS->dialogue_active(AREA) == false && !(INSTANCE->stats.size() > 1 && INSTANCE->party_following_player == false)) {
				do_pause = true;
			}
			else {
				do_mini_pause = true;
			}
		}
		else {
			// do nothing
			//mini_pause();
		}
		if (do_pause || do_mini_pause) {
			if (movement_step->is_moving()) {
				stop_at_next_tile = true;
				if (do_pause) {
					pause_at_next_tile = true;
				}
				else {
					mini_pause_at_next_tile = true;
				}
			}
			else {
				if (do_pause) {
					AREA->start_menu();
				}
				else {
					// do nothing
					//mini_pause();
				}
			}
		}
	}
	else if (
		((GLOBALS->key_back != GLOBALS->key_pause || MENU == nullptr) && event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_pause && event->keyboard.is_repeat == false && event->keyboard.simulated == false) ||
		(event->type == TGUI_KEY_DOWN && event->keyboard.code == PAUSE_VAL && event->keyboard.is_repeat == false) ||
		(event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_pause && event->joystick.is_repeat == false)
	) {
		mini_pause();
	}
	else if (input_paused == false) {
		Map_Entity_Input_Step::manual_handle_event(event);
	}
}

void Player_Input_Step::set_input_paused(bool paused)
{
	input_paused = paused;
}

}
