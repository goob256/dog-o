#include <wedge2/area_game.h>
#include <wedge2/battle_game.h>
#include <wedge2/general.h>

#include "battle_game.h"
#include "dialogue.h"
#include "general.h"
#include "globals.h"
#include "gui.h"
#include "gui_drawing_hook.h"
#include "inventory.h"
#include "menu.h"
#include "milestones.h"
#include "stats.h"

util::JSON *cfg;

Globals::Globals() :
	wedge::Globals()
{
	loaded_autosave = false;

	player_start_positions.push_back(util::Point<int>(14, 11));
	player_start_directions.push_back(wedge::DIR_W);
	player_sprite_names.push_back("pleasant");

	spell_interface = NULL; // FIXME, you can implement this
	object_interface = new Object_Interface();

	if (shim::opengl) {
		std::string alpha_fragment_source = util::load_text("gfx/shaders/glsl/alpha_fragment.txt");
		gfx::Shader::OpenGL_Shader *alpha_fragment = gfx::Shader::load_opengl_fragment_shader(alpha_fragment_source);
		alpha_shader = new gfx::Shader(default_opengl_vertex_shader, alpha_fragment, false, true);
		std::string blur_fragment_source = util::load_text("gfx/shaders/glsl/blur_fragment.txt");
		gfx::Shader::OpenGL_Shader *blur_fragment = gfx::Shader::load_opengl_fragment_shader(blur_fragment_source);
		blur_shader = new gfx::Shader(default_opengl_vertex_shader, blur_fragment, false, true);
		std::string blur_banded_fragment_source = util::load_text("gfx/shaders/glsl/blur_banded_fragment.txt");
		gfx::Shader::OpenGL_Shader *blur_banded_fragment = gfx::Shader::load_opengl_fragment_shader(blur_banded_fragment_source);
		blur_banded_shader = new gfx::Shader(default_opengl_vertex_shader, blur_banded_fragment, false, true);
		std::string solid_colour_fragment_source = util::load_text("gfx/shaders/glsl/solid_colour_fragment.txt");
		gfx::Shader::OpenGL_Shader *solid_colour_fragment = gfx::Shader::load_opengl_fragment_shader(solid_colour_fragment_source);
		solid_colour_shader = new gfx::Shader(default_opengl_vertex_shader, solid_colour_fragment, false, true);
		std::string alpha_test_fragment_source = util::load_text("gfx/shaders/glsl/alpha_test_fragment.txt");
		gfx::Shader::OpenGL_Shader *alpha_test_fragment;
#ifdef ANDROID
		if (have_highp_fragment_shaders) {
#endif
			std::string heat_wave_fragment_source = util::load_text("gfx/shaders/glsl/heat_wave_fragment.txt");
			gfx::Shader::OpenGL_Shader *heat_wave_fragment = gfx::Shader::load_opengl_fragment_shader(heat_wave_fragment_source, gfx::Shader::HIGH);
			heat_wave_shader = new gfx::Shader(default_opengl_vertex_shader, heat_wave_fragment, false, true);
			alpha_test_fragment = gfx::Shader::load_opengl_fragment_shader(alpha_test_fragment_source, gfx::Shader::HIGH);
#ifdef ANDROID
		}
		else {
			alpha_test_fragment = gfx::Shader::load_opengl_fragment_shader(alpha_test_fragment_source, gfx::Shader::MEDIUM);
		}
#endif
		alpha_test_shader = new gfx::Shader(default_opengl_vertex_shader, alpha_test_fragment, false, true);
	}
#ifdef _WIN32
	else {
		gfx::Shader::D3D_Fragment_Shader *alpha_fragment = gfx::Shader::load_d3d_fragment_shader("alpha_fragment");
		alpha_shader = new gfx::Shader(default_d3d_vertex_shader, alpha_fragment, false, true);
		gfx::Shader::D3D_Fragment_Shader *blur_fragment = gfx::Shader::load_d3d_fragment_shader("blur_fragment");
		blur_shader = new gfx::Shader(default_d3d_vertex_shader, blur_fragment, false, true);
		gfx::Shader::D3D_Fragment_Shader *blur_banded_fragment = gfx::Shader::load_d3d_fragment_shader("blur_banded_fragment");
		blur_banded_shader = new gfx::Shader(default_d3d_vertex_shader, blur_banded_fragment, false, true);
		gfx::Shader::D3D_Fragment_Shader *solid_colour_fragment = gfx::Shader::load_d3d_fragment_shader("solid_colour_fragment");
		solid_colour_shader = new gfx::Shader(default_d3d_vertex_shader, solid_colour_fragment, false, true);
		gfx::Shader::D3D_Fragment_Shader *heat_wave_fragment = gfx::Shader::load_d3d_fragment_shader("heat_wave_fragment");
		heat_wave_shader = new gfx::Shader(default_d3d_vertex_shader, heat_wave_fragment, false, true);
		gfx::Shader::D3D_Fragment_Shader *alpha_test_fragment = gfx::Shader::load_d3d_fragment_shader("alpha_test_fragment");
		alpha_test_shader = new gfx::Shader(default_d3d_vertex_shader, alpha_test_fragment, false, true);
	}
#endif

	bold_font = new gfx::Pixel_Font("bold");
	bold_font->set_vertex_cache_id(2);
	bold_font->start_batch();

	multi_font = new gfx::Pixel_Font("multi");
	multi_font->set_vertex_cache_id(3);
	multi_font->start_batch();

	up_arrow = new gfx::Image("ui/up_arrow.tga");
	down_arrow = new gfx::Image("ui/down_arrow.tga");
	nomore = new gfx::Image("ui/nomore.tga");
	speech_window = new gfx::Image("ui/speech_window.tga");
	message_window = new gfx::Image("ui/message_window.tga");
	gui_window = new gfx::Image("ui/gui_window.tga");
	battle_window = new gfx::Image("ui/battle_window.tga");
	menu_window = new gfx::Image("ui/menu_window.tga");
	shop_window = new gfx::Image("ui/shop_window.tga");
	tip_window = new gfx::Image("ui/tip_window.tga");
	tip_tri = new gfx::Image("ui/tip_tri.tga");

	profile_pics[PLEASANT] = new gfx::Image("misc/pleasant_profile.tga");
	profile_pics[TOM] = new gfx::Image("misc/tom_profile.tga");
	profile_pics[TIK] = new gfx::Image("misc/tik_profile.tga");
	profile_pics[WIT] = new gfx::Image("misc/wit_profile.tga");

	combo_l = new gfx::Image("misc/arrow_l.tga");
	combo_r = new gfx::Image("misc/arrow_r.tga");
	combo_u = new gfx::Image("misc/arrow_u.tga");
	combo_d = new gfx::Image("misc/arrow_d.tga");
	
	coin = new gfx::Image("misc/coin.tga");

	xb_a = new gfx::Image("buttons/xb_a.tga");
	xb_b = new gfx::Image("buttons/xb_b.tga");
	xb_x = new gfx::Image("buttons/xb_x.tga");
	xb_y = new gfx::Image("buttons/xb_y.tga");
	ps_x = new gfx::Image("buttons/ps_x.tga");
	ps_circle = new gfx::Image("buttons/ps_circle.tga");
	ps_square = new gfx::Image("buttons/ps_square.tga");
	ps_triangle = new gfx::Image("buttons/ps_triangle.tga");

	shim::font->set_extra_glyph_offset(util::Point<float>(0, 3));
	bold_font->set_extra_glyph_offset(util::Point<float>(0, 3));

	shim::font->add_extra_glyph(0, combo_l);
	shim::font->add_extra_glyph(1, combo_r);
	shim::font->add_extra_glyph(2, combo_u);
	shim::font->add_extra_glyph(3, combo_d);
	shim::font->add_extra_glyph(0xf0, xb_a);
	shim::font->add_extra_glyph(0xf1, xb_b);
	shim::font->add_extra_glyph(0xf2, xb_x);
	shim::font->add_extra_glyph(0xf3, xb_y);
	shim::font->add_extra_glyph(0xf4, ps_x);
	shim::font->add_extra_glyph(0xf5, ps_circle);
	shim::font->add_extra_glyph(0xf6, ps_square);
	shim::font->add_extra_glyph(0xf7, ps_triangle);
	bold_font->add_extra_glyph(0, combo_l);
	bold_font->add_extra_glyph(1, combo_r);
	bold_font->add_extra_glyph(2, combo_u);
	bold_font->add_extra_glyph(3, combo_d);
	bold_font->add_extra_glyph(0xf0, xb_a);
	bold_font->add_extra_glyph(0xf1, xb_b);
	bold_font->add_extra_glyph(0xf2, xb_x);
	bold_font->add_extra_glyph(0xf3, xb_y);
	bold_font->add_extra_glyph(0xf4, ps_x);
	bold_font->add_extra_glyph(0xf5, ps_circle);
	bold_font->add_extra_glyph(0xf6, ps_square);
	bold_font->add_extra_glyph(0xf7, ps_triangle);

	mini_pics["pleasant"] = new gfx::Image("misc/mini_pleasant.tga");
	mini_pics["tom"] = new gfx::Image("misc/mini_tom.tga");
	mini_pics["tik"] = new gfx::Image("misc/mini_tik.tga");
	mini_pics["wit"] = new gfx::Image("misc/mini_wit.tga");

	mini_frame = new gfx::Image("ui/mini_frame.tga");
	active_frame = new gfx::Image("ui/active_frame.tga");
	yellow_frame = new gfx::Image("ui/yellow_frame.tga");
	hourglass = new gfx::Image("misc/hourglass.tga");
	ninja_star = new gfx::Image("misc/ninja_star.tga");
	randomaxe = new gfx::Image("misc/randomaxe.tga");
	shadow = new gfx::Image("misc/shadow.tga");
	defend_img = new gfx::Image("misc/defend.tga");

	hit_preload = new gfx::Sprite("hit");
	fire_big = new gfx::Sprite("fire/big");
	fire_medium = new gfx::Sprite("fire/medium");
	fire_small = new gfx::Sprite("fire/small");
	coin_sprite = new gfx::Sprite("coin");
	fast_sprite = new gfx::Sprite("fast");
	dagger = new gfx::Sprite("dagger");
	mult1 = new gfx::Sprite("mult1");
	mult2 = new gfx::Sprite("mult2");
	mult3 = new gfx::Sprite("mult3");
	multi_check_checked = new gfx::Sprite("multi_check_checked");
	multi_check_unchecked = new gfx::Sprite("multi_check_unchecked");
	multi_check_unchecked_selected = new gfx::Sprite("multi_check_unchecked_selected");
	multi_check_checked_selected = new gfx::Sprite("multi_check_checked_selected");
	heal = new gfx::Sprite("heal");
	turn_arrow = new gfx::Sprite("turn_arrow");
	water_ripple = new gfx::Sprite("water_ripple");
	water_ripple2 = new gfx::Sprite("water_ripple2");

	blur_fire();

	for (int i = 0; i < ITEM_SIZE; i++) {
		item_sfx.push_back(NULL); // default no sound
	}
	item_sfx[ITEM_CARAMEL] = new audio::MML("sfx/caramel.mml");
	item_sfx[ITEM_APPLE] = new audio::MML("sfx/apple.mml");
	item_sfx[ITEM_CARROT] = new audio::MML("sfx/carrot.mml");
	item_sfx[ITEM_BANANA] = new audio::MML("sfx/apple.mml");
	item_sfx[ITEM_TURNIP] = new audio::MML("sfx/carrot.mml");
	item_sfx[ITEM_RED_POTION] = new audio::MML("sfx/potion.mml");
	item_sfx[ITEM_BLUE_POTION] = new audio::MML("sfx/potion.mml");
	item_sfx[ITEM_GREEN_POTION] = new audio::MML("sfx/potion.mml");
	item_sfx[ITEM_ORANGE] = new audio::MML("sfx/apple.mml");
	item_sfx[ITEM_HOT_PEPPER] = new audio::MML("sfx/carrot.mml");
	item_sfx[ITEM_PEACH] = new audio::MML("sfx/apple.mml");
	item_sfx[ITEM_EGGPLANT] = new audio::MML("sfx/carrot.mml");

	melee = new audio::MML("sfx/melee.mml");
	melee2 = new audio::MML("sfx/melee2.mml");
	melee_combo = new audio::MML("sfx/melee_combo.mml");
	double_melee = new audio::MML("sfx/double_melee.mml");
	melee_wrestling = new audio::MML("sfx/melee_wrestling.mml");
	melee_spin = new audio::MML("sfx/melee_spin.mml");
	jump = new audio::MML("sfx/jump.mml");
	big_jump = new audio::MML("sfx/big_jump.mml");
	fall = new audio::MML("sfx/fall.mml");
	throw_ = new audio::MML("sfx/throw.mml");
	shock = new audio::MML("sfx/shock.mml");
	hit = new audio::MML("sfx/hit.mml");
	rope = new audio::MML("sfx/rope.mml");
	shake = new audio::MML("sfx/shake.mml");
	spit_water = new audio::MML("sfx/spit_water.mml");
	error = new audio::MML("sfx/error.mml");
	last_error_play = 0;
	enemy_attack = new audio::MML("sfx/enemy_attack.mml");
	combo = new audio::MML("sfx/combo.mml");
	run_away = new audio::MML("sfx/run.mml");
	hadouken = new audio::MML("sfx/hadouken.mml");
	land = new audio::MML("sfx/land.mml");
	sensei_jump = new audio::MML("sfx/sensei_jump.mml");
	ninja_star_loop = new audio::MML("sfx/ninja_star_loop.mml");
	octo = new audio::MML("sfx/octo.mml");
	buysell = new audio::MML("sfx/buysell.mml");
	fast = new audio::MML("sfx/fast.mml");
	cannonball = new audio::MML("sfx/cannonball.mml");
	stomp = new audio::MML("sfx/stomp.mml");
	bellyflop = new audio::MML("sfx/bellyflop.mml");
	dodge = new audio::MML("sfx/dodge.mml");
	mult1_s = new audio::MML("sfx/mult1.mml");
	mult2_s = new audio::MML("sfx/mult2.mml");
	mult3_s = new audio::MML("sfx/mult3.mml");
	dive_bomb = new audio::MML("sfx/dive_bomb.mml");
	seering_slap = new audio::MML("sfx/seering_slap.mml");
	thunderclap = new audio::MML("sfx/thunderclap.mml");
	cartwheel = new audio::MML("sfx/cartwheel.mml");
	beatdown = new audio::MML("sfx/beatdown.mml");
	windmill = new audio::MML("sfx/windmill.mml");
	the_claw = new audio::MML("sfx/the_claw.mml");
	heal_sound = new audio::MML("sfx/heal.mml");
	pickup = new audio::MML("sfx/pickup.mml");
	trout_slap = new audio::MML("sfx/trout_slap.mml");
	twister = new audio::MML("sfx/twister.mml");
	kick_flip = new audio::MML("sfx/kick_flip.mml");
	big_stomp = new audio::MML("sfx/big_stomp.mml");
	nose_breaker = new audio::MML("sfx/nose_breaker.mml");
	timber = new audio::MML("sfx/timber.mml");
	crash = new audio::MML("sfx/crash.mml");
	loud_thud = new audio::MML("sfx/loud_thud.mml");
	limbs_a_flailin = new audio::MML("sfx/limbs_a_flailin.mml");
	defend = new audio::MML("sfx/defend.mml");
	
	waterfall_sfx = new audio::MML("sfx/waterfall.mml");
	
	laugh = new audio::Sample("laugh.flac");
	grunt = new audio::Sample("grunt.flac");
	bridge_collapse = new audio::Sample("bridge_collapse.flac");
	evil_laugh = new audio::Sample("evil_laugh.flac");
	coin_hit = new audio::Sample("coin_hit.flac");
	elevator_start = new audio::Sample("elevator_start.flac");
	elevator_raising = new audio::Sample("elevator_raising.flac");
	thud = new audio::Sample("thud.flac");
	bumper_growl = new audio::Sample("bumper_growl.flac");
	hoohoo = new audio::Sample("hoohoo.flac");

	util::JSON::Node *root = shim::shim_json->get_root();
	
	max_battle_steps = root->get_nested_int("wedge>globals>max_battle_steps", &max_battle_steps, 30);
	min_battle_steps = root->get_nested_int("wedge>globals>min_battle_steps", &min_battle_steps, 15);

	rand_max = 999999999;

	rands.clear();

	util::srand(0);

	for (int i = 0; i < 2000; i++) {
		rands.push_back(util::rand(0, rand_max));
	}
	
	util::srand((Uint32)time(NULL));

	jb_credits = new gfx::Sprite("jellybean");
	credits_music = new audio::MML("music/credits.mml");
	nooskewl_logo = new gfx::Image("misc/nooskewl.tga");

#ifdef STEAMWORKS
	input_handle_was_null = true;
#endif
}

