#include <wedge2/map_entity.h>
#include <wedge2/onscreen_controller.h>
#include <wedge2/special_number.h>

#include "area_game.h"
#include "combo.h"
#include "general.h"
#include "globals.h"
#include "menu.h"
#include "milestones.h"
#include "stats.h"
#include "widgets.h"

static std::vector<float> vert_vec;

static void draw_win(int x, int y, int w, int h)
{
	gfx::draw_9patch(DOGO_GLOBALS->menu_window, util::Point<int>(x, y), util::Size<int>(w, h));

	gfx::set_scissor(x+2, y+2, w-4, h-4);

	const int min_sz = 5;
	const int max_sz = 8;

	float p = MIN(1.0f, (float)h / (shim::screen_size.h * 0.75f));
	int start_size = min_sz + p * (max_sz - min_sz);

	int y2 = 0;

	for (int sz = start_size; sz > 1; sz--) {
		y += sz;
	}

	int r = 0;
	
	int count = 0;
	int count2 = 0;

	// just count how many first
	for (int sz = 2; sz <= start_size; sz++) {
		int n = (start_size - (start_size - sz)) / (float)start_size * max_sz;
		int num = (w-4) / sz * ((n*n)/16.0f);

		for (int i = 0; i < num; i++) {
			count++;
		}
	}

	vert_vec.reserve(count*6*12);
	vert_vec.resize(0);

	for (int sz = 2; sz <= start_size; sz++) {
		y -= sz;

		int n = (start_size - (start_size - sz)) / (float)start_size * max_sz;
		int num = (w-4) / sz * ((n*n)/16.0f);

		for (int i = 0; i < num; i++) {
			int rand = DOGO_GLOBALS->rands[r % DOGO_GLOBALS->rands.size()];
			r++;
			int yo = (rand / (float)DOGO_GLOBALS->rand_max) * sz;
			yo -= sz/2;
			rand = DOGO_GLOBALS->rands[r % DOGO_GLOBALS->rands.size()];
			r++;
			int xo = int((rand / (float)DOGO_GLOBALS->rand_max) * ((w-4)+sz)) - sz/2;

			float p = 1.0f - (sz / (float)start_size);

			SDL_Colour colour = shim::palette[25];
			SDL_Colour end = shim::palette[24];

			colour.r = MIN(255, colour.r + p * (end.r-colour.r));
			colour.g = MIN(255, colour.g + p * (end.g-colour.g));
			colour.b = MIN(255, colour.b + p * (end.b-colour.b));

			// xyz
			vert_vec[count2++] = x+2+xo;
			vert_vec[count2++] = y+2+y2+yo;
			vert_vec[count2++] = 0.0f;
			// normals
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// texcoord
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// colour
			vert_vec[count2++] = colour.r/255.0f;
			vert_vec[count2++] = colour.g/255.0f;
			vert_vec[count2++] = colour.b/255.0f;
			vert_vec[count2++] = colour.a/255.0f;

			// xyz
			vert_vec[count2++] = x+2+xo+sz;
			vert_vec[count2++] = y+2+y2+yo;
			vert_vec[count2++] = 0.0f;
			// normals
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// texcoord
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// colour
			vert_vec[count2++] = colour.r/255.0f;
			vert_vec[count2++] = colour.g/255.0f;
			vert_vec[count2++] = colour.b/255.0f;
			vert_vec[count2++] = colour.a/255.0f;

			// xyz
			vert_vec[count2++] = x+2+xo;
			vert_vec[count2++] = y+2+y2+yo+sz;
			vert_vec[count2++] = 0.0f;
			// normals
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// texcoord
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// colour
			vert_vec[count2++] = colour.r/255.0f;
			vert_vec[count2++] = colour.g/255.0f;
			vert_vec[count2++] = colour.b/255.0f;
			vert_vec[count2++] = colour.a/255.0f;

			// xyz
			vert_vec[count2++] = x+2+xo+sz;
			vert_vec[count2++] = y+2+y2+yo;
			vert_vec[count2++] = 0.0f;
			// normals
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// texcoord
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// colour
			vert_vec[count2++] = colour.r/255.0f;
			vert_vec[count2++] = colour.g/255.0f;
			vert_vec[count2++] = colour.b/255.0f;
			vert_vec[count2++] = colour.a/255.0f;

			// xyz
			vert_vec[count2++] = x+2+xo+sz;
			vert_vec[count2++] = y+2+y2+yo+sz;
			vert_vec[count2++] = 0.0f;
			// normals
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// texcoord
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// colour
			vert_vec[count2++] = colour.r/255.0f;
			vert_vec[count2++] = colour.g/255.0f;
			vert_vec[count2++] = colour.b/255.0f;
			vert_vec[count2++] = colour.a/255.0f;

			// xyz
			vert_vec[count2++] = x+2+xo;
			vert_vec[count2++] = y+2+y2+yo+sz;
			vert_vec[count2++] = 0.0f;
			// normals
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// texcoord
			vert_vec[count2++] = 0.0f;
			vert_vec[count2++] = 0.0f;
			// colour
			vert_vec[count2++] = colour.r/255.0f;
			vert_vec[count2++] = colour.g/255.0f;
			vert_vec[count2++] = colour.b/255.0f;
			vert_vec[count2++] = colour.a/255.0f;
		}
	}

	gfx::Vertex_Cache::instance()->start();
	gfx::Vertex_Cache::instance()->cache_3d_immediate(vert_vec.data(), count*2);
	gfx::Vertex_Cache::instance()->end();
	
	gfx::unset_scissor();

	/*
	gfx::enable_depth_test(false);
	gfx::set_depth_mode(gfx::COMPARE_LESSEQUAL);
	*/
}

Menu_Game::Menu_Game()
{
}

Menu_Game::~Menu_Game()
{
	for (std::vector<gui::GUI *>::iterator it = shim::guis.begin(); it != shim::guis.end(); it++) {
		gui::GUI *gui = *it;
		if (dynamic_cast<Menu_GUI *>(gui)) {
			gui->exit();
		}
	}
}

bool Menu_Game::start()
{
	if (Game::start() == false) {
		return false;
	}

	Main_Menu_GUI *gui = new Main_Menu_GUI(0);
	shim::guis.push_back(gui);

	return true;
}

bool Menu_Game::run()
{
	Game::run(); // don't return if false, there are no systems usually
	
	if (AREA->is_pausing() == false && AREA->is_paused() == false && (shim::guis.size() == 0 || dynamic_cast<Menu_GUI *>(shim::guis.back()) == NULL)) {
		return false;
	}

	return true;
}

void Menu_Game::draw()
{
	Game::draw();
}

void Menu_Game::draw_fore()
{
	Game::draw_fore();
}

//--

Menu_GUI::Menu_GUI()
{
	transition_is_slide = false;
	transition_is_appear = true;
}

Menu_GUI::~Menu_GUI()
{
}

void Menu_GUI::draw()
{
	GUI::draw();

	DOGO_GLOBALS->bold_font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
	// FIXME: translate
	DOGO_GLOBALS->bold_font->draw(shim::white, util::uppercase(menu_name), util::Point<int>(shim::screen_size.w/2-DOGO_GLOBALS->bold_font->get_text_width(menu_name)/2, EDGE_Y-DOGO_GLOBALS->bold_font->get_height()/2));
	DOGO_GLOBALS->bold_font->disable_shadow();
}

void Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}
	
	gui::GUI::handle_event(event);

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_menu) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_menu)) {
		cancel_all_special_numbers(MENU);
		AREA->end_menu();
		exit();
	}
}

//--

Multiplayer_Menu_GUI::Multiplayer_Menu_GUI() :
	player_index(0),
	draw_name(true)
{
}

Multiplayer_Menu_GUI::~Multiplayer_Menu_GUI()
{
}
	
void Multiplayer_Menu_GUI::draw_back()
{
	GUI::draw_back();

	int top_h = DOGO_GLOBALS->profile_pics[get_profile_pic_index(player_index)]->size.h + BORDER*2;

	draw_win(EDGE_X, EDGE_Y, shim::screen_size.w-EDGE_X*2, top_h);

	draw_win(EDGE_X, EDGE_Y+top_h, shim::screen_size.w-EDGE_X*2, shim::screen_size.h-top_h-EDGE_Y*2);
}

