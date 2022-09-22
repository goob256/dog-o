#include <wedge2/area.h>
#include <wedge2/area_game.h>
#include <wedge2/battle_game.h>
#include <wedge2/delay.h>
#include <wedge2/general.h>
#include <wedge2/globals.h>
#include <wedge2/input.h>
#include <wedge2/map_entity.h>
#include <wedge2/screen_shake.h>
#include <wedge2/special_number.h>
#include <wedge2/tile_movement.h>

#include "area_game.h"
#include "battle_game.h"
#include "battle_player.h"
#include "combo.h"
#include "dialogue.h"
#include "dog-o.h"
#include "enemies.h"
#include "general.h"
#include "gui.h"
#include "hit.h"
#include "inventory.h"
#include "menu.h"
#include "question.h"
#include "stats.h"

#ifdef __APPLE__
#include "apple.h"
#endif

#ifdef TVOS
#include <shim2/ios.h>
#endif

#ifdef ANDROID
#include <jni.h>

static bool osc_enabled = false;

extern "C" {
	JNIEXPORT void JNICALL Java_com_nooskewl_dogo_DogO_1Activity_pause
	  (JNIEnv *env, jobject obj)
	{
		if (GLOBALS) {
			osc_enabled = GLOBALS->onscreen_controller_was_enabled;
			if (osc_enabled) {
				GLOBALS->onscreen_controller_was_enabled = false;
				GLOBALS->onscreen_controller_temporarily_disabled = true;
			}

			while (shim::pop_pushed_event() != NULL) {
			}
			
			input::end();
		}
	}

	JNIEXPORT void JNICALL Java_com_nooskewl_dogo_DogO_1Activity_resume
	  (JNIEnv *env, jobject obj)
	{
		if (osc_enabled) {
			GLOBALS->onscreen_controller_was_enabled = true;
			GLOBALS->onscreen_controller_temporarily_disabled = false;
		}
		
		input::start();
	}

	JNIEXPORT jstring JNICALL Java_com_nooskewl_dogo_DogO_1Activity_translate
	  (JNIEnv *env, jobject obj, jint id)
	{
		jstring result;
		char translation[5000];
		strcpy(translation, GLOBALS->game_t->translate(id).c_str());
		result = env->NewStringUTF(translation); 
		return result; 
	}
}

#endif

static volatile bool do_autosave;
static volatile bool _end_autosave_thread;
static SDL_mutex *autosave_mutex;
static SDL_cond *autosave_cond;
static SDL_Thread *autosave_thread;

int autosave_func(void *data)
{
	while (true) {
		SDL_LockMutex(autosave_mutex);
		while (!_end_autosave_thread && !do_autosave) {
			SDL_CondWait(autosave_cond, autosave_mutex);
		}
		SDL_UnlockMutex(autosave_mutex);

		if (_end_autosave_thread) {
			break;
		}

		if (DOGO_GLOBALS->save_slot < 0) {
			do_autosave = false;
			continue;
		}

#ifdef TVOS
		std::string fn = "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-5.dat";
		util::tvos_delete_file(fn);
#else
		std::string fn = save_dir() + "/" + "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-5.dat";
#ifdef _WIN32
		DeleteFile(fn.c_str());
#elif defined __APPLE__
		apple_delete_file(fn);
#else
		remove(fn.c_str());
#endif
#endif

		for (int i = 4; i >= 1; i--) {
#ifdef TVOS
			util::tvos_rename("auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-" + util::itos(i) + ".dat", "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-" + util::itos(i+1) + ".dat");
#else
			rename(save_filename(i-1, "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-").c_str(), save_filename(i, "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-").c_str());
#endif
		}

		Uint32 now = GET_TICKS();
		Uint32 played_time = now - INSTANCE->play_start;
		INSTANCE->play_time += (played_time / 1000);
		INSTANCE->play_start = now;

		wedge::save(save_filename(0, "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-"));

		// files must be touched so syncing works correctly
		for (int i = 0; i < 5; i++) {
			std::string fn;
#ifdef TVOS
			fn = "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-" + util::itos(i+1) + ".dat";
			util::tvos_touch(fn);
#else
			fn = save_filename(i, "auto" + util::itos(DOGO_GLOBALS->save_slot+1) + "-");
			utime(fn.c_str(), NULL);
#endif
		}

		do_autosave = false;
	}

	_end_autosave_thread = false;

	return 0;
}

void autosave(bool wait)
{
	SDL_LockMutex(autosave_mutex);
	do_autosave = true;
	SDL_CondSignal(autosave_cond);
	SDL_UnlockMutex(autosave_mutex);

	if (wait) {
		while (do_autosave == true) {
			SDL_Delay(1);
		}
	}
}

bool can_autosave()
{
	return can_show_settings(true, true) && AREA->get_current_area()->get_next_area_name() == "";
}

void start_autosave_thread()
{
	do_autosave = false;
	_end_autosave_thread = false;
	autosave_mutex = SDL_CreateMutex();
	autosave_cond = SDL_CreateCond();
	autosave_thread = SDL_CreateThread(autosave_func, "autosave_thread", NULL);
	SDL_DetachThread(autosave_thread);
}

void end_autosave_thread()
{
	SDL_LockMutex(autosave_mutex);
	_end_autosave_thread = true;
	SDL_CondSignal(autosave_cond);
	SDL_UnlockMutex(autosave_mutex);

	while (_end_autosave_thread == true) {
		SDL_Delay(1);
	}

	SDL_DestroyCond(autosave_cond);
	SDL_DestroyMutex(autosave_mutex);
}

std::vector<Dialogue_Step *> active_dialogues(wedge::Game *game)
{
	std::vector<Dialogue_Step *> dialogues;

	wedge::System_List systems = game->get_systems();
	for (wedge::System_List::iterator it = systems.begin(); it != systems.end(); it++) {
		wedge::System *system = *it;
		wedge::Task_List tasks = system->get_tasks();
		for (wedge::Task_List::iterator it2 = tasks.begin(); it2 != tasks.end(); it2++) {
			wedge::Task *task = *it2;
			wedge::Step_List steps = task->get_steps();
			for (wedge::Step_List::iterator it3 = steps.begin(); it3 != steps.end(); it3++) {
				Dialogue_Step *d = dynamic_cast<Dialogue_Step *>(*it3);
				if (d != NULL) {
					dialogues.push_back(d);
				}
			}
		}
	}

	return dialogues;
}

SDL_Colour make_translucent(SDL_Colour colour, float alpha)
{
	SDL_Colour c = colour;
	c.r *= alpha;
	c.g *= alpha;
	c.b *= alpha;
	c.a *= alpha;
	return c;
}

std::string save_dir()
{
	std::string path;

#ifdef ANDROID
	path = util::get_standard_path(util::SAVED_GAMES, true);
#elif defined _WIN32
	path = util::get_standard_path(util::SAVED_GAMES, true);
	path += "/" + shim::game_name;
	util::mkdir(path);
#else
	path = util::get_appdata_dir();
#endif

	return path;
}

#ifndef TVOS
static std::string cfg_filename()
{
#ifdef ANDROID
	return save_dir() + "/config.json";
#else
	return util::get_appdata_dir() + "/config.json";
#endif
}
#endif