Globals::~Globals()
{
	delete bold_font;
	delete multi_font;

	delete up_arrow;
	delete down_arrow;
	delete nomore;
	delete speech_window;
	delete message_window;
	delete gui_window;
	delete battle_window;
	delete menu_window;
	delete shop_window;
	delete tip_window;
	delete tip_tri;

	for (int i = 0; i < NUM_PLAYERS; i++) {
		delete profile_pics[i];
	}

	delete combo_l;
	delete combo_r;
	delete combo_u;
	delete combo_d;
	
	delete coin;
	
	delete xb_a;
	delete xb_b;
	delete xb_x;
	delete xb_y;
	delete ps_x;
	delete ps_circle;
	delete ps_square;
	delete ps_triangle;

	delete alpha_shader;
	delete blur_shader;
	delete blur_banded_shader;
	delete solid_colour_shader;
#ifdef ANDROID
	if (have_highp_fragment_shaders) {
#endif
		delete heat_wave_shader;
#ifdef ANDROID
	}
#endif
	delete alpha_test_shader;

	for (auto p : mini_pics) {
		delete p.second;
	}

	delete mini_frame;
	delete active_frame;
	delete yellow_frame;
	delete hourglass;
	delete ninja_star;
	delete randomaxe;
	delete shadow;
	delete defend_img;

	delete hit_preload;
	delete fire_big;
	delete fire_medium;
	delete fire_small;
	delete coin_sprite;
	delete fast_sprite;
	delete dagger;
	delete mult1;
	delete mult2;
	delete mult3;
	delete multi_check_checked;
	delete multi_check_unchecked;
	delete multi_check_unchecked_selected;
	delete multi_check_checked_selected;
	delete heal;
	delete turn_arrow;
	delete water_ripple;
	delete water_ripple2;
	
	for (int i = 0; i < 3; i++) {
		for (auto img : fire_blurred[i]) {
			delete img;
		}
		fire_blurred[i].clear();
	}

	delete melee;
	delete melee2;
	delete melee_combo;
	delete double_melee;
	delete melee_wrestling;
	delete melee_spin;
	delete jump;
	delete big_jump;
	delete fall;
	delete throw_;
	delete shock;
	delete hit;
	delete rope;
	delete shake;
	delete spit_water;
	delete error;
	delete enemy_attack;
	delete combo;
	delete run_away;
	delete hadouken;
	delete land;
	delete sensei_jump;
	delete ninja_star_loop;
	delete octo;
	delete buysell;
	delete fast;
	delete cannonball;
	delete stomp;
	delete bellyflop;
	delete dodge;
	delete mult1_s;
	delete mult2_s;
	delete mult3_s;
	delete dive_bomb;
	delete seering_slap;
	delete thunderclap;
	delete cartwheel;
	delete beatdown;
	delete windmill;
	delete the_claw;
	delete heal_sound;
	delete pickup;
	delete trout_slap;
	delete twister;
	delete kick_flip;
	delete big_stomp;
	delete nose_breaker;
	delete timber;
	delete crash;
	delete loud_thud;
	delete limbs_a_flailin;
	delete defend;

	delete waterfall_sfx;
	
	delete laugh;
	delete grunt;
	delete bridge_collapse;
	delete evil_laugh;
	delete coin_hit;
	delete elevator_start;
	delete elevator_raising;
	delete thud;
	delete bumper_growl;
	delete hoohoo;
	
	delete jb_credits;
	delete credits_music;
	delete nooskewl_logo;

	shim::shim_json->remove("max_battle_steps", true);
	shim::shim_json->remove("min_battle_steps", true);

	shim::shim_json->remove("red_triangle_colour.r", true);
	shim::shim_json->remove("red_triangle_colour.g", true);
	shim::shim_json->remove("red_triangle_colour.b", true);
	shim::shim_json->remove("red_triangle_colour.a", true);
		
	shim::shim_json->remove("gameover_fade_colour.r", true);
	shim::shim_json->remove("gameover_fade_colour.g", true);
	shim::shim_json->remove("gameover_fade_colour.b", true);
	shim::shim_json->remove("gameover_fade_colour.a", true);
}