std::string Multiplayer_Menu_GUI::get_switch_text()
{
	if (input::is_joystick_connected() == false && input::system_has_keyboard() == false) {
		if (wedge::is_onscreen_controller_enabled()) {
			return "|16[|08" + get_key_name(TGUIK_RIGHT) + "|16] |22" + GLOBALS->game_t->translate(137)/* Originally: Next Character */;
		}
		else {
			return util::uppercase(std::string("|08") + GLOBALS->game_t->translate(137)/* Originally: Next Character */);
		}
	}
#ifdef STEAMWORKS
	else if (shim::steam_init_failed == false) {
		if (input::is_joystick_connected()) {
			EControllerActionOrigin *origins = new EControllerActionOrigin[STEAM_CONTROLLER_MAX_ORIGINS];

			ControllerHandle_t handle = input::get_controller_handle();

			int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->menu_action_set, DOGO_GLOBALS->menu_switch, origins);
			if (count > 0) {
				std::string colour = input::get_joystick_button_colour_code(origins[0]);
				std::string name = input::get_joystick_button_name(origins[0]);

				delete[] origins;

				return "|16[|08" + colour + name + "|16] |22";
			}
		}
		return "|16[|08" + get_key_name(GLOBALS->key_switch) + "|16] |22";
	}
#endif
	else
	{
		return "|16[|08" + (input::is_joystick_connected() ? input::get_joystick_button_colour_code(GLOBALS->joy_switch) + input::get_joystick_button_name(GLOBALS->joy_switch) : get_key_name(GLOBALS->key_switch)) + "|16] |22";
	}
}

void Multiplayer_Menu_GUI::draw()
{
	Menu_GUI::draw();

	gfx::Image *pp = DOGO_GLOBALS->profile_pics[get_profile_pic_index(player_index)];
	util::Point<int> pos(EDGE_X+BORDER, EDGE_Y+BORDER);
	SDL_Colour shadow_col = shim::white;
	shadow_col.r *= 0.15f;
	shadow_col.g *= 0.15f;
	shadow_col.b *= 0.15f;
	shadow_col.a *= 0.15f;
	draw_tinted_shadowed_image(shim::white, shadow_col, pp, pos, gfx::Font::FULL_SHADOW);

	if (draw_name) {
		DOGO_GLOBALS->bold_font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
		DOGO_GLOBALS->bold_font->draw(shim::white, GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(INSTANCE->stats[player_index].base.get_name())), util::Point<int>(DOGO_GLOBALS->profile_pics[0]->size.w + 3 + BORDER + EDGE_X, DOGO_GLOBALS->profile_pics[0]->size.h/2 + BORDER + EDGE_Y - DOGO_GLOBALS->bold_font->get_height()/2));
		DOGO_GLOBALS->bold_font->disable_shadow();
	}

	if (INSTANCE->stats.size() > 1) {
		if (input::is_joystick_connected() == false && input::system_has_keyboard() == false) {
			std::string text = get_switch_text();
			DOGO_GLOBALS->bold_font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
			// FIXME: translate
			DOGO_GLOBALS->bold_font->draw(shim::white, text, util::Point<int>(EDGE_X+5, shim::screen_size.h-EDGE_Y-DOGO_GLOBALS->bold_font->get_height()/2-2));
			DOGO_GLOBALS->bold_font->disable_shadow();
		}
		else
		{
			std::string text = get_switch_text();
			text += GLOBALS->game_t->translate(137)/* Originally: Next Character */;
			DOGO_GLOBALS->bold_font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
			// FIXME: translate
			DOGO_GLOBALS->bold_font->draw(shim::white, text, util::Point<int>(EDGE_X+5, shim::screen_size.h-EDGE_Y-DOGO_GLOBALS->bold_font->get_height()/2-2));
			DOGO_GLOBALS->bold_font->disable_shadow();
		}
	}
}

void Multiplayer_Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}
	
	Menu_GUI::handle_event(event);

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_switch) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_switch)) {
		switch_chars();
	}
	else if (wedge::is_onscreen_controller_enabled() && event->type == TGUI_FOCUS && event->focus.type == TGUI_FOCUS_RIGHT) {
		switch_chars();
	}
	else if (event->type == TGUI_MOUSE_DOWN) {
		std::string text = get_switch_text();
		int w = DOGO_GLOBALS->bold_font->get_text_width(text);
		util::Point<int> pos = util::Point<int>(EDGE_X+5, shim::screen_size.h-EDGE_Y-DOGO_GLOBALS->bold_font->get_height()/2-2);
		if (event->mouse.x >= pos.x && event->mouse.x < pos.x+w && event->mouse.y >= pos.y && event->mouse.y < pos.y+DOGO_GLOBALS->bold_font->get_height()) {
			switch_chars();
		}
	}
}

void Multiplayer_Menu_GUI::switch_chars()
{
	if (INSTANCE->stats.size() > 1) {
		player_index++;
		player_index %= INSTANCE->stats.size();
		shim::widget_mml->play(false);
		set_text();
	}
}

//--

void practice_sprite_callback(void *data)
{
	gfx::Sprite *sprite = static_cast<gfx::Sprite *>(data);
	sprite->set_animation("stand_w");
}

Practice_Menu_GUI::Practice_Menu_GUI() :
	practicing(false),
	good_end(0),
	last_good(-1),
	sfx(nullptr)
{
	// FIXME: translate
	menu_name = "|16[|08" + GLOBALS->game_t->translate(130)/* Originally: Practice */ + "|16]";

	TGUI_Widget *modal_main_widget = new TGUI_Widget(1.0f, 1.0f);
	modal_main_widget->set_padding_left(EDGE_X);
	modal_main_widget->set_padding_right(EDGE_X);
	modal_main_widget->set_padding_top(EDGE_Y);
	modal_main_widget->set_padding_bottom(EDGE_Y);

	TGUI_Widget *container = new TGUI_Widget(0.8f, 1.0f);
	container->set_centre_x(true);
	container->set_parent(modal_main_widget);

	list = new Widget_Quantity_List(1.0f, get_list_height(shim::screen_size));
	list->set_padding_top(DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*3);
	list->set_text_shadow_colour(shim::palette[23]);
	list->set_parent(container);

	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);

	set_text();

	get_konami();
}

Practice_Menu_GUI::~Practice_Menu_GUI()
{
}

void Practice_Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}

	if (practicing) {
		if (input::is_joystick_connected() && event->type == TGUI_KEY_DOWN && get_key_name(event->keyboard.code) == "") {
			// mystery key, still don't know what this is
			// edit: I think this is the 0x40000000 key for "back" like back button on mouse/alt-left
			return;
		}
		if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_cancel) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_cancel)) {
			if (sfx != nullptr) {
				sfx->stop();
			}
			cancel_all_screen_shake(MENU);

			DOGO_GLOBALS->button->play(false);
			gui->set_focus(list);
			practicing = false;
			GLOBALS->allow_global_mini_pause = true;
			GLOBALS->getting_combo = false;
			shim::convert_directions_to_focus_events = true;

			GLOBALS->onscreen_controller_was_enabled = osc_enabled;

			INSTANCE->stats[player_index].sprite->set_animation(orig_anim);

			wedge::set_onscreen_controller_generates_b1_b2(false);
			wedge::set_onscreen_controller_b2_enabled(false);
			wedge::set_onscreen_controller_b3_enabled(false, GLOBALS->key_b3);
			wedge::set_onscreen_controller_b4_enabled(false, GLOBALS->key_b4);
			delete detector;
		}
		else {
			detector->handle_event(event);
			detector->check();
			if (detector->error()) {
				last_good = -1;
				Uint32 now = GET_TICKS();
				Uint32 elap = now - DOGO_GLOBALS->last_error_play;
				if ((int)elap >= 500) {
					DOGO_GLOBALS->last_error_play = now;
					DOGO_GLOBALS->error->play(false);
				}
				detector->reset();
			}
			else if (detector->good() != -1) {
				last_good = detector->good();
				DOGO_GLOBALS->error->stop();
				detector->reset();
				/*
				int max = 0;
				for (auto c : combos) {
					if ((int)c.size() > last_good+1) {
						int max_hold = c[last_good+1].max_hold;
						if (max_hold > max) {
							max = max_hold;
						}
					}
				}
				if (max == 0) {
					max = 500;
				}
				util::debugmsg("max=%d\n", max);
				good_end = GET_TICKS() + max;
				*/
				good_end = GET_TICKS() + 500;
				get_konami();
				std::string attack_name;
				if (last_good < (int)combo_names.size()) {
					attack_name = util::lowercase(combo_names[last_good]);
					int pos;
					if ((pos = attack_name.find('\'')) != std::string::npos) {
						attack_name = attack_name.substr(0, pos) + attack_name.substr(pos+1);
					}
					for (size_t i = 0; i < attack_name.length(); i++) {
						if (attack_name[i] == ' ') {
							attack_name[i] = '_';
						}
					}
				}
				INSTANCE->stats[player_index].sprite->set_animation(last_good >= (int)combo_names.size() ? "mouth_open" : std::string("attack_") + attack_name, practice_sprite_callback, INSTANCE->stats[player_index].sprite);
				sfx = last_good >= (int)combo_names.size() ? DOGO_GLOBALS->shock : get_combo_sfx(attack_name);
				if (sfx != nullptr) {
					sfx->play(false);
				}
				do_combo_screen_shake(attack_name);
			}
		}
	}
	else {
		Multiplayer_Menu_GUI::handle_event(event);

		if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
			DOGO_GLOBALS->button->play(false);
			Main_Menu_GUI *gui = new Main_Menu_GUI(3);
			shim::guis.push_back(gui);
			exit();
		}
	}
}