bool save_settings()
{
	if (cfg == NULL) {
		return true;
	}

	// Hack because this saved settings depends on 2 values
	bool was = GLOBALS->onscreen_controller_was_enabled;
	GLOBALS->onscreen_controller_was_enabled = (GLOBALS->onscreen_controller_was_enabled || GLOBALS->onscreen_controller_temporarily_disabled);
	util::JSON::Node *node1 = cfg->get_root()->find("wedge>use_onscreen_controller");
	node1->update_value();

	extra_args = extra_args_orig;
	util::JSON::Node *node2 = cfg->get_root()->find("misc>extra_args");
	node2->update_value();

	std::string s = cfg->get_root()->to_json();

	// See hack above
	GLOBALS->onscreen_controller_was_enabled = was;
	node1->update_value();

	extra_args = "";

#ifdef TVOS
	util::tvos_save_file("config.json", s);
#else
	FILE *f = fopen(cfg_filename().c_str(), "w");

	if (f == NULL) {
		return false;
	}

	fprintf(f, "%s", s.c_str());

	fclose(f);
#endif

	return true;
}

bool load_settings()
{
    bool loaded = true;

#ifdef TVOS
	std::string s;
	if (util::tvos_read_file("config.json", s) == false) {
		loaded = false;
        s = "{}";
	}
#else
	int sz;
	char *bytes;
	std::string s;

	try {
		bytes = util::slurp_file_from_filesystem(cfg_filename(), &sz);
	}
	catch (util::Error &e) {
		loaded = false;
		s = "{}";
	}

	if (loaded) {
		s = std::string(bytes, sz);
		delete[] bytes;
	}
#endif

	SDL_RWops *memfile = SDL_RWFromMem((void *)s.c_str(), (int)s.length());

	cfg = new util::JSON(memfile);

	SDL_RWclose(memfile);

	util::JSON::Node *root = cfg->get_root();

	shim::adapter = root->get_nested_int("gfx>adapter", &shim::adapter, shim::adapter);
	windowed = root->get_nested_int("gfx>windowed", &windowed, -1);
	fullscreen = root->get_nested_int("gfx>fullscreen", &fullscreen, 1); // default = on
	screen_w = root->get_nested_int("gfx>screen_w", &screen_w, -1);
	screen_h = root->get_nested_int("gfx>screen_h", &screen_h, -1);
	tv_safe_mode = root->get_nested_bool("gfx>tv_safe_mode", &tv_safe_mode, false);

	sfx_volume = root->get_nested_float("audio>sfx_volume", &sfx_volume, 1.0f);
	shim::sfx_volume = sfx_volume * sfx_amp;
	music_volume = root->get_nested_float("audio>music_volume", &music_volume, 1.0f);
	shim::music_volume = music_volume * music_amp;
	
	easy_combos = root->get_nested_bool("misc>easy_combos", &easy_combos, true);
	simple_turn_display = root->get_nested_bool("misc>simple_turn_display", &simple_turn_display, false);

	extra_args = root->get_nested_string("misc>extra_args", &extra_args, "", true, true);
	extra_args_orig = extra_args;
	
	return loaded;
}

std::string save_filename(int slot, std::string prefix)
{
#ifdef TVOS
	return prefix + util::itos(slot+1) + ".dat";
#else
	return save_dir() + "/" + prefix + util::itos(slot+1) + ".dat";
#endif
}

util::JSON *load_savegame(int slot, std::string prefix)
{
	std::string filename = save_filename(slot, prefix);
	return wedge::load_savegame(filename);
}

std::string play_time_to_string(int time)
{
	int minutes = (time / 60) % 60;
	int hours = (time / 60 / 60) % 24;
	int days = time / 60 / 60 / 24;

	std::string s;

	if (days > 0) {
		s += util::string_printf("%d:", days);
		s += util::string_printf("%02d:", hours);
		s += util::string_printf("%02d", minutes);

	}
	else if (hours > 0) {
		s += util::string_printf("%d:", hours);
		s += util::string_printf("%02d", minutes);
	}
	else {
		s = util::string_printf("%dm", minutes);
	}

	return s;
}