void Globals::do_dialogue(std::string tag, std::string text, wedge::Dialogue_Type type, wedge::Dialogue_Position position, wedge::Step *monitor)
{
	wedge::Game *g;
	if (BATTLE) {
		g = BATTLE;
	}
	else {
		g = AREA;
	}
	NEW_SYSTEM_AND_TASK(g)
	Dialogue_Step *d = new Dialogue_Step(tag, text, type, position, new_task);
	if (monitor) {
		d->add_monitor(monitor);
	}
	for (size_t i = 0; i < next_dialogue_monitors.size(); i++) {
		d->add_monitor(next_dialogue_monitors[i]);
	}
	next_dialogue_monitors.clear();
	ADD_STEP(d)
	ADD_TASK(new_task)
	FINISH_SYSTEM(g)
}

bool Globals::add_title_gui()
{
	save_slot = -1;

	Title_GUI *title = new Title_GUI();
	shim::guis.push_back(title);

	return true;
}

bool Globals::dialogue_active(wedge::Game *game, bool only_if_initialised, bool wait_for_fade)
{
	std::vector<Dialogue_Step *> v = active_dialogues(game);

	for (size_t i = 0; i < v.size(); i++) {
		if ((only_if_initialised == false || v[i]->is_initialised()) && (wait_for_fade == true || v[i]->is_done() == false)) {
			return true;
		}
	}

	return false;
}