void Practice_Menu_GUI::draw()
{
	Multiplayer_Menu_GUI::draw();

	if (practicing) {
		Uint32 now = GET_TICKS();

		gfx::Font::end_batches();

		SDL_Colour tint = shim::black;
		tint.r *= 0.75f;
		tint.g *= 0.75f;
		tint.b *= 0.75f;
		tint.a *= 0.75f;
		gfx::draw_filled_rectangle(tint, util::Point<int>(0, 0), shim::screen_size);

		SDL_Colour shadow = shim::black;
		shadow.r *= 0.25f;
		shadow.g *= 0.25f;
		shadow.b *= 0.25f;
		shadow.a *= 0.25f;
		
		DOGO_GLOBALS->bold_font->enable_shadow(shadow, gfx::Font::DROP_SHADOW);
		//shim::font->enable_shadow(shadow, gfx::Font::DROP_SHADOW);

		int w = 180;
		int h;
		const float max_h = 5;

		if (combos.size() == 1) {
			h = DOGO_GLOBALS->bold_font->get_height()*2 + max_h + BORDER*4 + 10;
		}
		else {
			h = DOGO_GLOBALS->bold_font->get_height()*2 + BORDER*4 + 10;
		}

		gfx::Image *player_img = INSTANCE->stats[player_index].sprite->get_current_image();
		h += 10 + shim::tile_size*1.5f;

		int y = shim::screen_size.h/2-h/2;
		
		//gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(shim::screen_size.w/2-w/2, shim::screen_size.h/2-h/2), util::Size<int>(w, h));
		//draw_win(shim::screen_size.w/2-w/2, shim::screen_size.h/2-h/2, w, h);
		gfx::draw_9patch(DOGO_GLOBALS->gui_window, util::Point<int>(shim::screen_size.w/2-w/2, shim::screen_size.h/2-h/2), util::Size<int>(w, h));

		if (combos.size() == 1) {
			y += BORDER*2+max_h;
			Combo &c = combos[0];
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
			int x = shim::screen_size.w/2 - wid/2;
			int n_ok = detector->num_ok();
			now -= start_time;
			int duration = 1000;
			int wait = 2000;
			int t = now % (duration + wait);
			for (size_t i = 0; i < c.size(); i++) {
				Combo_Event &e = c[i];

				gfx::Image *img = NULL;
				std::string text;
				std::string colour_code;

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
						colour_code = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_A);
#else
						text = input::get_joystick_button_name(TGUI_B_A);
						colour_code = input::get_joystick_button_colour_code(TGUI_B_A);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_kick, origins);
							if (count > 0) {
								colour_code = input::get_joystick_button_colour_code(origins[0]);
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
							colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b1);
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
						colour_code = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_B);
#else
						text = input::get_joystick_button_name(TGUI_B_B);
						colour_code = input::get_joystick_button_colour_code(TGUI_B_B);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_fierce_kick, origins);
							if (count > 0) {
								colour_code = input::get_joystick_button_colour_code(origins[0]);
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
							colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b2);
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
						colour_code = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_X);
#else
						text = input::get_joystick_button_name(TGUI_B_X);
						colour_code = input::get_joystick_button_colour_code(TGUI_B_X);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_punch, origins);
							if (count > 0) {
								colour_code = input::get_joystick_button_colour_code(origins[0]);
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
							colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b3);
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
						colour_code = input::get_joystick_button_colour_code(k_EControllerActionOrigin_XBoxOne_Y);
#else
						text = input::get_joystick_button_name(TGUI_B_Y);
						colour_code = input::get_joystick_button_colour_code(TGUI_B_Y);
#endif
					}
					else if (input::is_joystick_connected()) {
#ifdef STEAMWORKS
						if (shim::steam_init_failed == false) {
							int count = SteamController()->GetDigitalActionOrigins(handle, DOGO_GLOBALS->combo_action_set, DOGO_GLOBALS->combo_fierce_punch, origins);
							if (count > 0) {
								colour_code = input::get_joystick_button_colour_code(origins[0]);
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
							colour_code = input::get_joystick_button_colour_code(GLOBALS->joy_b4);
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
				
				SDL_Colour colour;
				if ((int)i < n_ok || GET_TICKS() < good_end) {
					colour = shim::palette[5];
					colour_code = "";
				}
				else {
					colour = shim::white;
				}

				float yo = 0.0f;
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
					DOGO_GLOBALS->bold_font->draw(colour, colour_code + text, util::Point<float>(xx, y-2-yo));
				}

				x += this_w + space_size;
			}

			y += DOGO_GLOBALS->bold_font->get_height() + 10 + shim::tile_size*1.5f;

			util::Point<int> player_pos(shim::screen_size.w/2+w/2-BORDER*4-shim::tile_size-player_img->size.w/2, y-player_img->size.h);
			SDL_Colour shadow_col = shim::white;
			shadow_col.r *= 0.15f;
			shadow_col.g *= 0.15f;
			shadow_col.b *= 0.15f;
			shadow_col.a *= 0.15f;
			draw_tinted_shadowed_image(shim::white, shadow_col, player_img, player_pos, gfx::Font::FULL_SHADOW);
			//player_img->draw(player_pos);

			y += 10;
		}
		else {
			y += BORDER*2;

			if (last_good != -1) {
				Uint32 end = good_end + 1000;
				Uint32 start = good_end - 1000;
				if (now >= start && now < end) {
					int t = now - start;
					float p = t / 2000.0f;
					if (p < 0.5f) {
						p = 0.0f;
					}
					else {
						p = (p - 0.5f) / 0.5f;
					}
					p = 1.0f - p;
					SDL_Colour colour = shim::palette[5];
					colour.r *= p;
					colour.g *= p;
					colour.b *= p;
					colour.a *= p;
					std::string cmbo;
					if (last_good >= (int)list->get_items().size()-1) {
						cmbo = konami;
					}
					else {
						cmbo = list->get_item(last_good);
					}
					DOGO_GLOBALS->bold_font->draw(colour, cmbo, util::Point<int>(shim::screen_size.w/2, y), true, true);
				}
			}

			y += DOGO_GLOBALS->bold_font->get_height() + 10 + shim::tile_size*1.5f;
			
			util::Point<int> player_pos(shim::screen_size.w/2+w/2-BORDER*4-shim::tile_size-player_img->size.w/2, y-player_img->size.h);
			SDL_Colour shadow_col = shim::white;
			shadow_col.r *= 0.15f;
			shadow_col.g *= 0.15f;
			shadow_col.b *= 0.15f;
			shadow_col.a *= 0.15f;
			draw_tinted_shadowed_image(shim::white, shadow_col, player_img, player_pos, gfx::Font::FULL_SHADOW);
			//player_img->draw(player_pos);

			y += 10;
		}

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

		shim::font->draw(shim::palette[36], GLOBALS->game_t->translate(162)/* Originally: Press */ + " " + button_text + " " + GLOBALS->game_t->translate(163)/* Originally: to exit */, util::Point<int>(shim::screen_size.w/2, y), true, true);

		shim::font->disable_shadow();
	}
}

void Practice_Menu_GUI::resize(util::Size<int> new_size)
{
	list->set_height(get_list_height(new_size));
	gui->layout();
}

