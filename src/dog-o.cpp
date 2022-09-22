#include <wedge2/main.h>
#include <wedge2/globals.h>
#include <wedge2/onscreen_controller.h>

#include "battle_player.h"
#include "enemies.h"
#include "general.h"
#include "globals.h"
#include "gui.h"
#include "widgets.h"

int orig_argc;
char **orig_argv;

std::string extra_args;
std::string extra_args_orig;

float sfx_volume = 1.0f;
float music_volume = 1.0f;
bool tv_safe_mode = false;
int windowed = -1;
int fullscreen = -1;
int screen_w = -1;
int screen_h = -1;
bool autosave_help_shown = false;
bool easy_combos = true;
bool simple_turn_display = false;

#ifdef GOOGLE_PLAY_XXX
#include <jni.h>

void start_google_play_games_services()
{
	JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
	jobject activity = (jobject)SDL_AndroidGetActivity();
	jclass clazz(env->GetObjectClass(activity));

	jmethodID method_id = env->GetMethodID(clazz, "start_google_play_games_services", "()V");

	env->CallVoidMethod(activity, method_id);

	env->DeleteLocalRef(activity);
	env->DeleteLocalRef(clazz);
}
#endif

int main(int argc, char **argv)
{
	try {
#ifdef _WIN32
		SDL_RegisterApp("Dog-O", 0, 0);
#endif

#if defined IOS
		SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight Portrait PortraitUpsideDown");
#endif

		orig_argc = argc;
		orig_argv = argv;

		// this must be before static_start which inits SDL
#ifdef _WIN32
		bool directsound = util::bool_arg(true, argc, argv, "directsound");
		bool wasapi = util::bool_arg(false, argc, argv, "wasapi");
		bool winmm = util::bool_arg(false, argc, argv, "winmm");

		if (directsound) {
			_putenv_s("SDL_AUDIODRIVER", "directsound");
		}
		else if (wasapi) {
			_putenv_s("SDL_AUDIODRIVER", "wasapi");
		}
		else if (winmm) {
			_putenv_s("SDL_AUDIODRIVER", "winmm");
		}
#endif

		shim::window_title = "Dog-O";
		shim::organisation_name = "Nooskewl";
		shim::game_name = "Dog-O";
		//
		shim::logging = true;

		// Need to be set temporarily to original values so +error-level works. They get changed below
		shim::argc = orig_argc;
		shim::argv = orig_argv;
	
		shim::static_start_all();

		shim::create_depth_buffer = true;

		start_autosave_thread();
		
		bool settings_loaded = load_settings();

		// This can happen if you use a large external monitor and switch to a smaller screen with lower resolution
		// If this happens, use the default res.
		auto modes = gfx::get_supported_video_modes();
		int max_w = 0;
		int max_h = 0;
		for (auto m : modes) {
			if (m.w > max_w) {
				max_w = m.w;
			}
			if (m.h > max_h) {
				max_h = m.h;
			}
		}
		if (screen_w > max_w || screen_h > max_h) {
			screen_w = -1;
			screen_h = -1;
			util::infomsg("screen_w/screen_h greater than max resolution. Using default.\n");
		}

		set_shim_args(true, windowed == 1, fullscreen == 1);

		if (wedge::start(util::Size<int>(SCR_W, SCR_H), util::Size<int>(screen_w, screen_h)) == false) {
			util::debugmsg("wedge::start returned false.\n");
			return 1;
		}

		if (tv_safe_mode) {
			apply_tv_safe_mode(true);
		}

		Widget::static_start();

		Settings_GUI::static_start();

		Enemy_Wrath::static_start();
		Enemy_Unicorn::static_start();
		Enemy_Flame_Frog::static_start();
		Enemy_Bot::static_start();
		Enemy_Goblin::static_start();
		Enemy_Red_Wasp::static_start();
		Enemy_Snapper::static_start();
		Enemy_Aura::static_start();

		Widget_List::set_default_normal_text_colour1(shim::palette[34]);
		Widget_List::set_default_normal_text_colour2(shim::white);
		Widget_List::set_default_text_shadow_colour(shim::transparent);
		Widget_List::set_default_highlight_text_colour1(shim::palette[30]);
		Widget_List::set_default_highlight_text_colour2(shim::palette[31]);
		Widget_List::set_default_disabled_text_colour1(shim::black);
		Widget_List::set_default_disabled_text_colour2(shim::black);
		Widget_List::set_default_disabled_text_shadow_colour(shim::transparent);
		Widget_Label::set_default_text_colour(shim::white);
		Widget_Label::set_default_shadow_colour(shim::transparent);

		shim::white = shim::palette[33]; // use our own white, it's actual white but could change
		shim::black = shim::palette[39]; // black doesn't exist in our palette

		wedge::globals = new Globals();

		std::string tmp_language = "English";

#ifdef STEAMWORKS
		if (true) {
			(void)settings_loaded;
#else
		if (settings_loaded == false) {
#endif
			std::map<std::string, std::string> languages;

			languages["german"] = "German";
			languages["french"] = "French";
			languages["dutch"] = "Dutch";
			languages["greek"] = "Greek";
			languages["italian"] = "Italian";
			languages["polish"] = "Polish";
			languages["portuguese"] = "Portuguese";
			languages["russian"] = "Russian";
			languages["spanish"] = "Spanish";
			languages["korean"] = "Korean";
			languages["english"] = "English";
			languages["brazilian"] = "Brazilian";

			std::string syslang = util::get_system_language();

			tmp_language = languages[syslang];

			if (tmp_language != "English" && tmp_language != "French") { // FIXME: add others
				tmp_language = "English";
			}
		}

		// Defaults
		util::JSON::Node *root = cfg->get_root();

		GLOBALS->key_action = root->get_nested_int("input>key_action", &GLOBALS->key_action, GLOBALS->key_action);
		GLOBALS->key_b1 = root->get_nested_int("input>key_b1", &GLOBALS->key_b1, GLOBALS->key_b1);
		GLOBALS->key_b2 = root->get_nested_int("input>key_b2", &GLOBALS->key_b2, GLOBALS->key_b2);
		GLOBALS->key_b3 = root->get_nested_int("input>key_b3", &GLOBALS->key_b3, GLOBALS->key_b3);
		GLOBALS->key_b4 = root->get_nested_int("input>key_b4", &GLOBALS->key_b4, GLOBALS->key_b4);
		GLOBALS->key_back = root->get_nested_int("input>key_back", &GLOBALS->key_back, GLOBALS->key_back);
		GLOBALS->key_pause = root->get_nested_int("input>key_pause", &GLOBALS->key_pause, GLOBALS->key_pause);
		GLOBALS->key_menu = root->get_nested_int("input>key_menu", &GLOBALS->key_menu, GLOBALS->key_menu);
		GLOBALS->key_switch = root->get_nested_int("input>key_switch", &GLOBALS->key_switch, GLOBALS->key_switch);
		GLOBALS->key_cancel = root->get_nested_int("input>key_cancel", &GLOBALS->key_cancel, GLOBALS->key_cancel);
		GLOBALS->key_confirm = root->get_nested_int("input>key_confirm", &GLOBALS->key_confirm, GLOBALS->key_confirm);
		shim::fullscreen_key = root->get_nested_int("input>key_fullscreen", &shim::fullscreen_key, shim::fullscreen_key);
		GLOBALS->key_l = root->get_nested_int("input>key_l", &GLOBALS->key_l, GLOBALS->key_l);
		GLOBALS->key_r = root->get_nested_int("input>key_r", &GLOBALS->key_r, GLOBALS->key_r);
		GLOBALS->key_u = root->get_nested_int("input>key_u", &GLOBALS->key_u, GLOBALS->key_u);
		GLOBALS->key_d = root->get_nested_int("input>key_d", &GLOBALS->key_d, GLOBALS->key_d);
		GLOBALS->joy_action = root->get_nested_int("input>joy_action", &GLOBALS->joy_action, GLOBALS->joy_action);
		GLOBALS->joy_b1 = root->get_nested_int("input>joy_b1", &GLOBALS->joy_b1, GLOBALS->joy_b1);
		GLOBALS->joy_b2 = root->get_nested_int("input>joy_b2", &GLOBALS->joy_b2, GLOBALS->joy_b2);
		GLOBALS->joy_b3 = root->get_nested_int("input>joy_b3", &GLOBALS->joy_b3, GLOBALS->joy_b3);
		GLOBALS->joy_b4 = root->get_nested_int("input>joy_b4", &GLOBALS->joy_b4, GLOBALS->joy_b4);
		GLOBALS->joy_back = root->get_nested_int("input>joy_back", &GLOBALS->joy_back, GLOBALS->joy_back);
		GLOBALS->joy_pause = root->get_nested_int("input>joy_pause", &GLOBALS->joy_pause, GLOBALS->joy_pause);
		GLOBALS->joy_menu = root->get_nested_int("input>joy_menu", &GLOBALS->joy_menu, GLOBALS->joy_menu);
		GLOBALS->joy_switch = root->get_nested_int("input>joy_switch", &GLOBALS->joy_switch, GLOBALS->joy_switch);
		GLOBALS->joy_cancel = root->get_nested_int("input>joy_cancel", &GLOBALS->joy_cancel, GLOBALS->joy_cancel);
		GLOBALS->joy_confirm = root->get_nested_int("input>joy_confirm", &GLOBALS->joy_confirm, GLOBALS->joy_confirm);
#ifdef STEAMWORKS
		GLOBALS->language = tmp_language;
#else
		GLOBALS->language = root->get_nested_string("misc>language", &GLOBALS->language, tmp_language);
#endif
		autosave_help_shown = root->get_nested_bool("misc>autosave_help_shown", &autosave_help_shown, false);

		GLOBALS->onscreen_controller_was_enabled = root->get_nested_bool("wedge>use_onscreen_controller", &GLOBALS->onscreen_controller_was_enabled, GLOBALS->onscreen_controller_was_enabled);
#if !defined ANDROID && !(defined IOS && !defined TVOS)
		if (shim::force_tablet == false) {
			GLOBALS->onscreen_controller_was_enabled = false;
		}
#endif
		GLOBALS->rumble_enabled = root->get_nested_bool("wedge>rumble_enabled", &GLOBALS->rumble_enabled, GLOBALS->rumble_enabled);

		GLOBALS->load_translation();

#ifdef GOOGLE_PLAY_XXX
		start_google_play_games_services();
#endif

		if (wedge::go() == false) {
			util::debugmsg("wedge::go return false.\n");
			return 1;
		}

		save_settings();

		wedge::end();

		end_autosave_thread();
	
		shim::static_end();
	}
	catch (util::Error &e) {
		util::errormsg("Fatal error: %s\n", e.error_message.c_str());
		gui::fatalerror("Fatal Error", e.error_message, gui::OK);
		return 1;
	}

#if defined IOS || defined ANDROID
	exit(0);
#endif

	return 0;
}