bool can_show_settings(bool check_events, bool can_be_moving, bool only_initialised_dialogues_block, bool allow_paused_presses_if_changing_areas)
{
	if (check_events) {
		// if there are other key/joystick button/mouse button events in the queue, don't open settings
		// this is to prevent settings/dialogue or settings/battle action taking place at once
		SDL_Event events[100];
		int n;
		n = SDL_PeepEvents(&events[0], 100, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
		for (int i = 0; i < n; i++) {
			SDL_Event *e = &events[i];
			if (e->type == SDL_KEYDOWN || e->type == SDL_JOYBUTTONDOWN || e->type == SDL_MOUSEBUTTONDOWN) {
				return false;
			}
		}
	}

	/* FIXME: uncomment inn_active */
	if (AREA == NULL || settings_active() || /*inn_active() ||*/ (SHOP == NULL && GLOBALS->dialogue_active(AREA, only_initialised_dialogues_block)) || (BATTLE && GLOBALS->dialogue_active(BATTLE)) || (BATTLE == NULL && (wedge::are_presses_paused() && (allow_paused_presses_if_changing_areas == false || (AREA && AREA->changing_areas() == false)))) || AREA->is_pausing() || AREA->has_pause_ended()) {
		return false;
	}

	wedge::Map_Entity *player1 = AREA->get_player(0);
	wedge::Map_Entity_Input_Step *player1_input_step = player1->get_input_step();
	wedge::Map_Entity *player2 = INSTANCE->party_following_player ? AREA->get_player(1) : NULL;
	wedge::Map_Entity *player3 = INSTANCE->party_following_player ? AREA->get_player(2) : NULL;

	if (can_be_moving == false && (player1_input_step->is_following_path() || player1_input_step->get_movement_step()->is_moving() || (player2 != NULL && player2->get_input_step()->is_following_path()) || (player3 != NULL && player3->get_input_step()->is_following_path()))) {
		return false;
	}

	return true;
}

void get_save_info(int number, bool &__exists, bool &_corrupt, std::string &_time_text, std::string &_place_text, int &difficulty, std::string prefix)
{
	__exists = true;
	_corrupt = false;

	util::JSON *json = NULL;
	try {
		json = load_savegame(number, prefix);
	}
	catch (util::FileNotFoundError &e) {
		_time_text = GLOBALS->game_t->translate(515)/* Originally: - EMPTY - */;
		_place_text = "";
		__exists = false;
		_corrupt = false;
	}
	catch (util::Error &e) {
		util::debugmsg("Corrupt save, error=%s\n", e.error_message.c_str());
		_time_text = GLOBALS->game_t->translate(516)/* Originally: *** CORRUPT *** */;
		_place_text = "";
		__exists = true;
		_corrupt = true;
	}

	if (__exists && !_corrupt) {
		util::JSON::Node *root = json->get_root();
		util::JSON::Node *n;
		if (root) {
			n = root->find("game");
			if (n) {
				n = n->find("play_time");
				if (n) {
					_time_text = play_time_to_string(n->as_int());
					n = root->find("areas");
					if (n) {
						if (n->children.size() > 0) {
							n = n->children[0];
							std::string place_name = n->key;
							__exists = true;
							_place_text = get_friendly_name(place_name);
						}
						else {
							__exists = false;
						}
					}
					else {
						__exists = false;
					}
				}
				else {
					__exists = false;
				}
			}
			else {
				__exists = false;
			}
			n = root->find("difficulty");
			if (n) {
				difficulty = n->as_int();
			}
			else {
				__exists = false;
			}
		}
		else {
			__exists = false;
		}
		if (__exists == false) {
			_corrupt = true;
			_time_text = GLOBALS->game_t->translate(516)/* Originally: *** CORRUPT *** */;
			_place_text = "";
		}
		else {
			_corrupt = false;
		}

		delete json;
	}
}

bool save()
{
	std::string filename = save_filename(DOGO_GLOBALS->save_slot);
	return wedge::save(filename);
}

SDL_Colour selected_colour(SDL_Colour colour1, SDL_Colour colour2)
{
	SDL_Colour col;
	SDL_Colour c1 = colour1;
	SDL_Colour c2 = colour2;
	int dr = int(c2.r) - int(c1.r);
	int dg = int(c2.g) - int(c1.g);
	int db = int(c2.b) - int(c1.b);
	int da = int(c2.a) - int(c1.a);
	Uint32 t = GET_TICKS() % 1000;
	float p;
	if (t < 500) {
		p = t / 500.0f;
	}
	else {
		p = 1.0f - ((t - 500) / 500.0f);
	}
	p = sin(p * M_PI / 2.0f);
	col.r = c1.r + dr * p;
	col.g = c1.g + dg * p;
	col.b = c1.b + db * p;
	col.a = c1.a + da * p;
	return col;
}

void delete_shim_args()
{
	for (int i = 0; i < shim::argc; i++) {
		delete[] shim::argv[i];
	}
	delete[] shim::argv;
	shim::argc = 0;
	shim::argv = NULL;
}

static int set_orig_args(bool forced, bool count_only)
{
	int count = 0;

	for (int i = 0; i < orig_argc; i++) {
		int skip = 0;
		if (forced &&
			(std::string(orig_argv[i]) == "+windowed" ||
			std::string(orig_argv[i]) == "+fullscreen")) {
			skip = 1;
		}
		else if (forced &&
			(std::string(orig_argv[i]) == "+width" ||
			std::string(orig_argv[i]) == "+height" ||
			std::string(orig_argv[i]) == "+scale")) {
			skip = 2;
		}

		if (skip > 0) {
			i += skip-1;
			continue;
		}

		if (count_only == false) {
			shim::argv[count] = new char[strlen(orig_argv[i])+1];
			strcpy(shim::argv[count], orig_argv[i]);
		}
		
		count++;
	}

	return count;
}

void set_shim_args(bool initial, bool force_windowed, bool force_fullscreen)
{
	if (initial) {
		for (int i = 0; i < orig_argc; i++) {
			if (std::string(orig_argv[i]) == "+windowed" || std::string(orig_argv[i]) == "+fullscreen" || std::string(orig_argv[i]) == "+width" || std::string(orig_argv[i]) == "+height" || std::string(orig_argv[i]) == "+adapter") {
				force_windowed = false;
				force_fullscreen = false;
				break;
			}
		}
	}

	bool force = force_windowed || force_fullscreen;
	
	int count = set_orig_args(force, true);

	if (force) {
		count++;
	}

	std::vector<std::string> v;
	if (extra_args != "") {
		util::Tokenizer t(extra_args, ',');
		std::string tok;
		while ((tok = t.next()) != "") {
			v.push_back(tok);
		}
		count += v.size();
	}
	extra_args = ""; // Do this?

	shim::argc = count;
	shim::argv = new char *[count];

	int i = set_orig_args(force, false);

	if (force_windowed) {
		shim::argv[i] = new char[strlen("+windowed")+1];
		strcpy(shim::argv[i], "+windowed");
		i++;
	}
	else if (force_fullscreen) {
		shim::argv[i] = new char[strlen("+fullscreen")+1];
		strcpy(shim::argv[i], "+fullscreen");
		i++;
	}

	for (auto s : v) {
		shim::argv[i] = new char[s.length()+1];
		strcpy(shim::argv[i], s.c_str());
		i++;
	}
}

SDL_Colour brighten(SDL_Colour colour, float amount)
{
	int r = colour.r;
	int g = colour.g;
	int b = colour.b;
	r *= 1.0f + amount;
	g *= 1.0f + amount;
	b *= 1.0f + amount;
	r = MIN(255, r);
	g = MIN(255, g);
	b = MIN(255, b);
	colour.r = r;
	colour.g = g;
	colour.b = b;
	colour.a = colour.a;
	return colour;
}

std::string get_key_name(int code)
{
	std::string s = SDL_GetKeyName(code);
	s = util::uppercase(s);
	int id = GLOBALS->english_game_t->get_id(s);
	if (id == -1) {
		return s;
	}
	return GLOBALS->game_t->translate(id);
}

void show_notice(std::string text, bool flip)
{
	gfx::Font::end_batches();

	SDL_Colour tint = shim::black;
	tint.r *= 0.75f;
	tint.g *= 0.75f;
	tint.b *= 0.75f;
	tint.a *= 0.75f;

	gfx::draw_filled_rectangle(tint, util::Point<int>(0, 0), shim::screen_size);

	bool full;
	int num_lines, width;
	int line_height = DOGO_GLOBALS->bold_font->get_height() + 1;
	DOGO_GLOBALS->bold_font->draw_wrapped(shim::white, text, util::Point<int>(0, 0), 80, line_height, -1, -1, 0, true, full, num_lines, width);

	int w = width + WIN_BORDER*4;
	int h = line_height * num_lines + WIN_BORDER*4;
	int x = (shim::screen_size.w-w)/2;
	int y = (shim::screen_size.h-h)/2;
	
	//gfx::draw_filled_rectangle(shim::palette[38], util::Point<float>(x, y), util::Size<float>(w, h));
	gfx::draw_9patch(DOGO_GLOBALS->gui_window, util::Point<float>(x, y), util::Size<float>(w, h));

	DOGO_GLOBALS->bold_font->draw_wrapped(shim::white, text, util::Point<int>(x+WIN_BORDER*2, y+WIN_BORDER*2), 80, line_height, -1, -1, 0, false, full, num_lines, width);

	if (flip) {
		gfx::flip();
	}
}

void apply_tv_safe_mode(bool onoff)
{
	if (onoff) {
		shim::black_bar_percent = 0.05f;
	}
	else {
		shim::black_bar_percent = 0.0f;
	}
	gfx::set_screen_size(shim::real_screen_size);
	gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
	gfx::update_projection();
}

Combo get_combo(std::string name)
{
	Combo c;
	Combo_Event e;
	name = util::lowercase(name);
	// FIXME: remove spaces from combo name

	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	if (name == "punch") {
		e.button = B_X;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
	}
	else if (name == "kick") {
		e.button = B_A;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
	}
	else if (name == "roundhouse") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
	}
	else if (name == "uppercut") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "fierce punch") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
	}
	else if (name == "fierce kick") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
	}
	else if (name == "combo punch") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "combo kick") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
	}
	else if (name == "fast") {
		e.button = B_L;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		c.push_back(e);
		c.push_back(e);
	}
	else if (name == "stomp") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
	}
	else if (name == "cannonball") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
	}
	else if (name == "headbutt") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
	}
	else if (name == "axe hammer") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "dropkick") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
	}
	else if (name == "elbow drop") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "cyclone") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "spin kick") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_A;
		c.push_back(e);
	}
	else if (name == "bellyflop") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
	}
	else if (name == "dive bomb") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
	}
	else if (name == "seering slap") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
	}
	else if (name == "burning boot") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
	}
	else if (name == "karate chop") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "thunderclap") {
		e.button = B_L;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "cartwheel") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "beatdown") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "windmill") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		c.push_back(e);
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "the claw") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_X;
		c.push_back(e);
	}
	else if (name == "heal") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		c.push_back(e);
		c.push_back(e);
	}
	else if (name == "trout slap") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
	}
	else if (name == "twister") {
		e.button = B_L;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
	}
	else if (name == "kick flip") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
	}
	else if (name == "big stomp") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
	}
	else if (name == "nose breaker") {
		e.button = B_R;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
	}
	else if (name == "timberrr") {
		e.button = B_U;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_U;
		c.push_back(e);
	}
	else if (name == "limbs a flailin") {
		e.button = B_L;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_Y;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
		e.button = B_L;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
		e.button = B_R;
		c.push_back(e);
		e.button = B_B;
		c.push_back(e);
	}
	else if (name == "defend") {
		e.button = B_D;
		e.min_hold = 0;
		e.max_hold = 500;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
		e.button = B_D;
		c.push_back(e);
	}

	for (size_t i = 0; i < c.size(); i++) {
		c[i].max_hold *= combo_timing_mul();
	}

	return c;
}