int Practice_Menu_GUI::get_list_height(util::Size<int> screen_size)
{
	screen_size.h -= DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*4 + EDGE_Y*2;
	return screen_size.h;
}
	
void Practice_Menu_GUI::update()
{
	Dogo_GUI::update();

	int pressed;
	if ((pressed = list->pressed()) >= 0) {
		orig_anim = INSTANCE->stats[player_index].sprite->get_animation();
		INSTANCE->stats[player_index].sprite->set_animation("stand_w");

		practicing = true;
		GLOBALS->allow_global_mini_pause = false;
		GLOBALS->getting_combo = true;
		start_time = GET_TICKS();
		shim::convert_directions_to_focus_events = false;
		osc_enabled = GLOBALS->onscreen_controller_was_enabled;
//#if defined ANDROID || (defined IOS && !defined TVOS)
		if (input::is_joystick_connected() == false && input::system_has_keyboard() == false) {
/*#else
		if (false) { // if we find a way to check if keyboard connected, could check if no kb or joy connected here
#endif
*/
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
		gui->set_focus(NULL);

		combos.clear();
		combo_names.clear();

		std::string p = list->get_item(pressed);

		bool found = false;
		for (size_t i = 0; i < DOGO_INSTANCE->combos[player_index].size(); i++) {
			std::string name = DOGO_INSTANCE->combos[player_index][i];
			std::string t_name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(name));
			if (p == t_name) {
				found = true;
				Combo c = get_combo(name);
				combos.push_back(c);
				combo_names.push_back(name);
				break;
			}
		}
		if (found == false) {
			for (size_t i = 0; i < DOGO_INSTANCE->combos[player_index].size(); i++) {
				std::string name = DOGO_INSTANCE->combos[player_index][i];
				Combo c = get_combo(name);
				combos.push_back(c);
				combo_names.push_back(name);
			}

			Combo c;
			Combo_Event e;
			e.min_hold = 0;
			e.max_hold = 1000;
			e.button = B_U;
			c.push_back(e);
			e.button = B_U;
			c.push_back(e);
			e.button = B_D;
			c.push_back(e);
			e.button = B_D;
			c.push_back(e);
			e.button = B_L;
			c.push_back(e);
			e.button = B_R;
			c.push_back(e);
			e.button = B_L;
			c.push_back(e);
			e.button = B_R;
			c.push_back(e);
			e.button = B_B;
			c.push_back(e);
			e.button = B_A;
			c.push_back(e);
			combos.push_back(c);
		}

		detector = new Combo_Detector(combos);
	}

	if (practicing &&
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

	if (practicing) {
		detector->check();
		if (detector->error()) {
			last_good = -1;
			DOGO_GLOBALS->error->stop();
			Uint32 now = GET_TICKS();
			Uint32 elap = now - DOGO_GLOBALS->last_error_play;
			if ((int)elap >= 500) {
				DOGO_GLOBALS->last_error_play = now;
				DOGO_GLOBALS->error->play(false);
			}
			detector->reset();
		}
		else if (detector->good() != -1) {
			last_good = detector->good();
			DOGO_GLOBALS->error->stop();
			detector->reset();
			/*
			int max = 0;
			for (auto c : combos) {
				if ((int)c.size() > last_good+1) {
					int max_hold = c[last_good+1].max_hold;
					if (max_hold > max) {
						max = max_hold;
					}
				}
			}
			if (max == 0) {
				max = 500;
			}
			util::debugmsg("max=%d\n", max);
			good_end = GET_TICKS() + max;
			*/
			good_end = GET_TICKS() + 500;
			get_konami();
			std::string attack_name;
			if (last_good < (int)combo_names.size()) {
				attack_name = util::lowercase(combo_names[last_good]);
				int pos;
				if ((pos = attack_name.find('\'')) != std::string::npos) {
					attack_name = attack_name.substr(0, pos) + attack_name.substr(pos+1);
				}
				for (size_t i = 0; i < attack_name.length(); i++) {
					if (attack_name[i] == ' ') {
						attack_name[i] = '_';
					}
				}
			}
			INSTANCE->stats[player_index].sprite->set_animation(last_good >= (int)combo_names.size() ? "mouth_open" : std::string("attack_") + attack_name, practice_sprite_callback, INSTANCE->stats[player_index].sprite);
			sfx = last_good >= (int)combo_names.size() ? DOGO_GLOBALS->shock : get_combo_sfx(attack_name);
			if (sfx != nullptr) {
				sfx->play(false);
			}
			do_combo_screen_shake(attack_name);
		}
	}
}

void Practice_Menu_GUI::get_konami()
{
	int r = util::rand(133, 136);
	konami = GLOBALS->game_t->translate(r); // EXCELLENT! etc
}

void Practice_Menu_GUI::set_text()
{
	std::vector<std::string> names;
	std::vector<int> quantities;
	std::vector<std::string> descriptions;
	for (size_t i = 0; i < DOGO_INSTANCE->combos[player_index].size(); i++) {
		std::string combo = DOGO_INSTANCE->combos[player_index][i];
		std::string combo_t = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(combo));
		names.push_back(combo_t);
		quantities.push_back(get_combo_cost(combo));
	}
	names.push_back(GLOBALS->game_t->translate(520)/* Originally: Free Practice */);
	quantities.push_back(0xffffffff);
	list->set_selected(0);
	list->set_items_extra(quantities, names, descriptions);
}

bool Practice_Menu_GUI::is_practicing()
{
	return practicing;
}

void Practice_Menu_GUI::switch_chars()
{
	Multiplayer_Menu_GUI::switch_chars();
	list->set_top(0);
}

//--

Main_Menu_GUI::Main_Menu_GUI(int selected)
{
	// FIXME: translate
	menu_name = "|16[|08" + GLOBALS->game_t->translate(140)/* Originally: Menu */ + "|16]";

	TGUI_Widget *modal_main_widget = new TGUI_Widget(1.0f, 1.0f);
	modal_main_widget->set_padding_left(EDGE_X);
	modal_main_widget->set_padding_right(EDGE_X);
	modal_main_widget->set_padding_top(EDGE_Y);
	modal_main_widget->set_padding_bottom(EDGE_Y);

	/*
	TGUI_Widget *container = new TGUI_Widget(0.8f, 1.0f);
	container->set_centre_x(true);
	container->set_parent(modal_main_widget);
	*/

	buttons = new TGUI_Widget(TGUI_Widget::FIT_X, 1.0f);
	buttons->set_float_right(true);
	buttons->set_padding_right(5);
	buttons->set_padding_top(2);
	buttons->set_parent(modal_main_widget);

	items_button = new Widget_Text_Button(GLOBALS->game_t->translate(141)/* Originally: Items */);
	items_button->set_normal_shadow_colour(shim::palette[23]);
	items_button->set_focussed_shadow_colour(shim::palette[23]);
	items_button->set_parent(buttons);

	armour_button = new Widget_Text_Button(GLOBALS->game_t->translate(142)/* Originally: Belts */);
	armour_button->set_break_line(true);
	armour_button->set_normal_shadow_colour(shim::palette[23]);
	armour_button->set_focussed_shadow_colour(shim::palette[23]);
	armour_button->set_parent(buttons);
	
	stats_button = new Widget_Text_Button(GLOBALS->game_t->translate(148)/* Originally: Stats */);
	stats_button->set_break_line(true);
	stats_button->set_normal_shadow_colour(shim::palette[23]);
	stats_button->set_focussed_shadow_colour(shim::palette[23]);
	stats_button->set_parent(buttons);

	//if (easy_combos == false) {
		practice_button = new Widget_Text_Button(GLOBALS->game_t->translate(130)/* Originally: Practice */);
		practice_button->set_break_line(true);
		practice_button->set_normal_shadow_colour(shim::palette[23]);
		practice_button->set_focussed_shadow_colour(shim::palette[23]);
		practice_button->set_parent(buttons);
		if (easy_combos) {
			practice_button->set_enabled(false);
			practice_button->set_disabled_text_colour(shim::black);
		}
	//}
	//else {
		//practice_button = nullptr;
	//}

	#if 0
	progress_button = new Widget_Text_Button(GLOBALS->game_t->translate(183)/* Originally: Map */);
	progress_button->set_break_line(true);
	progress_button->set_normal_shadow_colour(shim::palette[23]);
	progress_button->set_focussed_shadow_colour(shim::palette[23]);
	progress_button->set_parent(buttons);
	#else
	progress_button = nullptr;
	#endif
	
	settings_button = new Widget_Text_Button(GLOBALS->game_t->translate(50)/* Originally: Settings */);
	settings_button->set_break_line(true);
	settings_button->set_normal_shadow_colour(shim::palette[23]);
	settings_button->set_focussed_shadow_colour(shim::palette[23]);
	settings_button->set_parent(buttons);
	
	save_button = new Widget_Text_Button(GLOBALS->game_t->translate(156)/* Originally: Save */);
	save_button->set_break_line(true);
	save_button->set_normal_shadow_colour(shim::palette[23]);
	save_button->set_focussed_shadow_colour(shim::palette[23]);
	if (INSTANCE->stats.size() > 1 && INSTANCE->party_following_player == false) {
		save_button->set_enabled(false);
		save_button->set_disabled_text_colour(shim::black);
	}
	save_button->set_parent(buttons);

	quit_button = new Widget_Text_Button(GLOBALS->game_t->translate(51)/* Originally: Quit */);
	quit_button->set_break_line(true);
	quit_button->set_normal_shadow_colour(shim::palette[23]);
	quit_button->set_focussed_shadow_colour(shim::palette[23]);
	quit_button->set_parent(buttons);

	// Wrap cursor
	items_button->set_up_widget(quit_button);
	quit_button->set_down_widget(items_button);
	//items_button->set_up_widget(save_button);
	//save_button->set_down_widget(items_button);

	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);

	switch (selected) {
		case 0:
			gui->set_focus(items_button);
			break;
		case 1:
			gui->set_focus(armour_button);
			break;
		case 2:
			gui->set_focus(stats_button);
			break;
		case 3:
			gui->set_focus(practice_button);
			break;
		#if 0
		case 4:
			gui->set_focus(progress_button);
			break;
		#endif
		case 5:
			gui->set_focus(settings_button);
			break;
		case 6:
			gui->set_focus(save_button);
			break;
		case 7:
			gui->set_focus(quit_button);
			//break;
	}
}

