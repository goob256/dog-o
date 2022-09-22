#include "battle_game.h"
#include "combo.h"
#include "general.h"
#include "globals.h"

//bool Combo_Detector::ret_early = false;

Combo_Detector::Combo_Detector(std::vector<Combo> combos) :
	combos(combos),
	combo_good(-1),
	combo_error(false),
	joy_axis0(0.0f),
	joy_axis1(0.0f),
	n_ok(0)
{
	if (BATTLE && static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
		for (size_t i = 0; i < this->combos.size(); i++) {
			for (size_t j = 0; j < this->combos[i].size(); j++) {
				if (this->combos[i][j].button == B_L) {
					this->combos[i][j].button = B_R;
				}
				else if (this->combos[i][j].button == B_R) {
					this->combos[i][j].button = B_L;
				}
			}
		}
	}

	reset();
}

Combo_Detector::~Combo_Detector()
{
}

void Combo_Detector::reset()
{
	down = false;
	detected.clear();
	max_delay_between = MAX_DELAY_BETWEEN * combo_timing_mul();
}

void Combo_Detector::handle_event(TGUI_Event *event)
{
	if (ignore_event(event)) {
		return;
	}

	process_event(event);
}

void Combo_Detector::check()
{
	Uint32 now = SDL_GetTicks();

	combo_good = -1;
	combo_error = false;
	n_ok = 0;

	if (down == false && detected.size() == 0) {
		return;
	}
	
	combo_error = true;

	for (size_t i = 0; i < combos.size(); i++) {
		Combo &c = combos[i];
		int min = MIN((down ? 1 : 0) + (int)detected.size(), (int)c.size());
		bool found2 = true;
		bool complete = false;
		int no = 0;
		for (size_t j = 0; (int)j < min; j++) {
			bool active = j >= detected.size();
			if (active) {
				if (c[j].button != button) {
					found2 = false;
					break;
				}
				Uint32 held_time = now - down_time;
				if (held_time > c[j].max_hold) {
					found2 = false;
					break;
				}
				if (j != 0) {
					Uint32 delay = down_time - detected[j-1].release_time;
					if (delay > (Uint32)max_delay_between) {
						found2 = false;
						break;
					}
				}
			}
			else {
				if (c[j].button != detected[j].button) {
					found2 = false;
					break;
				}
				Uint32 held_time = detected[j].release_time - detected[j].down_time;
				if (held_time < c[j].min_hold || held_time > c[j].max_hold) {
					found2 = false;
					break;
				}
				if (j != 0) {
					Uint32 delay = detected[j].down_time - detected[j-1].release_time;
					if (delay > (Uint32)max_delay_between) {
						found2 = false;
						break;
					}
				}
				if ((int)j == min-1) {
					Uint32 delay = now - detected[j].release_time;
					if (delay > (Uint32)max_delay_between) {
						found2 = false;
						break;
					}
				}
				if ((int)j == min-1 && min == (int)c.size()) {
					complete = true;
				}
			}
			no++;
		}

		if (no > n_ok) {
			n_ok = no;
		}

		if (found2) {
			if (complete) {
				combo_good = (int)i;
			}
			combo_error = false;
			break;
		}
	}
}

bool Combo_Detector::error()
{
	return combo_error;
}

int Combo_Detector::good()
{
	return combo_good;
}

bool Combo_Detector::ignore_event(TGUI_Event *event)
{
	if (event->type != TGUI_KEY_DOWN && event->type != TGUI_KEY_UP && event->type != TGUI_JOY_AXIS && event->type != TGUI_JOY_DOWN && event->type != TGUI_JOY_UP) {
		return true;
	}

	if (event->type == TGUI_KEY_DOWN && event->keyboard.is_repeat) {
		return true;
	}
	if (event->type == TGUI_JOY_DOWN && event->joystick.is_repeat) {
		return true;
	}

	if ((down == false && detected.size() == 0) && (event->type == TGUI_KEY_UP || event->type == TGUI_JOY_UP)) {
		return true;
	}

	if (event->type != TGUI_JOY_AXIS) {
		Combo_Button b = get_button(event);
		if (b == B_NONE) {
			// this one actually ends in error
			Detected d;
			d.button = B_NONE;
			d.down_time = 0;
			d.release_time = 0;
			detected.push_back(d);
			return true;
		}
	}

	return false;
}