void Globals::add_notification_gui(std::string text, util::Callback callback, void *callback_data)
{
	Notification_GUI *gui = new Notification_GUI(text, callback, callback_data);
	gui->hook_omnipresent(true, true);
	shim::guis.push_back(gui);
}

void Globals::add_yes_no_gui(std::string text, bool escape_cancels, bool selected, util::Callback callback, void *callback_data)
{
	Yes_No_GUI *gui = new Yes_No_GUI(text, escape_cancels, callback, callback_data, true);
	gui->set_selected(selected);
	gui->hook_omnipresent(true, true);
	shim::guis.push_back(gui);
}

bool Globals::can_walk()
{
	// If practicing, show controller
	for (size_t i = 0; i < shim::guis.size(); i++) {
		gui::GUI *g = shim::guis[i];
		Practice_Menu_GUI *pg = dynamic_cast<Practice_Menu_GUI *>(g);
		if (pg) {
			if (pg->is_practicing()) {
				return true;
			}
		}
	}
	return can_show_settings(false, true, true, true) && shim::guis.size() == 0 && (BATTLE == NULL || dynamic_cast<Battle_Game *>(BATTLE)->is_detecting_combo() == true);
	/*
	if (BATTLE != NULL && dynamic_cast<Battle_Game *>(BATTLE)->is_detecting_combo()) {
		return true;
	}
	return can_show_settings(false, true, true, true) && shim::guis.size() == 0;
	*/
}

bool Globals::title_gui_is_top()
{
	// FIXME
	return false;
}

util::Point<float> Globals::get_onscreen_button_position(wedge::Onscreen_Button button)
{
	dpad->set_animation("dpad");
	util::Size<int> dpad_size = dpad->get_current_image()->size / 3;
	dpad->set_animation("button1");
	util::Size<int> button_size = dpad->get_current_image()->size;
	int offset = shim::tile_size/2;
	float b1_x = shim::screen_size.w-offset-button_size.w*2;

	switch (button) {
		case wedge::ONSCREEN_UP:
			return util::Point<float>(offset+dpad_size.w, shim::screen_size.h-offset-dpad_size.h*3);
		case wedge::ONSCREEN_RIGHT:
			return util::Point<float>(offset+dpad_size.w*2, shim::screen_size.h-offset-dpad_size.h*2);
		case wedge::ONSCREEN_DOWN:
			return util::Point<float>(offset+dpad_size.w, shim::screen_size.h-offset-dpad_size.h);
		case wedge::ONSCREEN_LEFT:
			return util::Point<float>(offset, shim::screen_size.h-offset-dpad_size.h*2);
		case wedge::ONSCREEN_B1:
			return util::Point<float>(b1_x, shim::screen_size.h-offset-button_size.h);
		case wedge::ONSCREEN_B2:
			return util::Point<float>(b1_x + button_size.w, shim::screen_size.h-offset-button_size.h*2);
		case wedge::ONSCREEN_B3:
			return util::Point<float>(b1_x-button_size.w, shim::screen_size.h-offset-button_size.h*2);
		case wedge::ONSCREEN_B4:
			return util::Point<float>(b1_x, shim::screen_size.h-offset-button_size.h*3);
		default:
			return util::Point<float>(0, 0);
	}
}

void Globals::run()
{
	while (instance != nullptr && DOGO_INSTANCE->sp_replenish_count > 1000000) {
		DOGO_INSTANCE->sp_replenish_count -= 1000000;
		if (MENU == nullptr) {
			for (auto &s : INSTANCE->stats) {
				if (s.base.hp > 0) {
					s.base.mp = MIN(s.base.fixed.max_mp, s.base.mp+1);
				}
			}
		}
	}
}

void Globals::blur_fire()
{
	auto old_target = gfx::get_target_image();

	for (int sprite = 0; sprite < 3; sprite++) {
		gfx::Sprite *s;
		if (sprite == 0) {
			s = fire_big;
		}
		else if (sprite == 1) {
			s = fire_medium;
		}
		else {
			s = fire_small;
		}

		int nf = s->get_num_frames();

		for (int frame = 0; frame < nf; frame++) {
			auto tmp_shader = shim::current_shader;
			shim::current_shader = blur_banded_shader;
			shim::current_shader->use();

			gfx::Image *img = s->get_image(frame);

			gfx::Image *out = new gfx::Image(img->size * shim::scale);
			gfx::Image *out2 = new gfx::Image(img->size * shim::scale);

			gfx::set_target_image(out);
			gfx::clear(shim::transparent);
			gfx::set_target_image(out2);
			gfx::clear(shim::transparent);

			gfx::Image *curr = out2;

			const int NLOOPS = 2;

			for (int loops = 0; loops < NLOOPS; loops++) {
				const float OFFSET_INC_X = (shim::scale / NLOOPS) / out->size.w;
				const float OFFSET_INC_Y = (shim::scale / NLOOPS) / out->size.h;
				float offset_x = OFFSET_INC_X * (loops+1);
				float offset_y = OFFSET_INC_Y * (loops+1);
				shim::current_shader->set_float("offset_x", offset_x);
				shim::current_shader->set_float("offset_y", offset_y);

				if (loops == 0) {
					img->draw_rotated_scaled({img->size.w/2.0f, img->size.h/2.0f}, {out->size.w/2.0f, out->size.h/2.0f}, 0.0f, shim::scale);
				}
				else {
					gfx::Image *dest = curr;
					gfx::Image *src = dest == out ? out2 : out;
					gfx::set_target_image(dest);
					src->draw({0, 0});
				}

				if (curr == out) {
					curr = out2;
				}
				else {
					curr = out;
				}
			}

			shim::current_shader = tmp_shader;
			shim::current_shader->use();
			gfx::update_projection();

			if (curr == out) {
				fire_blurred[sprite].push_back(out2);
			}
			else {
				fire_blurred[sprite].push_back(out);
			}
			delete curr;
		}
	}

	gfx::set_target_image(old_target);
}

