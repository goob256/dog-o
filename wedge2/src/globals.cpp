#include "shim2/shaders/glsl/default_vertex.h"
#include "shim2/shaders/glsl/default_fragment.h"

#ifdef _WIN32
#include "shim2/shaders/hlsl/default_vertex.h"
#include "shim2/shaders/hlsl/default_fragment.h"
#endif

#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/battle_game.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/input.h"
#include "wedge2/map_entity.h"
#include "wedge2/omnipresent.h"
#include "wedge2/spells.h"

using namespace wedge;

namespace wedge {

static Uint32 mini_pause_start;
static Uint32 mini_pause_total;
static bool mini_paused;
static Uint32 next_mini_pause_can_start;
static bool old_convert_directions_to_focus_events;

static void quit_callback(void *data)
{
	Yes_No_GUI_Callback_Data *d = (Yes_No_GUI_Callback_Data *)data;
	bool quit = d->cancelled == false && d->choice;
	GLOBALS->quit(quit);
}

Globals *globals;

Globals::Globals()
{
	game_t = NULL;
	english_game_t = NULL;

	spell_interface = NULL;
	object_interface = NULL;
	area_game = NULL;
	battle_game = NULL;
	menu_game = NULL;
	omnipresent_game = NULL;
	shop_game = NULL;
	instance = NULL;

#if defined ANDROID || defined IOS
	rumble_enabled = false;
#else
	rumble_enabled = true;
#endif

	try {
		poison_sprite = new gfx::Sprite("poison");
	}
	catch (util::Error &e) {
		poison_sprite = NULL;
	}

	try {
		dpad = new gfx::Sprite("dpad");
	}
	catch (util::Error &e) {
		dpad = NULL;
	}
	cursor = new gfx::Sprite("cursor");
	red_triangle = new gfx::Sprite("red_triangle");
	red_x = new gfx::Sprite("red_x");
	water_ripple = new gfx::Sprite("water_ripple");

	if (shim::opengl) {
#ifdef ANDROID
		GLint range, precision;
		glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, &range, &precision);
		if (range == 0 && precision == 0) {
			have_highp_fragment_shaders = false;
		}
		else {
			have_highp_fragment_shaders = true;
		}
#endif

		std::string default_vertex_source = DEFAULT_GLSL_VERTEX_SHADER;
		std::string enemy_die_fragment_source = util::load_text("gfx/shaders/glsl/enemy_die_fragment.txt");
		std::string pixelate_fragment_source = util::load_text("gfx/shaders/glsl/pixelate_fragment.txt");
		default_opengl_vertex_shader = gfx::Shader::load_opengl_vertex_shader(default_vertex_source, gfx::Shader::HIGH);
		gfx::Shader::OpenGL_Shader *enemy_die_fragment = gfx::Shader::load_opengl_fragment_shader(enemy_die_fragment_source);
		enemy_die_shader = new gfx::Shader(default_opengl_vertex_shader, enemy_die_fragment, true, true);
		gfx::Shader::OpenGL_Shader *pixelate_fragment = gfx::Shader::load_opengl_fragment_shader(pixelate_fragment_source);
		pixelate_shader = new gfx::Shader(default_opengl_vertex_shader, pixelate_fragment, false, true);
	}
#ifdef _WIN32
	else {
#ifdef USE_D3DX
		default_vertex = util::load_text("gfx/shaders/hlsl/default_vertex.txt");
		enemy_die_fragment = util::load_text("gfx/shaders/hlsl/enemy_die_fragment.txt");
		enemy_die_shader = new gfx::Shader(shim::opengl, default_vertex, enemy_die_fragment);
		pixelate_fragment = util::load_text("gfx/shaders/hlsl/pixelate_fragment.txt");
		pixelate_shader = new gfx::Shader(shim::opengl, default_vertex, pixelate_fragment);
#else
		default_d3d_vertex_shader = gfx::Shader::load_d3d_vertex_shader("default_vertex");
		gfx::Shader::D3D_Fragment_Shader *enemy_die_fragment = gfx::Shader::load_d3d_fragment_shader("enemy_die_fragment");
		enemy_die_shader = new gfx::Shader(default_d3d_vertex_shader, enemy_die_fragment, true, true);
		gfx::Shader::D3D_Fragment_Shader *pixelate_fragment = gfx::Shader::load_d3d_fragment_shader("pixelate_fragment");
		pixelate_shader = new gfx::Shader(default_d3d_vertex_shader, pixelate_fragment, true, true);
#endif
	}
#endif