audio::Sound *get_combo_sfx(std::string name)
{
	name = util::lowercase(name);
	// FIXME: remove spaces from combo name
	
	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	if (name == "punch") {
		return DOGO_GLOBALS->melee;
	}
	else if (name == "kick") {
		return DOGO_GLOBALS->melee;
	}
	else if (name == "roundhouse") {
		return DOGO_GLOBALS->melee2;
	}
	else if (name == "uppercut") {
		return DOGO_GLOBALS->melee2;
	}
	else if (name == "fierce_punch") {
		return DOGO_GLOBALS->melee2;
	}
	else if (name == "fierce_kick") {
		return DOGO_GLOBALS->melee2;
	}
	else if (name == "combo_punch") {
		return DOGO_GLOBALS->melee_combo;
	}
	else if (name == "combo_kick") {
		return DOGO_GLOBALS->melee_combo;
	}
	else if (name == "headbutt") {
		return DOGO_GLOBALS->melee;
	}
	else if (name == "stomp") {
		return DOGO_GLOBALS->stomp;
	}
	else if (name == "cannonball") {
		return DOGO_GLOBALS->cannonball;
	}
	else if (name == "fast") {
		return DOGO_GLOBALS->fast;
	}
	else if (name == "axe_hammer") {
		return DOGO_GLOBALS->melee_wrestling;
	}
	else if (name == "dropkick") {
		return DOGO_GLOBALS->melee_wrestling;
	}
	else if (name == "elbow_drop") {
		return DOGO_GLOBALS->melee_wrestling;
	}
	else if (name == "cyclone") {
		return DOGO_GLOBALS->melee_spin;
	}
	else if (name == "spin_kick") {
		return DOGO_GLOBALS->melee_spin;
	}
	else if (name == "bellyflop") {
		return DOGO_GLOBALS->bellyflop;
	}
	else if (name == "dive_bomb") {
		return DOGO_GLOBALS->dive_bomb;
	}
	else if (name == "seering_slap") {
		return DOGO_GLOBALS->seering_slap;
	}
	else if (name == "burning_boot") {
		return DOGO_GLOBALS->seering_slap;
	}
	else if (name == "karate_chop") {
		return DOGO_GLOBALS->melee;
	}
	else if (name == "thunderclap") {
		return DOGO_GLOBALS->thunderclap;
	}
	else if (name == "cartwheel") {
		return DOGO_GLOBALS->cartwheel;
	}
	else if (name == "beatdown") {
		return DOGO_GLOBALS->beatdown;
	}
	else if (name == "windmill") {
		return DOGO_GLOBALS->windmill;
	}
	else if (name == "the_claw") {
		return DOGO_GLOBALS->the_claw;
	}
	else if (name == "heal") {
		return DOGO_GLOBALS->heal_sound;
	}
	else if (name == "trout_slap") {
		return DOGO_GLOBALS->trout_slap;
	}
	else if (name == "twister") {
		return DOGO_GLOBALS->twister;
	}
	else if (name == "kick_flip") {
		return DOGO_GLOBALS->kick_flip;
	}
	else if (name == "big_stomp") {
		return DOGO_GLOBALS->big_stomp;
	}
	else if (name == "nose_breaker") {
		return DOGO_GLOBALS->nose_breaker;
	}
	else if (name == "timberrr") {
		return DOGO_GLOBALS->timber;
	}
	else if (name == "limbs_a_flailin") {
		return DOGO_GLOBALS->limbs_a_flailin;
	}
	else if (name == "defend") {
		return DOGO_GLOBALS->defend;
	}

	return nullptr;
}

void do_combo_screen_shake(std::string name)
{
	name = util::lowercase(name);
	// FIXME: remove spaces from combo name

	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	float amount = 0.0f;
	Uint32 length = 0;
	int delay = 0;

	if (name == "punch") {
	}
	else if (name == "kick") {
	}
	else if (name == "roundhouse") {
	}
	else if (name == "uppercut") {
	}
	else if (name == "fierce_punch") {
	}
	else if (name == "fierce_kick") {
	}
	else if (name == "combo_punch") {
	}
	else if (name == "combo_kick") {
	}
	else if (name == "headbutt") {
	}
	else if (name == "stomp") {
	}
	else if (name == "cannonball") {
	}
	else if (name == "fast") {
	}
	else if (name == "seering_slap") {
	}
	else if (name == "burning_boot") {
	}
	else if (name == "axe_hammer") {
		amount = 0.75f;
		length = 350;
		delay = 900;
	}
	else if (name == "dropkick") {
		amount = 0.75f;
		length = 350;
		delay = 800;
	}
	else if (name == "elbow_drop") {
		amount = 0.75f;
		length = 350;
		delay = 850;
	}
	else if (name == "bellyflop") {
		amount = 1.0f;
		length = 400;
		delay = 1000;
	}
	else if (name == "dive_bomb") {
		amount = 1.0f;
		length = 500;
		delay = 1100;
	}
	else if (name == "beatdown") {
		amount = 0.75f;
		length = 1000;
		delay = 575;
	}
	else if (name == "windmill") {
		amount = 1.0f;
		length = 500;
		delay = 1250;
	}
	else if (name == "the_claw") {
		amount = 1.0f;
		length = 550;
		delay = 200;
	}
	else if (name == "trout_slap") {
		amount = 1.0f;
		length = 600;
		delay = 1675;
	}
	else if (name == "twister") {
		amount = 1.0f;
		length = 1200;
		delay = 450;
	}
	else if (name == "kick_flip") {
		amount = 1.0f;
		length = 500;
		delay = 575;
	}
	else if (name == "big_stomp") {
		amount = 1.0f;
		length = 750;
		delay = 975;
	}
	else if (name == "nose_breaker") {
		amount = 1.0f;
		length = 600;
		delay = 542;
	}
	else if (name == "timberrr") {
		amount = 1.0f;
		length = 1000;
		delay = 500;
	}
	else if (name == "limbs_a_flailin") {
		amount = 1.0f;
		length = 1500;
		delay = 264;
	}
	else if (name == "defend") {
		amount = 2.0f;
		length = 250;
		delay = 0;
	}

	if (length > 0) {
		wedge::Game *game;
		if (MENU) {
			game = MENU;
		}
		else if (BATTLE) {
			game = BATTLE;
		}
		else {
			game = AREA;
		}

		NEW_SYSTEM_AND_TASK(game)

		ADD_STEP(new wedge::Delay_Step(delay, new_task))
		ADD_STEP(new wedge::Screen_Shake_Step(amount, length, new_task))

		ADD_TASK(new_task)

		FINISH_SYSTEM(game)
	}
}