Main_Menu_GUI::~Main_Menu_GUI()
{
}
	
void Main_Menu_GUI::handle_event(TGUI_Event *event)
{
	Menu_GUI::handle_event(event);

	if (transitioning_in || transitioning_out) {
		return;
	}
	
	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
		AREA->end_menu();
		exit();
	}
}

void Main_Menu_GUI::draw_back()
{
	GUI::draw_back();

	gfx::draw_filled_rectangle(shim::black, util::Point<int>(0, 0), shim::screen_size);

	std::string menu_name = "[" + GLOBALS->game_t->translate(140)/* Originally: Menu */ + "]";

	int max_w = MAX(DOGO_GLOBALS->bold_font->get_text_width(menu_name), buttons->get_width()) + 10;

	draw_win(EDGE_X, EDGE_Y, shim::screen_size.w-EDGE_X*2-max_w, shim::screen_size.h-EDGE_Y*2);
	draw_win(EDGE_X+shim::screen_size.w-EDGE_X*2-max_w, EDGE_Y, max_w, shim::screen_size.h-EDGE_Y*2-DOGO_GLOBALS->coin->size.h-10);
	draw_win(EDGE_X+shim::screen_size.w-EDGE_X*2-max_w, EDGE_Y+shim::screen_size.h-EDGE_Y*2-DOGO_GLOBALS->coin->size.h-10, max_w, DOGO_GLOBALS->coin->size.h+10);

	DOGO_GLOBALS->coin->draw(util::Point<int>(shim::screen_size.w-EDGE_X-max_w+5, shim::screen_size.h-EDGE_Y-DOGO_GLOBALS->coin->size.h-5));

	shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
	std::string line = util::string_printf("%d", INSTANCE->get_gold());
	shim::font->draw(shim::palette[34], line, util::Point<int>(shim::screen_size.w-EDGE_X-max_w+5+DOGO_GLOBALS->coin->size.w+2, shim::screen_size.h-EDGE_Y-shim::font->get_height()-2));
	shim::font->disable_shadow();
}

void Main_Menu_GUI::draw()
{
	Menu_GUI::draw();

	const int h = 35;

	int y = EDGE_Y + 10;

	for (size_t i = 0; i < INSTANCE->stats.size(); i++) {
		wedge::Player_Stats &s = INSTANCE->stats[i];

		DOGO_GLOBALS->bold_font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);

		DOGO_GLOBALS->bold_font->draw(shim::white, s.base.get_name(), util::Point<int>(EDGE_X+5, y-5));
		
		DOGO_GLOBALS->bold_font->disable_shadow();

		gfx::Image *img = DOGO_GLOBALS->profile_pics[get_profile_pic_index(int(i))];
		util::Point<int> pos(EDGE_X+5, y+10);
		SDL_Colour shadow_col = shim::white;
		shadow_col.r *= 0.15f;
		shadow_col.g *= 0.15f;
		shadow_col.b *= 0.15f;
		shadow_col.a *= 0.15f;
		draw_tinted_shadowed_image(shim::white, shadow_col, img, pos, gfx::Font::FULL_SHADOW);

		shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);

		std::string line1 = util::string_printf("%s: %d/%d", GLOBALS->game_t->translate(144)/* Originally: HP */.c_str(), s.base.hp, s.base.fixed.max_hp);
		shim::font->draw(shim::palette[34], line1, util::Point<int>(EDGE_X+5+img->size.w+5, y+6));
		
		std::string line2 = util::string_printf("%s: %d/%d", GLOBALS->game_t->translate(145)/* Originally: SP */.c_str(), s.base.mp, s.base.fixed.max_mp);
		shim::font->draw(shim::palette[34], line2, util::Point<int>(EDGE_X+5+img->size.w+5, y+shim::font->get_height()+6));

		shim::font->disable_shadow();

		y += h;
	}
}

void Main_Menu_GUI::update()
{
	Dogo_GUI::update();

	if (practice_button != nullptr && practice_button->pressed()) {
		Practice_Menu_GUI *gui = new Practice_Menu_GUI();
		shim::guis.push_back(gui);
		exit();
	}
	else if (stats_button->pressed()) {
		Stats_Menu_GUI *gui = new Stats_Menu_GUI();
		shim::guis.push_back(gui);
		exit();
	}
	else if (items_button->pressed()) {
		Items_Menu_GUI *gui = new Items_Menu_GUI();
		shim::guis.push_back(gui);
		exit();
	}
	#if 0
	else if (progress_button->pressed()) {
		Map_Menu_GUI *gui = new Map_Menu_GUI();
		shim::guis.push_back(gui);
		exit();
	}
	#endif
	else if (armour_button->pressed()) {
		Armour_Menu_GUI *gui = new Armour_Menu_GUI();
		shim::guis.push_back(gui);
		exit();
	}
	else if (save_button->pressed()) {
		Save_Slot_GUI *gui = new Save_Slot_GUI(false, DOGO_GLOBALS->save_slot);
		shim::guis.push_back(gui);
		exit();
	}
	else if (quit_button->pressed()) {
		GLOBALS->mini_pause();
	}
	else if (settings_button->pressed()) {
		Settings_GUI *gui = new Settings_GUI(true);
		shim::guis.push_back(gui);
		exit();
	}
}

//--

Stats_Menu_GUI::Stats_Menu_GUI()
{
	// FIXME: translate
	menu_name = "|16[|08" + GLOBALS->game_t->translate(148)/* Originally: Stats */ + "|16]";

	TGUI_Widget *modal_main_widget = new TGUI_Widget(1.0f, 1.0f);
	modal_main_widget->set_padding_left(EDGE_X);
	modal_main_widget->set_padding_right(EDGE_X);
	modal_main_widget->set_padding_top(EDGE_Y);
	modal_main_widget->set_padding_bottom(EDGE_Y);

	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);

	set_text();
}

Stats_Menu_GUI::~Stats_Menu_GUI()
{
}

void Stats_Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}

	Multiplayer_Menu_GUI::handle_event(event);

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
		DOGO_GLOBALS->button->play(false);
		Main_Menu_GUI *gui = new Main_Menu_GUI(2);
		shim::guis.push_back(gui);
		exit();
	}
}