	//red_triangle = new gfx::Image("misc/red_triangle.tga");
	//red_x = new gfx::Image("misc/red_x.tga");

	chest = new audio::MML("sfx/chest.mml");
	battle_start = new audio::MML("sfx/battle_start.mml");
	hit = new audio::MML("sfx/hit.mml");
	enemy_die = new audio::MML("sfx/enemy_die.mml");
	enemy_attack = new audio::MML("sfx/enemy_attack.mml");

	poison = NULL;
	levelup = NULL;
	boss_die = NULL;
	button = NULL;

	try {
		poison = new audio::MML("sfx/poison.mml");
	}
	catch (util::Error &e) {
	}
	try {
		levelup = new audio::MML("sfx/levelup.mml");
	}
	catch (util::Error &e) {
	}
	try {
		boss_die = new audio::MML("sfx/boss_die.mml");
	}
	catch (util::Error &e) {
	}
	try {
		button = new audio::MML("sfx/button.mml");
	}
	catch (util::Error &e) {
	}
	button->set_pause_with_sfx(false);

	gameover = new audio::MML("music/gameover.mml");

	boss_press = DIR_NONE;
	retry_boss = false;
	retried_boss = false;

	create_work_image();
	noise_data = NULL;
	create_noise();

	onscreen_controller_was_enabled = false;
	onscreen_controller_temporarily_disabled = false;
	
	util::JSON::Node *root = shim::shim_json->get_root();

	max_gold = root->get_nested_int("wedge>globals>max_gold", &max_gold, 9999);
	gameover_timeout = root->get_nested_int("wedge>globals>gameover_timeout", &gameover_timeout, 10000);
	gameover_fade_time = root->get_nested_int("wedge>globals>gameover_fade_time", &gameover_fade_time, 2500);

#if (defined IOS && !defined TVOS)
	no_red_triangle = false;
#else
	no_red_triangle = input::system_has_touchscreen() ? false : true;
#endif

	no_red_triangle = root->get_nested_bool("wedge>globals>no_red_triangle", &no_red_triangle, no_red_triangle);

	onscreen_controls_alpha = root->get_nested_float("wedge>globals>onscreen_controls_alpha", &onscreen_controls_alpha, 0.333f);

	red_triangle_colour.r = root->get_nested_byte("wedge>globals>colours>red_triangle_colour.r", &red_triangle_colour.r, shim::palette[13].r);
	red_triangle_colour.g = root->get_nested_byte("wedge>globals>colours>red_triangle_colour.g", &red_triangle_colour.g, shim::palette[13].g);
	red_triangle_colour.b = root->get_nested_byte("wedge>globals>colours>red_triangle_colour.b", &red_triangle_colour.b, shim::palette[13].b);
	red_triangle_colour.a = root->get_nested_byte("wedge>globals>colours>red_triangle_colour.a", &red_triangle_colour.a, shim::palette[13].a);
	
	gameover_fade_colour.r = root->get_nested_byte("wedge>globals>colours>gameover_fade_colour.r", &gameover_fade_colour.r, shim::palette[13].r);
	gameover_fade_colour.g = root->get_nested_byte("wedge>globals>colours>gameover_fade_colour.g", &gameover_fade_colour.g, shim::palette[13].g);
	gameover_fade_colour.b = root->get_nested_byte("wedge>globals>colours>gameover_fade_colour.b", &gameover_fade_colour.b, shim::palette[13].b);
	gameover_fade_colour.a = root->get_nested_byte("wedge>globals>colours>gameover_fade_colour.a", &gameover_fade_colour.a, shim::palette[13].a);
	