void Globals::lost_device()
{
	wedge::Globals::lost_device();

	for (int i = 0; i < 3; i++) {
		for (auto img : fire_blurred[i]) {
			delete img;
		}
		fire_blurred[i].clear();
	}
}

void Globals::found_device()
{
	wedge::Globals::found_device();

	blur_fire();
}

void Globals::get_joy_xy(TGUI_Event *event, float joy_axis0, float joy_axis1, int *x, int *y)
{
	if (wedge::is_onscreen_controller_enabled()) {
		if (event->joystick.axis == 0) {
			float curr_y = 0.0f;
			*y = fabsf(event->joystick.value) >= shim::joystick_deactivate_threshold ? 0 : joy_axis1;
			*x = fabsf(curr_y) >= shim::joystick_deactivate_threshold ? 0 : wedge::JOYF_TO_I(event->joystick.value, joy_axis0);
		}
		else {
			float curr_x = 0.0f;
			*x = fabsf(event->joystick.value) >= shim::joystick_deactivate_threshold ? 0 : joy_axis0;
			*y = fabsf(curr_x) >= shim::joystick_deactivate_threshold ? 0 : wedge::JOYF_TO_I(event->joystick.value, joy_axis1);
		}
	}
#ifdef STEAMWORKS
	else if (shim::steam_init_failed == false) {
		util::Point<float> v;

		if (BATTLE || MENU) {
			if (combo_up_data) {
				v.y = -1;
			}
			else if (combo_down_data) {
				v.y = 1;
			}
			else {
				v.y = 0;
			}
			if (combo_left_data) {
				v.x = -1;
			}
			else if (combo_right_data) {
				v.x = 1;
			}
			else {
				v.x = 0;
			}
		}
		else {
			if (ingame_up_data) {
				v.y = -1;
			}
			else if (ingame_down_data) {
				v.y = 1;
			}
			else {
				v.y = 0;
			}
			if (ingame_left_data) {
				v.x = -1;
			}
			else if (ingame_right_data) {
				v.x = 1;
			}
			else {
				v.x = 0;
			}
		}
		
		if (event->joystick.axis == 0) {
			float curr_y = v.y;
			*y = fabsf(event->joystick.value) >= shim::joystick_deactivate_threshold ? 0 : joy_axis1;
			*x = fabsf(curr_y) >= shim::joystick_deactivate_threshold ? 0 : wedge::JOYF_TO_I(event->joystick.value, joy_axis0);
		}
		else {
			float curr_x = v.x;
			*x = fabsf(event->joystick.value) >= shim::joystick_deactivate_threshold ? 0 : joy_axis0;
			*y = fabsf(curr_x) >= shim::joystick_deactivate_threshold ? 0 : wedge::JOYF_TO_I(event->joystick.value, joy_axis1);
		}
	}
#endif
	else {
		SDL_Joystick *joy = SDL_JoystickFromInstanceID(event->joystick.id);
		if (event->joystick.axis == 0) {
			float curr_y = TGUI4_NORMALISE_JOY_AXIS(SDL_JoystickGetAxis(joy, 1));
			*y = fabsf(event->joystick.value) >= shim::joystick_deactivate_threshold ? 0 : joy_axis1;
			*x = fabsf(curr_y) >= shim::joystick_deactivate_threshold ? 0 : wedge::JOYF_TO_I(event->joystick.value, joy_axis0);
		}
		else {
			float curr_x = TGUI4_NORMALISE_JOY_AXIS(SDL_JoystickGetAxis(joy, 0));
			*x = fabsf(event->joystick.value) >= shim::joystick_deactivate_threshold ? 0 : joy_axis0;
			*y = fabsf(curr_x) >= shim::joystick_deactivate_threshold ? 0 : wedge::JOYF_TO_I(event->joystick.value, joy_axis1);
		}
	}
}

bool Globals::should_show_back_arrow()
{
	if (shim::guis.size() != 0) {
		auto last = shim::guis.back();
		Yes_No_GUI *yn = dynamic_cast<Yes_No_GUI *>(last);
		Multiple_Choice_GUI *m = dynamic_cast<Multiple_Choice_GUI *>(last);
		if (yn) {
			if (yn->get_escape_cancels() == false) {
				return false;
			}
		}
		if (m) {
			if (m->get_escape_choice() == -1) {
				return false;
			}
		}
		return true;
	}
	else if (BATTLE != nullptr) {
		if (GLOBALS->dialogue_active(BATTLE) == true) {
			return false;
		}
		else {
			return true;
		}
	}
	else if (wedge::are_presses_paused() || (AREA && GLOBALS->dialogue_active(AREA) == true)) {
		return false;
	}
	else {
		return true;
	}
}

#ifdef STEAMWORKS
static void maybe_stick_movement(SDL_JoystickID id, util::Point<float> &data, ControllerAnalogActionData_t aad)
{
	// Things work much better when BOTH axes are generated even if only 1 changed
	if (data.x != aad.x || data.y != aad.y) {
		TGUI_Event e;

		e.type = TGUI_JOY_AXIS;
		e.joystick.id = id;
		e.joystick.button = 0;
		e.joystick.axis = 0;
		e.joystick.value = aad.x;
		e.joystick.is_repeat = false;
		shim::push_event(e);
		
		data.x = aad.x;

		e.type = TGUI_JOY_AXIS;
		e.joystick.id = id;
		e.joystick.button = 0;
		e.joystick.axis = 1;
		e.joystick.value = aad.y;
		e.joystick.is_repeat = false;
		shim::push_event(e);

		data.y = aad.y;
	}
}

static void button_event(SDL_JoystickID id, int button, bool onoff)
{
	TGUI_Event e;
	e.type = onoff ? TGUI_JOY_DOWN : TGUI_JOY_UP;
	e.joystick.id = id; 
	e.joystick.button = button;
	e.joystick.axis = -1;
	e.joystick.value = 0;
	e.joystick.is_repeat = false;
	shim::push_event(e);
}

