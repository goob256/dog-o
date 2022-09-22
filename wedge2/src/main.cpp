#include "wedge2/main.h"
#include <wedge2/area.h>
#include <wedge2/area_game.h>
#include <wedge2/battle_game.h>
#include <wedge2/general.h>
#include <wedge2/globals.h>
#include <wedge2/internal.h>
#include <wedge2/omnipresent.h>
#include <wedge2/onscreen_controller.h>

#ifdef ANDROID
#include <jni.h>
#endif

#ifdef ASHEDIT_TRAILER
#include <SDL2/SDL_ttf.h>
#include <shim2/ttf.h>
#endif

#define NULL_FINGER -1000000

using namespace wedge;

namespace wedge {

static util::Size<int> last_screen_size;

static std::vector<util::A_Star::Way_Point> get_way_points(util::Point<int> start)
{
	Area *area = AREA->get_current_area();
	return area->get_way_points(start);
}

static void joystick_disconnected()
{
	GLOBALS->mini_pause(true);
}

static void lost_device()
{
	if (OMNIPRESENT != NULL) {
		OMNIPRESENT->lost_device();
	}
	if (BATTLE != NULL) {
		BATTLE->lost_device();
	}
	if (MENU != NULL) {
		MENU->lost_device();
	}
	if (SHOP != NULL) {
		SHOP->lost_device();
	}
	if (AREA != NULL) {
		AREA->lost_device();
	}

	GLOBALS->lost_device();
}

static void found_device()
{
	GLOBALS->found_device();

	if (OMNIPRESENT != NULL) {
		OMNIPRESENT->found_device();
	}
	if (BATTLE != NULL) {
		BATTLE->found_device();
	}
	if (MENU != NULL) {
		MENU->found_device();
	}
	if (SHOP != NULL) {
		SHOP->found_device();
	}
	if (AREA != NULL) {
		AREA->found_device();
	}
}

static void draw_all()
{
#ifdef ANDROID
	JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
	jobject activity = (jobject)SDL_AndroidGetActivity();
	jclass clazz(env->GetObjectClass(activity));

	jmethodID method_id = env->GetMethodID(clazz, "start_draw", "()V");

	if (method_id != NULL) {
		env->CallVoidMethod(activity, method_id);
	}

	env->DeleteLocalRef(activity);
	env->DeleteLocalRef(clazz);
#endif

#ifdef VERBOSE
	util::debugmsg("draw_all------------------------------------------------------------------\n");
#endif

#ifdef _WIN32
	if (shim::opengl == false && gfx::is_d3d_lost() == true) {
		gfx::flip(); // this is where lost devices are handled...
		return;
	}
#endif

	gfx::clear_buffers();

	if (BATTLE != NULL) {
		BATTLE->draw_back();
	}
	else if (MENU != NULL) {
		MENU->draw_back();
	}
	else if (SHOP != NULL) {
		SHOP->draw_back();
	}
	else if (AREA != NULL) {
		AREA->draw_back();
	}

	OMNIPRESENT->draw_back();

	if (BATTLE != NULL) {
		BATTLE->draw();
	}
	else if (MENU != NULL) {
		MENU->draw();
		if (AREA->has_pause_ended()) {
			AREA->draw();
		}
	}
	else if (SHOP != NULL) {
		SHOP->draw();
		if (AREA->has_pause_ended()) {
			AREA->draw();
		}
	}
	else if (AREA != NULL) {
		AREA->draw();
	}

	OMNIPRESENT->draw();

	if (BATTLE != NULL) {
		BATTLE->draw_fore();
	}
	else if (MENU != NULL) {
		MENU->draw_fore();
	}
	else if (SHOP != NULL) {
		SHOP->draw_fore();
	}
	else if (AREA != NULL) {
		AREA->draw_fore();
	}

	if (MENU || SHOP) {
		if (AREA->is_pausing()) {
			AREA->draw();
		}
	}

	gfx::draw_guis();

	if (AREA) {
		AREA->draw_appear_in();
	}

	OMNIPRESENT->draw_fore();

	gfx::draw_notifications();

	if (is_onscreen_controller_enabled()) {
		draw_onscreen_controller();
	}

	OMNIPRESENT->draw_controls();

	gfx::flip();
}

bool start(util::Size<int> base_screen_size, util::Size<int> window_size)
{
	// This is basically 16:9 only, with a tiny bit of leeway
	gfx::set_min_aspect_ratio(1.75f);
	gfx::set_max_aspect_ratio(1.79f);

	if (util::bool_arg(false, shim::argc, shim::argv, "logging")) {
		shim::logging = true;
	}

	if (util::bool_arg(false, shim::argc, shim::argv, "dump-images")) {
		gfx::Image::keep_data = true;
		gfx::Image::save_palettes = util::bool_arg(false, shim::argc, shim::argv, "save-palettes");
		gfx::Image::save_rle = true;
		gfx::Image::save_rgba = false;
		gfx::Image::premultiply_alpha = false;
	}

	/* get_desktop_resolution uses shim::adapter, but it's not normally set until shim::start is called, so set it here since
	 * it's used below.
	 */
	int adapter_i = util::check_args(shim::argc, shim::argv, "+adapter");
	if (adapter_i >= 0) {
		shim::adapter = atoi(shim::argv[adapter_i+1]);
		if (shim::adapter >= SDL_GetNumVideoDisplays()-1) {
			shim::adapter = 0;
		}
	}

	gfx::set_minimum_window_size(base_screen_size * 4);
	util::Size<int> desktop_resolution = gfx::get_desktop_resolution();
	gfx::set_maximum_window_size(desktop_resolution);

	/*
	int max = MAX(desktop_resolution.w, desktop_resolution.h);
	int min = MIN(desktop_resolution.w, desktop_resolution.h);
	*/

#if !defined IOS && !defined ANDROID
	const int min_supp_w = 1280;
	const int min_supp_h = 720;

	if (desktop_resolution.w < min_supp_w || desktop_resolution.h < min_supp_h) {
		gui::popup("Unsupported System", "The minimum resolution supported by this game is 1280x720, which this system does not meet. Exiting.", gui::OK);
		exit(1);
	}
#endif

#ifdef ASHEDIT_TRAILER
	if (shim::start_all(640, 360, false, 1920, 1080) == false) {
#else
	if (shim::start_all(base_screen_size.w, base_screen_size.h, false, window_size.w, window_size.h) == false) {
#endif
		gui::fatalerror("shim::start failed", "Initialization failed.", gui::OK, false);
		return false;
	}

#ifdef _WIN32
	gfx::enable_press_and_hold(false);
#endif

	if (shim::font == 0) {
		gui::fatalerror("Fatal Error", "Font not found! Aborting.", gui::OK, false);
		return false;
	}

	shim::font->set_vertex_cache_id(1);
	shim::font->start_batch();

	if (util::bool_arg(false, shim::argc, shim::argv, "dump-images")) {
		std::vector<std::string> filenames = shim::cpa->get_all_filenames();
		for (size_t i = 0; i < filenames.size(); i++) {
			std::string filename =  filenames[i];
			if (filename.find(".tga") != std::string::npos) {
				gfx::Image *image = new gfx::Image(filename, true);
				std::string path = "out/" + filename;
				std::string dir;
				size_t i;
				while ((i = path.find("/")) != std::string::npos) {
					dir += path.substr(0, i);
					util::mkdir(dir.c_str());
					path = path.substr(i+1);
					dir += "/";
				}
				image->save("out/" + filename);
				delete image;
			}
		}
		exit(0);
	}

	TGUI::set_focus_sloppiness(0);

	shim::user_render = draw_all;

	start_onscreen_controller(true);

	shim::get_way_points = get_way_points;

	gfx::register_lost_device_callbacks(lost_device, found_device);
	shim::joystick_disconnect_callback = joystick_disconnected;
#ifdef STEAMWORKS
	shim::steam_overlay_activated_callback = joystick_disconnected;
#endif

	return true;
}

void handle_event(TGUI_Event *event)
{
	if (event->type == TGUI_UNKNOWN) {
		return;
	}

	if (event->type == TGUI_QUIT) {
		quit_all();
		GLOBALS->terminate = true;
	}

	if (event->type == TGUI_MOUSE_AXIS) {
		mouse_position = util::Point<int>((int)event->mouse.x, (int)event->mouse.y);
	}
	
	if (BATTLE != NULL) {
		BATTLE->handle_event(event);
	}
	else if (MENU != NULL) {
		MENU->handle_event(event);
		if (AREA->is_pausing() == false && AREA->is_paused() == false) {
			AREA->handle_event(event);
		}
	}
	else if (SHOP != NULL) {
		if (AREA->is_pausing() == false && AREA->is_paused() == false) {
			SHOP->handle_event(event);
		}
	}
	else if (AREA != NULL) {
		AREA->handle_event(event);
	}
	
	if (shim::screen_size != last_screen_size) {
		if (BATTLE != NULL) {
			BATTLE->resize(shim::screen_size);
		}
		else if (MENU != NULL) {
			MENU->resize(shim::screen_size);
		}
		else if (SHOP != NULL) {
			SHOP->resize(shim::screen_size);
		}
		else if (AREA != NULL) {
			AREA->resize(shim::screen_size);
		}
		
		OMNIPRESENT->resize(shim::screen_size);

		last_screen_size = shim::screen_size;
	}
}

static void loop()
{
	bool quit = false;

	// These keep the logic running at 60Hz and drawing at refresh rate is possible
	// NOTE: screen refresh rate has to be 60Hz or higher for this to work.
	const float target_fps = shim::refresh_rate <= 0 ? 60.0f : shim::refresh_rate;
	Uint32 start = SDL_GetTicks();
	int logic_frames = 0;
	int drawing_frames = 0;
	bool can_draw = true;
	bool can_logic = true;
	std::string old_music_name = "";
#if defined IOS || defined ANDROID
	float old_volume = 1.0f;
#endif
	int curr_logic_rate = shim::logic_rate;

	last_screen_size = shim::screen_size;

	SDL_FingerID triangle_finger = NULL_FINGER;
	SDL_FingerID x_finger = NULL_FINGER;
	SDL_FingerID ignore_finger = NULL_FINGER;

#ifdef ASHEDIT_TRAILER
	gfx::Image *i1 = new gfx::Image("misc/1.tga");
	gfx::Image *i2 = new gfx::Image("misc/2.tga");
	gfx::Image *i3 = new gfx::Image("misc/3.tga");
	gfx::Image *i4 = new gfx::Image("misc/4.tga");
	gfx::Image *l = new gfx::Image("misc/l.tga");
	gfx::TTF *ttf = new gfx::TTF("font.ttf", 32, 1024);
	audio::MML *trailer_mml = new audio::MML("music/ashedit.mml");
	Uint32 t_start = GET_TICKS();
#endif

	while (quit == false) {
		if (GLOBALS->work_image != NULL && GLOBALS->work_image->size != shim::real_screen_size) {
			delete GLOBALS->work_image;
			GLOBALS->create_work_image();
		}

		// This works as a hybrid onscreen controller mode... no onscreen controller in menus/etc
		/*
		if (GLOBALS->onscreen_controller_was_enabled) {
			bool dsam = true;
			if (AREA) {
				auto hooks = AREA->get_current_area()->get_hooks();
				if (hooks) {
					dsam = hooks->get_done_started_and_moved();
				}
			}
			if (GLOBALS->can_walk() == false && dsam) {
				enable_onscreen_controller(false);
			}
			else {
				enable_onscreen_controller(true);
			}
		}
		else {
			enable_onscreen_controller(false);
		}
		*/

		// this is an always-on onscreen controller
		if (GLOBALS->onscreen_controller_was_enabled) {
			enable_onscreen_controller(true);
		}
		else {
			enable_onscreen_controller(false);
		}
	
		// EVENTS
		while (true) {
#ifdef STEAMWORKS
			GLOBALS->handle_steam_input();
#endif

			SDL_Event sdl_event;
			TGUI_Event *e = nullptr;

			bool all_done = false;

			if (!SDL_PollEvent(&sdl_event)) {
				e = shim::pop_pushed_event();
				if (e == nullptr) {
					all_done = true;
				}
			}

			if (all_done) {
				break;
			}

			bool is_red_triangle = false;
			bool is_red_x = false;

			if (e == nullptr) {
				if (sdl_event.type == SDL_QUIT) {
					if (can_logic == false || (AREA == NULL && BATTLE == NULL)) {
						shim::handle_event(&sdl_event);
						quit = true;
						break;
					}
				}
				// right mouse clicks are transformed to escape keys
				else if (can_logic && sdl_event.type == SDL_MOUSEBUTTONDOWN && sdl_event.button.button == SDL_BUTTON_RIGHT && GLOBALS->title_gui_is_top() == false) {
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
					continue;
				}
				else if ((sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) && sdl_event.key.keysym.sym == SDLK_SELECT) {
					sdl_event.key.keysym.sym = /*globals->key_b1*/GLOBALS->key_action;
				}
				else if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_F12) {
					GLOBALS->load_translation();
				}
#ifdef TVOS
				else if ((sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) && sdl_event.key.keysym.sym == SDLK_MENU) {
					sdl_event.key.keysym.sym = globals->key_back;
				}
				else if ((sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) && sdl_event.key.keysym.sym == SDLK_PAUSE) {
					sdl_event.key.keysym.sym = globals->key_switch;
				}
#endif
#ifdef ANDROID
				else if ((sdl_event.type == SDL_JOYBUTTONDOWN || sdl_event.type == SDL_JOYBUTTONUP) && sdl_event.jbutton.button == SDL_CONTROLLER_BUTTON_MAX+4) {
					TGUI_Event e;
					if (sdl_event.type == SDL_JOYBUTTONDOWN) {
						e.type = TGUI_KEY_DOWN;
					}
					else {
						e.type = TGUI_KEY_UP;
					}
					e.keyboard.code = GLOBALS->key_action;
					e.keyboard.is_repeat = false;
					e.keyboard.simulated = true;
					shim::push_event(e);
				}
				/*
				else if ((sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) && sdl_event.key.keysym.sym == SDLK_AUDIOPLAY) {
					sdl_event.key.keysym.sym = globals->key_switch;
				}
				else if ((sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) && sdl_event.key.keysym.sym == SDLK_AC_SEARCH) {
					sdl_event.key.keysym.sym = globals->key_b3;
				}
				*/
#endif
				if (can_logic) {
					// Android back key gets turned to key_back
					if ((sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_KEYUP) && sdl_event.key.keysym.sym == SDLK_AC_BACK) {
						//sdl_event.key.keysym.sym = globals->key_back;
						if (GLOBALS->should_show_back_arrow()) {
							if (GLOBALS->getting_combo) {
								sdl_event.key.keysym.sym = GLOBALS->key_cancel;
							}
							else if (shim::guis.size() == 0 && BATTLE == nullptr) {
								sdl_event.key.keysym.sym = GLOBALS->key_menu;
							}
							else {
								sdl_event.key.keysym.sym = GLOBALS->key_back;
							}
						}
						else {
							continue;
						}
					}

					bool assigning = GLOBALS->assigning_controls;

					if (sdl_event.type == SDL_FINGERDOWN || sdl_event.type == SDL_FINGERUP || sdl_event.type == SDL_FINGERMOTION) {
						float fx;
						float fy;

						fx = sdl_event.tfinger.x;
						fy = sdl_event.tfinger.y;
						fx *= shim::real_screen_size.w;
						fy *= shim::real_screen_size.h;
						fx -= shim::screen_offset.x;
						fy -= shim::screen_offset.y;
						fx /= shim::scale;
						fy /= shim::scale;
						is_red_triangle = fx < shim::tile_size && fy < shim::tile_size;

						if (assigning && sdl_event.type == SDL_FINGERDOWN) {
							ignore_finger = sdl_event.tfinger.fingerId;
						}
						else if (sdl_event.type == SDL_FINGERUP && sdl_event.tfinger.fingerId == ignore_finger) {
							ignore_finger = NULL_FINGER;
						}

						if (triangle_finger == NULL_FINGER && is_red_triangle) {
							if (sdl_event.type == SDL_FINGERDOWN || sdl_event.type == SDL_FINGERMOTION) {
								if (sdl_event.tfinger.fingerId != ignore_finger) {
									triangle_finger = sdl_event.tfinger.fingerId;
								}
							}
						}
						else if (triangle_finger != NULL_FINGER) {
							if (is_red_triangle) {
								if (sdl_event.type == SDL_FINGERUP) {
									triangle_finger = NULL_FINGER;
								}
								else if (sdl_event.tfinger.fingerId != ignore_finger) {
									triangle_finger = sdl_event.tfinger.fingerId;
								}
							}
							else {
								if (sdl_event.tfinger.fingerId == triangle_finger) {
									triangle_finger = NULL_FINGER;
								}
							}
						}
						
						fx = sdl_event.tfinger.x;
						fy = sdl_event.tfinger.y;
						fx *= shim::real_screen_size.w;
						fy *= shim::real_screen_size.h;
						fx -= shim::screen_offset.x;
						fy -= shim::screen_offset.y;
						fx /= shim::scale;
						fy /= shim::scale;
						is_red_x = fx >= shim::screen_size.w-shim::tile_size && fy < shim::tile_size;
						
						if (x_finger == NULL_FINGER && is_red_x) {
							if (sdl_event.type == SDL_FINGERDOWN || sdl_event.type == SDL_FINGERMOTION) {
								if (sdl_event.tfinger.fingerId != ignore_finger) {
									x_finger = sdl_event.tfinger.fingerId;
								}
							}
						}
						else if (x_finger != NULL_FINGER) {
							if (is_red_x) {
								if (sdl_event.type == SDL_FINGERUP) {
									x_finger = NULL_FINGER;
								}
								else if (sdl_event.tfinger.fingerId != ignore_finger) {
									x_finger = sdl_event.tfinger.fingerId;
								}
							}
							else {
								if (sdl_event.tfinger.fingerId == x_finger) {
									x_finger = NULL_FINGER;
								}
							}
						}
					}

					if (is_red_triangle == false && is_red_x == false && is_onscreen_controller_enabled() && (sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP || sdl_event.type == SDL_MOUSEMOTION || handle_onscreen_controller(&sdl_event))) {
						continue;
					}
				}
			}
				
			TGUI_Event *event;

			if (e) {
				event = e;
			}
			else {
				if (sdl_event.type == SDL_QUIT) {
					static TGUI_Event quit_event;
					quit_event.type = TGUI_QUIT;
					event = &quit_event;
				}
				else {
					event = shim::handle_event(&sdl_event);
				}
			}

			if (triangle_finger == NULL_FINGER) {
				OMNIPRESENT->set_triangle_pressed(false);
			}
			else {
				OMNIPRESENT->set_triangle_pressed(true);
			}
			if (x_finger == NULL_FINGER) {
				OMNIPRESENT->set_x_pressed(false);
			}
			else {
				OMNIPRESENT->set_x_pressed(true);
			}

			bool mp = GLOBALS->is_mini_paused();
			bool agmp = GLOBALS->allow_global_mini_pause;

			if (is_red_triangle || is_red_x) {
				OMNIPRESENT->handle_event(event);
			}
			else if (mp == false) {
				if (agmp && (
					(GLOBALS->key_pause != GLOBALS->key_back && event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_pause && event->keyboard.is_repeat == false) ||
					(event->type == TGUI_KEY_DOWN && event->keyboard.code == PAUSE_VAL && event->keyboard.is_repeat == false) ||
					(GLOBALS->joy_pause != GLOBALS->joy_back && event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_pause && event->joystick.is_repeat == false)
				)) {
					GLOBALS->mini_pause();
				}

				handle_event(event);
			}
		}

		if (quit) {
			break;
		}

		// Logic rate can change in devsettings
		if (shim::logic_rate != curr_logic_rate) {
			curr_logic_rate = shim::logic_rate;
			logic_frames = 0;
			drawing_frames = 0;
			start = SDL_GetTicks();
		}

		// TIMING
		int diff = SDL_GetTicks() - start;
		bool skip_drawing;
		int logic_reps;

		if (diff > 0) {
			float average;
			// Skip logic if running fast
			average = logic_frames / (diff / 1000.0f);
			if (average > shim::logic_rate-1) { // a little leeway
				logic_reps = 0;
			}
			else {
				logic_reps = shim::logic_rate - average;
			}
			// Skip drawing if running slow
			average = drawing_frames / (diff / 1000.0f);
			if (average < (target_fps-2.0f)) { // allow a little bit of fluctuation, i.e., not exactly target_fps here
				skip_drawing = true;
			}
			else {
				skip_drawing = false;
			}
		}
		else {
			skip_drawing = false;
			logic_reps = 1;
		}

		for (int logic = 0; logic < logic_reps; logic++) {
#if defined IOS || defined ANDROID
			bool could_draw = can_draw;
#endif
			GLOBALS->run();
			can_draw = shim::update();
#if defined IOS || defined ANDROID
			can_logic = can_draw;
			if (could_draw != can_draw) {
				if (can_draw == false) {
					if (AREA != NULL && MENU == NULL && SHOP == NULL) {
						Uint32 pause_start_time = GET_TICKS();
						Uint32 played_time = pause_start_time - INSTANCE->play_start;
						INSTANCE->play_time += (played_time / 1000);
					}
					old_volume = shim::music->get_master_volume(); 
					shim::music->pause();
				}
				else {
					if (AREA != NULL && MENU == NULL && SHOP == NULL) {
						INSTANCE->play_start = GET_TICKS();
					}
					start = SDL_GetTicks();
					logic_frames = 0;
					drawing_frames = 0;
					shim::music->play(old_volume, true);
				}
			}
#endif

			// Generate a timer tick event (TGUI_TICK)
			SDL_Event sdl_event;
			sdl_event.type = shim::timer_event_id;
			TGUI_Event *event = shim::handle_event(&sdl_event);
			handle_event(event);

			if (is_onscreen_controller_enabled()) {
				update_onscreen_controller();
			}

			logic_frames++;
		}

		// LOGIC
		if (can_logic && GLOBALS->is_mini_paused() == false) {
			for (int logic = 0; logic < logic_reps; logic++) {
				gfx::update_animations();

				if (BATTLE != NULL) {
					if (BATTLE->run() == false) {
						delete BATTLE;
						BATTLE = NULL;
						OMNIPRESENT->end_fade();
					}
				}
				else if (MENU != NULL) {
					/*
					Uint32 now = GET_TICKS();
					int pause_fade_time = AREA->get_pause_fade_time();
					Uint32 pause_end_time = AREA->get_pause_end_time();
					*/
					if (AREA->is_pausing() || AREA->has_pause_ended()) {
						AREA->run();
					}
					if (MENU->run() == false) {
						delete MENU;
						MENU = NULL;
					}
				}
				else if (SHOP != NULL) {
					/*
					Uint32 now = GET_TICKS();
					int pause_fade_time = AREA->get_pause_fade_time();
					Uint32 pause_end_time = AREA->get_pause_end_time();
					*/
					if (AREA->is_pausing() || AREA->has_pause_ended()) {
						AREA->run();
					}
					if (SHOP->run() == false) {
						delete SHOP;
						SHOP = NULL;
					}
				}
				else if (AREA != NULL) {
					if (AREA->run() == false) {
						delete AREA;
						AREA = NULL;
					}
				}

				OMNIPRESENT->run();
			}
		}
		else if (GLOBALS->is_mini_paused()) {
			for (int logic = 0; logic < logic_reps; logic++) {
				GLOBALS->cursor->update();
			}
		}

		if (shim::guis.size() == 0 && AREA == NULL && BATTLE == NULL) {
			delete INSTANCE;
			INSTANCE = nullptr;
			if (GLOBALS->terminate) {
				break;
			}
			if (globals->add_title_gui() == false) {
				break;
			}
		}

		// DRAWING
		if (skip_drawing == false && can_draw) {
#ifdef ASHEDIT_TRAILER
			gfx::clear(shim::black);

			Uint32 now = GET_TICKS();
			Uint32 elapsed = now - t_start;
			static bool played_mml = false;
			if (played_mml == false && elapsed >= 500) {
				played_mml = true;
				trailer_mml->play(true);
			}
			if (elapsed > 1000 && elapsed < 2500) {
				float p = (elapsed - 1000) / 1500.0f;
				float right_x = p * shim::screen_size.w * 0.95f;
				float x = right_x - i1->size.w;
				float y = shim::screen_size.h * 0.9f - i1->size.h;
				SDL_Colour tint = shim::white;
				float a = (elapsed < 2000) ? 1.0f : 1.0f - ((elapsed - 2000) / 500.0f);
				tint.r *= a;
				tint.g *= a;
				tint.b *= a;
				tint.a *= a;
				i1->draw_tinted(tint, {x, y});
			}
			if (elapsed > 2000 && elapsed < 3500) {
				float p = (elapsed - 2000) / 1500.0f;
				float x = shim::screen_size.w - (p * shim::screen_size.w * 0.95f);
				float y = shim::screen_size.h * 0.1f;
				SDL_Colour tint = shim::white;
				float a = (elapsed < 3000) ? 1.0f : 1.0f - ((elapsed - 3000) / 500.0f);
				tint.r *= a;
				tint.g *= a;
				tint.b *= a;
				tint.a *= a;
				i2->draw_tinted(tint, {x, y});
			}
			if (elapsed > 3000 && elapsed < 4500) {
				float p = (elapsed - 3000) / 1500.0f;
				float y = shim::screen_size.h - (p * shim::screen_size.h * 1.5f);
				float x = shim::screen_size.w * 0.9f - i3->size.w;
				float a = (elapsed < 4000) ? 1.0f : 1.0f - ((elapsed - 4000) / 500.0f);
				SDL_Colour tint = shim::white;
				tint.r *= a;
				tint.g *= a;
				tint.b *= a;
				tint.a *= a;
				i3->draw_tinted(tint, {x, y});
			}
			if (elapsed > 4500 && elapsed < 6000) {
				float p = (elapsed - 4500) / 1500.0f;
				float angle = -10.0f / 180.0f * M_PI * p;
				float cy = shim::screen_size.h/2.0f - i4->size.h/2.0f;
				float diff = cy - -100.0f;
				float scale = 1.0f + p * 0.5f;
				i4->draw_rotated_scaled({i4->size.w/2.0f, i4->size.h/2.0f}, {shim::screen_size.w/2.0f, shim::screen_size.h/2.0f+p*diff}, angle, scale, 0);
				float a = (elapsed < 5500) ? 0.0f : ((elapsed - 5500) / 500.0f) * 0.75f;
				SDL_Colour tint = shim::white;
				tint.r *= a;
				tint.g *= a;
				tint.b *= a;
				tint.a *= a;
				gfx::draw_filled_rectangle(tint, {0.0f, 0.0f}, {(float)shim::screen_size.w, (float)shim::screen_size.h});
			}
			if (elapsed >= 6000) {
				float p = 1.0f;
				float angle = -10.0f / 180.0f * M_PI * p;
				float cy = shim::screen_size.h/2.0f - i4->size.h/2.0f;
				float diff = cy - -100.0f;
				float scale = 1.0f + p * 0.5f;
				i4->draw_rotated_scaled({i4->size.w/2.0f, i4->size.h/2.0f}, {shim::screen_size.w/2.0f, shim::screen_size.h/2.0f+p*diff}, angle, scale, 0);
				float a = 0.75f;
				SDL_Colour tint = shim::white;
				tint.r *= a;
				tint.g *= a;
				tint.b *= a;
				tint.a *= a;
				gfx::draw_filled_rectangle(tint, {0.0f, 0.0f}, {(float)shim::screen_size.w, (float)shim::screen_size.h});
			}
			std::string t1 = "• Edit Game Maps";
			std::string t2 = "• Simple Map Format";
			std::string t3 = "• Multiplatform";
			int w = ttf->get_text_width(t1);
			w = MAX(w, ttf->get_text_width(t2));
			w = MAX(w, ttf->get_text_width(t3));
			int th = ttf->get_height()*3 + 10 * 3;
			float y1 = shim::screen_size.h/2.0f - th/2.0f;
			float y2 = y1 + 10 + ttf->get_height();
			float y3 = y2 + 10 + ttf->get_height();
			float x1 = shim::screen_size.w/2.0f - w/2.0f;
			if (elapsed >= 7000 && elapsed < 12000) {
				ttf->draw(shim::black, t1, {x1, y1});
			}
			if (elapsed >= 8000 && elapsed < 12000) {
				ttf->draw(shim::black, t2, {x1, y2});
			}
			if (elapsed >= 9000 && elapsed < 12000) {
				ttf->draw(shim::black, t3, {x1, y3});
			}
			if (elapsed > 12000) {
				float a = ((elapsed - 12000) / 500.0f);
				if (elapsed >= 12500) {
					a = 1.0f;
				}
				SDL_Colour c = shim::white;
				c.r *= a;
				c.g *= a;
				c.b *= a;
				c.a *= a;
				gfx::draw_filled_rectangle(c, {0.0f, 0.0f}, {(float)shim::screen_size.w, (float)shim::screen_size.h});
			}
			if (elapsed > 12000 && elapsed < 13500) {
				float p = ((elapsed - 12000) / 1500.0f);
				float scale = 0.01f + p * 0.2f;
				l->draw_rotated_scaled({l->size.w/2.0f, l->size.h/2.0f}, {shim::screen_size.w/2.0f, shim::screen_size.h/2.0f}, 0.0f, scale);
			}
			if (elapsed > 13500) {
				float scale = 0.01f + 0.2f;
				l->draw_rotated_scaled({l->size.w/2.0f, l->size.h/2.0f}, {shim::screen_size.w/2.0f, shim::screen_size.h/2.0f}, 0.0f, scale);
			}

			gfx::flip();
#else
			draw_all();
#endif
		}

		drawing_frames++;
	}
}

bool go()
{
	OMNIPRESENT = new Omnipresent_Game();
	OMNIPRESENT->start();

	globals->add_title_gui();

	loop();

	return true;
}

void end()
{
	shim::user_render = NULL;

	delete BATTLE;
	BATTLE = nullptr;
	delete MENU;
	MENU = nullptr;
	delete SHOP;
	SHOP = nullptr;
	delete AREA;
	AREA = nullptr;
	delete OMNIPRESENT;
	OMNIPRESENT = nullptr;

	// If Alt-F4 is pressed the title gui can remain in shim::guis. Leaving it to shim to destruct won't work, because ~Title_GUI accesses Globals which is destroyed below
	for (std::vector<gui::GUI *>::iterator it = shim::guis.begin(); it != shim::guis.end();) {
		gui::GUI *gui = *it;
		delete gui;
		it = shim::guis.erase(it);
	}

	delete globals;

	shim::end_all();
}

}