	enemy_death_tint.r = root->get_nested_byte("wedge>globals>colours>enemy_death_tint.r", &enemy_death_tint.r, shim::palette[0].r);
	enemy_death_tint.g = root->get_nested_byte("wedge>globals>colours>enemy_death_tint.g", &enemy_death_tint.g, shim::palette[0].g);
	enemy_death_tint.b = root->get_nested_byte("wedge>globals>colours>enemy_death_tint.b", &enemy_death_tint.b, shim::palette[0].b);
	enemy_death_tint.a = root->get_nested_byte("wedge>globals>colours>enemy_death_tint.a", &enemy_death_tint.a, shim::palette[0].a);
	
	poison_attack_colour.r = root->get_nested_byte("wedge>globals>colours>poison_attack_colour.r", &poison_attack_colour.r, shim::palette[21].r);
	poison_attack_colour.g = root->get_nested_byte("wedge>globals>colours>poison_attack_colour.g", &poison_attack_colour.g, shim::palette[21].g);
	poison_attack_colour.b = root->get_nested_byte("wedge>globals>colours>poison_attack_colour.b", &poison_attack_colour.b, shim::palette[21].b);
	poison_attack_colour.a = root->get_nested_byte("wedge>globals>colours>poison_attack_colour.a", &poison_attack_colour.a, shim::palette[21].a);

	poison_attack_shadow.r = root->get_nested_byte("wedge>globals>colours>poison_attack_shadow.r", &poison_attack_shadow.r, shim::black.r);
	poison_attack_shadow.g = root->get_nested_byte("wedge>globals>colours>poison_attack_shadow.g", &poison_attack_shadow.g, shim::black.g);
	poison_attack_shadow.b = root->get_nested_byte("wedge>globals>colours>poison_attack_shadow.b", &poison_attack_shadow.b, shim::black.b);
	poison_attack_shadow.a = root->get_nested_byte("wedge>globals>colours>poison_attack_shadow.a", &poison_attack_shadow.a, shim::black.a);
	
	helpful_attack_colour.r = root->get_nested_byte("wedge>globals>colours>helpful_attack_colour.r", &helpful_attack_colour.r, shim::palette[5].r);
	helpful_attack_colour.g = root->get_nested_byte("wedge>globals>colours>helpful_attack_colour.g", &helpful_attack_colour.g, shim::palette[5].g);
	helpful_attack_colour.b = root->get_nested_byte("wedge>globals>colours>helpful_attack_colour.b", &helpful_attack_colour.b, shim::palette[5].b);
	helpful_attack_colour.a = root->get_nested_byte("wedge>globals>colours>helpful_attack_colour.a", &helpful_attack_colour.a, shim::palette[5].a);
	
	helpful_attack_shadow.r = root->get_nested_byte("wedge>globals>colours>helpful_attack_shadow.r", &helpful_attack_shadow.r, shim::black.r);
	helpful_attack_shadow.g = root->get_nested_byte("wedge>globals>colours>helpful_attack_shadow.g", &helpful_attack_shadow.g, shim::black.g);
	helpful_attack_shadow.b = root->get_nested_byte("wedge>globals>colours>helpful_attack_shadow.b", &helpful_attack_shadow.b, shim::black.b);
	helpful_attack_shadow.a = root->get_nested_byte("wedge>globals>colours>helpful_attack_shadow.a", &helpful_attack_shadow.a, shim::black.a);
	
	regular_attack_colour.r = root->get_nested_byte("wedge>globals>colours>regular_attack_colour.r", &regular_attack_colour.r, shim::white.r);
	regular_attack_colour.g = root->get_nested_byte("wedge>globals>colours>regular_attack_colour.g", &regular_attack_colour.g, shim::white.g);
	regular_attack_colour.b = root->get_nested_byte("wedge>globals>colours>regular_attack_colour.b", &regular_attack_colour.b, shim::white.b);
	regular_attack_colour.a = root->get_nested_byte("wedge>globals>colours>regular_attack_colour.a", &regular_attack_colour.a, shim::white.a);
	