bool get_combo_multi(std::string name)
{
	name = util::lowercase(name);
	
	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	if (name == "dive bomb") {
		return true;
	}
	else if (name == "seering slap") {
		return true;
	}
	else if (name == "burning boot") {
		return true;
	}
	else if (name == "heal") {
		return true;
	}
	else if (name == "trout slap") {
		return true;
	}
	else if (name == "twister") {
		return true;
	}
	else if (name == "kick flip") {
		return true;
	}
	else if (name == "big stomp") {
		return true;
	}
	else if (name == "nose breaker") {
		return true;
	}
	else if (name == "timberrr") {
		return true;
	}
	else if (name == "limbs a flailin") {
		return true;
	}
	else if (name == "defend") {
		return false;
	}

	return false;
}

int get_combo_cost(std::string name)
{
	name = util::lowercase(name);
	// FIXME: remove spaces from combo name

	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	if (name == "roundhouse") {
		return 5;
	}
	else if (name == "uppercut") {
		return 5;
	}
	else if (name == "fierce punch") {
		return 10;
	}
	else if (name == "fierce kick") {
		return 10;
	}
	else if (name == "combo punch") {
		return 15;
	}
	else if (name == "combo kick") {
		return 15;
	}
	else if (name == "headbutt") {
		return 0;
	}
	else if (name == "stomp") {
		return 10;
	}
	else if (name == "cannonball") {
		return 15;
	}
	else if (name == "fast") {
		return 15;
	}
	else if (name == "axe hammer") {
		return 20;
	}
	else if (name == "dropkick") {
		return 20;
	}
	else if (name == "elbow drop") {
		return 20;
	}
	else if (name == "cyclone") {
		return 25;
	}
	else if (name == "spin kick") {
		return 25;
	}
	else if (name == "bellyflop") {
		return 25;
	}
	else if (name == "dive bomb") {
		return 30;
	}
	else if (name == "seering slap") {
		return 40;
	}
	else if (name == "burning boot") {
		return 40;
	}
	else if (name == "karate chop") {
		return 0;
	}
	else if (name == "thunderclap") {
		return 10;
	}
	else if (name == "cartwheel") {
		return 15;
	}
	else if (name == "beatdown") {
		return 20;
	}
	else if (name == "windmill") {
		return 25;
	}
	else if (name == "the claw") {
		return 30;
	}
	else if (name == "heal") {
		return 40;
	}
	else if (name == "trout slap") {
		return 50;
	}
	else if (name == "twister") {
		return 50;
	}
	else if (name == "kick flip") {
		return 50;
	}
	else if (name == "big stomp") {
		return 60;
	}
	else if (name == "nose breaker") {
		return 60;
	}
	else if (name == "timberrr") {
		return 60;
	}
	else if (name == "limbs a flailin") {
		return 75;
	}
	else if (name == "defend") {
		return 100;
	}

	return 0;
}

float get_combo_multiplier(std::string name)
{
	name = util::lowercase(name);
	// FIXME: remove spaces from combo name

	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	if (name == "roundhouse") {
		return 2.0f;
	}
	else if (name == "uppercut") {
		return 2.0f;
	}
	else if (name == "fierce_punch") {
		return 4.0f;
	}
	else if (name == "fierce_kick") {
		return 4.0f;
	}
	else if (name == "combo_punch") {
		return 6.0f;
	}
	else if (name == "combo_kick") {
		return 6.0f;
	}
	else if (name == "headbutt") {
		return 1.25f;
	}
	else if (name == "stomp") {
		return 4.25f;
	}
	else if (name == "cannonball") {
		return 6.25f;
	}
	else if (name == "axe_hammer") {
		return 7.5f;
	}
	else if (name == "dropkick") {
		return 7.5f;
	}
	else if (name == "elbow_drop") {
		return 7.5f;
	}
	else if (name == "cyclone") {
		return 8.0f;
	}
	else if (name == "spin_kick") {
		return 8.0f;
	}
	else if (name == "bellyflop") {
		return 8.25f;
	}
	else if (name == "dive_bomb") {
		return 9.0f;
	}
	else if (name == "seering_slap") {
		return 9.0f;
	}
	else if (name == "burning_boot") {
		return 9.0f;
	}
	else if (name == "karate_chop") {
		return 2.0f;
	}
	else if (name == "thunderclap") {
		return 4.25f;
	}
	else if (name == "cartwheel") {
		return 6.25f;
	}
	else if (name == "beatdown") {
		return 7.5f;
	}
	else if (name == "windmill") {
		return 8.25f;
	}
	else if (name == "the_claw") {
		return 9.25f;
	}
	else if (name == "trout_slap") {
		return 10.0f;
	}
	else if (name == "twister") {
		return 10.0f;
	}
	else if (name == "kick_flip") {
		return 10.0f;
	}
	else if (name == "big_stomp") {
		return 11.0f;
	}
	else if (name == "nose_breaker") {
		return 11.0f;
	}
	else if (name == "timberrr") {
		return 11.0f;
	}
	else if (name == "limbs_a_flailin") {
		return 12.0f;
	}

	return 1.0f;
}

bool get_combo_friendly(std::string name)
{
	name = util::lowercase(name);

	int pos;
	if ((pos = name.find('\'')) != std::string::npos) {
		name = name.substr(0, pos) + name.substr(pos+1);
	}

	if (name == "fast") {
		return true;
	}
	else if (name == "heal") {
		return true;
	}
	
	return false;
}

void get_use_item_info(int amount, int id, SDL_Colour &colour, SDL_Colour &shadow_colour, std::string &text)
{
	if (amount < 0) {
		colour = shim::palette[13];
		//shadow_colour = shim::palette[17];
		shadow_colour = shim::black;
		text = util::itos(abs(amount));
	}
	// FIXME: ADD NEW VEGGIES
	else if (id == ITEM_CARROT || id == ITEM_TURNIP || id == ITEM_HOT_PEPPER) {
		colour = shim::palette[30];
		//shadow_colour = shim::palette[22];
		shadow_colour = shim::black;
		//text = GLOBALS->game_t->translate(154)/* Originally: SP! */;
		text = util::itos(amount);
	}
	else if (amount > 0) {
		colour = shim::palette[5];
		//shadow_colour = shim::palette[1];
		shadow_colour = shim::black;
		text = util::itos(amount);
	}
	else {
		if (id == ITEM_CARAMEL) {
			colour = shim::palette[5];
			//shadow_colour = shim::palette[1];
			shadow_colour = shim::black;
			text = GLOBALS->game_t->translate(155)/* Originally: LIFE! */;
		}
		else if (id == ITEM_RED_POTION || id == ITEM_BLUE_POTION || id == ITEM_GREEN_POTION) {
			colour = shim::palette[8];
			//shadow_colour = shim::palette[60];
			shadow_colour = shim::black;
			text = GLOBALS->game_t->translate(353)/* Originally: EXTRA! */;
		}
		else {
			colour = shim::white;
			colour.a = 0;
			//shadow_colour = shim::white;
			//shadow_colour.a = 0;
			shadow_colour = shim::black;
			text = "";
		}
	}
}