static void dpad_event(SDL_JoystickID id, float x, float y, bool onoff)
{
	TGUI_Event e;

	e.type = onoff ? TGUI_JOY_DOWN : TGUI_JOY_UP;
	e.joystick.id = id;
	e.joystick.is_repeat = false;

	if (x < 0) {
		e.joystick.button = TGUI_B_L;
	}
	else if (x > 0) {
		e.joystick.button = TGUI_B_R;
	}
	else if (y < 0) {
		e.joystick.button = TGUI_B_U;
	}
	else {
		e.joystick.button = TGUI_B_D;
	}

	shim::push_event(e);
}

void Globals::read_steam_input_handles()
{
#ifdef STEAMWORKS
	if (shim::steam_init_failed == false) {
		ingame_action_set = SteamController()->GetActionSetHandle("InGameControls");
		menu_action_set = SteamController()->GetActionSetHandle("MenuControls");
		combo_action_set = SteamController()->GetActionSetHandle("ComboControls");
		ingame_up = SteamController()->GetDigitalActionHandle("ingame_up");
		ingame_down = SteamController()->GetDigitalActionHandle("ingame_down");
		ingame_left = SteamController()->GetDigitalActionHandle("ingame_left");
		ingame_right = SteamController()->GetDigitalActionHandle("ingame_right");
		ingame_use = SteamController()->GetDigitalActionHandle("ingame_use");
		ingame_pause = SteamController()->GetDigitalActionHandle("ingame_pause");
		ingame_menu = SteamController()->GetDigitalActionHandle("ingame_menu");
		menu_up = SteamController()->GetDigitalActionHandle("menu_up");
		menu_down = SteamController()->GetDigitalActionHandle("menu_down");
		menu_left = SteamController()->GetDigitalActionHandle("menu_left");
		menu_right = SteamController()->GetDigitalActionHandle("menu_right");
		menu_select = SteamController()->GetDigitalActionHandle("menu_select");
		menu_cancel = SteamController()->GetDigitalActionHandle("menu_cancel");
		menu_confirm = SteamController()->GetDigitalActionHandle("menu_confirm");
		menu_switch = SteamController()->GetDigitalActionHandle("menu_switch");
		menu_pause = SteamController()->GetDigitalActionHandle("m_pause");
		menu_menu = SteamController()->GetDigitalActionHandle("menu_menu");
		combo_up = SteamController()->GetDigitalActionHandle("combo_up");
		combo_down = SteamController()->GetDigitalActionHandle("combo_down");
		combo_left = SteamController()->GetDigitalActionHandle("combo_left");
		combo_right = SteamController()->GetDigitalActionHandle("combo_right");
		combo_punch = SteamController()->GetDigitalActionHandle("combo_punch");
		combo_kick = SteamController()->GetDigitalActionHandle("combo_kick");
		combo_fierce_punch = SteamController()->GetDigitalActionHandle("combo_fierce_punch");
		combo_fierce_kick = SteamController()->GetDigitalActionHandle("combo_fierce_kick");
		combo_cancel = SteamController()->GetDigitalActionHandle("combo_cancel");

		ingame_up_data = false;
		ingame_down_data = false;
		ingame_left_data = false;
		ingame_right_data = false;
		ingame_use_data = false;
		ingame_pause_data = false;
		ingame_menu_data = false;
		menu_up_data = false;
		menu_down_data = false;
		menu_left_data = false;
		menu_right_data = false;
		menu_select_data = false;
		menu_cancel_data = false;
		menu_confirm_data = false;
		menu_switch_data = false;
		menu_pause_data = false;
		menu_menu_data = false;
		combo_up_data = false;
		combo_down_data = false;
		combo_left_data = false;
		combo_right_data = false;
		combo_punch_data = false;
		combo_kick_data = false;
		combo_fierce_punch_data = false;
		combo_fierce_kick_data = false;
		combo_cancel_data = false;

		prev_ingame = false;
		prev_menu = true;
		prev_combo = false;

		last_action_set_change = SDL_GetTicks();
	}
#endif
}

void Globals::run_ingame_actions(bool ups_only)
{
	ControllerHandle_t handle = input::get_controller_handle();

	if (handle == 0) {
		input_handle_was_null = true;
		return;
	}
	else if (input_handle_was_null) {
		input_handle_was_null = false;
		read_steam_input_handles();
	}

	SDL_JoystickID id = input::get_controller_id();

	ControllerDigitalActionData_t dad;
	bool b;

	SteamController()->RunFrame();

	SteamController()->ActivateActionSet(handle, DOGO_GLOBALS->ingame_action_set);

	dad = SteamController()->GetDigitalActionData(handle, ingame_up);
	b = dad.bState;
	if (b != ingame_up_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 0.0f, -1.0f, b);
		}
		ingame_up_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, ingame_down);
	b = dad.bState;
	if (b != ingame_down_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 0.0f, 1.0f, b);
		}
		ingame_down_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, ingame_left);
	b = dad.bState;
	if (b != ingame_left_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, -1.0f, 0.0f, b);
		}
		ingame_left_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, ingame_right);
	b = dad.bState;
	if (b != ingame_right_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 1.0f, 0.0f, b);
		}
		ingame_right_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, ingame_use);
	b = dad.bState;
	if (b != ingame_use_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_action, b);
		}
		ingame_use_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, ingame_pause);
	b = dad.bState;
	if (b != ingame_pause_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_pause, b);
		}
		ingame_pause_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, ingame_menu);
	b = dad.bState;
	if (b != ingame_menu_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_menu, b);
		}
		ingame_menu_data = b;
	}
}

void Globals::run_menu_actions(bool ups_only)
{
	ControllerHandle_t handle = input::get_controller_handle();

	if (handle == 0) {
		input_handle_was_null = true;
		return;
	}
	else if (input_handle_was_null) {
		input_handle_was_null = false;
		read_steam_input_handles();
	}

	SDL_JoystickID id = input::get_controller_id();

	ControllerDigitalActionData_t dad;
	bool b;

	SteamController()->RunFrame();

	SteamController()->ActivateActionSet(handle, DOGO_GLOBALS->menu_action_set);

	dad = SteamController()->GetDigitalActionData(handle, menu_up);
	b = dad.bState;
	if (b != menu_up_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 0.0f, -1.0f, b);
		}
		menu_up_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_down);
	b = dad.bState;
	if (b != menu_down_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 0.0f, 1.0f, b);
		}
		menu_down_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_left);
	b = dad.bState;
	if (b != menu_left_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, -1.0f, 0.0f, b);
		}
		menu_left_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_right);
	b = dad.bState;
	if (b != menu_right_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 1.0f, 0.0f, b);
		}
		menu_right_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_select);
	b = dad.bState;
	if (b != menu_select_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_action, b);
		}
		menu_select_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_cancel);
	b = dad.bState;
	if (b != menu_cancel_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_back, b);
		}
		menu_cancel_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_confirm);
	b = dad.bState;
	if (b != menu_confirm_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_confirm, b);
		}
		menu_confirm_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_switch);
	b = dad.bState;
	if (b != menu_switch_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_switch, b);
		}
		menu_switch_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_pause);
	b = dad.bState;
	if (b != menu_pause_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_pause, b);
		}
		menu_pause_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, menu_menu);
	b = dad.bState;
	if (b != menu_menu_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_menu, b);
		}
		menu_menu_data = b;
	}
}