void Stats_Menu_GUI::draw()
{
	Multiplayer_Menu_GUI::draw();

	const int ADD_X = 8;
	
	int y = EDGE_Y + DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*3;
	
	shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);

	wedge::Player_Stats &s = INSTANCE->stats[player_index];

	std::string line1 = util::string_printf("%s: ", GLOBALS->game_t->translate(144)/* Originally: HP */.c_str());
	int hpw = shim::font->get_text_width(line1);
	draw_gauge(EDGE_X+ADD_X+hpw, y+1, 100, s.base.hp/(float)s.base.fixed.max_hp*100);
	std::string line2 = util::string_printf("%d/%d", s.base.hp, s.base.fixed.max_hp);
	shim::font->draw(shim::palette[34], line1, util::Point<int>(EDGE_X+ADD_X, y+2));
	shim::font->draw(shim::palette[34], line2, util::Point<int>(EDGE_X+ADD_X+hpw+50-shim::font->get_text_width(line2)/2, y+4));

	y += 15;
	
	std::string line3 = util::string_printf("%s: ", GLOBALS->game_t->translate(145)/* Originally: SP */.c_str());
	int spw = shim::font->get_text_width(line3);
	draw_gauge(EDGE_X+ADD_X+spw, y+1, 100, s.base.mp/(float)s.base.fixed.max_mp*100);
	std::string line4 = util::string_printf("%d/%d", s.base.mp, s.base.fixed.max_mp);
	shim::font->draw(shim::palette[34], line3, util::Point<int>(EDGE_X+ADD_X, y+2));
	shim::font->draw(shim::palette[34], line4, util::Point<int>(EDGE_X+ADD_X+spw+50-shim::font->get_text_width(line4)/2, y+4));

	y += 17;

	// First draw the labels

	std::string lab1 = GLOBALS->game_t->translate(146)/* Originally: ATK */ + ": ";
	std::string lab2 = GLOBALS->game_t->translate(147)/* Originally: DEF */ + ": ";
	std::string lab3 = GLOBALS->game_t->translate(362)/* Originally: LUK */ + ": ";

	shim::font->draw(shim::palette[34], lab1, util::Point<int>(EDGE_X+ADD_X, y));
	shim::font->draw(shim::palette[34], lab2, util::Point<int>(EDGE_X+ADD_X, y+15));
	shim::font->draw(shim::palette[34], lab3, util::Point<int>(EDGE_X+ADD_X, y+30));

	// Now draw the numbers using the max width of the above 3

	int two = shim::font->get_text_width(lab2);
	int max = MAX(shim::font->get_text_width(lab1), two);
	max = MAX(two, shim::font->get_text_width(lab3));

	shim::font->draw(shim::palette[34], util::itos(s.base.fixed.attack), util::Point<int>(EDGE_X+ADD_X+max, y));
	shim::font->draw(shim::palette[34], util::itos(s.base.fixed.defence + s.armour.stats.defence), util::Point<int>(EDGE_X+ADD_X+max, y+15));
	shim::font->draw(shim::palette[34], util::itos(s.base.fixed.get_extra(LUCK)), util::Point<int>(EDGE_X+ADD_X+max, y+30));

	shim::font->disable_shadow();
}

void Stats_Menu_GUI::set_text()
{
}

void Stats_Menu_GUI::draw_gauge(int x, int y, int width, int full)
{
	gfx::draw_filled_rectangle(shim::black, util::Point<int>(x, y), util::Size<int>(width+2, 4));

	float p = full / (float)width;

	SDL_Colour colour1;
	SDL_Colour colour2;

	get_gauge_colour(p, colour1, colour2);

	gfx::draw_filled_rectangle(colour1, util::Point<int>(x+1, y+1), util::Size<int>(full, 1));
	gfx::draw_filled_rectangle(colour2, util::Point<int>(x+1, y+2), util::Size<int>(full, 1));
}

//--

Items_Menu_GUI::Items_Menu_GUI()
{
	draw_name = false;

	// FIXME: translate
	menu_name = "|16[|08" + GLOBALS->game_t->translate(141)/* Originally: Items */ + "|16]";

	TGUI_Widget *modal_main_widget = new TGUI_Widget(1.0f, 1.0f);
	modal_main_widget->set_padding_left(EDGE_X);
	modal_main_widget->set_padding_right(EDGE_X);
	modal_main_widget->set_padding_top(EDGE_Y);
	modal_main_widget->set_padding_bottom(EDGE_Y);

	container = new TGUI_Widget(0.8f, 1.0f);
	container->set_centre_x(true);
	container->set_parent(modal_main_widget);

	list = new Widget_Quantity_List(1.0f, get_list_height(shim::screen_size));
	list->set_padding_top(DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*3);
	list->set_text_shadow_colour(shim::palette[23]);
	list->set_parent(container);

	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);

	set_text();
}

Items_Menu_GUI::~Items_Menu_GUI()
{
}

void Items_Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}

	Multiplayer_Menu_GUI::handle_event(event);

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
		DOGO_GLOBALS->button->play(false);
		Main_Menu_GUI *gui = new Main_Menu_GUI(0);
		shim::guis.push_back(gui);
		cancel_all_special_numbers(MENU);
		exit();
	}
}

void Items_Menu_GUI::draw()
{
	Multiplayer_Menu_GUI::draw();

	wedge::Player_Stats &s = INSTANCE->stats[player_index];

	std::string str = GLOBALS->game_t->translate(144)/* Originally: HP */ + ": " + util::itos(s.base.hp) + "/" + util::itos(s.base.fixed.max_hp) + "  " + GLOBALS->game_t->translate(145)/* Originally: SP */ + ": " + util::itos(s.base.mp) + "/" + util::itos(s.base.fixed.max_mp);

	shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
	shim::font->draw(shim::white, str, util::Point<int>(DOGO_GLOBALS->profile_pics[0]->size.w + 3 + BORDER + EDGE_X, DOGO_GLOBALS->profile_pics[0]->size.h/2 + BORDER + EDGE_Y - shim::font->get_height()/2));
	shim::font->disable_shadow();
}

void Items_Menu_GUI::resize(util::Size<int> new_size)
{
	list->set_height(get_list_height(new_size));
	gui->layout();
}

int Items_Menu_GUI::get_list_height(util::Size<int> screen_size)
{
	screen_size.h -= DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*4 + EDGE_Y*2;
	return screen_size.h;
}
	
void Items_Menu_GUI::update()
{
	Dogo_GUI::update();

	if (transitioning_in || transitioning_out) {
		return;
	}

	if (list != NULL) {
		if (INSTANCE->is_milestone_complete(MS_ITEM_DESC_HELP) == false) {
			INSTANCE->set_milestone_complete(MS_ITEM_DESC_HELP, true);
			Notification_GUI *notification_gui = new Notification_GUI(GLOBALS->game_t->translate(152)/* Originally: Press and hold for item descriptions. */);
			shim::guis.push_back(notification_gui);
		}
	}

	if (list != NULL) {
		int pressed;
		if ((pressed = list->pressed()) >= 0) {
			// set and reset this (reset because do_dialogue might not be called) in case it's a scroll
			int index = inventory_indices[pressed];
			int id = INSTANCE->inventory.get_all()[index].id;
			if (for_use_in_battle(id)) {
				gfx::add_notification(GLOBALS->game_t->translate(245)/* Originally: For use in battle! */);
			}
			else {
				int amount = INSTANCE->inventory.use(index, &INSTANCE->stats[player_index].base);
				if (amount != 0 || id >= 0) {
					SDL_Colour colour, shadow_colour;
					std::string text;
					get_use_item_info(amount, id, colour, shadow_colour, text);
					if (colour.a != 0 || shadow_colour.a != 0) {
						util::Point<int> number_pos = util::Point<int>(EDGE_X+DOGO_GLOBALS->profile_pics[0]->size.w+2, EDGE_Y+BORDER);
						NEW_SYSTEM_AND_TASK(MENU)
						wedge::Special_Number_Step *step = new wedge::Special_Number_Step(colour, shadow_colour, text, number_pos, wedge::Special_Number_Step::RISE, new_task, true);
						ADD_STEP(step)
						ADD_TASK(new_task)
						FINISH_SYSTEM(MENU)
					}
				}
				set_text();
			}
		}
	}
}