bool settings_active()
{
	for (size_t i = 0; i < shim::guis.size(); i++) {
		gui::GUI *g = shim::guis[i];
		if (
			dynamic_cast<Settings_GUI *>(g) ||
			dynamic_cast<Language_GUI *>(g) ||
			dynamic_cast<Video_Settings_GUI *>(g) ||
			dynamic_cast<Audio_Settings_GUI *>(g) ||
			dynamic_cast<Controls_Settings_GUI *>(g) ||
			dynamic_cast<Miscellaneous_Settings_GUI *>(g) ||
			dynamic_cast<Save_Slot_GUI *>(g)
		) {
			return true;
		}
	}
	return false;
}

void draw_tinted_shadowed_image(SDL_Colour tint, SDL_Colour shadow_colour, gfx::Image *image, util::Point<float> dest_pos, gfx::Font::Shadow_Type shadow_type)
{
	gfx::enable_depth_test(true);
	gfx::enable_depth_write(true);
	gfx::clear_depth_buffer(1.0f);
	gfx::set_depth_mode(gfx::COMPARE_NOTEQUAL);
	if (shadow_type != gfx::Font::NO_SHADOW) {
		shim::current_shader = DOGO_GLOBALS->solid_colour_shader;
		shim::current_shader->use();
		gfx::update_projection();
		shim::current_shader->set_colour("solid_colour", shadow_colour);
		image->start_batch();
		if (shadow_type == gfx::Font::DROP_SHADOW) {
			image->draw(dest_pos+util::Point<int>(1, 0));
			image->draw(dest_pos+util::Point<int>(0, 1));
			image->draw(dest_pos+util::Point<int>(1, 1));
		}
		else if (shadow_type == gfx::Font::FULL_SHADOW) {
			image->draw(dest_pos+util::Point<int>(1, 0));
			image->draw(dest_pos+util::Point<int>(-1, 0));
			image->draw(dest_pos+util::Point<int>(0, 1));
			image->draw(dest_pos+util::Point<int>(0, -1));
			image->draw(dest_pos+util::Point<int>(1, 1));
			image->draw(dest_pos+util::Point<int>(-1, 1));
			image->draw(dest_pos+util::Point<int>(1, -1));
			image->draw(dest_pos+util::Point<int>(-1, -1));
		}
		image->end_batch();
		shim::current_shader = shim::default_shader;
		shim::current_shader->use();
		gfx::update_projection();
	}
	gfx::enable_depth_test(false);
	gfx::enable_depth_write(false);
	gfx::set_depth_mode(gfx::COMPARE_LESSEQUAL);
	image->draw_tinted(tint, dest_pos);
}

void get_gauge_colour(float p, SDL_Colour& colour1, SDL_Colour& colour2)
{
	if (p >= 0.5f) {
		colour1 = shim::palette[31];
		colour2 = shim::palette[28];
	}
	else if (p >= 0.25f) {
		colour1 = shim::palette[8];
		colour2 = shim::palette[10];
	}
	else {
		colour1 = shim::palette[13];
		colour2 = shim::palette[15];
	}
}

SDL_Colour blend_colours(SDL_Colour a, SDL_Colour b, float p)
{
	a.r *= p;
	a.g *= p;
	a.b *= p;
	a.a *= p;
	p = 1.0f - p;
	b.r *= p;
	b.g *= p;
	b.b *= p;
	b.a *= p;
	a.r += b.r;
	a.g += b.g;
	a.b += b.b;
	a.a += b.a;
	return a;
}

// FIXME: add all players/belts
std::vector<std::string> get_belt_moves(std::string player_name, int belt)
{
	std::vector<std::string> moves;

	if (belt == BELT_WHITE) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
		}
		else if (player_name == "Tik") {
			moves.push_back("Stomp");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
		}
	}
	else if (belt == BELT_YELLOW) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
		}
		else if (player_name == "Tik") {
			moves.push_back("Stomp");
			moves.push_back("Cannonball");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
		}
	}
	else if (belt == BELT_GOLD) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
		}
		else if (player_name == "Tik") {
			moves.push_back("Stomp");
			moves.push_back("Cannonball");
			moves.push_back("Fast");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
		}
	}
	else if (belt == BELT_ORANGE) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
		}
		else if (player_name == "Tik") {
			moves.push_back("Stomp");
			moves.push_back("Cannonball");
			moves.push_back("Fast");
			moves.push_back("Elbow Drop");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
		}
	}
	else if (belt == BELT_GREEN) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
			moves.push_back("Cyclone");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
			moves.push_back("Spin Kick");
		}
		else if (player_name == "Tik") {
			moves.push_back("Stomp");
			moves.push_back("Cannonball");
			moves.push_back("Fast");
			moves.push_back("Elbow Drop");
			moves.push_back("Bellyflop");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
			moves.push_back("The Claw");
		}
	}
	else if (belt == BELT_BLUE) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
			moves.push_back("Cyclone");
			moves.push_back("Seering Slap");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
			moves.push_back("Spin Kick");
			moves.push_back("Burning Boot");
		}
		else if (player_name == "Tik") {
			moves.push_back("Stomp");
			moves.push_back("Cannonball");
			moves.push_back("Fast");
			moves.push_back("Elbow Drop");
			moves.push_back("Bellyflop");
			moves.push_back("Dive Bomb");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
			moves.push_back("The Claw");
			moves.push_back("Heal");
		}
	}
	else if (belt == BELT_PURPLE) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
			moves.push_back("Cyclone");
			moves.push_back("Seering Slap");
			moves.push_back("Trout Slap");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
			moves.push_back("Spin Kick");
			moves.push_back("Burning Boot");
			moves.push_back("Kick Flip");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
			moves.push_back("The Claw");
			moves.push_back("Heal");
			moves.push_back("Twister");
		}
	}
	else if (belt == BELT_BROWN) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
			moves.push_back("Cyclone");
			moves.push_back("Seering Slap");
			moves.push_back("Trout Slap");
			moves.push_back("Nose Breaker");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
			moves.push_back("Spin Kick");
			moves.push_back("Burning Boot");
			moves.push_back("Kick Flip");
			moves.push_back("Big Stomp");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
			moves.push_back("The Claw");
			moves.push_back("Heal");
			moves.push_back("Twister");
			moves.push_back("Timberrr");
		}
	}
	else if (belt == BELT_RED) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
			moves.push_back("Cyclone");
			moves.push_back("Seering Slap");
			moves.push_back("Trout Slap");
			moves.push_back("Nose Breaker");
			moves.push_back("Limbs a Flailin'");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
			moves.push_back("Spin Kick");
			moves.push_back("Burning Boot");
			moves.push_back("Kick Flip");
			moves.push_back("Big Stomp");
			moves.push_back("Limbs a Flailin'");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
			moves.push_back("The Claw");
			moves.push_back("Heal");
			moves.push_back("Twister");
			moves.push_back("Timberrr");
			moves.push_back("Limbs a Flailin'");
		}
	}
	else if (belt == BELT_BLACK) {
		if (player_name == "Pleasant") {
			moves.push_back("Uppercut");
			moves.push_back("Fierce Punch");
			moves.push_back("Combo Punch");
			moves.push_back("Axe Hammer");
			moves.push_back("Cyclone");
			moves.push_back("Seering Slap");
			moves.push_back("Trout Slap");
			moves.push_back("Nose Breaker");
			moves.push_back("Limbs a Flailin'");
			moves.push_back("Defend");
		}
		else if (player_name == "Tom") {
			moves.push_back("Roundhouse");
			moves.push_back("Fierce Kick");
			moves.push_back("Combo Kick");
			moves.push_back("Dropkick");
			moves.push_back("Spin Kick");
			moves.push_back("Burning Boot");
			moves.push_back("Kick Flip");
			moves.push_back("Big Stomp");
			moves.push_back("Limbs a Flailin'");
			moves.push_back("Defend");
		}
		else if (player_name == "Wit") {
			moves.push_back("Thunderclap");
			moves.push_back("Cartwheel");
			moves.push_back("Beatdown");
			moves.push_back("Windmill");
			moves.push_back("The Claw");
			moves.push_back("Heal");
			moves.push_back("Twister");
			moves.push_back("Timberrr");
			moves.push_back("Limbs a Flailin'");
			moves.push_back("Defend");
		}
	}
	
	return moves;
}