void Globals::run_combo_actions(bool ups_only)
{
	ControllerHandle_t handle = input::get_controller_handle();

	if (handle == 0) {
		input_handle_was_null = true;
		return;
	}
	else if (input_handle_was_null) {
		input_handle_was_null = false;
		read_steam_input_handles();
	}

	SDL_JoystickID id = input::get_controller_id();

	ControllerDigitalActionData_t dad;
	bool b;

	SteamController()->RunFrame();

	SteamController()->ActivateActionSet(handle, DOGO_GLOBALS->combo_action_set);

	dad = SteamController()->GetDigitalActionData(handle, combo_up);
	b = dad.bState;
	if (b != combo_up_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 0.0f, -1.0f, b);
		}
		combo_up_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_down);
	b = dad.bState;
	if (b != combo_down_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 0.0f, 1.0f, b);
		}
		combo_down_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_left);
	b = dad.bState;
	if (b != combo_left_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, -1.0f, 0.0f, b);
		}
		combo_left_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_right);
	b = dad.bState;
	if (b != combo_right_data) {
		if ((b == false && ups_only) || ups_only == false) {
			dpad_event(id, 1.0f, 0.0f, b);
		}
		combo_right_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_punch);
	b = dad.bState;
	if (b != combo_punch_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_b3, b);
		}
		combo_punch_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_kick);
	b = dad.bState;
	if (b != combo_kick_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_b1, b);
		}
		combo_kick_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_fierce_punch);
	b = dad.bState;
	if (b != combo_fierce_punch_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_b4, b);
		}
		combo_fierce_punch_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_fierce_kick);
	b = dad.bState;
	if (b != combo_fierce_kick_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, GLOBALS->joy_b2, b);
		}
		combo_fierce_kick_data = b;
	}

	dad = SteamController()->GetDigitalActionData(handle, combo_cancel);
	b = dad.bState;
	if (b != combo_cancel_data) {
		if ((b == false && ups_only) || ups_only == false) {
			button_event(id, joy_cancel, b);
		}
		combo_cancel_data = b;
	}
}

void Globals::handle_steam_input()
{
	if (shim::steam_init_failed == false) {
		bool use_ingame = false;
		bool use_menu = false;
		bool use_combo = false;

		bool all_fading_settings = true;

		for (size_t i = 0; i < shim::guis.size(); i++) {
			gui::GUI *g = shim::guis[i];
			if (!((
				dynamic_cast<Settings_GUI *>(g) ||
				dynamic_cast<Language_GUI *>(g) ||
				dynamic_cast<Video_Settings_GUI *>(g) ||
				dynamic_cast<Audio_Settings_GUI *>(g) ||
				dynamic_cast<Controls_Settings_GUI *>(g) ||
				dynamic_cast<Miscellaneous_Settings_GUI *>(g) ||
				dynamic_cast<Save_Slot_GUI *>(g) ||
				dynamic_cast<Menu_GUI *>(g) ||
				dynamic_cast<Multiple_Choice_GUI *>(g) // this ones a hack for the load screen "Load/Autosaves/Erase"
			) && g->is_transitioning_out())) {
				all_fading_settings = false;
				break;
			}
		}

		if (all_fading_settings == false && shim::guis.size() > 0) {
			bool in_battle = BATTLE != nullptr;
			bool detecting_combo = false;
			if (in_battle) {
				detecting_combo = static_cast<Battle_Game *>(BATTLE)->is_detecting_combo();
			}
			else {
				for (size_t i = 0; i < shim::guis.size(); i++) {
					gui::GUI *g = shim::guis[i];
					Practice_Menu_GUI *pg = dynamic_cast<Practice_Menu_GUI *>(g);
					if (pg) {
						if (pg->is_practicing()) {
							detecting_combo = true;
							break;
						}
					}
				}
			}
			if (detecting_combo) {
				use_combo = true;
			}
			else {
				use_menu = true;
			}
		}
		else if ((BATTLE == nullptr || BATTLE->is_done()) && AREA) {
			use_ingame = true;
		}
		else if (BATTLE) {
			bool detecting_combo = static_cast<Battle_Game *>(BATTLE)->is_detecting_combo();
			if (detecting_combo) {
				use_combo = true;
			}
			else {
				use_menu = true;
			}
		}

		// This fixes a weird/bug behaviour of Steam Input: when you switch to a new action set,
		// if a button was held from before the switch it's reported as down so it trigger a down.
		// So you can get a button repeating across different menus/action types
		// EDIT: does not work anyway... maybe remove some of this
		if (prev_ingame != use_ingame || prev_menu != use_menu || prev_combo != use_combo) {
			last_action_set_change = SDL_GetTicks();
			if (prev_ingame) {
				ingame_buttons_up();
			}
			else if (prev_menu) {
				menu_buttons_up();
			}
			else if (prev_combo) {
				combo_buttons_up();
			}
		}

		if (SDL_GetTicks()-last_action_set_change < 20) {
			if (use_ingame) {
				run_ingame_actions(true);
			}
			else if (use_menu) {
				run_menu_actions(true);
			}
			else if (use_combo) {
				run_combo_actions(true);
			}
		}
		else {
			if (use_ingame) {
				run_ingame_actions(false);
			}
			else if (use_menu) {
				run_menu_actions(false);
			}
			else if (use_combo) {
				run_combo_actions(false);
			}
		}

		prev_ingame = use_ingame;
		prev_menu = use_menu;
		prev_combo = use_combo;
	}
}

void Globals::ingame_buttons_up()
{
	SDL_JoystickID id = input::get_controller_id();

	if (ingame_up_data) {
		ingame_up_data = false;
		dpad_event(id, 0.0f, -1.0f, false);
	}

	if (ingame_down_data) {
		ingame_down_data = false;
		dpad_event(id, 0.0f, 1.0f, false);
	}

	if (ingame_left_data) {
		ingame_left_data = false;
		dpad_event(id, -1.0f, 0.0f, false);
	}

	if (ingame_right_data) {
		ingame_right_data = false;
		dpad_event(id, 1.0f, 0.0f, false);
	}

	if (ingame_use_data) {
		ingame_use_data = false;
		button_event(id, GLOBALS->joy_action, false);
	}

	if (ingame_pause_data) {
		ingame_pause_data = false;
		button_event(id, GLOBALS->joy_pause, false);
	}

	if (ingame_menu_data) {
		ingame_menu_data = false;
		button_event(id, GLOBALS->joy_menu, false);
	}
}