	regular_attack_shadow.r = root->get_nested_byte("wedge>globals>colours>regular_attack_shadow.r", &regular_attack_shadow.r, shim::black.r);
	regular_attack_shadow.g = root->get_nested_byte("wedge>globals>colours>regular_attack_shadow.g", &regular_attack_shadow.g, shim::black.g);
	regular_attack_shadow.b = root->get_nested_byte("wedge>globals>colours>regular_attack_shadow.b", &regular_attack_shadow.b, shim::black.b);
	regular_attack_shadow.a = root->get_nested_byte("wedge>globals>colours>regular_attack_shadow.a", &regular_attack_shadow.a, shim::black.a);

	strong_attack_colour.r = root->get_nested_byte("wedge>globals>colours>strong_attack_colour.r", &strong_attack_colour.r, shim::palette[13].r);
	strong_attack_colour.g = root->get_nested_byte("wedge>globals>colours>strong_attack_colour.g", &strong_attack_colour.g, shim::palette[13].g);
	strong_attack_colour.b = root->get_nested_byte("wedge>globals>colours>strong_attack_colour.b", &strong_attack_colour.b, shim::palette[13].b);
	strong_attack_colour.a = root->get_nested_byte("wedge>globals>colours>strong_attack_colour.a", &strong_attack_colour.a, shim::palette[13].a);
	
	strong_attack_shadow.r = root->get_nested_byte("wedge>globals>colours>strong_attack_shadow.r", &strong_attack_shadow.r, shim::black.r);
	strong_attack_shadow.g = root->get_nested_byte("wedge>globals>colours>strong_attack_shadow.g", &strong_attack_shadow.g, shim::black.g);
	strong_attack_shadow.b = root->get_nested_byte("wedge>globals>colours>strong_attack_shadow.b", &strong_attack_shadow.b, shim::black.b);
	strong_attack_shadow.a = root->get_nested_byte("wedge>globals>colours>strong_attack_shadow.a", &strong_attack_shadow.a, shim::black.a);

	key_action = TGUIK_RETURN;
	key_b1 = TGUIK_s;
	key_b2 = TGUIK_d;
	key_b3 = TGUIK_a;
	key_b4 = TGUIK_w;
	key_back = TGUIK_ESCAPE;
	key_pause = TGUIK_ESCAPE;
	key_menu = TGUIK_TAB;
	key_switch = TGUIK_COMMA;
	key_cancel = TGUIK_ESCAPE;
	key_confirm = TGUIK_SPACE;
	key_l = TGUIK_LEFT;
	key_r = TGUIK_RIGHT;
	key_u = TGUIK_UP;
	key_d = TGUIK_DOWN;
	joy_action = TGUI_B_A;
	joy_b1 = TGUI_B_A;
	joy_b2 = TGUI_B_B;
	joy_b3 = TGUI_B_X;
	joy_b4 = TGUI_B_Y;
	joy_back = TGUI_B_B;
	joy_pause = TGUI_B_START;
	joy_menu = TGUI_B_BACK;
	joy_switch = TGUI_B_RB;
	joy_cancel = TGUI_B_LB;
	joy_confirm = TGUI_B_X;

	language = "English";

	terminate = false;

	allow_global_mini_pause = true;
	getting_combo = false;
	assigning_controls = false;
}