bool for_use_in_battle(int id)
{
	if (id == ITEM_NINJA_STAR) {
		return true;
	}
	else if (id == ITEM_DAGGER) {
		return true;
	}
	else if (id == ITEM_RANDOMAXE) {
		return true;
	}
	return false;
}

static void dodge_callback(void *data)
{
	auto e = static_cast<wedge::Battle_Entity *>(data);
	auto sprite = e->get_sprite();
	if (sprite->get_animation() == "dodge") {
		e->get_sprite()->set_animation("idle");
	}
}

bool add_special_number(wedge::Battle_Entity *actor, wedge::Battle_Entity *target, int damage, bool lucky_misses, bool play_sounds)
{
	bool ret = false;

	SDL_Colour colour;
	SDL_Colour shadow_colour;
		
	audio::Sound *hit_sfx;
	audio::Sound *hit_sfx2 = nullptr;
		
	if (dynamic_cast<Enemy_Coinman *>(target) != nullptr) {
		hit_sfx = DOGO_GLOBALS->coin_hit;
		hit_sfx2 = DOGO_GLOBALS->hit;
	}
	else {
		hit_sfx = DOGO_GLOBALS->hit;
	}

	wedge::Base_Stats *actor_stats = actor->get_stats();
	wedge::Base_Stats *target_stats = target->get_stats();

	if (lucky_misses) {
		int actor_luck = actor_stats->fixed.get_extra(LUCK);
		int target_luck = target_stats->fixed.get_extra(LUCK);
		int diff = actor_luck - target_luck;

		int r = util::rand(0, 99);
		int r2 = util::rand(0, 99);

		int l_max;
		int m_max;

		if (diff >= 0) {
			l_max = MAX(MIN(50, diff), 1);
			m_max = 1;
		}
		else {
			l_max = 1;
			m_max = MAX(MIN(50, -diff), 1);
		}

		bool do_dodge = false;

		if (dynamic_cast<Enemy_Red_Wasp *>(target)) {
			do_dodge = true;
			// if it's 0, it's a dodge
			r2 = util::rand(0, 3);
			m_max = 1;
		}

		float r3 = util::rand(0, 10);
		r3 -= 5.0f;
		damage *= (1.0f + r3/100.0f);
		damage = MAX(1, damage);

		if (r < l_max) {
			damage *= 1.25f;
			if (play_sounds) {
				hit_sfx->play(false);
				if (hit_sfx2 != nullptr) {
					hit_sfx2->play(false);
				}
			}
			colour = GLOBALS->strong_attack_colour;
			shadow_colour = GLOBALS->strong_attack_shadow;
		}
		else if (r2 < m_max) {
			ret = true;
			damage = 0;
			if (play_sounds) {
				DOGO_GLOBALS->shock->play(false);
			}
			colour = shim::palette[5];
			colour = GLOBALS->helpful_attack_colour;
			shadow_colour = GLOBALS->helpful_attack_shadow;
			if (do_dodge) {
				gfx::add_notification(GLOBALS->game_t->translate(366)/* Originally: Dodged! */);
				DOGO_GLOBALS->dodge->play(false);
				target->get_sprite()->set_animation("dodge", dodge_callback, target);
			}
		}
		else {
			if (play_sounds) {
				hit_sfx->play(false);
				if (hit_sfx2 != nullptr) {
					hit_sfx2->play(false);
				}
			}
			colour = GLOBALS->regular_attack_colour;
			shadow_colour = GLOBALS->regular_attack_shadow;
		}
	}
	else {
		colour = GLOBALS->regular_attack_colour;
		shadow_colour = GLOBALS->regular_attack_shadow;
		if (damage > 0 && play_sounds) {
			hit_sfx->play(false);
			if (hit_sfx2 != nullptr) {
				hit_sfx2->play(false);
			}
		}
	}
	
	if (target->get_sprite()->get_animation() == "attack_defend") {
		damage = 1;
	}

	std::string text = damage == 0 ? GLOBALS->game_t->translate(185)/* Originally: MISS! */ : util::itos(damage);

	util::Point<int> number_pos;

	if (dynamic_cast<Battle_Player *>(target)) {
		auto p = target;
		p->take_hit(actor, damage);
		auto player = dynamic_cast<Battle_Player *>(p);
		number_pos = player->get_draw_pos();
		number_pos.x -= (shim::font->get_text_width(text)+5);
		number_pos.y -= shim::tile_size/4;
	}
	else {
		auto p = target;
		p->take_hit(actor, damage);
		auto enemy = dynamic_cast<Battle_Enemy *>(p);
		number_pos = enemy->get_position();
		gfx::Sprite *sprite = enemy->get_sprite();
		util::Point<int> topleft, bottomright;
		sprite->get_bounds(topleft, bottomright);
		number_pos += topleft;
		number_pos.x += (bottomright.x-topleft.x) + 5;
		number_pos.y -= shim::tile_size/4;
	}
	if (BATTLE && static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
		number_pos.x = shim::screen_size.w - number_pos.x - shim::font->get_text_width(text);
	}
		
	NEW_SYSTEM_AND_TASK(BATTLE)
	wedge::Special_Number_Step *step = new wedge::Special_Number_Step(colour, shadow_colour, text, number_pos, damage == 0 ? wedge::Special_Number_Step::RISE : wedge::Special_Number_Step::SHAKE, new_task, false);
	ADD_STEP(step)
	ADD_TASK(new_task)
	if (damage != 0) {
		ANOTHER_TASK
		ADD_STEP(new Hit_Step(target, 0, new_task))
		ADD_TASK(new_task)
	}
	FINISH_SYSTEM(BATTLE)

	return ret;
}
	
void real_draw_darkness(gfx::Image *darkness_image1, gfx::Image *darkness_image2, util::Point<float> darkness_offset1, util::Point<float> darkness_offset2, util::Point<float> map_offset, float alpha)
{
	SDL_Colour tint;
	tint.r = 255 * alpha;
	tint.g = 255 * alpha;
	tint.b = 255 * alpha;
	tint.a = 255 * alpha;

	int x = int(darkness_offset1.x) % darkness_image1->size.w;
	int y = int(darkness_offset1.y) % darkness_image1->size.h;
	x -= darkness_image1->size.w;
	y -= darkness_image1->size.h;
	x += map_offset.x;
	y += map_offset.y;
	x = -(-x % darkness_image1->size.w);
	y = -(-y % darkness_image1->size.h);
	int w = shim::screen_size.w / darkness_image1->size.w + 2;
	int h = shim::screen_size.h / darkness_image1->size.h + 2;
	darkness_image1->start_batch();
	for (int yy = 0; yy < h; yy++) {
		for (int xx = 0; xx < w; xx++) {
			darkness_image1->draw_tinted(tint, util::Point<int>(x + xx*darkness_image1->size.w, y + yy*darkness_image1->size.h));
		}
	}
	darkness_image1->end_batch();

	x = int(darkness_offset2.x) % darkness_image2->size.w;
	y = int(darkness_offset2.y) % darkness_image2->size.h;
	x -= darkness_image1->size.w;
	y -= darkness_image1->size.h;
	x += map_offset.x;
	y += map_offset.y;
	x = -(-x % darkness_image2->size.w);
	y = -(-y % darkness_image2->size.h);
	w = shim::screen_size.w / darkness_image2->size.w + 2;
	h = shim::screen_size.h / darkness_image2->size.h + 2;
	darkness_image2->start_batch();
	for (int yy = 0; yy < h; yy++) {
		for (int xx = 0; xx < w; xx++) {
			darkness_image2->draw_tinted(tint, util::Point<int>(x + xx*darkness_image2->size.w, y + yy*darkness_image2->size.h));
		}
	}
	darkness_image2->end_batch();
}