void Globals::menu_buttons_up()
{
	SDL_JoystickID id = input::get_controller_id();

	if (menu_up_data) {
		menu_up_data = false;
		dpad_event(id, 0.0f, -1.0f, false);
	}

	if (menu_down_data) {
		menu_down_data = false;
		dpad_event(id, 0.0f, 1.0f, false);
	}

	if (menu_left_data) {
		menu_left_data = false;
		dpad_event(id, -1.0f, 0.0f, false);
	}

	if (menu_right_data) {
		menu_right_data = false;
		dpad_event(id, 1.0f, 0.0f, false);
	}

	if (menu_select_data) {
		menu_select_data = false;
		button_event(id, GLOBALS->joy_action, false);
	}

	if (menu_cancel_data) {
		menu_cancel_data = false;
		button_event(id, GLOBALS->joy_back, false);
	}

	if (menu_switch_data) {
		menu_switch_data = false;
		button_event(id, GLOBALS->joy_switch, false);
	}

	if (menu_confirm_data) {
		menu_confirm_data = false;
		button_event(id, GLOBALS->joy_confirm, false);
	}

	if (menu_pause_data) {
		menu_pause_data = false;
		button_event(id, GLOBALS->joy_pause, false);
	}

	if (menu_menu_data) {
		menu_menu_data = false;
		button_event(id, GLOBALS->joy_menu, false);
	}
}

void Globals::combo_buttons_up()
{
	SDL_JoystickID id = input::get_controller_id();

	if (combo_up_data) {
		combo_up_data = false;
		dpad_event(id, 0.0f, -1.0f, false);
	}

	if (combo_down_data) {
		combo_down_data = false;
		dpad_event(id, 0.0f, 1.0f, false);
	}

	if (combo_left_data) {
		combo_left_data = false;
		dpad_event(id, -1.0f, 0.0f, false);
	}

	if (combo_right_data) {
		combo_right_data = false;
		dpad_event(id, 1.0f, 0.0f, false);
	}

	if (combo_punch_data) {
		combo_punch_data = false;
		button_event(id, GLOBALS->joy_b3, false);
	}

	if (combo_kick_data) {
		combo_kick_data = false;
		button_event(id, GLOBALS->joy_b1, false);
	}

	if (combo_fierce_punch_data) {
		combo_fierce_punch_data = false;
		button_event(id, GLOBALS->joy_b4, false);
	}

	if (combo_fierce_kick_data) {
		combo_fierce_kick_data = false;
		button_event(id, GLOBALS->joy_b2, false);
	}

	if (combo_cancel_data) {
		combo_cancel_data = false;
		button_event(id, GLOBALS->joy_cancel, false);
	}
}
#endif

//--

Globals::Instance::Instance(util::JSON::Node *root) :
	wedge::Globals::Instance(root)
{
	sp_replenish_count = 0;

	enemies_killed = 0;

	util::JSON::Node *shim_json = shim::shim_json->get_root();

	milestones = new bool[MS_SIZE];

	for (int i = 0; i < MS_SIZE; i++) {
		milestones[i] = shim_json->get_nested_bool("wedge>instance>milestones>" + util::itos(i), &milestones[i], false);
	}

	saw_sneak = false;

	if (root) {
		util::JSON::Node *n;
		
		util::JSON::Node *ms = root->find("milestones");
		if (ms != NULL) {
			for (size_t i = 0; i < ms->children.size(); i++) {
				util::JSON::Node *node = ms->children[i];
				milestones[i] = node->as_bool();
			}
		}

		n = root->find("difficulty");
		if (n != NULL) {
			difficulty = (Difficulty)n->as_int();
		}
		n = root->find("saw_sneak");
		if (n != NULL) {
			saw_sneak = n->as_bool();
		}
		n = root->find("enemies_killed");
		if (n != NULL) {
			enemies_killed = n->as_int();
		}
		n = root->find("combos");
		for (size_t i = 0; i < n->children.size(); i++) {
			std::vector<std::string> v;
			util::JSON::Node *n2 = n->children[i];
			for (size_t j = 0; j < n2->children.size(); j++) {
				v.push_back(n2->children[j]->as_string());
			}
			combos.push_back(v);
		}

		if (version < 2) {
			for (auto &s : stats) {
				if (version == 0) {
					s.base.fixed.set_extra(LUCK, 25);
				}
				else {
					int extra_turns = s.base.fixed.get_extra(0);
					s.base.fixed.set_extra(LUCK, 25);
					s.base.fixed.set_extra(EXTRA_TURNS, extra_turns);
				}
			}
		}
	}
	else {
		stats.push_back(wedge::Player_Stats());
		stats[0].sprite = NULL; // fix crash if line below throws exception
		stats[0].sprite = new gfx::Sprite("pleasant");
		
		stats[0].base.set_name("Pleasant");

		for (size_t i = 0; i < stats.size(); i++) {
			stats[i].level = 1;
			stats[i].experience = 0;
			stats[i].base.fixed.max_hp = 100;
			stats[i].base.fixed.max_mp = 25;
			stats[i].base.fixed.attack = 25;
			stats[i].base.fixed.defence = 25;
			stats[i].base.fixed.set_extra(LUCK, 25);
			stats[i].base.hp = stats[i].base.fixed.max_hp;
			stats[i].base.mp = stats[i].base.fixed.max_mp;
		}

		party_following_player = true;

		std::vector<std::string> v;

		v.push_back("Punch");
		combos.push_back(v);

		v.clear();
		
		v.push_back("Kick");
		combos.push_back(v);
	}
}

Globals::Instance::~Instance()
{
	delete[] milestones;
	for (int i = 0; i < MS_SIZE; i++) {
		shim::shim_json->remove("wedge>instance>milestones>" + util::itos(i), true);
	}
}

int Globals::Instance::num_milestones()
{
	return MS_SIZE;
}

std::string Globals::Instance::save()
{
	std::string s = wedge::Globals::Instance::save();
	s += ",";
	s += "\"difficulty\": " + util::itos((int)difficulty) + ",\n";
	s += "\"saw_sneak\": " + std::string(saw_sneak ? "true" : "false") + ",\n";
	s += "\"enemies_killed\": " + util::itos(enemies_killed) + ",\n";
	s += "\"combos\": [\n";
	for (size_t i = 0; i < combos.size(); i++) {
		std::vector<std::string> &v = combos[i];
		s += "[\n";
		for (size_t j = 0; j < v.size(); j++) {
			s += util::string_printf("\"%s\"%s\n", v[j].c_str(), j == v.size()-1 ? "" : ",");
		}
		s += "]\n";
	}
	s += "]\n";
	return s;
}