void Combo_Detector::process_event(TGUI_Event *event)
{
	Uint32 now = SDL_GetTicks();
	
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

	if (event->type == TGUI_JOY_AXIS) {
		return;
	}

	if (down && (event->type == TGUI_KEY_DOWN || event->type == TGUI_JOY_DOWN)) {
		Detected d;

		d.button = button;
		d.down_time = down_time;
		d.release_time = now;

		detected.push_back(d);
	}
	else if (down && (event->type == TGUI_KEY_UP || event->type == TGUI_JOY_UP) && get_button(event) == button) {
		Detected d;

		d.button = button;
		d.down_time = down_time;
		d.release_time = now;

		detected.push_back(d);

		down = false;
	}

	if (event->type == TGUI_KEY_DOWN || event->type == TGUI_JOY_DOWN) {
		button = get_button(event);
		down_time = now;

		down = true;
	}

	// convert back to original
	input::Focus_Event *focus;
	if ((focus = dynamic_cast<input::Focus_Event *>(event)) != NULL) {
		input::convert_to_focus_event(event, focus);
	}

/*
	if (ret_early) {
		return;
	}

	ret_early = true;

	TGUI_Event *e;
	while ((e = shim::pop_pushed_event()) != NULL) {
		wedge::handle_event(e);
	}

	ret_early = false;
*/
}

Combo_Button Combo_Detector::get_button(TGUI_Event *event)
{
	if (event->type == TGUI_KEY_DOWN || event->type == TGUI_KEY_UP) {
		// FIXME:
		if (event->keyboard.code == TGUIK_w) {
			return B_Y;
		}
		else if (event->keyboard.code == TGUIK_d) {
			return B_B;
		}
		else if (event->keyboard.code == TGUIK_s) {
			return B_A;
		}
		else if (event->keyboard.code == TGUIK_a) {
			return B_X;
		}
		else if (event->keyboard.code == GLOBALS->key_l) {
			return B_L;
		}
		else if (event->keyboard.code == GLOBALS->key_r) {
			return B_R;
		}
		else if (event->keyboard.code == GLOBALS->key_u) {
			return B_U;
		}
		else if (event->keyboard.code == GLOBALS->key_d) {
			return B_D;
		}
	}
	else if (event->type == TGUI_JOY_DOWN || event->type == TGUI_JOY_UP) {
		if (event->joystick.button == GLOBALS->joy_b1) {
			return B_A;
		}
		else if (event->joystick.button == GLOBALS->joy_b2) {
			return B_B;
		}
		else if (event->joystick.button == GLOBALS->joy_b3) {
			return B_X;
		}
		else if (event->joystick.button == GLOBALS->joy_b4) {
			return B_Y;
		}
		else if (event->joystick.button == TGUI_B_L) {
			return B_L;
		}
		else if (event->joystick.button == TGUI_B_R) {
			return B_R;
		}
		else if (event->joystick.button == TGUI_B_U) {
			return B_U;
		}
		else if (event->joystick.button == TGUI_B_D) {
			return B_D;
		}
	}

	return B_NONE;
}

void Combo_Detector::handle_joy(TGUI_Event *event, int x, int y, int old_x, int old_y)
{
	std::vector<TGUI_Event> events;
	TGUI_Event e;

	e.keyboard.simulated = true;

	if (x != old_x) {
		if (old_x != 0) {
			e.type = TGUI_KEY_UP;
			e.keyboard.code = old_x < 0 ? GLOBALS->key_l : GLOBALS->key_r;
			e.keyboard.is_repeat = false;
			events.push_back(e);
		}
		if (x != 0) {
			e.type = TGUI_KEY_DOWN;
			e.keyboard.code = x < 0 ? GLOBALS->key_l : GLOBALS->key_r;
			e.keyboard.is_repeat = false;
			events.push_back(e);
		}

	}
	if (y != old_y) {
		if (old_y != 0) {
			e.type = TGUI_KEY_UP;
			e.keyboard.code = old_y < 0 ? GLOBALS->key_u : GLOBALS->key_d;
			e.keyboard.is_repeat = false;
			events.push_back(e);
		}
		if (y != 0) {
			e.type = TGUI_KEY_DOWN;
			e.keyboard.code = y < 0 ? GLOBALS->key_u : GLOBALS->key_d;
			e.keyboard.is_repeat = false;
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

int Combo_Detector::num_ok()
{
	return n_ok;
}

std::vector<Combo_Detector::Detected> Combo_Detector::get_detected()
{
	return detected;
}