Globals::~Globals()
{
	delete poison_sprite;
	delete dpad;
	delete cursor;
	delete red_triangle;
	delete red_x;
	delete water_ripple;

	delete enemy_die_shader;
	delete pixelate_shader;

	//delete red_triangle;
	//delete red_x;

	delete chest;
	delete battle_start;
	delete hit;
	delete enemy_die;
	delete boss_die;
	delete enemy_attack;
	delete button;
	delete poison;
	delete levelup;

	delete gameover;

	for (size_t i = 0; i < item_sfx.size(); i++) {
		delete item_sfx[i];
	}

	std::map<std::string, audio::Sound *>::iterator it;
	for (it = spell_sfx.begin(); it != spell_sfx.end(); it++) {
		std::pair<std::string, audio::Sound *> p = *it;
		delete p.second;
	}

	delete spell_interface;
	delete object_interface;

	delete instance;

	delete work_image;

	delete noise;
	delete[] noise_data;

	delete game_t;
	delete english_game_t;
	
	shim::shim_json->remove("wedge>globals>max_gold", true);
	shim::shim_json->remove("wedge>globals>gameover_timeout", true);
	shim::shim_json->remove("wedge>globals>gameover_fade_time", true);

	shim::shim_json->remove("wedge>globals>key_b1", true);
	shim::shim_json->remove("wedge>globals>key_b2", true);
	shim::shim_json->remove("wedge>globals>key_b3", true);
	shim::shim_json->remove("wedge>globals>key_b4", true);
	shim::shim_json->remove("wedge>globals>key_back", true);
	shim::shim_json->remove("wedge>globals>key_switch", true);
	shim::shim_json->remove("wedge>globals>key_l", true);
	shim::shim_json->remove("wedge>globals>key_r", true);
	shim::shim_json->remove("wedge>globals>key_u", true);
	shim::shim_json->remove("wedge>globals>key_d", true);
	shim::shim_json->remove("wedge>globals>joy_action", true);
	shim::shim_json->remove("wedge>globals>joy_b1", true);
	shim::shim_json->remove("wedge>globals>joy_b2", true);
	shim::shim_json->remove("wedge>globals>joy_b3", true);
	shim::shim_json->remove("wedge>globals>joy_b4", true);
	shim::shim_json->remove("wedge>globals>joy_back", true);

	shim::shim_json->remove("wedge>globals>onscreen_controls_alpha", true);

	shim::shim_json->remove("wedge>globals>enemy_death_tint.r", true);
	shim::shim_json->remove("wedge>globals>enemy_death_tint.g", true);
	shim::shim_json->remove("wedge>globals>enemy_death_tint.b", true);
	shim::shim_json->remove("wedge>globals>enemy_death_tint.a", true);
	
	shim::shim_json->remove("wedge>globals>poison_attack_colour.r", true);
	shim::shim_json->remove("wedge>globals>poison_attack_colour.g", true);
	shim::shim_json->remove("wedge>globals>poison_attack_colour.b", true);
	shim::shim_json->remove("wedge>globals>poison_attack_colour.a", true);

	shim::shim_json->remove("wedge>globals>poison_attack_shadow.r", true);
	shim::shim_json->remove("wedge>globals>poison_attack_shadow.g", true);
	shim::shim_json->remove("wedge>globals>poison_attack_shadow.b", true);
	shim::shim_json->remove("wedge>globals>poison_attack_shadow.a", true);
	
	shim::shim_json->remove("wedge>globals>helpful_attack_colour.r", true);
	shim::shim_json->remove("wedge>globals>helpful_attack_colour.g", true);
	shim::shim_json->remove("wedge>globals>helpful_attack_colour.b", true);
	shim::shim_json->remove("wedge>globals>helpful_attack_colour.a", true);
	
	shim::shim_json->remove("wedge>globals>helpful_attack_shadow.r", true);
	shim::shim_json->remove("wedge>globals>helpful_attack_shadow.g", true);
	shim::shim_json->remove("wedge>globals>helpful_attack_shadow.b", true);
	shim::shim_json->remove("wedge>globals>helpful_attack_shadow.a", true);
	
	shim::shim_json->remove("wedge>globals>regular_attack_colour.r", true);
	shim::shim_json->remove("wedge>globals>regular_attack_colour.g", true);
	shim::shim_json->remove("wedge>globals>regular_attack_colour.b", true);
	shim::shim_json->remove("wedge>globals>regular_attack_colour.a", true);
	
	shim::shim_json->remove("wedge>globals>regular_attack_shadow.r", true);
	shim::shim_json->remove("wedge>globals>regular_attack_shadow.g", true);
	shim::shim_json->remove("wedge>globals>regular_attack_shadow.b", true);
	shim::shim_json->remove("wedge>globals>regular_attack_shadow.a", true);

	shim::shim_json->remove("English", true);
}

