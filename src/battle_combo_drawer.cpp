#include "wedge2/globals.h"
#include "wedge2/omnipresent.h"

#include "battle_game.h"
#include "battle_combo_drawer.h"
#include "combo.h"
#include "general.h"
#include "globals.h"
#include "milestones.h"

Battle_Combo_Drawer_Step::Battle_Combo_Drawer_Step(wedge::Task *task) :
	wedge::Step(task),
	end_draw(0),
	count(0),
	last_caption(-1),
	ended(false)
{
	next_caption();
}

Battle_Combo_Drawer_Step::~Battle_Combo_Drawer_Step()
{
}

void Battle_Combo_Drawer_Step::draw_fore()
{
	// this Step hooks OMNIPRESENT::draw_fore
	int mod = count % 2;
	count++;
	if (mod == 0) {
		OMNIPRESENT->hook_draw_last(this);
		return;
	}

	// draw cursors first
	if (cursors.size() > 0) {
		for (auto p : cursors) {
			DOGO_GLOBALS->cursor->get_current_image()->draw(p.first, p.second);
		}
		cursors.clear();
	}
	if (multi_checkboxes.size() > 0) {
		for (auto m : multi_checkboxes) {
			m.second->get_current_image()->draw(m.first);
		}
		multi_checkboxes.clear();
	}

	if (GET_TICKS() >= end_draw) {
		if (ended == false) {
			ended = true;
			next_caption();
		}
		return;
	}
	
	gfx::Font::end_batches();

	const int EDGE_X = shim::screen_size.w * 0.05f;
	int num_lines = 3;
	int w = (shim::screen_size.w - EDGE_X*2);
	int h = shim::font->get_height() * num_lines + 3*2 + 3*2;
	int x = (shim::screen_size.w-w)/2;
	int y = (shim::screen_size.h-h)*5/6;

	SDL_Colour win_colour = shim::white;//shim::palette[38];
	win_colour.r *= 0.9f;
	win_colour.g *= 0.9f;
	win_colour.b *= 0.9f;
	win_colour.a *= 0.9f;
	//gfx::draw_filled_rectangle(win_colour, util::Point<float>(x, y), util::Size<float>(w, h));
	gfx::draw_9patch_tinted(win_colour, DOGO_GLOBALS->gui_window, {float(x), float(y)}, {w, h});

	y += 3;

	SDL_Colour shadow = shim::black;
	shadow.r *= 0.25f;
	shadow.g *= 0.25f;
	shadow.b *= 0.25f;
	shadow.a *= 0.25f;
	
	DOGO_GLOBALS->bold_font->enable_shadow(shadow, gfx::Font::DROP_SHADOW);
	shim::font->enable_shadow(shadow, gfx::Font::DROP_SHADOW);

	SDL_Colour col = selected_colour(shim::palette[34], shim::white);
	DOGO_GLOBALS->bold_font->draw(col, caption, util::Point<int>(shim::screen_size.w/2-DOGO_GLOBALS->bold_font->get_text_width(caption)/2, y));

	y += shim::font->get_height() + 3;

	if (combo_name != "") {
		std::string combo_name_t = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(combo_name));
		if (GET_TICKS() < end_name) {
			DOGO_GLOBALS->bold_font->draw(shim::palette[5], combo_name_t, util::Point<int>(shim::screen_size.w/2-DOGO_GLOBALS->bold_font->get_text_width(combo_name_t)/2, y));
		}
		else {
			combo_name = "";
		}
	}
	else {
		const int char_size = 8;
		const int space_size = 5;
		int wid = int(c.size()-1) * space_size;;
		for (size_t i = 0; i < c.size(); i++) {
			Combo_Event &e = c[i];
			if (e.button == B_L || e.button == B_R || e.button == B_D || e.button == B_U) {
				wid += char_size;
			}
			else {
				std::string text;
#ifdef STEAMWORKS
				EControllerActionOrigin *origins = nullptr;
				ControllerHandle_t handle = 0;
				if (shim::steam_init_failed == false) {
					origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];
					handle = input::get_controller_handle();
				}
#endif

				if (e.button == B_A) {
					if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
						text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_A);
#else
						text = input::get_joystick_button_name(TGUI_B_A);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_kick, origins);
							if (count > 0) {
								text = input::get_joystick_button_name(origins[0]);
							}
							else {
								text = get_key_name(GLOBALS->key_b1);
							}
						}
						else