void do_question(std::string tag, std::string text, wedge::Dialogue_Type type, std::vector<std::string> choices, wedge::Step *monitor, int escape_choice)
{
	wedge::Game *g;
	if (BATTLE) {
		g = BATTLE;
	}
	else {
		g = AREA;
	}

	NEW_SYSTEM_AND_TASK(g)
	Question_Step *q = new Question_Step(choices, escape_choice, new_task);
	if (monitor) {
		q->add_monitor(monitor);
	}
	ADD_STEP(q)
	ADD_TASK(new_task)
	ANOTHER_TASK
	Dialogue_Step *d = new Dialogue_Step(tag, text, type, wedge::DIALOGUE_TOP, new_task);
	d->set_dismissable(false);
	d->add_monitor(q);
	ADD_STEP(d)
	ADD_TASK(new_task)
	FINISH_SYSTEM(g)
}

bool save_play_time()
{
	if (DOGO_GLOBALS->save_slot < 0) {
		return false;
	}
	// If you load an autosave, don't save playtime to main save. You could e.g., load on old autosave and you wouldn't want that reflected on your main save.
	if (DOGO_GLOBALS->loaded_autosave) {
		return false;
	}
	std::string filename = save_filename(DOGO_GLOBALS->save_slot);
	return wedge::save_play_time(filename);
}

float enemy_speed_mul()
{
	switch (DOGO_INSTANCE->difficulty) {
		case DIFFICULTY_EASY:
			return 1.2f;
		case DIFFICULTY_NORMAL:
			return 1.1f;
		case DIFFICULTY_HARD:
			return 1.0f;
	}

	return 1.0f;
}

float enemy_attack_mul()
{
	switch (DOGO_INSTANCE->difficulty) {
		case DIFFICULTY_EASY:
			return 0.9f;
		case DIFFICULTY_NORMAL:
			return 1.0f;
		case DIFFICULTY_HARD:
			return 1.1f;
	}

	return 1.0f;
}

float enemy_defence_mul()
{
	switch (DOGO_INSTANCE->difficulty) {
		case DIFFICULTY_EASY:
			return 0.9f;
		case DIFFICULTY_NORMAL:
			return 1.0f;
		case DIFFICULTY_HARD:
			return 1.1f;
	}

	return 1.0f;
}

float combo_timing_mul()
{
	switch (DOGO_INSTANCE->difficulty) {
		case DIFFICULTY_EASY:
			return 3.0f;
		case DIFFICULTY_NORMAL:
			return 1.0f;
		case DIFFICULTY_HARD:
			return 0.8f;
	}

	return 1.0f;
}

void pop_player()
{
	AREA->pop_player();
	int sz = (int)INSTANCE->stats.size();
	delete INSTANCE->stats[sz-1].sprite;

	wedge::Object *inv = INSTANCE->inventory.get_all();
	
	for (int belt = BELT_WHITE; belt <= BELT_BLACK; belt++) {
		for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
			if (inv[i].type == wedge::OBJECT_ARMOUR && inv[i].id == belt) {
				inv[i].type = wedge::OBJECT_NONE;
				inv[i].id = wedge::ITEM_NONE;
				inv[i].quantity = 0;
				break;
			}
		}
	}
	
	for (int belt = BELT_WHITE; belt <= BELT_BLACK; belt++) {
		int count = 0;
		for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
			if (inv[i].type == wedge::OBJECT_ARMOUR && inv[i].id == belt) {
				count++;
			}
		}
		int equipped = 0;
		for (int i = 0; i < sz-1; i++) {
			if (INSTANCE->stats[i].armour.id == belt) {
				equipped++;
			}
		}
		int remove = equipped - count;
		for (int i = 0; i < remove; i++) {
			for (int j = sz-2; j >= 0; j--) {
				if (INSTANCE->stats[j].armour.id == belt) {
					std::string player_name = INSTANCE->stats[j].base.get_name();
					std::vector<std::string> belt_moves = get_belt_moves(player_name, INSTANCE->stats[j].armour.id);
					remove_belt(j, belt_moves);
					INSTANCE->stats[j].armour = wedge::Armour_Stats(wedge::ARMOUR_NONE);
					break;
				}
			}
		}
	}

	INSTANCE->stats.pop_back();
	DOGO_INSTANCE->combos.pop_back();
}

int get_profile_pic_index(int player_index)
{
	if (player_index == 2) {
		if (INSTANCE->stats[2].base.get_name() == "Tik") {
			return TIK;
		}
		else {
			return WIT;
		}
	}
	else {
		return player_index;
	}
}

void remove_belt(int player_index, std::vector<std::string> moves)
{
	std::vector<std::string> &v = DOGO_INSTANCE->combos[player_index];

	for (auto it = v.begin(); it != v.end();) {
		std::string s = *it;
		if (std::find(moves.begin(), moves.end(), s) != moves.end()) {
			it = v.erase(it);
		}
		else {
			it++;
		}
	}
}

void cancel_all_screen_shake(wedge::Game *game)
{
	wedge::System_List systems = game->get_systems();
	for (wedge::System_List::iterator it = systems.begin(); it != systems.end(); it++) {
		wedge::System *system = *it;
		wedge::Task_List tasks = system->get_tasks();
		for (wedge::Task_List::iterator it2 = tasks.begin(); it2 != tasks.end(); it2++) {
			wedge::Task *task = *it2;
			wedge::Step_List steps = task->get_steps();
			for (wedge::Step_List::iterator it3 = steps.begin(); it3 != steps.end(); it3++) {
				auto ss = dynamic_cast<wedge::Screen_Shake_Step *>(*it3);
				if (ss) {
					ss->set_cancelled(true);
				}
			}
		}
	}

	gfx::screen_shake(0, 0);
}

void cancel_all_special_numbers(wedge::Game *game)
{
	wedge::System_List systems = game->get_systems();
	for (wedge::System_List::iterator it = systems.begin(); it != systems.end(); it++) {
		wedge::System *system = *it;
		wedge::Task_List tasks = system->get_tasks();
		for (wedge::Task_List::iterator it2 = tasks.begin(); it2 != tasks.end(); it2++) {
			wedge::Task *task = *it2;
			wedge::Step_List steps = task->get_steps();
			for (wedge::Step_List::iterator it3 = steps.begin(); it3 != steps.end(); it3++) {
				auto sn = dynamic_cast<wedge::Special_Number_Step *>(*it3);
				if (sn) {
					sn->set_cancelled(true);
				}
			}
		}
	}
}