void Globals::add_next_dialogue_monitor(wedge::Step *monitor)
{
	next_dialogue_monitors.push_back(monitor);
}

void Globals::create_work_image()
{
	bool old_create_depth_buffer = gfx::Image::create_depth_buffer;
	gfx::Image::create_depth_buffer = true;
	work_image = new gfx::Image(shim::real_screen_size);
	gfx::Image::create_depth_buffer = old_create_depth_buffer;
}

void Globals::create_noise()
{
	const int size = 256;

	if (noise_data == NULL) {
		noise_data = new Uint8[size*size*4];

		util::srand(0); // make it the same each time

		for (int i = 0; i < size*size*4; i += 4) {
			Uint8 *p = noise_data + i;
			p[0] = util::rand(0, 255);
			p[1] = util::rand(0, 255);
			p[2] = util::rand(0, 255);
			p[3] = util::rand(0, 255);
		}

		util::srand((uint32_t)time(NULL));
	}

	noise = new gfx::Image(noise_data, util::Size<int>(size, size));
}

util::Point<float> Globals::get_onscreen_button_position(Onscreen_Button button)
{
	return util::Point<float>(0.0f, 0.0f);
}

void Globals::mini_pause(bool is_joystick_disconnect_notification, bool is_sneak_attack_notification)
{
	if (instance == NULL || mini_paused || SDL_GetTicks() < next_mini_pause_can_start) {
		return;
	}

	button->play(false);

	mini_paused = true;
	mini_pause_start = SDL_GetTicks();
	old_convert_directions_to_focus_events = shim::convert_directions_to_focus_events;
	shim::convert_directions_to_focus_events = true;

	audio::pause_sfx(true);

	if (is_joystick_disconnect_notification) {
		add_notification_gui(GLOBALS->game_t->translate(192)/* Originally: Joystick disconnected! */, quit_callback);
	}
	else if (is_sneak_attack_notification) {
		add_notification_gui(GLOBALS->game_t->translate(392)/* Originally: You've been attacked from behind! For combos, left is now right and right is left! */, quit_callback);
	}
	else {
		add_yes_no_gui(GLOBALS->game_t->translate(10)/* Originally: Quit to title? */, true, false, quit_callback);
	}
}

bool Globals::is_mini_paused()
{
	if (instance == NULL) {
		return false;
	}
	return mini_paused;
}

void Globals::quit(bool yesno)
{
	if (yesno) {
		quit_all();
	}
	
	mini_paused = false;
	mini_pause_total += SDL_GetTicks() - mini_pause_start;
	next_mini_pause_can_start = SDL_GetTicks() + 100;
	shim::convert_directions_to_focus_events = yesno ? true : old_convert_directions_to_focus_events;
	
	audio::pause_sfx(false);
}

Uint32 Globals::get_ticks()
{
	if (instance == NULL) {
		return SDL_GetTicks();
	}
	else if (mini_paused) {
		return mini_pause_start - mini_pause_total;
	}
	else {
		return SDL_GetTicks() - mini_pause_total;
	}
}

void Globals::run()
{
}

//--