void Items_Menu_GUI::set_text()
{
	if (list == NULL) {
		return;
	}

	inventory_indices.clear();

	std::vector<int> quantities;
	std::vector<std::string> names;
	std::vector<std::string> descriptions;
	std::vector<bool> disabled;

	wedge::Object *inv = INSTANCE->inventory.get_all();

	for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
		if (inv[i].type == wedge::OBJECT_ITEM || inv[i].type == wedge::OBJECT_SPECIAL) {
			inventory_indices.push_back(i);
			quantities.push_back(inv[i].quantity);
			names.push_back(inv[i].name);
			descriptions.push_back(inv[i].description);
			if (for_use_in_battle(inv[i].id) || inv[i].type == wedge::OBJECT_SPECIAL) {
				disabled.push_back(true);
			}
			else {
				disabled.push_back(false);
			}
		}
	}

	if (names.size() == 0) {
		list->set_parent(NULL);
		delete list;
		list = NULL;
		if (gui != NULL) {
			gui->focus_something();
		}
		Widget_Label *empty_label = new Widget_Label(GLOBALS->game_t->translate(153)/* Originally: No items */, -1);
		empty_label->set_shadow_colour(shim::palette[23]);
		empty_label->set_padding_top(DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*3);
		empty_label->set_centre_x(true);
		empty_label->set_break_line(true);
		empty_label->set_parent(container);
		if (gui) {
			gui->layout();
		}
	}
	else {
		int sel = list->get_selected();
		if (sel >= (int)names.size()) {
			sel--;
			list->set_selected(sel);
		}
		list->set_items_extra(quantities, names, descriptions);

		for (size_t i = 0; i < names.size(); i++) {
			list->set_disabled((int)i, disabled[i]);
		}
	}
}

//--

Map_Menu_GUI::Map_Menu_GUI()
{
	// FIXME: translate
	menu_name = "|16[|08" + GLOBALS->game_t->translate(143)/* Originally: Map */ + "|16]";

	TGUI_Widget *modal_main_widget = new TGUI_Widget(1.0f, 1.0f);
	modal_main_widget->set_padding_left(EDGE_X);
	modal_main_widget->set_padding_right(EDGE_X);
	modal_main_widget->set_padding_top(EDGE_Y);
	modal_main_widget->set_padding_bottom(EDGE_Y);

	map_widget = new Widget_Map();
	map_widget->set_centre_x(true);
	map_widget->set_padding_top((shim::screen_size.h/2-EDGE_Y*2)/2-(shim::font->get_height()*2+5)/2);
	map_widget->set_clear_float_x(true);
	map_widget->set_break_line(true);
	map_widget->set_parent(modal_main_widget);

	found_device();
	
	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);
}

Map_Menu_GUI::~Map_Menu_GUI()
{
}
	
void Map_Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}

	Menu_GUI::handle_event(event);

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
		DOGO_GLOBALS->button->play(false);
		Main_Menu_GUI *gui = new Main_Menu_GUI(4);
		shim::guis.push_back(gui);
		exit();
	}
}

void Map_Menu_GUI::draw_back()
{
	GUI::draw_back();

	draw_win(EDGE_X, EDGE_Y, shim::screen_size.w-EDGE_X*2, shim::screen_size.h-EDGE_Y*2);
}

void Map_Menu_GUI::lost_device()
{
	delete map;
	delete map2;
	map = NULL;
	map2 = NULL;
	map_widget->set_images(NULL, NULL);
}

void Map_Menu_GUI::found_device()
{
	gfx::Font::end_batches();

	int w = shim::screen_size.w * 0.5f;
	int h = shim::screen_size.h * 0.5f;

	map = new gfx::Image(util::Size<int>(w, h+5+shim::font->get_height()*2));
	map2 = new gfx::Image(util::Size<int>(w, h+5+shim::font->get_height()*2));

	gfx::set_target_image(map);

	draw_map(w, h, false);

	set_target_image(map2);

	draw_map(w, h, true);

	gfx::set_target_backbuffer();

	map_widget->set_images(map, map2);
}

void Map_Menu_GUI::draw_map(int w, int h, bool draw_portals)
{
	gfx::clear(shim::transparent);

	wedge::Area *area = AREA->get_current_area();
	std::string name = area->get_name();

	if (name == "FIXME_NO_MAP") {
		shim::font->draw(shim::white, "?", util::Point<float>(w/2.0f, h/2.0f-shim::font->get_height()/2.0f), false, true);
		gfx::Font::end_batches();
	}
	else {
		gfx::Tilemap *tilemap = area->get_tilemap();

		wedge::Map_Entity *player1 = AREA->get_player(0);
		util::Point<int> player_pos = player1->get_position();

		wedge::Area_Hooks *hooks = area->get_hooks();

		gfx::draw_primitives_start();

		for (int y = 0; y < h; y ++) {
			int yy = y - h/2 + player_pos.y;
			if (yy < 0 || yy >= tilemap->get_size().h) {
				continue;
			}
			int dy = MIN(y, h-y-1);
			for (int x = 0; x < w; x++) {
				int xx = x - w/2 + player_pos.x;
				if (xx < 0 || xx >= tilemap->get_size().w) {
					continue;
				}
				int dx = MIN(x, w-x-1);
				int d = MIN(dx, dy);
				util::Point<int> pos(xx, yy);
				if (draw_portals && hooks != NULL && (hooks->get_fade_zone(pos) || hooks->get_scroll_zones(pos).size() > 0)) {
					SDL_Colour c = shim::magenta;
					if (d <= 8) {
						float p = d / 8.0f;
						c.r *= p;
						c.g *= p;
						c.b *= p;
						c.a *= p;
					}
					gfx::draw_filled_rectangle(c, util::Point<int>(x, y), util::Size<int>(1, 1));
				}
				else if (draw_portals == false && tilemap->is_solid(-1, pos)) {
					SDL_Colour c = shim::white;
					if (d <= 8) {
						float p = d / 8.0f;
						c.r *= p;
						c.g *= p;
						c.b *= p;
						c.a *= p;
					}
					gfx::draw_filled_rectangle(c, util::Point<int>(x, y), util::Size<int>(1, 1));
				}
			}
		}
	
		if (draw_portals == false) {
			gfx::draw_filled_rectangle(shim::palette[8]/*yellow*/, util::Point<int>(w/2, h/2), util::Size<int>(1, 1));
			
		}
		
		gfx::draw_primitives_end();
	}

	std::string text1 = GLOBALS->game_t->translate(15)/* Pleasant */;
	std::string text2 = GLOBALS->game_t->translate(184)/* Originally: Enter/Exit */;
	int w1 = shim::font->get_text_width(text1);
	int w2 = shim::font->get_text_width(text2);
	int box_size = shim::font->get_height() * 0.5f;

	shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
	shim::font->draw(shim::white, text1, util::Point<int>(w/2-(w1+box_size+2)/2+box_size+2, h+5));
	shim::font->draw(shim::white, text2, util::Point<int>(w/2-(w2+box_size+2)/2+box_size+2, h+5+shim::font->get_height()));
	shim::font->disable_shadow();

	gfx::draw_filled_rectangle(shim::palette[23], util::Point<int>(w/2-(w1+box_size+2)/2, h+5+shim::font->get_height()/2-box_size/2), util::Size<int>(box_size+1, box_size+1));
	gfx::draw_filled_rectangle(shim::palette[8], util::Point<int>(w/2-(w1+box_size+2)/2, h+5+shim::font->get_height()/2-box_size/2), util::Size<int>(box_size, box_size));

	gfx::draw_filled_rectangle(shim::palette[23], util::Point<int>(w/2-(w2+box_size+2)/2, h+5+shim::font->get_height()+shim::font->get_height()/2-box_size/2), util::Size<int>(box_size+1, box_size+1));
	gfx::draw_filled_rectangle(shim::magenta, util::Point<int>(w/2-(w2+box_size+2)/2, h+5+shim::font->get_height()+shim::font->get_height()/2-box_size/2), util::Size<int>(box_size, box_size));
		
	gfx::Font::end_batches();
}

//--

Armour_Menu_GUI::Armour_Menu_GUI()
{
	draw_name = false;

	// FIXME: translate
	menu_name = "|16[|08" + GLOBALS->game_t->translate(142)/* Originally: Belts */ + "|16]";

	TGUI_Widget *modal_main_widget = new TGUI_Widget(1.0f, 1.0f);
	modal_main_widget->set_padding_left(EDGE_X);
	modal_main_widget->set_padding_right(EDGE_X);
	modal_main_widget->set_padding_top(EDGE_Y);
	modal_main_widget->set_padding_bottom(EDGE_Y);

	container = new TGUI_Widget(0.8f, 1.0f);
	container->set_centre_x(true);
	container->set_parent(modal_main_widget);

	list = new Widget_Quantity_List(1.0f, get_list_height(shim::screen_size));
	list->set_padding_top(DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*3);
	list->set_text_shadow_colour(shim::palette[23]);
	list->set_hide_quantities(true);
	list->set_parent(container);

	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);
	
	set_text();
}