#endif
						{
							text = input::get_joystick_button_name(GLOBALS->joy_b1);
						}
					}
					else {
						text = get_key_name(GLOBALS->key_b1);
					}
				}
				else if (e.button == B_B) {
					if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
						text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_B);
#else
						text = input::get_joystick_button_name(TGUI_B_B);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_fierce_kick, origins);
							if (count > 0) {
								text = input::get_joystick_button_name(origins[0]);
							}
							else {
								text = get_key_name(GLOBALS->key_b2);
							}
						}
						else
#endif
						{
							text = input::get_joystick_button_name(GLOBALS->joy_b2);
						}
					}
					else {
						text = get_key_name(GLOBALS->key_b2);
					}
				}
				else if (e.button == B_X) {
					if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
						text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_X);
#else
						text = input::get_joystick_button_name(TGUI_B_X);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_punch, origins);
							if (count > 0) {
								text = input::get_joystick_button_name(origins[0]);
							}
							else {
								text = get_key_name(GLOBALS->key_b3);
							}
						}
						else
#endif
						{
							text = input::get_joystick_button_name(GLOBALS->joy_b3);
						}
					}
					else {
						text = get_key_name(GLOBALS->key_b3);
					}
				}
				else if (e.button == B_Y) {
					if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
						text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_Y);
#else
						text = input::get_joystick_button_name(TGUI_B_Y);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_fierce_punch, origins);
							if (count > 0) {
								text = input::get_joystick_button_name(origins[0]);
							}
							else {
								text = get_key_name(GLOBALS->key_b4);
							}
						}
						else
#endif
						{
							text = input::get_joystick_button_name(GLOBALS->joy_b4);
						}
					}
					else {
						text = get_key_name(GLOBALS->key_b4);
					}
				}

#ifdef STEAMWORKS
				if (shim::steam_init_failed == false) {
					delete[] origins;
				}
#endif

				wid += DOGO_GLOBALS->bold_font->get_text_width(text);
			}
		}
		x = shim::screen_size.w/2 - wid/2;
		//Uint32 now = GET_TICKS();
		//int duration = 1000;
		//int wait = 2000;
		//int t = now % (duration + wait);
		for (size_t i = 0; i < c.size(); i++) {
			Combo_Event &e = c[i];

			gfx::Image *img = NULL;
			std::string text;

			int this_w = -1;

#ifdef STEAMWORKS
			EControllerActionOrigin *origins = nullptr;
			ControllerHandle_t handle = 0;
			if (shim::steam_init_failed == false) {
				origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];
				handle = input::get_controller_handle();
			}
#endif

			if (e.button == B_L) {
				img = DOGO_GLOBALS->combo_l;
				this_w = char_size;
			}
			else if (e.button == B_R) {
				img = DOGO_GLOBALS->combo_r;
				this_w = char_size;
			}
			else if (e.button == B_U) {
				img = DOGO_GLOBALS->combo_u;
				this_w = char_size;
			}
			else if (e.button == B_D) {
				img = DOGO_GLOBALS->combo_d;
				this_w = char_size;
			}
			else if (e.button == B_A) {
				if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
					text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_A);
#else
					text = input::get_joystick_button_name(TGUI_B_A);
#endif
				}
				else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
					if (shim::steam_init_failed == false) {
						int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_kick, origins);
						if (count > 0) {
							//colour_code = input::get_joystick_button_colour_code(origins[0]);
							text = input::get_joystick_button_name(origins[0]);
						}
						else {
							text = get_key_name(GLOBALS->key_b1);
						}
					}
					else
#endif
					{
						text = input::get_joystick_button_name(GLOBALS->joy_b1);
						//colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b1);
					}
				}
				else {
					text = get_key_name(GLOBALS->key_b1);
				}
			}
			else if (e.button == B_B) {
				if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
					text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_B);
#else
					text = input::get_joystick_button_name(TGUI_B_B);