Globals::Instance::Instance(util::JSON::Node *root) :
	milestones(nullptr)
{
	play_time = 0;
	
	mini_paused = false;
	mini_pause_total = 0;
	next_mini_pause_can_start = 0;

	version = VERSION;
	
	util::JSON::Node *shim_json = shim::shim_json->get_root();

	if (root) {
		util::JSON::Node *cfg = root->find("game");
		util::JSON::Node *n;

		n = cfg->find("version");
		if (n == NULL) {
			version = 0;
		}
		else {
			version = n->as_int();
		}

		n = cfg->find("gold");
		if (n != NULL) {
			gold = shim_json->get_nested_int("wedge>instance>gold", &gold, n->as_int());
		}
		n = cfg->find("step_count");
		if (n != NULL) {
			step_count = n->as_int();
		}
		n = cfg->find("play_time");
		if (n != NULL) {
			play_time = n->as_int();
		}
		n = cfg->find("party_following_player");
		if (n != NULL) {
			party_following_player = n->as_bool();
		}
		n = cfg->find("chests_opened");
		if (n != NULL) {
			chests_opened = n->as_int();
		}

		n = root->find("players");

		for (size_t i = 0; i < n->children.size(); i++) {
			stats.push_back(Player_Stats(n->children[i]));
		}

		n = root->find("inventory");
		if (n != NULL) {
			inventory = Inventory(n);
			inventory.sort();
		}
	}
	else {
		gold = shim_json->get_nested_int("wedge>instance>gold", &gold, 0);
		step_count = 0;

		party_following_player = false;

		chests_opened = 0;
	}
	play_start = GLOBALS->get_ticks();
}

Globals::Instance::~Instance()
{
	for (size_t i = 0; i < stats.size(); i++) {
		delete stats[i].sprite;
	}
	
	shim::shim_json->remove("wedge>instance>gold", true);
}	

bool Globals::Instance::is_milestone_complete(int milestone)
{
	return milestones[milestone];
}

void Globals::Instance::set_milestone_complete(int milestone, bool complete)
{
	milestones[milestone] = complete;
}

int Globals::Instance::get_gold()
{
	return gold;
}

void Globals::Instance::add_gold(int amount)
{
	gold += amount;
	if (gold < 0) {
		gold = 0;
	}
	if (gold > globals->max_gold) {
		gold = globals->max_gold;
	}
}

std::string Globals::Instance::save()
{
	std::string s;
	s += "\"game\": {";
	s += "\"version\": " + util::itos(VERSION) + ",";
	s += "\"gold\": " + util::itos(get_gold()) + ",";
	s += "\"step_count\": " + util::itos(step_count) + ",";
	s += "\"party_following_player\": " + bool_to_string(party_following_player) + ",";
	s += "\"chests_opened\": " + util::itos(chests_opened) + ",";
	s += "\"play_time\": " + util::itos(play_time);
	s += "},";
	s += "\"players\": [";
	for (size_t i = 0; i < stats.size(); i++) {
		s += stats[i].save();
		if (i < stats.size()-1) {
			s += ",";
		}
	}
	s += "],";
	s += "\"inventory\": [";
	s += inventory.save();
	s += "],";
	s += "\"milestones\": [";
	for (int i = 0; i < num_milestones(); i++) {
		if (is_milestone_complete(i)) {
			s += "true";
		}
		else {
			s += "false";
		}
		if (i < num_milestones()-1) {
			s += ",";
		}
	}
	s += "]";
	return s;
}

int Globals::Instance::get_version()
{
	return version;
}

void Globals::draw_custom_status(Map_Entity *entity, int status, util::Point<float> draw_pos)
{
}

void Globals::load_translation()
{
	delete game_t;
	delete english_game_t;

	std::string game_t_text = util::load_text(std::string("text/") + language + std::string(".utf8"));
	std::string english_game_t_text = util::load_text("text/English.utf8");

	game_t = new util::Translation(game_t_text);
	english_game_t = new util::Translation(english_game_t_text);

	/*
	// Do this if using a TTF
	entire_translation = game_t->get_entire_translation();

	std::vector<std::string> filenames = shim::cpa->get_all_filenames();

	for (size_t i = 0; i < filenames.size(); i++) {
		util::Tokenizer t(filenames[i], '|');
		std::string s = t.next();
		s = t.next();
		entire_translation += s;
	}
	*/
}

std::string Globals::tag_end()
{
	if (language == "French") {
		return std::string(" : ");
	}
	else {
		return std::string(": ");
	}
}

void Globals::lost_device()
{
	delete globals->work_image;
	globals->work_image = NULL;
	delete globals->noise;
	globals->noise = NULL;
}

void Globals::found_device()
{
	globals->create_work_image();
	globals->create_noise();

}

}
