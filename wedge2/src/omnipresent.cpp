#include "wedge2/globals.h"
#include "wedge2/omnipresent.h"

using namespace wedge;

namespace wedge {

Omnipresent_Game::Omnipresent_Game() :
	fading(false),
	quit_game(false),
	triangle_pressed(false),
	x_pressed(false)
{
	hide_red_triangle = GLOBALS->no_red_triangle;
}

Omnipresent_Game::~Omnipresent_Game()
{
}
	
void Omnipresent_Game::draw_fore()
{
	for (size_t i = 0; i < draw_fore_hooks.size(); i++) {
		draw_fore_hooks[i]->draw_fore();
	}
	draw_fore_hooks.clear();

	gfx::Font::end_batches();

	if (fading) {
		Uint32 now = GET_TICKS();
		Uint32 end = fade_start + fade_delay + fade_duration;
		Uint32 diff;
		if (now > end) {
			diff = 0;
		}
		else {
			diff = end-now;
		}
		if ((int)diff <= fade_duration) {
			float p = 1.0f - ((float)diff / fade_duration);
			SDL_Colour colour;
			colour.r = fade_colour.r * p;
			colour.g = fade_colour.g * p;
			colour.b = fade_colour.b * p;
			colour.a = fade_colour.a * p;
			gfx::draw_filled_rectangle(colour, util::Point<int>(0, 0), shim::screen_size);
		}
	}

	for (size_t i = 0; i < draw_last_hooks.size(); i++) {
		draw_last_hooks[i]->draw_fore();
	}
	draw_last_hooks.clear();
}

void Omnipresent_Game::draw_controls()
{
	bool draw_controls = buttons_enabled();

	if (draw_controls && hide_red_triangle == false) {
		SDL_Colour colour = shim::white;
		colour.r *= GLOBALS->onscreen_controls_alpha;
		colour.g *= GLOBALS->onscreen_controls_alpha;
		colour.b *= GLOBALS->onscreen_controls_alpha;
		colour.a *= GLOBALS->onscreen_controls_alpha;

		if (GLOBALS->should_show_back_arrow()) {
			GLOBALS->red_triangle->get_image(triangle_pressed ? 1 : 0)->draw_tinted(colour, {0, 0});
		}

		if (GLOBALS->allow_global_mini_pause == true && GLOBALS->instance != nullptr && GLOBALS->is_mini_paused() == false) {
			auto img = GLOBALS->red_x->get_image(x_pressed ? 1 : 0);
			img->draw_tinted(colour, {float(shim::screen_size.w-img->size.w), 0.0f});
		}
	}
}

bool Omnipresent_Game::run()
{
	hide_red_triangle = hide_red_triangle || GLOBALS->no_red_triangle;

	if (quit_game) {
		quit_game = false;
		GLOBALS->quit(true);
	}
	return true;
}

void Omnipresent_Game::start_fade(SDL_Colour colour, int delay, int duration)
{
	fade_colour = colour;
	fade_delay = delay;
	fade_duration = duration;
	fading = true;
	fade_start = GET_TICKS();
}

void Omnipresent_Game::end_fade()
{
	fading = false;
}

void Omnipresent_Game::hook_draw_fore(Step *step)
{
	draw_fore_hooks.push_back(step);
}

void Omnipresent_Game::hook_draw_last(Step *step)
{
	draw_last_hooks.push_back(step);
}

void Omnipresent_Game::set_hide_red_triangle(bool hide)
{
	hide_red_triangle = hide;
}

void Omnipresent_Game::set_quit_game(bool quit)
{
	quit_game = quit;
}

void Omnipresent_Game::set_triangle_pressed(bool pressed)
{
	bool process_event = buttons_enabled();

	// pump escape key events when top left triangle clicked
	if (process_event && hide_red_triangle == false && pressed == true && triangle_pressed == false) {
		if (GLOBALS->should_show_back_arrow()) {
			TGUI_Event e;
			e.type = TGUI_KEY_DOWN;
			if (GLOBALS->getting_combo) {
				e.keyboard.code = GLOBALS->key_cancel;
			}
			else if (shim::guis.size() == 0 && BATTLE == nullptr) {
				e.keyboard.code = GLOBALS->key_menu;
			}
			else {
				e.keyboard.code = GLOBALS->key_back;
			}
			e.keyboard.is_repeat = false;
			e.keyboard.simulated = true;
			shim::push_event(e);
			e.type = TGUI_KEY_UP;
			shim::push_event(e);
		}
	}
	triangle_pressed = pressed;
}

void Omnipresent_Game::set_x_pressed(bool pressed)
{
	bool process_event = buttons_enabled();

	if (process_event && hide_red_triangle == false && pressed == true && x_pressed == false) {
		if (GLOBALS->allow_global_mini_pause == true && GLOBALS->instance != nullptr && GLOBALS->is_mini_paused() == false) {
			TGUI_Event e;
			e.type = TGUI_KEY_DOWN;
			e.keyboard.code = PAUSE_VAL;
			e.keyboard.is_repeat = false;
			e.keyboard.simulated = true;
			shim::push_event(e);
			e.type = TGUI_KEY_UP;
			shim::push_event(e);
		}
	}
	x_pressed = pressed;
}

bool Omnipresent_Game::buttons_enabled()
{
	bool enabled = true;

#if !defined ANDROID && !(defined IOS && !defined TVOS)
	if (is_onscreen_controller_enabled() == false && (input::system_has_keyboard() || input::is_joystick_connected())) {
		enabled = false;
	}
#endif

	return enabled;
}

}