#endif
				}
				else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
					if (shim::steam_init_failed == false) {
						int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_fierce_kick, origins);
						if (count > 0) {
							//colour_code = input::get_joystick_button_colour_code(origins[0]);
							text = input::get_joystick_button_name(origins[0]);
						}
						else {
							text = get_key_name(GLOBALS->key_b2);
						}
					}
					else
#endif
					{
						text = input::get_joystick_button_name(GLOBALS->joy_b2);
						//colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b2);
					}
				}
				else {
					text = get_key_name(GLOBALS->key_b2);
				}
			}
			else if (e.button == B_X) {
				if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
					text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_X);
#else
					text = input::get_joystick_button_name(TGUI_B_X);
#endif
				}
				else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
					if (shim::steam_init_failed == false) {
						int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_punch, origins);
						if (count > 0) {
							//colour_code = input::get_joystick_button_colour_code(origins[0]);
							text = input::get_joystick_button_name(origins[0]);
						}
						else {
							text = get_key_name(GLOBALS->key_b3);
						}
					}
					else
#endif
					{
						text = input::get_joystick_button_name(GLOBALS->joy_b3);
						//colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b3);
					}
				}
				else {
					text = get_key_name(GLOBALS->key_b3);
				}
			}
			else if (e.button == B_Y) {
				if (wedge::is_onscreen_controller_enabled()) {
#ifdef STEAMWORKS
					text = input::get_joystick_button_name(k_EControllerActionOrigin_XBoxOne_Y);
#else
					text = input::get_joystick_button_name(TGUI_B_Y);
#endif
				}
				else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
					if (shim::steam_init_failed == false) {
						int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_fierce_punch, origins);
						if (count > 0) {
							//colour_code = input::get_joystick_button_colour_code(origins[0]);
							text = input::get_joystick_button_name(origins[0]);
						}
						else {
							text = get_key_name(GLOBALS->key_b4);
						}
					}
					else
#endif
					{
						text = input::get_joystick_button_name(GLOBALS->joy_b4);
						//colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b4);
					}
				}
				else {
					text = get_key_name(GLOBALS->key_b4);
				}
			}

#ifdef STEAMWORKS
			if (shim::steam_init_failed == false) {
				delete[] origins;
			}
#endif

			if (this_w < 0) {
				this_w = DOGO_GLOBALS->bold_font->get_text_width(text);
			}
			
			SDL_Colour colour = shim::palette[5];

			float yo = 0.0f;
			/*
			if (t < duration) {
				float c_p;
				if (c.size() == 1) {
					c_p = 0.0f;
				}
				else {
					c_p = i / (float)(c.size()-1);
				}
				int start_time = sin(c_p * M_PI / 2.0f) * duration / 3;
				int end_time = start_time + duration * 2 / 3.0f;
				if (t >= start_time && t < end_time) {
					int tt = t - start_time;
					float p = tt / (duration*2.0f/3.0f);
					yo = sin(p * M_PI) * max_h;
				}
			}
			*/

			if (img) {
				int xx = x + char_size/2 - img->size.w/2;

				img->start_batch();
				img->draw_tinted(shadow, util::Point<float>(xx+1, y-yo));
				img->draw_tinted(shadow, util::Point<float>(xx+1, y-yo+1));
				img->draw_tinted(shadow, util::Point<float>(xx, y-yo+1));
				img->draw_tinted(colour, util::Point<float>(xx, y-yo));
				img->end_batch();
			}
			else {
				int xx = x;
				DOGO_GLOBALS->bold_font->draw(colour, text, util::Point<float>(xx, y-yo));
			}

			x += this_w + space_size;
		}

		if (INSTANCE->is_milestone_complete(MS_DID_AN_ATTACK) == false) {
#ifdef STEAMWORKS
			EControllerActionOrigin *origins = nullptr;
			ControllerHandle_t handle = 0;
			if (shim::steam_init_failed == false) {
				origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];
				handle = input::get_controller_handle();
			}
#endif

			std::string pleasant_button;
			std::string tom_button;

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