Armour_Menu_GUI::~Armour_Menu_GUI()
{
}

void Armour_Menu_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}

	Multiplayer_Menu_GUI::handle_event(event);

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
		DOGO_GLOBALS->button->play(false);
		Main_Menu_GUI *gui = new Main_Menu_GUI(1);
		shim::guis.push_back(gui);
		exit();
	}
}

void Armour_Menu_GUI::draw()
{
	Multiplayer_Menu_GUI::draw();

	wedge::Player_Stats &s = INSTANCE->stats[player_index];

	int attack = s.base.fixed.attack + s.armour.stats.attack;
	int defence = s.base.fixed.defence + s.armour.stats.defence;
	
	int would_be_attack = attack;
	int would_be_defence = defence;
	
	wedge::Object *inv = INSTANCE->inventory.get_all();

	if (list != NULL) {
		wedge::Armour_Stats curr = wedge::Armour_Stats(inv[inventory_indices[list->get_selected()]].id);

		would_be_attack = s.base.fixed.attack + curr.stats.attack;
		would_be_defence = s.base.fixed.defence + curr.stats.defence;
	}

	std::string atk_change;
	std::string def_change;

	if (would_be_attack > attack) {
		atk_change = "|05@02|21";
	}
	else if (would_be_attack < attack) {
		atk_change = "|0D@03|21";
	}

	if (would_be_defence > defence) {
		def_change = "|05@02|21";
	}
	else if (would_be_defence < defence) {
		def_change = "|0D@03|21";
	}

	std::string str = GLOBALS->game_t->translate(146)/* Originally: ATK */ + ": " + util::itos(attack) + atk_change + "  " + GLOBALS->game_t->translate(147)/* Originally: DEF */ + ": " + util::itos(defence) + def_change;

	shim::font->enable_shadow(shim::palette[23], gfx::Font::DROP_SHADOW);
	shim::font->draw(shim::white, str, util::Point<int>(DOGO_GLOBALS->profile_pics[0]->size.w + 3 + BORDER + EDGE_X, DOGO_GLOBALS->profile_pics[0]->size.h/2 + BORDER + EDGE_Y - shim::font->get_height()/2));
	shim::font->disable_shadow();
}

void Armour_Menu_GUI::resize(util::Size<int> new_size)
{
	list->set_height(get_list_height(new_size));
	gui->layout();
}

int Armour_Menu_GUI::get_list_height(util::Size<int> screen_size)
{
	screen_size.h -= DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*4 + EDGE_Y*2;
	return screen_size.h;
}
	
void Armour_Menu_GUI::update()
{
	Dogo_GUI::update();

	if (transitioning_in || transitioning_out) {
		return;
	}

	if (list != NULL) {
		int pressed;
		if ((pressed = list->pressed()) >= 0) {
			wedge::Object *inv = INSTANCE->inventory.get_all();
			std::string player_name = INSTANCE->stats[player_index].base.get_name();
			
			// Remove current belt
			if (INSTANCE->stats[player_index].armour.id != wedge::ARMOUR_NONE) {
				std::vector<std::string> belt_moves = get_belt_moves(player_name, INSTANCE->stats[player_index].armour.id);
				remove_belt(player_index, belt_moves);
				INSTANCE->stats[player_index].armour = wedge::Armour_Stats(wedge::ARMOUR_NONE);
#if 0
				INSTANCE->stats[player_index].armour_index = -1;
#endif
			}

			if (list->is_highlighted(pressed) == false) {
				int id = inv[inventory_indices[pressed]].id;
				INSTANCE->stats[player_index].armour = wedge::Armour_Stats(id);
#if 0
				for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
					if (inv[i].type == wedge::OBJECT_ARMOUR && inv[i].id == id) {
						bool found = false;
						for (size_t j = 0; j < INSTANCE->stats.size(); j++) {
							if (j == player_index) {
								continue;
							}
							if (INSTANCE->stats[j].armour_index == i) {
								found = true;
								break;
							}
						}
						if (found == false) {
							INSTANCE->stats[player_index].armour_index = i;
							break;
						}
					}
				}
#endif
				std::vector<std::string> belt_moves = get_belt_moves(player_name, INSTANCE->stats[player_index].armour.id);
				equip_belt(belt_moves);
			}

			set_text();
		}
	}
}

void Armour_Menu_GUI::set_text()
{
	if (list == NULL) {
		return;
	}

	inventory_indices.clear();

	std::vector<int> quantities;
	std::vector<std::string> names;
	std::vector<std::string> descriptions;

	wedge::Object *inv = INSTANCE->inventory.get_all();

	for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
		if (inv[i].type == wedge::OBJECT_ARMOUR) {
			if (std::find(names.begin(), names.end(), inv[i].name) == names.end()) {
				inventory_indices.push_back(i);
				quantities.push_back(inv[i].quantity);
				names.push_back(inv[i].name);
			}
		}
	}
	
	std::vector<int> tmp_i;
	std::vector<int> tmp_q;
	std::vector<std::string> tmp_n;

	// Sort them in Karate order
	push_belt("White Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Yellow Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Gold Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Orange Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Green Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Blue Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Purple Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Brown Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Red Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);
	push_belt("Black Belt", tmp_i, tmp_q, tmp_n, inventory_indices, quantities, names);

	inventory_indices = tmp_i;
	quantities = tmp_q;
	names = tmp_n;

	if (names.size() == 0) {
		list->set_parent(NULL);
		delete list;
		list = NULL;
		if (gui != NULL) {
			gui->focus_something();
		}
		Widget_Label *empty_label = new Widget_Label(GLOBALS->game_t->translate(161)/* Originally: No belts */, -1);
		empty_label->set_shadow_colour(shim::palette[23]);
		empty_label->set_padding_top(DOGO_GLOBALS->profile_pics[0]->size.h + BORDER*3);
		empty_label->set_centre_x(true);
		empty_label->set_break_line(true);
		empty_label->set_parent(container);
		if (gui) {
			gui->layout();
		}
	}
	else {
		int sel = list->get_selected();
		if (sel >= (int)names.size()) {
			sel--;
			list->set_selected(sel);
		}
		list->set_items_extra(quantities, names, descriptions);

		for (size_t i = 0; i < names.size(); i++) {
			list->set_disabled((int)i, false);
			list->set_highlight((int)i, false);
		}

		for (size_t i = 0; i < inventory_indices.size(); i++) {
			int id = inv[inventory_indices[i]].id;
			bool highlighted = INSTANCE->stats[player_index].armour.id == id;
			bool disabled;
			if (highlighted) {
				disabled = false;
			}
			else {
				int count = 0;
				for (int j = 0; j < wedge::Inventory::MAX_OBJECTS; j++) {
					if (inv[j].type == wedge::OBJECT_ARMOUR && inv[j].id == id) {
						count++;
					}
				}
				int equipped = 0;
				for (size_t j = 0; j < INSTANCE->stats.size(); j++) {
					if (INSTANCE->stats[j].armour.id == id) {
						equipped++;
					}
				}
				if (equipped >= count) {
					disabled = true;
				}
				else {
					disabled = false;
				}
			}

			if (highlighted) {
				list->set_highlight((int)i, true);
			}
			else if (disabled) {
				list->set_disabled((int)i, true);
			}
		}
	}
}

void Armour_Menu_GUI::equip_belt(std::vector<std::string> moves)
{
	std::vector<std::string> &v = DOGO_INSTANCE->combos[player_index];

	v.insert(v.end(), moves.begin(), moves.end());
}

void Armour_Menu_GUI::push_belt(std::string belt, std::vector<int> &i, std::vector<int> &q, std::vector<std::string> &n, std::vector<int> i2, std::vector<int> q2, std::vector<std::string> n2)
{
	for (size_t j = 0; j < n2.size(); j++) {
		if (n2[j] == GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(belt))) {
			i.push_back(i2[j]);
			q.push_back(q2[j]);
			n.push_back(n2[j]);
		}
	}
}