#if defined ANDROID || (defined IOS && !defined TVOS)
			if (input::system_has_keyboard() == false) {
#else
			if (wedge::is_onscreen_controller_enabled()) {
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

			int player = static_cast<Battle_Game *>(BATTLE)->get_active_player();

			std::string button;

			if (player == 0) {
				button = pleasant_button;
			}
			else {
				button = tom_button;
			}

			std::string open_quote = GLOBALS->game_t->translate(402)/* Originally: " */;
			std::string close_quote = GLOBALS->game_t->translate(403)/* Originally: " */;

			std::string text = GLOBALS->game_t->translate(162)/* Originally: Press */ + " " + open_quote + button + close_quote;

			int xx = shim::screen_size.w/2;

			int t = GET_TICKS() % 1000;
			int tip_xo = t < 500 ? 1 : 0;
			gfx::draw_9patch(DOGO_GLOBALS->tip_window, util::Point<int>(xx-20-shim::font->get_text_width(text)-10+tip_xo, y-2), util::Size<int>(shim::font->get_text_width(text) + 10, shim::font->get_height() + 3));
			DOGO_GLOBALS->tip_tri->draw({float(xx-20-2+tip_xo), float(y+shim::font->get_height()/2-DOGO_GLOBALS->tip_tri->size.h/2-1)}, gfx::Image::FLIP_H);
			shim::font->disable_shadow();
			shim::font->draw(shim::black, text, util::Point<int>(xx-20-shim::font->get_text_width(text)-5+tip_xo, y));
			shim::font->enable_shadow(shadow, gfx::Font::DROP_SHADOW);

#ifdef STEAMWORKS
			if (shim::steam_init_failed == false) {
				delete[] origins;
			}
#endif

		}
	}
	
	y += shim::font->get_height() + 3;

	std::string button_text;

	if (input::is_joystick_connected() == false) {
		if (input::system_has_keyboard()) {
			button_text = get_key_name(GLOBALS->key_cancel);
		}
		else {
			button_text = GLOBALS->game_t->translate(368)/* Originally: the back arrow */;
		}
	}
	else {
#ifdef STEAMWORKS
		if (shim::steam_init_failed == false) {
			EControllerActionOrigin *origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];

			ControllerHandle_t handle = input::get_controller_handle();

			int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_cancel, origins);
			if (count > 0) {
				std::string colour = input::get_joystick_button_colour_code(origins[0]);
				std::string name = input::get_joystick_button_name(origins[0]);

				button_text = colour + name;
			}
			else {
				button_text = get_key_name(GLOBALS->key_cancel);
			}
		}
		else
#endif
		{
			std::string colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_cancel);
			std::string name = input::get_joystick_button_name(GLOBALS->joy_cancel);
			button_text = colour_code + name;
		}
	}

	shim::font->draw(shim::palette[36], button_text + " " + GLOBALS->game_t->translate(191)/* Originally: exits */, util::Point<int>(shim::screen_size.w/2, y), true, true);

	DOGO_GLOBALS->bold_font->disable_shadow();
	shim::font->disable_shadow();
}

void Battle_Combo_Drawer_Step::set(Combo c)
{
	this->c = c;
	end_draw = GET_TICKS() + 250;
	ended = false;
}

void Battle_Combo_Drawer_Step::set(std::string combo_name)
{
	if (get_combo_multi(combo_name) == true) {
		end_draw = 0;
	}
	else {
		this->combo_name = combo_name;
		end_name = GET_TICKS() + 250;
		end_draw = GET_TICKS() + 250;
		ended = false;
	}
}

void Battle_Combo_Drawer_Step::end_now()
{
	end_draw = 0;
}

void Battle_Combo_Drawer_Step::set_cursors(std::vector< std::pair<util::Point<float>, int> > cursors)
{
	this->cursors = cursors;
}

void Battle_Combo_Drawer_Step::set_multi_checkboxes(std::vector< std::pair<util::Point<float>, gfx::Sprite *> > multi_checkboxes)
{
	this->multi_checkboxes = multi_checkboxes;
}

void Battle_Combo_Drawer_Step::next_caption()
{
	int r;
	do {
		r = util::rand(0, 4);
		r += 186; // caption #1
	} while (r == last_caption);
	last_caption = r;
	caption = GLOBALS->game_t->translate(r);
}
