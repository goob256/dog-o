#include <wedge2/achieve.h>
#include <wedge2/area_game.h>
#include <wedge2/a_star.h>
#include <wedge2/change_angle.h>
#include <wedge2/check_positions.h>
#include <wedge2/chest.h>
#include <wedge2/delay.h>
#include <wedge2/delete_map_entity.h>
#include <wedge2/general.h>
#include <wedge2/generic_immediate_callback.h>
#include <wedge2/give_object.h>
#include <wedge2/input.h>
#include <wedge2/npc.h>
#include <wedge2/offset_arc.h>
#include <wedge2/omnipresent.h>
#include <wedge2/pause_presses.h>
#include <wedge2/play_animation.h>
#include <wedge2/play_music.h>
#include <wedge2/play_sound.h>
#include <wedge2/rumble.h>
#include <wedge2/set_animation.h>
#include <wedge2/set_direction.h>
#include <wedge2/set_integer.h>
#include <wedge2/set_milestone_complete.h>
#include <wedge2/set_solid.h>
#include <wedge2/set_visible.h>
#include <wedge2/shop_step.h>
#include <wedge2/slide_entity.h>
#include <wedge2/slide_entity_offset.h>
#include <wedge2/stop_sound.h>
#include <wedge2/tile_movement.h>
#include <wedge2/wait.h>
#include <wedge2/wait_for_integer.h>

#include "achievements.h"
#include "area_game.h"
#include "battle_transition_in.h"
#include "battle_transition_in2.h"
#include "battle_transition_out.h"
#include "battle_transition_out2.h"
#include "autosave.h"
#include "barter.h"
#include "battles.h"
#include "dialogue.h"
#include "general.h"
#include "globals.h"
#include "inventory.h"
#include "menu.h"
#include "milestones.h"
#include "pan_camera.h"
#include "shop.h"
#include "shop_game.h"
#include "start_battle.h"
#include "stats.h"

#include <shim2/internal/audio.h>

static const int credits_len = 7250;
static const int credits_len2 = 2500;
static const int credits_len3 = 1000;
static const int credits_buf = 32;

static void credits_draw1(float p)
{
	gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(0, 0), shim::screen_size);

	util::Point<float> jb_pos(int(-shim::tile_size*2+(shim::screen_size.w+shim::tile_size*3)*p), shim::screen_size.h/2-shim::tile_size/2);
	DOGO_GLOBALS->jb_credits->get_current_image()->draw(jb_pos);

	std::string l1 = GLOBALS->game_t->translate(563)/* Originally: Code, Art & Audio */;
	std::string l2 = GLOBALS->game_t->translate(564)/* Originally: Trent Gamblin */;

	int w1 = shim::font->get_text_width(l1);
	int w2 = DOGO_GLOBALS->bold_font->get_text_width(l2);

	int max_w = MAX(w1, w2);

	int dx1 = shim::screen_size.w / 2 - w1/2;
	int dx2 = shim::screen_size.w / 2 - w2/2;

	const int h1 = shim::font->get_height();
	const int h2 = DOGO_GLOBALS->bold_font->get_height();

	int y1 = shim::screen_size.h / 2 - (h1+h2+10)/2;
	int y2 = y1 + h1 + 10;

	shim::font->draw(shim::palette[27], l1, util::Point<float>(MIN(jb_pos.x-credits_buf-max_w+(max_w-w1)/2, dx1), y1));
	DOGO_GLOBALS->bold_font->draw(shim::white, l2, util::Point<float>(MIN(jb_pos.x-credits_buf-max_w+(max_w-w2)/2, dx2), y2));
}

static void credits_draw2(float p)
{
	gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(0, 0), shim::screen_size);

	int credits_buf = shim::tile_size*2;

	util::Point<float> jb_pos((shim::screen_size.w+shim::tile_size)-(shim::screen_size.w+shim::tile_size*3)*p, shim::screen_size.h/2-shim::tile_size/2);
	DOGO_GLOBALS->jb_credits->get_current_image()->draw(jb_pos);

	std::string l1 = GLOBALS->game_t->translate(565)/* Originally: Fonts */;
	std::string l2 = GLOBALS->game_t->translate(566)/* Originally: Craig Kroeger */;

	int w1 = shim::font->get_text_width(l1);
	int w2 = DOGO_GLOBALS->bold_font->get_text_width(l2);

	int max_w = MAX(w1, w2);

	int dx1 = shim::screen_size.w / 2 - w1/2;
	int dx2 = shim::screen_size.w / 2 - w2/2;

	const int h1 = shim::font->get_height();
	const int h2 = DOGO_GLOBALS->bold_font->get_height();

	int y1 = shim::screen_size.h / 2 - (h1+h2+10)/2;
	int y2 = y1 + h1 + 10;

	shim::font->draw(shim::palette[27], l1, util::Point<float>(MAX(jb_pos.x+credits_buf+max_w-(max_w-w1)/2-w1, dx1), y1));
	DOGO_GLOBALS->bold_font->draw(shim::white, l2, util::Point<float>(MAX(jb_pos.x+credits_buf+max_w-(max_w-w2)/2-w2, dx2), y2));
}

static void credits_draw3(float p)
{
	gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(0, 0), shim::screen_size);

	int tot_sz = shim::screen_size.w + shim::tile_size;

	util::Point<float> jb_pos(int(-shim::tile_size*2+(shim::screen_size.w+shim::tile_size*3)*p), shim::screen_size.h/2-shim::tile_size/2);
	DOGO_GLOBALS->jb_credits->get_current_image()->draw(jb_pos);
	
	std::string l1 = GLOBALS->game_t->translate(567)/* Originally: Testing */;
	std::string l2 = GLOBALS->game_t->translate(568)/* Originally: Eric Johnson */;
	std::string l3 = GLOBALS->game_t->translate(569)/* Originally: darkmikasonfire */;

	int w1 = shim::font->get_text_width(l1);
	int w2 = DOGO_GLOBALS->bold_font->get_text_width(l2);
	int w3 = DOGO_GLOBALS->bold_font->get_text_width(l3);

	int max_w = MAX(w1, w2);
	max_w = MAX(max_w, w3);

	int dx1 = shim::screen_size.w / 2 - w1/2;
	int dx2 = shim::screen_size.w / 2 - w2/2;
	int dx3 = shim::screen_size.w / 2 - w3/2;

	const int h1 = shim::font->get_height();
	const int h2 = DOGO_GLOBALS->bold_font->get_height();

	int y1 = shim::screen_size.h / 2 - (h1+h2+h2+15)/2;
	int y2 = y1 + h1 + 10;
	int y3 = y2 + h2 + 5;

	shim::font->draw(shim::palette[27], l1, util::Point<float>(MIN(jb_pos.x-credits_buf-max_w+(max_w-w1)/2, dx1), y1));
	DOGO_GLOBALS->bold_font->draw(shim::white, l2, util::Point<float>(MIN(jb_pos.x-credits_buf-max_w+(max_w-w2)/2, dx2), y2));
	DOGO_GLOBALS->bold_font->draw(shim::white, l3, util::Point<float>(MIN(jb_pos.x-credits_buf-max_w+(max_w-w3)/2, dx3), y3));
}

static void credits_draw4(float p)
{
	gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(0, 0), shim::screen_size);

	int tot_sz = shim::screen_size.w + shim::tile_size;

	util::Point<float> jb_pos((shim::screen_size.w+shim::tile_size)-(shim::screen_size.w+shim::tile_size*3)*p, shim::screen_size.h/2-shim::tile_size/2);
	DOGO_GLOBALS->jb_credits->get_current_image()->draw(jb_pos);
	
	std::string l1 = GLOBALS->game_t->translate(570)/* Originally: © 2022 Nooskewl */;
	
	int w1 = 32;
	int w2 = DOGO_GLOBALS->bold_font->get_text_width(l1);

	int max_w = MAX(w1, w2);

	int dx1 = shim::screen_size.w / 2 - w1/2;
	int dx2 = shim::screen_size.w / 2 - w2/2;

	const int h = DOGO_GLOBALS->bold_font->get_height();

	int y1 = shim::screen_size.h / 2 - (h+10+32)/2;
	int y2 = y1 + 32 + 10;

	DOGO_GLOBALS->nooskewl_logo->draw(util::Point<float>(MAX(jb_pos.x+credits_buf+max_w-(max_w-w1)/2-w1, dx1), y1));

	DOGO_GLOBALS->bold_font->draw(shim::white, l1, util::Point<float>(MAX(jb_pos.x+credits_buf+max_w-(max_w-w2)/2-w2, dx2), y2));
}

static void credits_draw5(float p)
{
	gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(0, 0), shim::screen_size);

	std::string l1 = GLOBALS->game_t->translate(570)/* Originally: © 2022 Nooskewl */;
	
	int w1 = 32;
	int w2 = DOGO_GLOBALS->bold_font->get_text_width(l1);

	int max_w = MAX(w1, w2);

	int dx1 = shim::screen_size.w / 2 - w1/2;
	int dx2 = shim::screen_size.w / 2 - w2/2;

	const int h = DOGO_GLOBALS->bold_font->get_height();

	int y1 = shim::screen_size.h / 2 - (h+10+32)/2;
	int y2 = y1 + 32 + 10;

	DOGO_GLOBALS->nooskewl_logo->draw(util::Point<float>(dx1, y1));

	DOGO_GLOBALS->bold_font->draw(shim::white, l1, util::Point<float>(dx2, y2));
}

static void credits_draw6(float p)
{
	gfx::draw_filled_rectangle(shim::palette[38], util::Point<int>(0, 0), shim::screen_size);
}

class Credits_Step : public wedge::Step
{
public:
	// tag is used for Name: and maybe other things in bold
	Credits_Step(wedge::Task *task);
	virtual ~Credits_Step();
	
	virtual void draw_fore();

	bool run();

private:
	int credits_i;
	int credits_screen;
	bool am_done;
};

Credits_Step::Credits_Step(wedge::Task *task) :
	wedge::Step(task)
{
	credits_i = 0;
	credits_screen = 0;
	am_done = false;
}

Credits_Step::~Credits_Step()
{
}

bool Credits_Step::run()
{
	static const int credits_rate = 1000 / shim::logic_rate;
	int credits_loops;

	if (credits_screen == 5) {
		credits_loops = credits_len3 / credits_rate;
	}
	else if (credits_screen == 4) {
		credits_loops = credits_len2 / credits_rate;
	}
	else {
		credits_loops = credits_len / credits_rate;
	}

	if (credits_screen == 5 && credits_i == credits_loops-1) {
		am_done = true;
	}

	credits_i++;

	if (am_done == true) {
		send_done_signal();
	}
	return am_done == false;
}

void Credits_Step::draw_fore()
{
	if (credits_i == 0 && credits_screen == 0) {
		OMNIPRESENT->end_fade();
		shim::music->stop();
		DOGO_GLOBALS->credits_music->play(shim::music_volume, false);
		DOGO_GLOBALS->jb_credits->set_animation("walk_e");
	}

	static const int credits_rate = 1000 / shim::logic_rate;
	int credits_loops;

	if (credits_screen == 5) {
		credits_loops = credits_len3 / credits_rate;
	}
	else if (credits_screen == 4) {
		credits_loops = credits_len2 / credits_rate;
	}
	else {
		credits_loops = credits_len / credits_rate;
	}

	if (credits_screen == 0) {
		credits_draw1(credits_i/(float)credits_loops);
	}
	else if (credits_screen == 1) {
		credits_draw2(credits_i/(float)credits_loops);
	}
	else if (credits_screen == 2) {
		credits_draw3(credits_i/(float)credits_loops);
	}
	else if (credits_screen == 3) {
		credits_draw4(credits_i/(float)credits_loops);
	}
	else if (credits_screen == 4) {
		credits_draw5(credits_i/(float)credits_loops);
	}
	else if (credits_screen == 5) {
		credits_draw6(credits_i/(float)credits_loops);
	}

	if (credits_i >= credits_loops) {
		credits_i = 0;
		credits_screen++;
		if (DOGO_GLOBALS->jb_credits->get_animation() == "walk_w") {
			DOGO_GLOBALS->jb_credits->set_animation("walk_e");
		}
		else {
			DOGO_GLOBALS->jb_credits->set_animation("walk_w");
		}
	}
}

static void exit_to_menu(void *data)
{
	AREA->set_gameover(true);
	AREA->set_gameover_time(0);
}

static void credits(void *data)
{
	OMNIPRESENT->start_fade(shim::palette[38], 0, 2500);
	
	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Delay_Step(2500, new_task))
	ADD_STEP(new Credits_Step(new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(exit_to_menu, nullptr, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks : public wedge::Area_Hooks
{
public:
	Area_Hooks(wedge::Area *area) :
		wedge::Area_Hooks(area)
	{
	}
	
	void started()
	{
		maybe_autosave();
	
		wedge::Area_Hooks::started();
	}

	void draw_darkness(util::Point<float> map_offset, float alpha = 1.0f)
	{
		real_draw_darkness(darkness_image1, darkness_image2, darkness_offset1, darkness_offset2, map_offset, alpha);
	}

	void maybe_autosave()
	{
		wedge::Area_Hooks::maybe_autosave();

		if (static_cast<Area_Game *>(AREA)->get_num_areas_created() > 1 && can_autosave()) {
			if (INSTANCE->is_milestone_complete(MS_NO_AUTOSAVE) == false) {
				autosave(true);
			}
		}
	}

protected:
	gfx::Image *darkness_image1;
	gfx::Image *darkness_image2;
	util::Point<float> darkness_offset1;
	util::Point<float> darkness_offset2;
};

static void remove_stick_start(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	area->remove_entity(area->find_entity("stick"), true);
}

static void remove_jellybean_start(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	area->remove_entity(area->find_entity("jellybean"), true);
}

static void pickup_stick(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	wedge::Map_Entity *stick = area->find_entity("stick");
	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	stick->set_visible(false);
	pleasant->get_sprite()->set_animation("stick");
}

static void throw_stick(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	wedge::Map_Entity *stick = area->find_entity("stick");
	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	stick->set_visible(true);
	pleasant->get_sprite()->set_animation("threw_w");
	DOGO_GLOBALS->throw_->play(false);
}

static void no_tom(void *data)
{
	wedge::Map_Entity *descent = static_cast<wedge::Map_Entity *>(data);
	wedge::Map_Entity *tom = AREA->get_player(TOM);
	descent->get_sprite()->set_animation("no_tom");
	descent->set_shadow(false, {0, 0});
	tom->set_visible(true);
	tom->get_sprite()->set_animation("kick");
	tom->set_sub_offset_from_shadow_pos(true);
	DOGO_GLOBALS->jump->play(false);
}

static void kick_sfx_callback(void *data)
{
	DOGO_GLOBALS->hit->play(false);
	DOGO_GLOBALS->jump->play(false);
	auto area = static_cast<wedge::Area *>(data);
	area->remove_entity(area->find_entity("bucket_shadow"), true);
}

static void just_tom_start(void *data)
{
	wedge::Map_Entity *descent = static_cast<wedge::Map_Entity *>(data);
	wedge::Map_Entity *pleasant = AREA->get_current_area()->find_entity("pleasant_bucket");
	wedge::Map_Entity *tom = AREA->get_player(TOM);

	descent->get_sprite()->set_animation("just_tom_start");
	descent->set_shadow(true, {12, 6});
	tom->set_visible(false);
	pleasant->set_visible(true);
	pleasant->get_sprite()->set_animation("only");
	DOGO_GLOBALS->fall->play(false);
	DOGO_GLOBALS->shock->play(false);

	DOGO_GLOBALS->rope->play(true);
}

static void just_tom(void *data)
{
	wedge::Map_Entity *descent = static_cast<wedge::Map_Entity *>(data);
	descent->get_sprite()->set_animation("just_tom");
}

static void tom_going_in_well(void *data)
{
	wedge::Map_Entity *descent = static_cast<wedge::Map_Entity *>(data);
	wedge::Map_Entity *tom = AREA->get_player(TOM);
	descent->get_sprite()->set_animation("just_rope");
	descent->set_shadow(false, {0, 0});
	tom->set_visible(true);
	DOGO_GLOBALS->rope->stop();
	DOGO_GLOBALS->big_jump->play(false);
}

static void done_cave1_scene1(void *data)
{
	INSTANCE->set_milestone_complete(MS_NO_AUTOSAVE, false);
	autosave(true);
}

static void change_to_cave1(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	std::vector< util::Point<int> > pos;
	std::vector<wedge::Direction> dir;
	pos.push_back(util::Point<int>(7, 10));
	pos.push_back(util::Point<int>(7, 11));
	dir.push_back(wedge::DIR_S);
	dir.push_back(wedge::DIR_S);
	area->set_next_area("cave1_start", pos, dir);
}

static void remove_jb_shadow(void *data)
{
	auto area = static_cast<wedge::Area *>(data);
	auto jb = area->find_entity("jellybean");
	jb->set_shadow(false, {0, 0});
}

static void remove_bucket_shadow(void *data)
{
	auto area = static_cast<wedge::Area *>(data);
	area->find_entity("pleasant_bucket")->set_shadow(false, {0, 0});
}

class Area_Hooks_Start : public Area_Hooks
{
public:
	Area_Hooks_Start(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(15, 6), util::Size<int>(1, 9));
		z.area_name = "village";
		z.topleft_dest = util::Point<int>(0, 6);
		z.direction = wedge::DIR_E;
		scroll_zones.push_back(z);

		if (INSTANCE->is_milestone_complete(MS_TOM_JOINED)) {
			INSTANCE->set_milestone_complete(MS_NO_AUTOSAVE, true);
		}
	}
	
	virtual ~Area_Hooks_Start()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/town.mml");

		if (new_game) {
			wedge::Map_Entity *stick = new wedge::Map_Entity("stick");
			stick->start(area);
			stick->set_position(util::Point<int>(11, 11));
			stick->set_sprite(new gfx::Sprite("stick"));
			stick->set_solid(false);
			area->add_entity(stick);
			
			wedge::Map_Entity *jellybean = new wedge::Map_Entity("jellybean");
			jellybean->start(area);
			jellybean->set_position(util::Point<int>(14, 9));
			jellybean->set_sprite(new gfx::Sprite("jellybean"));
			jellybean->set_solid(false);
			jellybean->set_direction(wedge::DIR_S, true, false);
			jellybean->set_shadow(true, util::Size<int>(12, 6));
			jellybean->set_sub_offset_from_shadow_pos(true);
			area->add_entity(jellybean);
			
			wedge::Map_Entity *well = new wedge::Map_Entity("well");
			well->start(area);
			well->set_position(util::Point<int>(6, 10));
			well->set_size(util::Size<int>(3, 3));
			area->add_entity(well);
								
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1));
			chest->start(area);
			chest->set_position(util::Point<int>(5, 11));
			area->add_entity(chest);
		}

		if (INSTANCE->is_milestone_complete(MS_OPENING_SAVE) == false) {
			INSTANCE->set_milestone_complete(MS_OPENING_SAVE, true);
			::save();
		}

		return true;
	}

	void started()
	{
		if (INSTANCE->is_milestone_complete(MS_OPENING_SCENE) == false) {
			INSTANCE->set_milestone_complete(MS_OPENING_SCENE, true);

			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *jellybean = area->find_entity("jellybean");
			wedge::Map_Entity *stick = area->find_entity("stick");

			wedge::pause_presses(true);

			NEW_SYSTEM_AND_TASK(AREA)
			Dialogue_Step *ds = new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(18)/* Originally: Nothing interesting ever happens around here, eh Jellybean? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task);
			ADD_STEP(ds)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(12, 11), new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(pickup_stick, area, new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "throw_stick", new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			Dialogue_Step *ds2 = new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(19)/* Originally: Fetch! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task);
			ADD_STEP(ds2)
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(throw_stick, area, new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
			ds->add_monitor(wait_step);
			ADD_STEP(wait_step)
			ADD_STEP(new wedge::A_Star_Step(jellybean, util::Point<int>(12, 9), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(jellybean, wedge::DIR_S, true, false, new_task))
			wedge::Wait_Step *wait_step2 = new wedge::Wait_Step(new_task);
			ds2->add_monitor(wait_step2);
			ADD_STEP(wait_step2)
			wedge::A_Star_Step *as = new wedge::A_Star_Step(jellybean, util::Point<int>(10, 9), new_task);
			ADD_STEP(as)
			ADD_STEP(new wedge::Set_Animation_Step(jellybean, "jump_w", new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step3 = new wedge::Wait_Step(new_task);
			ds2->add_monitor(wait_step3);
			ADD_STEP(wait_step3)
			auto cas = new wedge::Change_Angle_Step(stick, 0.0f, float(-M_PI)*6.0f, 800, new_task);
			ADD_STEP(cas)
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step4 = new wedge::Wait_Step(new_task);
			ds2->add_monitor(wait_step4);
			ADD_STEP(wait_step4)
			ADD_STEP(new wedge::Offset_Arc_Step(stick, util::Point<float>(0.5f, -0.5f), util::Point<float>(-3.9f, -2.0f), 2.0f, 800, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(stick, util::Point<float>(-3.9f, -2.0f), util::Point<float>(-4.0f, -1.2f), 0.0f, 66, new_task))
			wedge::Wait_Step *wait_step7 = new wedge::Wait_Step(new_task);
			cas->add_monitor(wait_step7);
			ADD_STEP(wait_step7)
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_stick_start, area, new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step5 = new wedge::Wait_Step(new_task);
			as->add_monitor(wait_step5);
			ADD_STEP(wait_step5)
			auto cas2 = new wedge::Change_Angle_Step(jellybean, float(M_PI)/4.0f, float(-M_PI)/2.0f, 500, new_task);
			ADD_STEP(cas2)
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step6 = new wedge::Wait_Step(new_task);
			as->add_monitor(wait_step6);
			ADD_STEP(wait_step6)
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->jump, false, false, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(jellybean, util::Point<float>(0.0f, 0.0f), util::Point<float>(-2.9f, 0.0f), 1.5f, 533, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_jb_shadow, area, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(jellybean, util::Point<float>(-2.9f, 0.0f), util::Point<float>(-3.0f, 0.8f), 0.0f, 66, new_task))
			wedge::Wait_Step *wait_step8 = new wedge::Wait_Step(new_task);
			cas2->add_monitor(wait_step8);
			ADD_STEP(wait_step8)
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_jellybean_start, area, new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_w_exclamation", new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(20)/* Originally: WHAAAT???!!! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(9, 9), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(21)/* Originally: JELLYBEAN!!! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(22)/* Originally: That did not just happen!^Jellybean, I'll be right back. I'm going to find Tom! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(23)/* Originally: Oh no. Where could Tom be? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)

			FINISH_SYSTEM(AREA)
		}
		else if (INSTANCE->is_milestone_complete(MS_TOM_JOINED)) {
			// start the descent scene

			wedge::pause_presses(true);

			gfx::Tilemap *tilemap = area->get_tilemap();
			tilemap->set_tile(1, util::Point<int>(5, 11), util::Point<int>(-1, -1), false);
			tilemap->set_tile(1, util::Point<int>(6, 11), util::Point<int>(-1, -1), false);
			tilemap->set_tile(2, util::Point<int>(7, 11), util::Point<int>(0, 0), false);
			tilemap->set_tile(3, util::Point<int>(7, 11), util::Point<int>(1, 2), false);
			tilemap->set_tile(3, util::Point<int>(7, 12), util::Point<int>(0, 0), false);

			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *tom = AREA->get_player(TOM);

			pleasant->set_position(util::Point<int>(7, 9));
			pleasant->set_visible(false);

			tom->set_position(util::Point<int>(10, 9));
			tom->set_visible(false);

			wedge::Map_Entity *descent = new wedge::Map_Entity("descent");
			descent->start(area);
			descent->set_position(util::Point<int>(7, 11));
			gfx::Sprite *descent_sprite = new gfx::Sprite("descent");
			descent_sprite->set_animation("start");
			descent->set_sprite(descent_sprite);
			descent->set_shadow(true, {12, 6});
			descent->set_shadow_offset({3.0f*shim::tile_size, -shim::tile_size*2.0f});
			area->add_entity(descent);

			// dummy entity for bucket shadow
			wedge::Map_Entity *bucket_shadow = new wedge::Map_Entity("bucket_shadow");
			bucket_shadow->start(area);
			bucket_shadow->set_position({8, 9});
			bucket_shadow->set_sprite(new gfx::Sprite("blank16x16"));
			bucket_shadow->set_shadow(true, {12, 6});
			bucket_shadow->set_shadow_offset({-shim::tile_size/4.0f, -1.0f});
			area->add_entity(bucket_shadow);

			wedge::Map_Entity *pleasant_bucket = new wedge::Map_Entity("pleasant_bucket");
			pleasant_bucket->start(area);
			pleasant_bucket->set_position(util::Point<int>(7, 10));
			gfx::Sprite *pleasant_bucket_sprite = new gfx::Sprite("pleasant_bucket");
			pleasant_bucket->set_sprite(pleasant_bucket_sprite);
			pleasant_bucket->set_visible(false);
			pleasant_bucket->set_shadow(true, {12, 6});
			area->add_entity(pleasant_bucket);

			NEW_SYSTEM_AND_TASK(AREA)

			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(44)/* Originally: So like I said, lower me down slowly. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(45)/* Originally: Yup, let's get this over with. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(46)/* OK, when I count to three.^One.^Two. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(no_tom, descent, new_task))
			wedge::Offset_Arc_Step *off_arc1 = new wedge::Offset_Arc_Step(tom, util::Point<float>(0.0f, 0.0f), util::Point<float>(-24.0f/16.0f, -6.0f/16.0f), 1.0f, 333, new_task);
			ADD_STEP(off_arc1)
			ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(kick_sfx_callback, area, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(tom, util::Point<float>(-24.0f/16.0f, -6.0f/16.0f), util::Point<float>(0.0f, 0.0f), 1.0f, 333, new_task))
			wedge::Generic_Immediate_Callback_Step *pleasant_fall = new wedge::Generic_Immediate_Callback_Step(just_tom_start, descent, new_task);
			ADD_STEP(pleasant_fall)
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(47)/* Originally: Uh oh... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(just_tom, descent, new_task))
			ADD_STEP(new wedge::Delay_Step(1800, new_task))
			wedge::Generic_Immediate_Callback_Step *tgiw = new wedge::Generic_Immediate_Callback_Step(tom_going_in_well, descent, new_task);
			ADD_STEP(tgiw)
			ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(tom, util::Point<float>(0.0f, 0.0f), util::Point<float>(-3.0f, 0.0f), 3.0f, 750, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(tom, util::Point<float>(-3.0f, 0.0f), util::Point<float>(-3.0f, 2.0f), 0.0f, 250, new_task))
			ADD_STEP(new wedge::Delay_Step(2500, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(change_to_cave1, area, new_task))

			ADD_TASK(new_task)

			ANOTHER_TASK

			wedge::Wait_Step *wait_step0 = new wedge::Wait_Step(new_task);
			pleasant_fall->add_monitor(wait_step0);
			ADD_STEP(wait_step0)
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_bucket_shadow, area, new_task))
			ADD_STEP(new wedge::Offset_Arc_Step(pleasant_bucket, util::Point<float>(0.0f, 0.0f), util::Point<float>(0.0f, 2.0f), 0.0f, 250, new_task))

			ADD_TASK(new_task)

			ANOTHER_TASK

			wedge::Wait_Step *wait_step2 = new wedge::Wait_Step(new_task);
			tgiw->add_monitor(wait_step2);
			ADD_STEP(wait_step2)
			ADD_STEP(new wedge::Change_Angle_Step(tom, 0.0f, float(-M_PI)*8.0f, 1000, new_task))

			ADD_TASK(new_task)

			FINISH_SYSTEM(AREA)
		}

		Area_Hooks::started();
	}

	void end()
	{
		DOGO_GLOBALS->rope->stop();
	}

	bool activate(wedge::Map_Entity *activator, wedge::Map_Entity *activated)
	{
		if (activated->get_name() == "well") {
			GLOBALS->do_dialogue(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(24)/* Originally: Jellybean! I'll be right back sweetie! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
			return true;
		}
		return false;
	}
	
	void run()
	{
		auto jb = area->find_entity("jellybean");
		if (jb && jb->get_offset().x <= -2.0f) {
			jb->set_shadow_offset({0.0f, -8.0f});
		}
		auto tom = area->find_entity("Tom");
		if (tom && tom->get_offset().x <= -2.0f) {
			tom->set_shadow_offset({0.0f, -8.0f});
		}
	}
};

static void tom_join(void *data)
{
	wedge::Area *area = AREA->get_current_area();

	wedge::Map_Entity *tom = area->find_entity("Tom");

	INSTANCE->stats.push_back(wedge::Player_Stats());
	INSTANCE->stats[1].sprite = new gfx::Sprite("tom");
	INSTANCE->stats[1].level = 1;
	INSTANCE->stats[1].experience = 0;
	INSTANCE->stats[1].base.fixed.max_hp = 100;
	INSTANCE->stats[1].base.fixed.max_mp = 25;
	INSTANCE->stats[1].base.fixed.attack = 25;
	INSTANCE->stats[1].base.fixed.defence = 25;
	INSTANCE->stats[1].base.fixed.set_extra(LUCK, 25);
	INSTANCE->stats[1].base.hp = INSTANCE->stats[1].base.fixed.max_hp;
	INSTANCE->stats[1].base.mp = INSTANCE->stats[1].base.fixed.max_mp;
	INSTANCE->stats[1].base.set_name("Tom");

	AREA->set_player(TOM, tom);

	AREA->get_current_area()->order_player_input_steps();

	tom->set_extra_offset(util::Point<int>(0, 0));
	tom->face(AREA->get_player(PLEASANT), false);
	tom->set_shadow_offset({0.0f, 0.0f});

	area->find_entity("fishing_rod")->set_visible(true);
}

static void fish_jump_callback(void *data)
{
	wedge::Map_Entity *fish_jump = static_cast<wedge::Map_Entity *>(data);
	// can't delete the entity here as this is the entity's sprite callback, so schedule it for later
	// but set it invisible first
	fish_jump->set_visible(false);
	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Delete_Map_Entity_Step(fish_jump, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Village : public Area_Hooks
{
public:
	Area_Hooks_Village(wedge::Area *area) :
		Area_Hooks(area)
	{
		if (INSTANCE->is_milestone_complete(MS_TOM_JOINED)) {
			Fade_Zone z;
			z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 9));
			z.area_name = "start";
			z.player_positions.push_back(util::Point<int>(0, 0));
			z.player_positions.push_back(util::Point<int>(0, 0));
			z.player_positions.push_back(util::Point<int>(0, 0));
			z.directions.push_back(wedge::DIR_N);
			z.directions.push_back(wedge::DIR_N);
			z.directions.push_back(wedge::DIR_N);
			fade_zones.push_back(z);
		}
		else {
			Scroll_Zone z;
			z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 9));
			z.area_name = "start";
			z.topleft_dest = util::Point<int>(15, 6);
			z.direction = wedge::DIR_W;
			scroll_zones.push_back(z);
		}
		
		Fade_Zone fz1;
		fz1.zone = util::Rectangle<int>(util::Point<int>(30, 7), util::Size<int>(1, 1));
		fz1.area_name = "toms";
		fz1.player_positions.push_back(util::Point<int>(6, 9));
		fz1.player_positions.push_back(util::Point<int>(6, 9));
		fz1.player_positions.push_back(util::Point<int>(6, 9));
		fz1.directions.push_back(wedge::DIR_N);
		fz1.directions.push_back(wedge::DIR_N);
		fz1.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(fz1);
		
		Fade_Zone fz2;
		fz2.zone = util::Rectangle<int>(util::Point<int>(19, 8), util::Size<int>(1, 1));
		fz2.area_name = "pleasants";
		fz2.player_positions.push_back(util::Point<int>(6, 9));
		fz2.player_positions.push_back(util::Point<int>(6, 9));
		fz2.player_positions.push_back(util::Point<int>(6, 9));
		fz2.directions.push_back(wedge::DIR_N);
		fz2.directions.push_back(wedge::DIR_N);
		fz2.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(fz2);
		
		Fade_Zone fz3;
		fz3.zone = util::Rectangle<int>(util::Point<int>(16, 5), util::Size<int>(1, 1));
		fz3.area_name = "pleasants";
		fz3.player_positions.push_back(util::Point<int>(3, 1));
		fz3.player_positions.push_back(util::Point<int>(3, 1));
		fz3.player_positions.push_back(util::Point<int>(3, 1));
		fz3.directions.push_back(wedge::DIR_S);
		fz3.directions.push_back(wedge::DIR_S);
		fz3.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(fz3);
		
		Fade_Zone fz4;
		fz4.zone = util::Rectangle<int>(util::Point<int>(8, 7), util::Size<int>(1, 1));
		fz4.area_name = "vances";
		fz4.player_positions.push_back(util::Point<int>(8, 9));
		fz4.player_positions.push_back(util::Point<int>(8, 9));
		fz4.player_positions.push_back(util::Point<int>(8, 9));
		fz4.directions.push_back(wedge::DIR_N);
		fz4.directions.push_back(wedge::DIR_N);
		fz4.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(fz4);
	}
	
	virtual ~Area_Hooks_Village()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/town.mml");

		if (loaded == false) {
			wedge::Chest *chest1 = new wedge::Chest("chest", "apple", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1));
			chest1->start(area);
			chest1->set_position(util::Point<int>(11, 9));
			chest1->set_solid(false);
			chest1->set_layer(0);
			chest1->set_remove_when_activated(true);
			chest1->set_achievement((void *)ACH_APPLE);
			area->add_entity(chest1);
			
			wedge::Chest *chest2 = new wedge::Chest("chest", "apple", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1));
			chest2->start(area);
			chest2->set_position(util::Point<int>(12, 10));
			chest2->set_solid(false);
			chest2->set_layer(0);
			chest2->set_remove_when_activated(true);
			chest2->set_achievement((void *)ACH_APPLE);
			area->add_entity(chest2);
			
			wedge::Chest *chest3 = new wedge::Chest("chest", "apple", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1));
			chest3->start(area);
			chest3->set_position(util::Point<int>(13, 10));
			chest3->set_solid(false);
			chest3->set_layer(0);
			chest3->set_remove_when_activated(true);
			chest3->set_achievement((void *)ACH_APPLE);
			area->add_entity(chest3);
			
			wedge::Chest *chest4 = new wedge::Chest("chest", "carrot", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARROT, 1));
			chest4->start(area);
			chest4->set_position(util::Point<int>(6, 18));
			chest4->set_solid(false);
			chest4->set_layer(0);
			chest4->set_remove_when_activated(true);
			area->add_entity(chest4);
			
			wedge::Chest *chest5 = new wedge::Chest("chest", "carrot", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARROT, 1));
			chest5->start(area);
			chest5->set_position(util::Point<int>(7, 17));
			chest5->set_solid(false);
			chest5->set_layer(0);
			chest5->set_remove_when_activated(true);
			area->add_entity(chest5);
			
			wedge::Chest *chest6 = new wedge::Chest("chest", "apple", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1));
			chest6->start(area);
			chest6->set_position(util::Point<int>(28, 12));
			chest6->set_solid(false);
			chest6->set_layer(0);
			chest6->set_remove_when_activated(true);
			chest6->set_achievement((void *)ACH_APPLE);
			area->add_entity(chest6);
			
			wedge::Map_Entity *scarecrow = new wedge::Map_Entity("scarecrow");
			scarecrow->start(area);
			scarecrow->set_position(util::Point<int>(8, 18));
			/*
			// Use a blank sprite since the scarecrow is in tiles, needs a sprite to show shadow
			scarecrow->set_sprite(new gfx::Sprite("blank16x16"));
			scarecrow->set_shadow(true, util::Size<int>(12, 6));
			*/
			area->add_entity(scarecrow);
			
			wedge::NPC *frances = new wedge::NPC("Frances", "Frances", "frances", "frances");
			frances->start(area);
			frances->set_position(util::Point<int>(18, 23));
			frances->set_shadow(true, util::Size<int>(12, 6));
			frances->set_shadow_offset({-4.0f, 0.0f});
			area->add_entity(frances);
		
			util::Point<int> cat_pos(20, 12);
			cat = new wedge::Map_Entity("cat");
			cat->set_wanders(true, true, true, true, cat_pos, 10);
			cat->start(area);
			cat->set_position(cat_pos);
			gfx::Sprite *cat_sprite = new gfx::Sprite("cat");
			cat->set_sprite(cat_sprite);
			cat->set_shadow(true, util::Size<int>(10, 5));
			cat->set_auto_shadow_pos(true);
			area->add_entity(cat);
			
			tom = new wedge::Map_Entity("Tom");
			tom->start(area);
			tom->set_position(util::Point<int>(31, 15));
			gfx::Sprite *tom_sprite = new gfx::Sprite("tom");
			tom->set_sprite(tom_sprite);
			tom_sprite->set_animation("fishing");
			tom->set_extra_offset(util::Point<int>(-32, 0));
			tom->set_shadow(true, util::Size<int>(12, 6));
			tom->set_shadow_offset(util::Point<float>(32.0f, 0.0f));
			area->add_entity(tom);
			
			wedge::Map_Entity *fishing_rod = new wedge::Map_Entity("fishing_rod");
			fishing_rod->start(area);
			fishing_rod->set_position(util::Point<int>(31, 15));
			gfx::Sprite *fishing_rod_sprite = new gfx::Sprite("fishing_rod");
			fishing_rod->set_sprite(fishing_rod_sprite);
			fishing_rod->set_extra_offset(util::Point<int>(-32, 0));
			fishing_rod->set_solid(false);
			fishing_rod->set_layer(0);
			fishing_rod->set_visible(false);
			area->add_entity(fishing_rod);

			dog_house = new wedge::Map_Entity("dog_house");
			dog_house->set_position(util::Point<int>(15, 3));
			dog_house->set_size(util::Size<int>(2, 1));
			area->add_entity(dog_house);
		}
		else {
			cat = area->find_entity("cat");
			tom = area->find_entity("Tom");
			dog_house = area->find_entity("dog_house");
		}

		return true;
	}
	
	bool activate(wedge::Map_Entity *activator, wedge::Map_Entity *activated)
	{
		if (activated->get_name() == "scarecrow") {
			GLOBALS->do_dialogue(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(26)/* Originally: Is that my old hat? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
			return true;
		}
		else if (activated == cat) {
			GLOBALS->do_dialogue(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(31)/* Originally: You're not Jellybean... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
		}
		else if (activated == tom && INSTANCE->is_milestone_complete(MS_TOM_JOINED) == false) {
			Fade_Zone z;
			z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 9));
			z.area_name = "start";
			z.player_positions.push_back(util::Point<int>(0, 0));
			z.player_positions.push_back(util::Point<int>(0, 0));
			z.player_positions.push_back(util::Point<int>(0, 0));
			z.directions.push_back(wedge::DIR_N);
			z.directions.push_back(wedge::DIR_N);
			z.directions.push_back(wedge::DIR_N);
			fade_zones.push_back(z);
			scroll_zones.clear();

			INSTANCE->set_milestone_complete(MS_TOM_JOINED, true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Achieve_Step((void *)ACH_FISH, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(32)/* Originally: There you are! I need your help! Follow me! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(33)/* Originally: Woah woah woah. I'm busy! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(34)/* Originally: JB's in trouble! Besides, you can't catch anything in this puddle! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(35)/* Originally: OK, OK! But for the record, Vance caught a 3 foot speckler here! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(36)/* Originally: Follow me. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(tom_join, area, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}
		else if (activated == dog_house) {
			GLOBALS->do_dialogue("", GLOBALS->game_t->translate(48)/* Originally: This is a dog house.^It might look like you can go inside...^But alas, you cannot. */, wedge::DIALOGUE_MESSAGE, wedge::DIALOGUE_AUTO, NULL);
		}

		return false;
	}
	
	bool try_tile(wedge::Map_Entity *entity, util::Point<int> tile_pos)
	{
		if (Area_Hooks::try_tile(entity, tile_pos)) {
			return true;
		}

		if (entity == AREA->get_player(0)) {
			if (tile_pos.x >= 19 && tile_pos.x <= 20 && tile_pos.y >= 23) {
				entity->get_input_step()->end_movement();
				NEW_SYSTEM_AND_TASK(AREA)
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(27)/* Originally: Frances */ + TAG_END, GLOBALS->game_t->translate(28)/* Originally: Don't you have anything to do, young lady? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(29)/* Originally: Jellybean! How could I forget? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_TASK(new_task)
				FINISH_SYSTEM(AREA)
				return true;
			}
		}

		return false;
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(8, 7);
		water_anim.size = util::Size<int>(4, 3);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(12, 7));
		tilemap->add_animation_data(water_anim);
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			if (INSTANCE->is_milestone_complete(MS_TOM_JOINED) && area->get_next_area_name() == "start") {
				AREA->set_after_fade_delay(3000);
			}
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && INSTANCE->is_milestone_complete(MS_TOM_JOINED) && !INSTANCE->is_milestone_complete(MS_FISH_JUMPED)) {
			INSTANCE->set_milestone_complete(MS_FISH_JUMPED, true);

			wedge::Map_Entity *fish_jump = new wedge::Map_Entity("fish_jump");
			fish_jump->start(area);
			fish_jump->set_position(util::Point<int>(29, 15));
			fish_jump->set_extra_offset(util::Point<float>(-5.0f, 7.0f));
			gfx::Sprite *sprite = new gfx::Sprite("fish_jump");
			fish_jump->set_sprite(sprite);
			area->add_entity(fish_jump);

			sprite->set_animation("only", fish_jump_callback, fish_jump);
				
			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Play_Sound_Step("sfx/splash.mml", new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}

		return false;
	}

private:
	wedge::Map_Entity *cat;
	wedge::Map_Entity *tom;
	wedge::Map_Entity *dog_house;
};

class Area_Hooks_Toms : public Area_Hooks
{
public:
	Area_Hooks_Toms(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone fz1;
		fz1.zone = util::Rectangle<int>(util::Point<int>(6, 9), util::Size<int>(1, 1));
		fz1.area_name = "village";
		fz1.player_positions.push_back(util::Point<int>(30, 7));
		fz1.player_positions.push_back(util::Point<int>(30, 7));
		fz1.player_positions.push_back(util::Point<int>(30, 7));
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(fz1);
	}
	
	virtual ~Area_Hooks_Toms()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/town.mml");

		if (loaded == false) {
			util::Point<int> toms_mom_pos(8, 3);
			wedge::NPC *toms_mom = new wedge::NPC("Tom's Mom", "Tom's Mom", "toms_mom", "toms_mom");
			toms_mom->set_wanders(true, true, true, true, toms_mom_pos, 2);
			toms_mom->start(area);
			toms_mom->set_position(toms_mom_pos);
			toms_mom->set_speed(0.025f);
			toms_mom->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(toms_mom);
			
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1));
			chest->start(area);
			chest->set_position(util::Point<int>(5, 2));
			area->add_entity(chest);
		}

		return true;
	}
};

class Area_Hooks_Pleasants : public Area_Hooks
{
public:
	Area_Hooks_Pleasants(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone fz1;
		fz1.zone = util::Rectangle<int>(util::Point<int>(6, 9), util::Size<int>(1, 1));
		fz1.area_name = "village";
		fz1.player_positions.push_back(util::Point<int>(19, 8));
		fz1.player_positions.push_back(util::Point<int>(19, 8));
		fz1.player_positions.push_back(util::Point<int>(19, 8));
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(fz1);
		
		Fade_Zone fz2;
		fz2.zone = util::Rectangle<int>(util::Point<int>(3, 1), util::Size<int>(1, 1));
		fz2.area_name = "village";
		fz2.player_positions.push_back(util::Point<int>(16, 5));
		fz2.player_positions.push_back(util::Point<int>(16, 5));
		fz2.player_positions.push_back(util::Point<int>(16, 5));
		fz2.directions.push_back(wedge::DIR_N);
		fz2.directions.push_back(wedge::DIR_N);
		fz2.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(fz2);
		
		Fade_Zone fz3;
		fz3.zone = util::Rectangle<int>(util::Point<int>(5, 2), util::Size<int>(1, 1));
		fz3.area_name = "pleasants_u";
		fz3.player_positions.push_back(util::Point<int>(2, 2));
		fz3.player_positions.push_back(util::Point<int>(2, 2));
		fz3.player_positions.push_back(util::Point<int>(2, 2));
		fz3.directions.push_back(wedge::DIR_S);
		fz3.directions.push_back(wedge::DIR_S);
		fz3.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(fz3);
	}
	
	virtual ~Area_Hooks_Pleasants()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/town.mml");

		if (loaded == false) {
			wedge::Chest *chest1 = new wedge::Chest("chest", "apple", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1));
			chest1->start(area);
			chest1->set_position(util::Point<int>(4, 5));
			chest1->set_solid(false);
			chest1->set_layer(3);
			area->add_entity(chest1);
		}

		return true;
	}
};

class Area_Hooks_Pleasants_U : public Area_Hooks
{
public:
	Area_Hooks_Pleasants_U(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone fz1;
		fz1.zone = util::Rectangle<int>(util::Point<int>(2, 2), util::Size<int>(1, 1));
		fz1.area_name = "pleasants";
		fz1.player_positions.push_back(util::Point<int>(5, 2));
		fz1.player_positions.push_back(util::Point<int>(5, 2));
		fz1.player_positions.push_back(util::Point<int>(5, 2));
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(fz1);
	}
	
	virtual ~Area_Hooks_Pleasants_U()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/town.mml");

		if (loaded == false) {
		}

		return true;
	}
};

class Area_Hooks_Vances : public Area_Hooks
{
public:
	Area_Hooks_Vances(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone fz1;
		fz1.zone = util::Rectangle<int>(util::Point<int>(8, 9), util::Size<int>(1, 1));
		fz1.area_name = "village";
		fz1.player_positions.push_back(util::Point<int>(8, 7));
		fz1.player_positions.push_back(util::Point<int>(8, 7));
		fz1.player_positions.push_back(util::Point<int>(8, 7));
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fz1.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(fz1);
	}
	
	virtual ~Area_Hooks_Vances()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/town.mml");

		if (loaded == false) {
			util::Point<int> vance_pos(5, 4);
			wedge::NPC *vance = new wedge::NPC("Vance", "Vance", "vance", "vance");
			vance->set_wanders(true, true, true, true, vance_pos, 3);
			vance->start(area);
			vance->set_position(vance_pos);
			vance->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(vance);
			
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1));
			chest->start(area);
			chest->set_position(util::Point<int>(5, 2));
			area->add_entity(chest);
		}

		return true;
	}
};

//--

static void done_drag(void *data)
{
	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);

	util::Point<int> pleasant_start_pos(8, 10);

	pleasant->set_direction(wedge::DIR_W, true, false);
	pleasant->set_extra_offset(util::Point<int>(0, 0));

	tom->set_visible(true);
	tom->set_position(pleasant_start_pos + util::Point<int>(-1, 0));
	tom->get_sprite()->set_animation("lay");
}

static void stop_using_camera(void *data)
{
	Area_Game *area_game = static_cast<Area_Game *>(AREA);
	area_game->set_use_camera(false);
}

static void start_spit_cb(void *data);
static void start_spray_cb(void *data);

class Area_Hooks_Cave1_Start : public Area_Hooks
{
public:
	Area_Hooks_Cave1_Start(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(22, 12), util::Size<int>(1, 1));
		z1.area_name = "cave1_battletut";
		z1.topleft_dest = util::Point<int>(0, 8);
		z1.direction = wedge::DIR_E;
		scroll_zones.push_back(z1);
	}
	
	virtual ~Area_Hooks_Cave1_Start()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
			wedge::Map_Entity *jellybean = new wedge::Map_Entity("jellybean");
			jellybean->start(area);
			jellybean->set_position(util::Point<int>(16, 10));
			jellybean->set_sprite(new gfx::Sprite("jellybean"));
			jellybean->set_direction(wedge::DIR_W, true, false);
			jellybean->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(jellybean);
		}

		return true;
	}

	void started()
	{
		if (INSTANCE->is_milestone_complete(MS_UNDERGROUND_SCENE1) == false) {
			wedge::pause_presses(true);

			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *tom = AREA->get_player(TOM);

			util::Point<int> pleasant_start_pos(8, 10);

			// Blank entity to give a shadow while Tom is hidden
			wedge::Map_Entity *blank = new wedge::Map_Entity("blank");
			blank->start(area);
			blank->set_sprite(new gfx::Sprite("blank16x16"));
			blank->set_shadow(true, {12, 6});
			blank->set_shadow_offset({-5.0f, 0.0f});
			blank->set_position(pleasant_start_pos);
			blank->set_solid(false);
			area->add_entity(blank);

			pleasant->set_visible(true);
			pleasant->get_sprite()->set_animation("drag_tom");
			pleasant->set_position(pleasant_start_pos);
			pleasant->set_offset(util::Point<float>(-1.0f, 0.0f));
			pleasant->set_extra_offset(util::Point<int>(-shim::tile_size, 0));

			tom->set_visible(false);
			tom->set_sub_offset_from_shadow_pos(false); // was set in well fall scene...
			tom->set_shadow_offset({0.0f, 0.0f}); // was to to 0,-8 before

			std::vector<wedge::Map_Entity *> entities;
			entities.push_back(pleasant);
			std::vector< util::Point<int> > positions;
			positions.push_back(pleasant_start_pos);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Offset_Arc_Step(pleasant, util::Point<float>(-1.0f, 0.0f), util::Point<float>(0.0f, 0.0f), 0.0f, 2500, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(done_drag, NULL, new_task))
			ADD_STEP(new wedge::Delay_Step(2500, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "sit_up_spit", new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(start_spit_cb, this, new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "sit_up", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(75)/* Originally: Oh my goodness, Tom, are you OK? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(76)/* Originally: *cough* *cough* PLEASANT! I'LL SAVE YOU! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(77)/* Originally: I just pulled you out of the water you knucklehead! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(78)/* Originally: Hey, look at this place. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(79)/* Originally: I didn't know this was down here. Where's Jellybean? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(80)/* Originally: He couldn't have gone far. Let's look around. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Milestone_Complete_Step(MS_UNDERGROUND_SCENE1, true, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(done_cave1_scene1, NULL, new_task))
			ADD_TASK(new_task)
			ANOTHER_TASK
			ADD_STEP(new wedge::Play_Sound_Step("sfx/drag.mml", new_task))
			ADD_TASK(new_task)
			ANOTHER_TASK
			ADD_STEP(new wedge::Offset_Arc_Step(blank, util::Point<float>(-1.0f, 0.0f), util::Point<float>(0.0f, 0.0f), 0.0f, 2500, new_task))
			ADD_STEP(new wedge::Delete_Map_Entity_Step(blank, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}

		Area_Hooks::started();
	}
	
	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(22, 0);
		water_anim.size = util::Size<int>(5, 3);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(22, 3));
		tilemap->add_animation_data(water_anim);
	}
	
	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && INSTANCE->is_milestone_complete(MS_JB_SHAKE) == false && entity->get_position() == util::Point<int>(11, 10)) {
			wedge::pause_presses(true);

			INSTANCE->set_milestone_complete(MS_JB_SHAKE, true);
	
			wedge::Map_Entity *player = AREA->get_player(0);
			util::Point<int> pos = player->get_position();
			util::Point<float> offset = player->get_offset();
			util::Size<int> player_size = player->get_size();
			util::Point<float> sz(player_size.w / 2.0f, 1.0f - player_size.h / 2.0f);
			wedge::add_tiles(pos, offset, sz);
			util::Point<float> map_offset = area->get_centred_offset(pos, offset, true);

			util::Point<float> start_offset = map_offset;
			util::Point<float> end_offset = map_offset + util::Point<float>(-shim::tile_size*2.0f, 0.0f);

			draw_offset = end_offset;

			static_cast<Area_Game *>(AREA)->set_use_camera(true);

			wedge::Map_Entity *jellybean = area->find_entity("jellybean");

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new Pan_Camera_Step(start_offset, end_offset, 2000, new_task));
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(player, "stand_e_exclamation", new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shake, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(jellybean, "shake", new_task))
			ADD_STEP(new wedge::Set_Direction_Step(player, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(start_spray_cb, this, new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(jellybean, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(81)/* Originally: JB! There you are! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(jellybean, util::Point<int>(22, 12), new_task))
			ADD_STEP(new wedge::Delete_Map_Entity_Step(jellybean, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(82)/* Originally: Wait! Come back Jellybean! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Pan_Camera_Step(end_offset, start_offset, 1000, new_task));
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(stop_using_camera, NULL, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (spray.size() > 0) {
				Uint32 now = GET_TICKS();

				bool found = false;
				gfx::draw_primitives_start();
				for (size_t i = 0; i < spray.size(); i++) {
					Spray &s = spray[i];
					if (s.delay + s.life + spray_start_time > now && s.delay + spray_start_time <= now) {
						Uint32 elapsed = now - (spray_start_time + s.delay);
						float p = elapsed / (float)s.life;
						float inv = 1.0f - p;
						s.start += s.vel;
						SDL_Colour c = make_translucent(shim::palette[28], inv);
						gfx::draw_filled_rectangle(c, s.start, util::Size<int>(1, 1));
						found = true;
					}
					else if (s.delay + spray_start_time > now) {
						found = true;
					}
				}
				gfx::draw_primitives_end();

				if (found == false) {
					spray.clear();
				}
			}
			if (spit.size() > 0) {
				float radius = 8.0f;

				wedge::Map_Entity *tom = AREA->get_player(TOM);
				util::Point<float> draw_pos = map_offset + (tom->get_offset() + tom->get_position()) * shim::tile_size + util::Point<float>(5.0f, 9.5f) + util::Point<float>(0, radius); // mouth pos, the extra 0, 8 is for radius of circle

				Uint32 now = GET_TICKS();

				bool found = false;
				gfx::draw_primitives_start();
				for (size_t i = 0; i < spit.size(); i++) {
					Spit &s = spit[i];
					if (s.start + 250 + spit_start_time > now && s.start + spit_start_time <= now) {
						float p = (now - (s.start + spit_start_time)) / 250.0f;

						float a = (M_PI * 1.5f) + (-p * M_PI / 2);

						float r = util::rand(0, 1000) / 1000.0f * 2.0f * (i / (float)spit.size());

						float x = cosf(a) * (radius + r);
						float y = sinf(a) * (radius + r);

						float alpha = 1.0f - p;

						SDL_Colour c = make_translucent(shim::palette[28], alpha);

						gfx::draw_filled_rectangle(c, draw_pos + util::Point<float>(x, y), util::Size<int>(1, 1));

						found = true;
					}
					else if (s.start + spit_start_time > now) {
						found = true;
					}
				}
				gfx::draw_primitives_end();

				if (found == false) {
					spit.clear();
				}
			}
		}
	}
	
	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void start_spit()
	{
		for (int i = 0; i < 10; i++) {
			Spit s;

			s.start = util::rand(0, 250);

			spit.push_back(s);
		}

		spit_start_time = GET_TICKS();
	}

	void start_spray()
	{
		wedge::Map_Entity *jb = area->find_entity("jellybean");
		util::Point<float> p = (jb->get_offset() + jb->get_position()) * shim::tile_size + util::Point<int>(shim::tile_size/2, shim::tile_size/2);
		util::Point<float> centre = draw_offset + p;

		float min_v = 0.1f;
		float max_v = 0.25f;

		for (int i = 0; i < 200; i++) {
			Spray s;

			s.start = centre;
			s.start.x += int(util::rand(0, shim::tile_size*2/3)) - shim::tile_size/3;
			s.start.y += int(util::rand(0, shim::tile_size*2/3)) - shim::tile_size/3;

			float angle = util::rand(0, 1000) / 1000.0f * M_PI * 2.0f;

			float v = ((util::rand(0, 1000) / 1000.0f) * (max_v-min_v)) + min_v;

			s.vel.x = cos(angle) * v;
			s.vel.y = sin(angle) * v;

			s.delay = util::rand(0, 500);

			s.life = util::rand(200, 400);

			spray.push_back(s);
		}

		spray_start_time = GET_TICKS();
	}

private:
	util::Point<float> draw_offset;
	Uint32 spit_start_time;
	Uint32 spray_start_time;

	struct Spit {
		Uint32 start;
	};

	struct Spray {
		util::Point<float> start;
		util::Point<float> vel;
		Uint32 delay;
		Uint32 life;
	};

	std::vector<Spit> spit;
	std::vector<Spray> spray;
};

static void start_spit_cb(void *data)
{
	Area_Hooks_Cave1_Start *a = static_cast<Area_Hooks_Cave1_Start *>(data);
	a->start_spit();
	DOGO_GLOBALS->spit_water->play(false);
}

static void start_spray_cb(void *data)
{
	Area_Hooks_Cave1_Start *a = static_cast<Area_Hooks_Cave1_Start *>(data);
	a->start_spray();
}

//--

static void set_following(void *data)
{
	INSTANCE->party_following_player = data != NULL;
}

const int SENSEI_FALL_TIME = 1100;
const int SENSEI_DELAY = 1000;

static void add_sensei_battletut(void *data)
{
	wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");
	
	sensei->set_shadow(true, util::Size<int>(12, 6));
	sensei->set_sub_offset_from_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(sensei, util::Point<float>(0.0f, -6.0f), util::Point<float>(0.0f, 0.0f), 3.0f, SENSEI_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(SENSEI_DELAY, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_s", new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Cave1_BattleTut : public Area_Hooks
{
public:
	Area_Hooks_Cave1_BattleTut(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(0, 8), util::Size<int>(1, 1));
		z1.area_name = "cave1_start";
		z1.topleft_dest = util::Point<int>(22, 12);
		z1.direction = wedge::DIR_W;
		scroll_zones.push_back(z1);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(9, 17), util::Size<int>(1, 1));
		z2.area_name = "cave1_3";
		z2.topleft_dest = util::Point<int>(9, 0);
		z2.direction = wedge::DIR_S;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave1_BattleTut()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
		}

		return true;
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && entity->get_position() == util::Point<int>(6, 8)) {
			if (INSTANCE->is_milestone_complete(MS_BATTLETUT) == false) {
				INSTANCE->set_milestone_complete(MS_BATTLETUT, true);

				wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
				wedge::Map_Entity *tom = AREA->get_player(TOM);
				
				wedge::Map_Entity *sensei = new wedge::Map_Entity("sensei");
				sensei->start(area);
				sensei->set_position(util::Point<int>(8, 7));
				sensei->set_offset({0.0f, -6.0f});
				sensei->set_sprite(new gfx::Sprite("sensei"));
				sensei->set_solid(false);
				area->add_entity(sensei);

				wedge::pause_presses(true);

				set_following((void *)0);

				NEW_SYSTEM_AND_TASK(AREA)

				ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(7, 8), new_task))
				wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
				ADD_STEP(wait_step)
				ADD_STEP(new wedge::Delay_Step(250, new_task))
				ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_N, true, false, new_task))
				ADD_STEP(new wedge::Delay_Step(SENSEI_FALL_TIME+SENSEI_DELAY-500, new_task))
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
				ADD_STEP(new wedge::Play_Music_Step("music/sensei.mml", new_task))
				ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_n_exclamation", new_task))
				ADD_STEP(new wedge::Delay_Step(1000, new_task))
				ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_n", new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(194)/* Originally: Why hello there! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(195)/* Originally: Yikes! You scared us mister */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->laugh, false, false, new_task))
				ADD_STEP(new wedge::Play_Animation_Step(sensei->get_sprite(), "laugh", new_task))
				ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_s", new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(196)/* Originally: You ain't seen nothing yet!^Do you know what lurks in the shadows down here? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(197)/* Originally: Weird fungii? Crazy old men jumping out of the ceiling? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->laugh, false, false, new_task))
				ADD_STEP(new wedge::Play_Animation_Step(sensei->get_sprite(), "laugh", new_task))
				ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_s", new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(198)/* Originally: Worse! This place isn't like quiet Cozy River.^There are things down here that will chew you up and spit you out! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(199)/* Originally: We're not leaving until we find Jellybean! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(200)/* Originally: Then you'll need some battling skills! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(201)/* Originally: Ha! And you're going to teach us?! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(202)/* Originally: Look alive! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
				ADD_STEP(new wedge::Set_Visible_Step(sensei, false, new_task))
				ADD_STEP(new Start_Battle_Step(new Battle_Sensei1(), new_task))
				ADD_TASK(new_task)

				ANOTHER_TASK
				
				ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(9, 8), new_task))
				ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
				auto gics = new wedge::Generic_Immediate_Callback_Step(add_sensei_battletut, nullptr, new_task);
				ADD_STEP(gics)
				gics->add_monitor(wait_step);
				ADD_STEP(new wedge::Delay_Step(250, new_task))
				ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_N, true, false, new_task))
				ADD_STEP(new wedge::Delay_Step(SENSEI_FALL_TIME+SENSEI_DELAY-500, new_task))
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
				ADD_STEP(new wedge::Set_Animation_Step(tom, "stand_n_exclamation", new_task))
				ADD_STEP(new wedge::Delay_Step(1000, new_task))
				ADD_STEP(new wedge::Set_Animation_Step(tom, "stand_n", new_task))
				ADD_TASK(new_task)

				FINISH_SYSTEM(AREA)

				return true;
			}
		}

		return false;
	}
private:
};

//--

class Area_Hooks_Cave1_3 : public Area_Hooks
{
public:
	Area_Hooks_Cave1_3(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(9, 0), util::Size<int>(1, 1));
		z1.area_name = "cave1_battletut";
		z1.topleft_dest = util::Point<int>(9, 17);
		z1.direction = wedge::DIR_N;
		scroll_zones.push_back(z1);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(19, 8), util::Size<int>(1, 1));
		z2.area_name = "cave1_4";
		z2.topleft_dest = util::Point<int>(0, 15);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave1_3()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
			wedge::Chest *chest1 = new wedge::Chest("chest1", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1));
			chest1->start(area);
			chest1->set_position(util::Point<int>(5, 7));
			chest1->set_sprite(new gfx::Sprite("chest"));
			chest1->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(chest1);

			wedge::Chest *chest2 = new wedge::Chest("chest2", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1));
			chest2->start(area);
			chest2->set_position(util::Point<int>(7, 7));
			chest2->set_sprite(new gfx::Sprite("chest"));
			chest2->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(chest2);

			wedge::Chest *chest3 = new wedge::Chest("chest3", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1));
			chest3->start(area);
			chest3->set_position(util::Point<int>(14, 7));
			chest3->set_sprite(new gfx::Sprite("chest"));
			chest3->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(chest3);
			
			wedge::Map_Entity *jb = new wedge::Map_Entity("jb");
			jb->start(area);
			jb->set_position(util::Point<int>(15, 8));
			gfx::Sprite *jb_sprite = new gfx::Sprite("jellybean");
			jb_sprite->set_animation("stand_w");
			jb->set_sprite(jb_sprite);
			jb->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(jb);
		}

		return true;
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}
		
		if (entity == AREA->get_player(PLEASANT) && entity->get_position() == util::Point<int>(9, 5)) {
			if (INSTANCE->is_milestone_complete(MS_CAVE1_3_JB_LEAVE) == false) {
				INSTANCE->set_milestone_complete(MS_CAVE1_3_JB_LEAVE, true);
		
				wedge::Map_Entity *jb = area->find_entity("jb");

				NEW_SYSTEM_AND_TASK(AREA)
				ADD_STEP(new wedge::A_Star_Step(jb, util::Point<int>(19, 8), new_task))
				ADD_STEP(new wedge::Delete_Map_Entity_Step(jb, new_task))
				ADD_TASK(new_task)
				FINISH_SYSTEM(AREA)

				// return false here...
			}

			// return false here...
		}

		return false;
	}

	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_1Coinman1Puddle();
		}
		else if (type == 2) {
			return new Battle_2Coinman();
		}
		else if (type == 1) {
			return new Battle_3Unicorn();
		}
		else {
			return new Battle_2Puddle();
		}
	}
};

//--

class Area_Hooks_Cave1_4 : public Area_Hooks
{
public:
	Area_Hooks_Cave1_4(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(0, 15), util::Size<int>(1, 1));
		z1.area_name = "cave1_3";
		z1.topleft_dest = util::Point<int>(19, 8);
		z1.direction = wedge::DIR_W;
		scroll_zones.push_back(z1);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(19, 22), util::Size<int>(1, 1));
		z2.area_name = "cave1_5";
		z2.topleft_dest = util::Point<int>(0, 9);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave1_4()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_NINJA_STAR, 3));
			chest->start(area);
			chest->set_position(util::Point<int>(14, 7));
			chest->set_sprite(new gfx::Sprite("chest"));
			chest->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(chest);
		}

		return true;
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_1Coinman1Puddle();
		}
		else if (type == 2) {
			return new Battle_2Coinman();
		}
		else if (type == 1) {
			return new Battle_3Unicorn();
		}
		else {
			return new Battle_2Puddle();
		}
	}
	
	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(18, 7);
		water_anim.size = util::Size<int>(8, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(18, 8));
		tilemap->add_animation_data(water_anim);
	}
};

//--

static void bridge_collapse1(void *data)
{
	auto area = static_cast<wedge::Area *>(data);
	gfx::Tilemap *tilemap = area->get_tilemap();
	tilemap->set_tile(1, {14, 8}, {19, 11}, true);
	tilemap->set_tile(1, {15, 8}, {20, 11}, true);
	tilemap->set_tile(1, {16, 8}, {21, 11}, true);
	tilemap->set_tile(1, {14, 9}, {19, 12}, true);
	tilemap->set_tile(1, {15, 9}, {20, 12}, true);
	tilemap->set_tile(1, {16, 9}, {21, 12}, true);
			
	wedge::Map_Entity *board1 = new wedge::Map_Entity("board1");
	board1->start(area);
	board1->set_position(util::Point<int>(14, 8));
	board1->set_sprite(new gfx::Sprite("board1"));
	board1->set_solid(false);
	board1->set_offset({0.5f, 0.25f});
	area->add_entity(board1);
	
	wedge::Map_Entity *board2 = new wedge::Map_Entity("board2");
	board2->start(area);
	board2->set_position(util::Point<int>(15, 8));
	board2->set_sprite(new gfx::Sprite("board2"));
	board2->set_solid(false);
	board2->set_offset({0.0f, 0.5f});
	area->add_entity(board2);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Offset_Arc_Step(board1, util::Point<float>(0.5f, 0.25f), util::Point<float>(0.5f, 20.0f), 0.0f, 2000, new_task))
	ADD_STEP(new wedge::Delete_Map_Entity_Step(board1, new_task))
	ADD_TASK(new_task)
	ANOTHER_TASK
	ADD_STEP(new wedge::Offset_Arc_Step(board2, util::Point<float>(0.0f, 0.5f), util::Point<float>(0, 20.0f), 0.0f, 1500, new_task))
	ADD_STEP(new wedge::Delete_Map_Entity_Step(board2, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void bridge_collapse2(void *data)
{
	auto area = static_cast<wedge::Area *>(data);
	gfx::Tilemap *tilemap = area->get_tilemap();
	tilemap->set_tile(1, {14, 8}, {-1, -1}, true);
	tilemap->set_tile(1, {15, 8}, {-1, -1}, true);
	tilemap->set_tile(1, {16, 8}, {-1, -1}, true);
	tilemap->set_tile(1, {14, 9}, {-1, -1}, true);
	tilemap->set_tile(1, {15, 9}, {-1, -1}, true);
	tilemap->set_tile(1, {16, 9}, {-1, -1}, true);

	tilemap->set_tile(3, {14, 8}, {19, 13}, true);
	tilemap->set_tile(3, {15, 8}, {20, 13}, true);
	tilemap->set_tile(3, {16, 8}, {21, 13}, true);
	tilemap->set_tile(3, {14, 9}, {19, 14}, true);
	tilemap->set_tile(3, {15, 9}, {20, 14}, true);
	tilemap->set_tile(3, {16, 9}, {21, 14}, true);

	wedge::Map_Entity *board3 = new wedge::Map_Entity("board3");
	board3->start(area);
	board3->set_position(util::Point<int>(14, 8));
	board3->set_sprite(new gfx::Sprite("board3"));
	board3->set_solid(false);
	board3->set_pivot_offset({-7.0f, 0.0f});
	board3->set_offset({0.66f, 0.5f});
	area->add_entity(board3);
	
	wedge::Map_Entity *board4 = new wedge::Map_Entity("board4");
	board4->start(area);
	board4->set_position(util::Point<int>(14, 8));
	board4->set_sprite(new gfx::Sprite("board1"));
	board4->set_solid(false);
	board4->set_pivot_offset({-7.0f, 0.0f});
	board4->set_offset({0.25f, 0.25f});
	area->add_entity(board4);
	
	wedge::Map_Entity *board5 = new wedge::Map_Entity("board5");
	board5->start(area);
	board5->set_position(util::Point<int>(15, 8));
	board5->set_sprite(new gfx::Sprite("board1"));
	board5->set_solid(false);
	board5->set_pivot_offset({-7.0f, 0.0f});
	board5->set_offset({0.2f, 0.5f});
	area->add_entity(board5);
	
	NEW_SYSTEM_AND_TASK(AREA)
	auto cas = new wedge::Change_Angle_Step(board3, 0.0f, float(-M_PI)*12.0f, 1900, new_task);
	ADD_STEP(cas)
	ADD_TASK(new_task)
	ANOTHER_TASK
	ADD_STEP(new wedge::Offset_Arc_Step(board3, util::Point<float>(0.66f, 0.5f), util::Point<float>(0.66f, 20.0f), 0.0f, 2000, new_task))
	auto wait_step1 = new wedge::Wait_Step(new_task);
	cas->add_monitor(wait_step1);
	ADD_STEP(wait_step1)
	ADD_STEP(new wedge::Delete_Map_Entity_Step(board3, new_task))
	ADD_TASK(new_task)
	ANOTHER_TASK
	auto cas2 = new wedge::Change_Angle_Step(board4, 0.0f, float(M_PI)*14.0f, 1500, new_task);
	ADD_TASK(new_task)
	ANOTHER_TASK
	ADD_STEP(new wedge::Offset_Arc_Step(board4, util::Point<float>(0.25f, 0.25f), util::Point<float>(0.25f, 20.0f), 0.0f, 1600, new_task))
	auto wait_step2 = new wedge::Wait_Step(new_task);
	cas2->add_monitor(wait_step2);
	ADD_STEP(wait_step2)
	ADD_STEP(new wedge::Delete_Map_Entity_Step(board4, new_task))
	ADD_TASK(new_task)
	ANOTHER_TASK
	ADD_STEP(new wedge::Offset_Arc_Step(board5, util::Point<float>(0.2f, 0.5f), util::Point<float>(0.2f, 20.0f), 0.0f, 1600, new_task))
	ADD_STEP(new wedge::Delete_Map_Entity_Step(board5, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void bridge_done(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	std::vector< util::Point<int> > pos;
	std::vector<wedge::Direction> dir;
	if (INSTANCE->is_milestone_complete(MS_CAVE1_5_LEFT)) {
		pos.push_back(util::Point<int>(11, 9));
		pos.push_back(util::Point<int>(10, 9));
	}
	else {
		pos.push_back(util::Point<int>(10, 9));
		pos.push_back(util::Point<int>(11, 9));
	}
	dir.push_back(wedge::DIR_S);
	dir.push_back(wedge::DIR_S);
	area->set_next_area("cave1_drop", pos, dir);
}

static void bridge_fall(void *data)
{
	DOGO_GLOBALS->fall->play(false);

	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);

	pleasant->get_sprite()->set_animation("fall");
	tom->get_sprite()->set_animation("fall");

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Offset_Arc_Step(pleasant, util::Point<float>(0.0f, 0.0f), util::Point<float>(0, 20.0f), 0.0f, 2000, new_task))
	ADD_TASK(new_task)
	ANOTHER_TASK
	ADD_STEP(new wedge::Offset_Arc_Step(tom, util::Point<float>(0.0f, 0.0f), util::Point<float>(0, 20.0f), 0.0f, 2000, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(bridge_done, data, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Cave1_5 : public Area_Hooks
{
public:
	Area_Hooks_Cave1_5(wedge::Area *area) :
		Area_Hooks(area),
		collapse(false)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(0, 9), util::Size<int>(1, 1));
		z1.area_name = "cave1_4";
		z1.topleft_dest = util::Point<int>(19, 22);
		z1.direction = wedge::DIR_W;
		scroll_zones.push_back(z1);

		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(27, 9), util::Size<int>(1, 1));
		z2.area_name = "cave1_6";
		z2.topleft_dest = util::Point<int>(0, 6);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave1_5()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
		}

		if (area->find_entity("chest") == nullptr) {
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BLUE_POTION, 5));
			chest->start(area);
			chest->set_sprite(new gfx::Sprite("chest"));
			chest->set_position(util::Point<int>(6, 13));
			area->add_entity(chest);
		}

		if (INSTANCE->is_milestone_complete(MS_CAVE1_5_COLLAPSE)) {
			gfx::Tilemap *tilemap = area->get_tilemap();
			tilemap->set_tile(1, {14, 8}, {19, 13}, true);
			tilemap->set_tile(1, {15, 8}, {20, 13}, true);
			tilemap->set_tile(1, {16, 8}, {21, 13}, true);
			tilemap->set_tile(1, {14, 9}, {19, 14}, true);
			tilemap->set_tile(1, {15, 9}, {20, 14}, true);
			tilemap->set_tile(1, {16, 9}, {21, 14}, true);
		}

		return true;
	}

	void end()
	{
		wedge::Map_Entity *e;

		e = area->find_entity("board1");
		if (e) {
			area->remove_entity(e, true);
		}

		e = area->find_entity("board2");
		if (e) {
			area->remove_entity(e, true);
		}

		e = area->find_entity("board3");
		if (e) {
			area->remove_entity(e, true);
		}

		e = area->find_entity("board4");
		if (e) {
			area->remove_entity(e, true);
		}

		e = area->find_entity("board5");
		if (e) {
			area->remove_entity(e, true);
		}
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		// Skip battles on the bridge (decrease step count as a hack so they don't always get a battle after the bridge)
		auto player_pos = AREA->get_player(0)->get_position();
		if (player_pos.x >= 8 && player_pos.x <= 19) {
			INSTANCE->step_count--;
			return nullptr;
		}

		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_1Coinman1Puddle();
		}
		else if (type == 2) {
			return new Battle_2Coinman();
		}
		else if (type == 1) {
			return new Battle_3Unicorn();
		}
		else {
			return new Battle_2Puddle();
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		// this takes priority over battles!
		if (entity == AREA->get_player(PLEASANT) && INSTANCE->is_milestone_complete(MS_CAVE1_5_COLLAPSE) == false) {
			util::Point<int> pos = entity->get_position();
			if (pos == util::Point<int>(15, 8)) {
				if (INSTANCE->is_milestone_complete(MS_CAVE1_5_LEFT)) {
					collapse = true;
				}
			}
			else if (pos == util::Point<int>(14, 8)) {
				if (INSTANCE->is_milestone_complete(MS_CAVE1_5_RIGHT)) {
					collapse = true;
				}
			}
			else if (pos == util::Point<int>(8, 8)) {
				INSTANCE->set_milestone_complete(MS_CAVE1_5_LEFT, false);
			}
			else if (pos == util::Point<int>(9, 8)) {
				INSTANCE->set_milestone_complete(MS_CAVE1_5_LEFT, true);
			}
			else if (pos == util::Point<int>(18, 8)) {
				INSTANCE->set_milestone_complete(MS_CAVE1_5_RIGHT, true);
			}
			else if (pos == util::Point<int>(19, 8)) {
				INSTANCE->set_milestone_complete(MS_CAVE1_5_RIGHT, false);
			}

			if (collapse) {
				INSTANCE->set_milestone_complete(MS_CAVE1_5_COLLAPSE, true);

				wedge::pause_presses(true);
				AREA->set_after_fade_delay(3000);

				NEW_SYSTEM_AND_TASK(AREA)
				ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->bridge_collapse, false, false, new_task, 10.0f))
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(bridge_collapse1, area, new_task))
				ADD_STEP(new wedge::Delay_Step(250, new_task))
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(bridge_collapse2, area, new_task))
				ADD_STEP(new wedge::Delay_Step(150, new_task))
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(bridge_fall, area, new_task))
				ADD_TASK(new_task)
				FINISH_SYSTEM(AREA)
				return true;
			}
		}

		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		return false;
	}
	
	std::vector<util::A_Star::Way_Point> get_way_points(util::Point<int> from)
	{
		auto v = wedge::Area_Hooks::get_way_points(from);

		for (int i = 8; i <= 19; i++) {
			util::A_Star::Way_Point wp;
			wp.to.push_back(util::Point<int>(i, 9));
			wp.by.push_back(util::Point<int>(i, 8));
			wp.only_by = true;
			v.push_back(wp);
		}
		return v;
	}

protected:
	bool collapse;
};

//--

class Area_Hooks_Cave1_6 : public Area_Hooks
{
public:
	Area_Hooks_Cave1_6(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 1));
		z1.area_name = "cave1_5";
		z1.topleft_dest = util::Point<int>(27, 9);
		z1.direction = wedge::DIR_W;
		scroll_zones.push_back(z1);
			
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(4, 10), util::Size<int>(1, 1));
		z.area_name = "cave1_7";
		z.player_positions.push_back(util::Point<int>(9, 5));
		z.player_positions.push_back(util::Point<int>(9, 5));
		z.player_positions.push_back(util::Point<int>(9, 5));
		z.directions.push_back(wedge::DIR_S);
		z.directions.push_back(wedge::DIR_S);
		z.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(z);
	}
	
	virtual ~Area_Hooks_Cave1_6()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
			wedge::Chest *chest1 = new wedge::Chest("chest1", "", OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_YELLOW, 2));
			chest1->start(area);
			chest1->set_position(util::Point<int>(5, 9));
			chest1->set_sprite(new gfx::Sprite("chest"));
			chest1->set_shadow(true, util::Size<int>(12, 6));
			chest1->set_achievement((void *)ACH_YELLOWBELT);
			area->add_entity(chest1);
		}

		return true;
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_1Coinman1Puddle();
		}
		else if (type == 2) {
			return new Battle_2Coinman();
		}
		else if (type == 1) {
			return new Battle_3Unicorn();
		}
		else {
			return new Battle_2Puddle();
		}
	}
};

//--

class Area_Hooks_Cave1_7 : public Area_Hooks
{
public:
	Area_Hooks_Cave1_7(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(0, 9), util::Size<int>(1, 1));
		z1.area_name = "cave1_drop";
		z1.topleft_dest = util::Point<int>(17, 9);
		z1.direction = wedge::DIR_W;
		scroll_zones.push_back(z1);
			
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(18, 9), util::Size<int>(1, 1));
		z.area_name = "cave1_8";
		z.player_positions.push_back(util::Point<int>(0, 9));
		z.player_positions.push_back(util::Point<int>(0, 9));
		z.player_positions.push_back(util::Point<int>(0, 9));
		z.directions.push_back(wedge::DIR_E);
		z.directions.push_back(wedge::DIR_E);
		z.directions.push_back(wedge::DIR_E);
		fade_zones.push_back(z);

		Fade_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(9, 5), util::Size<int>(1, 1));
		z2.area_name = "cave1_6";
		z2.player_positions.push_back(util::Point<int>(4, 10));
		z2.player_positions.push_back(util::Point<int>(4, 10));
		z2.player_positions.push_back(util::Point<int>(4, 10));
		z2.directions.push_back(wedge::DIR_N);
		z2.directions.push_back(wedge::DIR_N);
		z2.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave1_7()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
		}

		return true;
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_1Wrath1Puddle();
		}
		else if (type == 2) {
			return new Battle_2Wrath();
		}
		else if (type == 1) {
			return new Battle_3Unicorn();
		}
		else {
			return new Battle_2Puddle();
		}
	}
};

//--

class Area_Hooks_Cave1_Drop : public Area_Hooks
{
public:
	Area_Hooks_Cave1_Drop(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z1;
		z1.zone = util::Rectangle<int>(util::Point<int>(17, 9), util::Size<int>(1, 1));
		z1.area_name = "cave1_7";
		z1.topleft_dest = util::Point<int>(0, 9);
		z1.direction = wedge::DIR_E;
		scroll_zones.push_back(z1);
	}
	
	virtual ~Area_Hooks_Cave1_Drop()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		if (loaded == false) {
		}
		
		if (INSTANCE->is_milestone_complete(MS_CAVE1_5_COLLAPSE)) {
			auto tilemap = area->get_tilemap();
			tilemap->set_tile(1, util::Point<int>(10, 9), util::Point<int>(30, 5), false);
			tilemap->set_tile(1, util::Point<int>(11, 9), util::Point<int>(31, 5), false);
			tilemap->set_tile(1, util::Point<int>(10, 10), util::Point<int>(30, 6), false);
			tilemap->set_tile(1, util::Point<int>(11, 10), util::Point<int>(31, 6), false);
		}

		return true;
	}
	
	void started()
	{
		if (INSTANCE->is_milestone_complete(MS_CAVE1_5_COLLAPSE) && INSTANCE->is_milestone_complete(MS_CAVE1_5_COLLAPSE_DONE) == false) {
			INSTANCE->set_milestone_complete(MS_CAVE1_5_COLLAPSE_DONE, true);
			
			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *tom = AREA->get_player(TOM);
			
			pleasant->get_sprite()->set_animation("dead");
			tom->get_sprite()->set_animation("dead");

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Delay_Step(3000, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "sit_up", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(241)/* Originally: Oof! Nice bridge! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_s", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(242)/* Originally: Grrr! JB is in big trouble when I find him... No treats for a week! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "stand_s", new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}

		Area_Hooks::started();
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_1Wrath1Puddle();
		}
		else if (type == 2) {
			return new Battle_2Wrath();
		}
		else if (type == 1) {
			return new Battle_3Unicorn();
		}
		else {
			return new Battle_2Puddle();
		}
	}
};

//--

class Area_Hooks_Cave1_8 : public Area_Hooks
{
public:
	Area_Hooks_Cave1_8(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 9), util::Size<int>(1, 1));
		z.area_name = "cave1_7";
		z.player_positions.push_back(util::Point<int>(18, 9));
		z.player_positions.push_back(util::Point<int>(18, 9));
		z.player_positions.push_back(util::Point<int>(18, 9));
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		fade_zones.push_back(z);
		
		Fade_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(24, 9), util::Size<int>(1, 1));
		z2.area_name = "cave1_boss";
		z2.player_positions.push_back(util::Point<int>(0, 12));
		z2.player_positions.push_back(util::Point<int>(0, 12));
		z2.player_positions.push_back(util::Point<int>(0, 12));
		z2.directions.push_back(wedge::DIR_E);
		z2.directions.push_back(wedge::DIR_E);
		z2.directions.push_back(wedge::DIR_E);
		fade_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave1_8()
	{
		delete darkness_image1;
		delete darkness_image2;
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
		darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
		darkness_offset1 = util::Point<float>(0, 0);
		darkness_offset2 = util::Point<float>(128, 64);

		if (loaded == false) {
		}

		return true;
	}

	bool started_and_moved()
	{
		if (Area_Hooks::started_and_moved()) {
			return true;
		}

		if (INSTANCE->is_milestone_complete(MS_MENTION_STEAM) == false) {
			INSTANCE->set_milestone_complete(MS_MENTION_STEAM, true);

			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);

			wedge::pause_presses(true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(2, 9), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(246)/* Originally: It sure is getting hot and steamy in here... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(23, 9);
		water_anim.size = util::Size<int>(9, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 10));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(18, 7);
		water_anim.size = util::Size<int>(8, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(18, 8));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 11);
		water_anim.size = util::Size<int>(3, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 2);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 5));
		tilemap->add_animation_data(water_anim);
	}

	void run()
	{
		darkness_offset1.x += 0.04f;
		darkness_offset1.y += 0.02f;
		darkness_offset2.x -= 0.04f;
		darkness_offset2.y -= 0.02f;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 5) {
			draw_darkness(map_offset, 0.35f);
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v = Area_Hooks::get_post_draw_layers();
		v.push_back(5);
		return v;
	}

	void lost_device()
	{
		Area_Hooks::lost_device();
		delete darkness_image1;
		darkness_image1 = NULL;
		delete darkness_image2;
		darkness_image2 = NULL;
	}

	void found_device()
	{
		Area_Hooks::found_device();
		darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
		darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
	}

	// no battles leading up to boss
};

//--

static void boss1_bridge_callback(void *data);

static void boss1_raise(void *data);

class Area_Hooks_Cave1_Boss : public Area_Hooks
{
public:
	Area_Hooks_Cave1_Boss(wedge::Area *area) :
		Area_Hooks(area),
		bridge_count(0)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 12), util::Size<int>(1, 1));
		z.area_name = "cave1_8";
		z.player_positions.push_back(util::Point<int>(24, 9));
		z.player_positions.push_back(util::Point<int>(24, 9));
		z.player_positions.push_back(util::Point<int>(24, 9));
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		fade_zones.push_back(z);

		Fade_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(36, 12), util::Size<int>(1, 1));
		z2.area_name = "cave2_1";
		z2.player_positions.push_back(util::Point<int>(0, 6));
		z2.player_positions.push_back(util::Point<int>(0, 6));
		z2.player_positions.push_back(util::Point<int>(0, 6));
		z2.directions.push_back(wedge::DIR_E);
		z2.directions.push_back(wedge::DIR_E);
		z2.directions.push_back(wedge::DIR_E);
		fade_zones.push_back(z2);

		thump = new audio::Sample("thump.flac");
	}
	
	virtual ~Area_Hooks_Cave1_Boss()
	{
		delete darkness_image1;
		delete darkness_image2;

		delete thump;
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/underground.mml");

		darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
		darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
		darkness_offset1 = util::Point<float>(0, 0);
		darkness_offset2 = util::Point<float>(128, 64);

		if (loaded == false) {
		}
		
		if (INSTANCE->is_milestone_complete(MS_BOSS1) == false && area->find_entity("jellybean") == nullptr) {
			wedge::Map_Entity *jellybean = new wedge::Map_Entity("jellybean");
			jellybean->start(area);
			jellybean->set_position(util::Point<int>(31, 12));
			jellybean->set_sprite(new gfx::Sprite("jellybean"));
			jellybean->set_direction(wedge::DIR_W, true, false);
			jellybean->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(jellybean);
		}
		
		if (INSTANCE->is_milestone_complete(MS_BOSS1) == true) {
			raise_bridge(13);
		}

		return true;
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(23, 9);
		water_anim.size = util::Size<int>(9, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 10));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(18, 7);
		water_anim.size = util::Size<int>(8, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(18, 8));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 11);
		water_anim.size = util::Size<int>(3, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 2);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 5));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(26, 11);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(26, 12));
		tilemap->add_animation_data(water_anim);
	}

	void run()
	{
		darkness_offset1.x += 0.04f;
		darkness_offset1.y += 0.02f;
		darkness_offset2.x -= 0.04f;
		darkness_offset2.y -= 0.02f;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 5) {
			draw_darkness(map_offset, 0.5f);
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v = Area_Hooks::get_post_draw_layers();
		v.push_back(5);
		return v;
	}

	void lost_device()
	{
		Area_Hooks::lost_device();
		delete darkness_image1;
		darkness_image1 = NULL;
		delete darkness_image2;
		darkness_image2 = NULL;
	}

	void found_device()
	{
		Area_Hooks::found_device();
		darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
		darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
	}

	void raise_bridge(int count)
	{
		const int y = 12;

		int x = 16 + count;
		int start_x = x;

		gfx::Tilemap *tilemap = area->get_tilemap();

		for (int i = 0; i < count; i++) {
			if (x <= 26) {
				tilemap->set_tile(0, {x, y}, {19+(1-(x%2)), 6}, false);
				tilemap->set_tile(0, {x, y-1}, {x == 17 ? 23 : (x == 26 ? 25 : 24), 7}, true);
				tilemap->set_tile(2, {x, y-1}, {28, 7}, true);
				tilemap->set_tile(0, {x, y+1}, {28, 6}, true);
				tilemap->set_tile(1, {x, y+1}, {x == 17 ? 18 : (x == 26 ? 20 : 19), 7}, true);
				tilemap->set_tile(2, {x, y+1}, {28, 5}, true);

				if (x == 17) {
					tilemap->set_tile(1, {x, y}, {-1, -1}, false);
					tilemap->set_tile(3, {x, y+1}, {27, 6}, false);
				}
				else if (x == 26) {
					tilemap->set_tile(1, {x, y}, {-1, -1}, false);
					tilemap->set_tile(3, {x, y+1}, {29, 6}, false);
				}

				int yy;
				if (x == 17) {
					yy = 11;
				}
				else if (x == 26) {
					yy = 13;
				}
				else {
					yy = 12;
				}

				if (x == start_x) {
					tilemap->set_tile(4, {x, y-1}, {28, yy}, true);
					tilemap->set_tile(4, {x, y}, {28, yy}, false);
					tilemap->set_tile(4, {x, y+1}, {28, yy}, true);
				}
				else if (x == start_x-1) {
					tilemap->set_tile(4, {x, y-1}, {29, yy}, true);
					tilemap->set_tile(4, {x, y}, {29, yy}, false);
					tilemap->set_tile(4, {x, y+1}, {29, yy}, true);
				}
				else if (x == start_x-2) {
					tilemap->set_tile(4, {x, y-1}, {30, yy}, true);
					tilemap->set_tile(4, {x, y}, {30, yy}, false);
					tilemap->set_tile(4, {x, y+1}, {30, yy}, true);
				}
				else {
					tilemap->set_tile(4, {x, y-1}, {-1, -1}, true);
					tilemap->set_tile(4, {x, y}, {-1, -1}, false);
					tilemap->set_tile(4, {x, y+1}, {-1, -1}, true);
				}
			}
			x--;
		}
	}
	
	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (INSTANCE->is_milestone_complete(MS_BOSS1) == false && entity->get_position().x == 13) {
			INSTANCE->set_milestone_complete(MS_BOSS1, true);

			wedge::Map_Entity *tentacles[8];

			for (int i = 0; i < 5; i++) {
				tentacles[i] = new wedge::Map_Entity("tentacle" + util::itos(i));
				tentacles[i]->start(area);
				tentacles[i]->set_position(util::Point<int>(16, 13));
				// FIXME: set offset
				tentacles[i]->set_sprite(new gfx::Sprite("tentacle"));
				tentacles[i]->set_visible(false);
				tentacles[i]->set_draw_flags(gfx::Image::FLIP_H);
				tentacles[i]->get_sprite()->set_rand_start(true);
				area->add_entity(tentacles[i]);
			}

			auto octo = new wedge::Map_Entity("octo");
			octo->start(area);
			octo->set_position({20, 13});
			octo->set_offset({0.0f, 0.25f});
			// FIXME: set offset
			octo->set_sprite(new gfx::Sprite("octo"));
			octo->set_visible(false);
			octo->set_draw_flags(gfx::Image::FLIP_H);
			area->add_entity(octo);

			for (int i = 5; i < 8; i++) {
				tentacles[i] = new wedge::Map_Entity("tentacle" + util::itos(i));
				tentacles[i]->start(area);
				tentacles[i]->set_position(util::Point<int>(16, 13));
				// FIXME: set offset
				tentacles[i]->set_sprite(new gfx::Sprite("tentacle"));
				tentacles[i]->set_visible(false);
				tentacles[i]->set_draw_flags(gfx::Image::FLIP_H);
				tentacles[i]->get_sprite()->set_rand_start(true);
				area->add_entity(tentacles[i]);
			}

			tentacles[1]->set_offset({1.0f, -0.9f});
			tentacles[2]->set_offset({2.5f, -1.65f});
			tentacles[3]->set_offset({4.5f, -1.5f});
			tentacles[4]->set_offset({6.25f, 0.2f});

			tentacles[5]->set_offset({1.1f, 1.0f});
			tentacles[6]->set_offset({2.3f, 1.8f});
			tentacles[7]->set_offset({4.6f, 1.6f});

			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *tom = AREA->get_player(TOM);

			wedge::pause_presses(true);

			set_following((void *)0);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(16, 12), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(255)/* Originally: Did you ever learn to swim, Tom? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(256)/* Originally: Uh, no. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(257)/* Originally: I guess I go it alone then... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			auto raise_step = new wedge::Generic_Immediate_Callback_Step(boss1_raise, area, new_task);
			ADD_STEP(raise_step)
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(15, 12), new_task))
			wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
			raise_step->add_monitor(wait_step);
			ADD_STEP(wait_step)
			ADD_STEP(new wedge::Delay_Step(250, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_e_exclamation", new_task))
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(15, 11), new_task))
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(16, 11), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(258)/* Originally: Octo */ + TAG_END, GLOBALS->game_t->translate(259)/* Originally: Oh, here for your dinner, eh? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(octo, "angry", new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(258)/* Originally: Octo */ + TAG_END, GLOBALS->game_t->translate(260)/* Originally: Grrr. I'll have you know I've been here for 11 years and I'm overcooked by now, so back off! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(octo, "idle", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(261)/* Originally: Relax, we're not here to eat you. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(octo, "angry", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(258)/* Originally: Octo */ + TAG_END, GLOBALS->game_t->translate(262)/* Originally: Haha! Right! And I'm not going to send you to Davy Jones' Locker! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(263)/* Originally: Wait! We're just looking for my dog! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(258)/* Originally: Octo */ + TAG_END, GLOBALS->game_t->translate(264)/* Originally: The dog that took a bite out of my 8th tentacle? Now you're really in trouble!^On guard! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Start_Battle_Step(new Battle_Octo(), new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}
		else if (INSTANCE->is_milestone_complete(MS_BOSS1_JB) == false && entity->get_position().x == 25) {
			INSTANCE->set_milestone_complete(MS_BOSS1_JB, true);
			
			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *jb = area->find_entity("jellybean");
			
			wedge::pause_presses(true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(272)/* Originally: Jellybean!!! Come here sweetie! Let's go home! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(jb, util::Point<int>(36, 12), new_task))
			ADD_STEP(new wedge::Delete_Map_Entity_Step(jb, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(273)/* Originally: GUHHH! *sobs* */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(274)/* Originally: We'll get him back, don't worry. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	void inc_bridge()
	{
		bridge_count++;
		raise_bridge(bridge_count);
	}

	audio::Sample *get_thump()
	{
		return thump;
	}

	// no battles leading up to boss

protected:
	int bridge_count;
	audio::Sample *thump;
};

static void boss1_bridge_callback(void *data)
{
	auto d = static_cast<Area_Hooks_Cave1_Boss *>(data);
	d->inc_bridge();
}

static void done_octo_raise(void *data)
{
	auto sprite = static_cast<gfx::Sprite *>(data);

	sprite->set_animation("idle");
}

static void boss1_raise(void *data)
{
	auto area = static_cast<wedge::Area *>(data);

	wedge::Map_Entity *tentacles[8];

	for (int i = 0; i < 8; i++) {
		tentacles[i] = area->find_entity("tentacle" + util::itos(i));
		tentacles[i]->set_visible(true);
		gfx::Sprite *sprite = tentacles[i]->get_sprite();
		sprite->set_animation("raise", done_octo_raise, sprite);
	}

	wedge::Map_Entity *octo = area->find_entity("octo");
	octo->set_visible(true);
	gfx::Sprite *sprite = octo->get_sprite();
	sprite->set_animation("raise", done_octo_raise, sprite);

	DOGO_GLOBALS->octo->play(false);
}

static void tik_join(void *data)
{
	wedge::Area *area = AREA->get_current_area();

	wedge::Map_Entity *tik = area->find_entity("Tik");

	INSTANCE->stats.push_back(wedge::Player_Stats());
	INSTANCE->stats[2].sprite = new gfx::Sprite("tik");
	INSTANCE->stats[2].level = 1;
	INSTANCE->stats[2].experience = 0;
	INSTANCE->stats[2].base.fixed.max_hp = 150;
	INSTANCE->stats[2].base.fixed.max_mp = 50;
	INSTANCE->stats[2].base.fixed.attack = 40;
	INSTANCE->stats[2].base.fixed.defence = 25;
	INSTANCE->stats[2].base.fixed.set_extra(LUCK, 25);
	INSTANCE->stats[2].base.hp = INSTANCE->stats[2].base.fixed.max_hp;
	INSTANCE->stats[2].base.mp = INSTANCE->stats[2].base.fixed.max_mp;
	INSTANCE->stats[2].base.set_name("Tik");
	
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_WHITE, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_YELLOW, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_GOLD, 1));
	
	INSTANCE->stats[2].armour = wedge::Armour_Stats(BELT_GOLD);

	std::vector<std::string> v;
	v.push_back("Headbutt");
	v.push_back("Stomp");
	v.push_back("Cannonball");
	v.push_back("Fast");
	DOGO_INSTANCE->combos.push_back(v);

	AREA->set_player(TIK, tik);
	
	AREA->get_current_area()->order_player_input_steps();

	tik->set_solid(true);
}

static void tik_bounce2(void *data)
{
	auto area = static_cast<wedge::Area *>(data);

	auto tik = area->find_entity("Tik");
	auto tok = area->find_entity("tok");
	auto pleasant = AREA->get_player(PLEASANT);
	auto tom = AREA->get_player(TOM);

	// This makes him "walk" with bounce_s anim
	tik->get_sprite()->set_animation("bounce_w");
	tik->set_position({12, 11});
	tik->set_offset({1.0f, 0.0});
	tik->set_moving(true);
	tik->get_input_step()->get_movement_step()->set_moving(true);
	tik->get_input_step()->get_movement_step()->set_increment({-1.0f, 0.0f});
			
	NEW_SYSTEM_AND_TASK(AREA)
	std::vector<wedge::Map_Entity *> entities;
	entities.push_back(tik);
	std::vector<util::Point<int>> positions;
	positions.push_back({12, 11});
	ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(303)/* Originally: Hey hey! Look what we have here Tok! Customers! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(304)/* Originally: I can't believe my eyes! Tik! Am I hallucinating??? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	auto ds = new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(305)/* Originally: No you are not! Right this way folks! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task);
	ADD_STEP(ds)
	ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(13, 10), new_task))
	wedge::Wait_Step *wait_step3 = new wedge::Wait_Step(new_task);
	ADD_STEP(wait_step3)
	ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(306)/* Originally: I'm Tik and this is my brother Tok.^Tok has the finest goods for your perusal, uh... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(307)/* Originally: He's Tom and I'm Pleasant...^And, well, we could use some things, but we're actually just looking for our dog.^Have you seen him? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(308)/* Originally: Why, that fine doggo of yours passed through here just minutes ago.^Uh... I'd be worried. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(309)/* Originally: These two look like fine warriors to me. Much burlier than most who come through here. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(310)/* Originally: True, true. Hmmm... maybe I can interest you in a deal?^On the other end of this cavern, once you pass through all the nastiest bits...^There are these certain green gemstones I'm keen on.^Perhaps you could lead Tik there in return for some supplies to help you on your way? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(311)/* Originally: We are running low on supplies... What do you think, Tom? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(312)/* Originally: It's a deal! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(313)/* Originally: Excellent! Take these. And these! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::Give_Object_Step(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 3), wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::Give_Object_Step(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_TURNIP, 2), wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::Give_Object_Step(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1), wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(314)/* Originally: Tik, guide these fine folks through the caves.^And if you need any extra supplies, I've got plenty more! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(315)/* Originally: OK folks, I'll be back here where I can keep an eye out for that pup of yours! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(12, 10), new_task))
	ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(12, 11), new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(tik_join, area, new_task))
	ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
	ADD_STEP(new Autosave_Step(new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
	ds->add_monitor(wait_step);
	ADD_STEP(wait_step)
	auto as = new wedge::A_Star_Step(pleasant, util::Point<int>(14, 11), new_task);
	as->add_monitor(wait_step3);
	ADD_STEP(as)
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_N, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_N, true, false, new_task))
	ADD_TASK(new_task)

	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Cave2_1 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_1(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 1));
		z.area_name = "cave1_boss";
		z.player_positions.push_back(util::Point<int>(36, 12));
		z.player_positions.push_back(util::Point<int>(36, 12));
		z.player_positions.push_back(util::Point<int>(36, 12));
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		fade_zones.push_back(z);

		Fade_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(23, 11), util::Size<int>(1, 1));
		z2.area_name = "cave2_2";
		z2.player_positions.push_back(util::Point<int>(0, 8));
		z2.player_positions.push_back(util::Point<int>(0, 8));
		z2.player_positions.push_back(util::Point<int>(0, 8));
		z2.directions.push_back(wedge::DIR_E);
		z2.directions.push_back(wedge::DIR_E);
		z2.directions.push_back(wedge::DIR_E);
		fade_zones.push_back(z2);

		bounce = new audio::MML("sfx/bounce.mml");

		/*
		Slope s;

		s.position = { 10, 11 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 11, 10 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 10, 9 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 9, 10 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 18, 9 };
		s.direction = wedge::DIR_E;
		s.up = false;
		slopes.push_back(s);

		s.position = { 19, 10 };
		s.direction = wedge::DIR_E;
		s.up = false;
		slopes.push_back(s);

		s.position = { 18, 11 };
		s.direction = wedge::DIR_W;
		s.up = true;
		slopes.push_back(s);

		s.position = { 17, 10 };
		s.direction = wedge::DIR_W;
		s.up = true;
		slopes.push_back(s);
		*/
	}
	
	virtual ~Area_Hooks_Cave2_1()
	{
		delete darkness_image1;
		delete darkness_image2;

		delete bounce;
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
		darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
		darkness_offset1 = util::Point<float>(0, 0);
		darkness_offset2 = util::Point<float>(128, 64);

		if (loaded == false) {
			wedge::Map_Entity *tik = new wedge::Map_Entity("Tik");
			tik->start(area);
			tik->set_position(util::Point<int>(13, 10));
			tik->set_sprite(new gfx::Sprite("tik"));
			tik->set_solid(false);
			tik->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(tik);

			wedge::Map_Entity *bag = new wedge::Map_Entity("bag");
			bag->start(area);
			bag->set_position(util::Point<int>(14, 10));
			bag->set_sprite(new gfx::Sprite("bag"));
			bag->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(bag);

			wedge::Map_Entity *tok = new wedge::Map_Entity("tok");
			tok->start(area);
			tok->set_position(util::Point<int>(15, 10));
			gfx::Sprite *tok_sprite = new gfx::Sprite("tok");
			tok_sprite->set_animation("idle");
			tok->set_sprite(tok_sprite);
			tok->set_shadow(true, util::Size<int>(12, 6));
			area->add_entity(tok);
		}

		return true;
	}

	void run()
	{
		darkness_offset1.x += 0.04f;
		darkness_offset1.y += 0.02f;
		darkness_offset2.x -= 0.04f;
		darkness_offset2.y -= 0.02f;
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
			
			draw_darkness(map_offset, 0.35f);
		}
	}

	void lost_device()
	{
		Area_Hooks::lost_device();
		delete darkness_image1;
		darkness_image1 = NULL;
		delete darkness_image2;
		darkness_image2 = NULL;
	}

	void found_device()
	{
		Area_Hooks::found_device();
		darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
		darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && INSTANCE->is_milestone_complete(MS_TIK_JOINED) == false && entity->get_position() == util::Point<int>{11, 11}) {
			INSTANCE->set_milestone_complete(MS_TIK_JOINED, true);

			wedge::Map_Entity *tik = area->find_entity("Tik");
			wedge::Map_Entity *tok = area->find_entity("tok");

			tok->get_sprite()->set_animation("stand_w");

			// This makes him "walk" with bounce_s anim
			tik->get_sprite()->set_animation("bounce_s");
			tik->set_position({13, 11});
			tik->set_offset({0.0f, -1.0f});
			tik->set_moving(true);
			tik->get_input_step()->get_movement_step()->set_moving(true);
			tik->get_input_step()->get_movement_step()->set_increment({0.0f, 1.0f});
			
			wedge::pause_presses(true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Play_Sound_Step(bounce, false, false, new_task))
			std::vector<wedge::Map_Entity *> entities;
			entities.push_back(tik);
			std::vector<util::Point<int>> positions;
			positions.push_back({13, 11});
			ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(tik_bounce2, area, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	bool activate(wedge::Map_Entity *activator, wedge::Map_Entity *activated)
	{
		if (activated->get_name() == "tok") {
			std::vector<wedge::Object> items;
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 10));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 30));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARROT, 40));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 50));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_TURNIP, 75));

			NEW_SYSTEM_AND_TASK(AREA)
			Barter_Step *barter_step = new Barter_Step(items, new_task);
			ADD_STEP(new wedge::Pause_Presses_Step(true, false, new_task))
			ADD_STEP(barter_step)
			ADD_STEP(new wedge::Pause_Presses_Step(false, false, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			std::vector<std::string> choices;
			choices.push_back(GLOBALS->game_t->translate(293)/* Originally: Buy */);
			choices.push_back(GLOBALS->game_t->translate(294)/* Originally: Sell */);
			choices.push_back(GLOBALS->game_t->translate(295)/* Originally: Nevermind */);
			do_question(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(296)/* Originally: Fancy any of the merch? Or perhaps you want to lighten your load? */, wedge::DIALOGUE_SPEECH, choices, barter_step, 2);
		}
		return false;
	}

protected:
	bool do_heatwave;
	audio::Sound *bounce;
};

class Area_Hooks_Cave2_2 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_2(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 8), util::Size<int>(1, 1));
		z.area_name = "cave2_1";
		z.player_positions.push_back(util::Point<int>(23, 11));
		z.player_positions.push_back(util::Point<int>(23, 11));
		z.player_positions.push_back(util::Point<int>(23, 11));
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		fade_zones.push_back(z);
	
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(14, 8), util::Size<int>(1, 1));
		z2.area_name = "cave2_3";
		z2.topleft_dest = util::Point<int>(0, 7);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);

		/*
		Fade_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(14, 8), util::Size<int>(1, 1));
		z2.area_name = "cave2_5";
		z2.player_positions.push_back(util::Point<int>(7, 0));
		z2.player_positions.push_back(util::Point<int>(7, 0));
		z2.player_positions.push_back(util::Point<int>(7, 0));
		z2.directions.push_back(wedge::DIR_S);
		z2.directions.push_back(wedge::DIR_S);
		z2.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(z2);
		*/
	}
	
	virtual ~Area_Hooks_Cave2_2()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		if (loaded == false) {
		}

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	bool activate_with(wedge::Map_Entity *activator)
	{
		if (INSTANCE->is_milestone_complete(MS_GOT_TOKS_GEMS) == false && activator->get_position() == util::Point<int>(7, 6) && activator->get_direction() == wedge::DIR_N) {
			INSTANCE->set_milestone_complete(MS_GOT_TOKS_GEMS, true);

			DOGO_GLOBALS->pickup->play(false);

			auto o = OBJECT->make_object(wedge::OBJECT_SPECIAL, SPECIAL_GREEN_GEM, 2);
			INSTANCE->inventory.add(o);

			INSTANCE->stats[0].base.fixed.max_hp += 50;
			INSTANCE->stats[1].base.fixed.max_hp += 50;
		}

		return false;
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (INSTANCE->is_milestone_complete(MS_TIK_MENTIONED_GEMS) == false && entity->get_position() == util::Point<int>(7, 8)) {
			INSTANCE->set_milestone_complete(MS_TIK_MENTIONED_GEMS, true);
			
			wedge::pause_presses(true);

			auto pleasant = AREA->get_player(PLEASANT);
			auto tom = AREA->get_player(TOM);
			
			pleasant->set_direction(wedge::DIR_W, true, false);
			tom->set_direction(wedge::DIR_W, true, false);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(319)/* Originally: Hey guys... I really shouldn't tell you this, but...^At the end of this little nook, there's a bag where Tok hid some of those green gems.^You guys seem tough, but you might want to grab the gems before we head off into danger...^You see, these gems increase your power as long as you're holding onto them. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}

protected:
	bool do_heatwave;
};

class Area_Hooks_Cave2_3 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_3(wedge::Area *area) :
		Area_Hooks(area)
	{
		/*
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 7), util::Size<int>(1, 1));
		z.area_name = "cave2_2";
		z.player_positions.push_back(util::Point<int>(14, 8));
		z.player_positions.push_back(util::Point<int>(14, 8));
		z.player_positions.push_back(util::Point<int>(14, 8));
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		fade_zones.push_back(z);
		*/
		
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 7), util::Size<int>(1, 1));
		z.area_name = "cave2_2";
		z.topleft_dest = util::Point<int>(14, 8);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);

		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(33, 7), util::Size<int>(1, 1));
		z2.area_name = "cave2_4";
		z2.topleft_dest = util::Point<int>(0, 29);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);

		Slope s;

		s.position = { 7, 7 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 8, 6 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 7, 5 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 6, 6 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 26, 5 };
		s.direction = wedge::DIR_E;
		s.up = false;
		slopes.push_back(s);

		s.position = { 27, 6 };
		s.direction = wedge::DIR_E;
		s.up = false;
		slopes.push_back(s);

		s.position = { 26, 7 };
		s.direction = wedge::DIR_W;
		s.up = true;
		slopes.push_back(s);

		s.position = { 25, 6 };
		s.direction = wedge::DIR_W;
		s.up = true;
		slopes.push_back(s);
	}
	
	virtual ~Area_Hooks_Cave2_3()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		if (loaded == false) {
		}

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(5);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 5) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}

	std::vector<util::A_Star::Way_Point> get_way_points(util::Point<int> from)
	{
		auto v = wedge::Area_Hooks::get_way_points(from);

		// Some just to make clicks easier
		util::A_Star::Way_Point wp;
		std::pair<util::Point<int>, util::Point<int>> pair;
		
		pair.first = {7, 7};
		pair.second = {7, 6};
		wp.click_swaps.push_back(pair);

		pair.first = {26, 7};
		pair.second = {26, 6};
		wp.click_swaps.push_back(pair);

		for (int i = 9; i <= 24; i++) {
			pair.first = {i, 5};
			pair.second = {i, 6};
			wp.click_swaps.push_back(pair);
		}

		v.push_back(wp);

		return v;
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		util::Point<int> pos = AREA->get_player(0)->get_position();
		std::string bgname;
		int delay;
		if (pos.x >= 8 && pos.x <= 25) {
			bgname = "lavabridge-";
			delay = 500;
		}
		else {
			bgname = "cave2-";
			delay = 0;
		}

		if (type == 3) {
			return new Battle_3Goblin(bgname, delay);
		}
		else if (type == 2) {
			return new Battle_1RedWasp2Goblin(bgname, delay);
		}
		else if (type == 1) {
			return new Battle_2RedWasp(bgname, delay);
		}
		else {
			return new Battle_2Bot(bgname, delay);
		}
	}

protected:
	bool do_heatwave;
};

static void add_a_rock(void *data);

class Area_Hooks_Cave2_4 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_4(wedge::Area *area) :
		Area_Hooks(area),
		go(0),
		ending(false),
		rock_sfx(nullptr),
		have_dialogues(false),
		rock_vol_adj(1.0f),
		slide_task(nullptr)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 29), util::Size<int>(1, 1));
		z.area_name = "cave2_3";
		z.topleft_dest = util::Point<int>(33, 7);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);

		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(26, 45), util::Size<int>(1, 1));
		z2.area_name = "cave2_5";
		z2.topleft_dest = util::Point<int>(7, 0);
		z2.direction = wedge::DIR_S;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave2_4()
	{
	}

	// This has to be here: end is called before saving state when changing areas, must remove 'rock' before that so destructor not good enough
	void end()
	{
		if (rock_sfx != nullptr) {
			rock_sfx->stop();
			delete rock_sfx;
		}

		auto sys = slide_task->get_system();
		auto &tasks = sys->get_tasks();
		for (auto it = tasks.begin(); it != tasks.end(); it++) {
			if (*it == slide_task) {
				tasks.erase(it);
				break;
			}
		}
		delete slide_task;
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		area->dont_save("rock");

		audio::play_music("music/cave2.mml");

		if (loaded == false) {
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_ORANGE, 3));
			chest->start(area);
			chest->set_position(util::Point<int>(14, 23));
			chest->set_sprite(new gfx::Sprite("chest"));
			chest->set_shadow(true, util::Size<int>(12, 6));
			//chest->set_achievement((void *)ACH_ORANGEBELT);
			area->add_entity(chest);
		}
		else {
			auto chest = area->find_entity("chest");
			if (chest && chest->get_position().y == 15) {
				chest->set_position({14, 23});
			}
		}

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		rock_sfx->stop();

		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_3Goblin("cave2-", 0);
		}
		else if (type == 2) {
			return new Battle_1RedWasp2Goblin("cave2-", 0);
		}
		else if (type == 1) {
			return new Battle_2RedWasp("cave2-", 0);
		}
		else {
			return new Battle_2Bot("cave2-", 0);
		}
	}

	void run()
	{
		bool dial = GLOBALS->dialogue_active(AREA, true, false);

		if (dial != have_dialogues) {
			if (dial) {
				rock_vol_adj = 0.25f;
			}
			else {
				rock_vol_adj = 1.0f;
			}
		}

		have_dialogues = dial;

		auto entity = area->find_entity("rock");
		if (entity == nullptr) {
			return;
		}

		auto pleasant = AREA->get_player(PLEASANT);

		auto rock_pos = entity->get_position();
		auto rock_o = entity->get_offset();
		auto ppos = pleasant->get_position();
		auto po = pleasant->get_offset();

		util::Point<float> rock_abs;
		util::Point<float> p_abs;

		wedge::tile_to_abs(rock_pos, rock_o, rock_abs);
		wedge::tile_to_abs(ppos, po, p_abs);

		rock_abs += util::Point<float>(0.75f, 0.75f);
		p_abs += util::Point<float>(0.5f, 0.5f);

		float volume;
		float dist = (rock_abs-p_abs).length();

		float cutoff = 20.0f*shim::tile_size;

		if (dist > cutoff) {
			volume = 0.0f;
		}
		else {
			dist = cutoff - dist;
			volume = dist / cutoff;
		}

		rock_sfx->set_master_volume(volume*rock_vol_adj);

		if (AREA->get_gameover() == false && ppos.x >= 10 && ppos.x <= 12 && entity->get_position().y == ppos.y) {
			wedge::rumble(1.0f, 1000);
			gfx::screen_shake(1.0f, 500);
			rock_vol_adj = 0.25f;
			DOGO_GLOBALS->hit->play(false);
			save_play_time();
			INSTANCE->stats[0].base.hp = 0;
			pleasant->get_input_step()->end_movement(false, false, false);
			wedge::pause_presses(true);
			pleasant->get_sprite()->set_animation("dead");
			audio::stop_music();
			GLOBALS->gameover->play(shim::music_volume, true);
			gfx::add_notification(GLOBALS->game_t->translate(332)/* Originally: You were crushed! */);
			AREA->set_gameover(true);
			return;
		}

		bool br_east = INSTANCE->is_milestone_complete(MS_BOULDER_RUN_EAST);
		bool br_west = INSTANCE->is_milestone_complete(MS_BOULDER_RUN_WEST);

		if ((br_east || br_west) && go == 0) {
			// Rock just passes players
			if (entity->get_position().y == 31) {
				go = 1;
			}
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		bool br_east = INSTANCE->is_milestone_complete(MS_BOULDER_RUN_EAST);
		bool br_west = INSTANCE->is_milestone_complete(MS_BOULDER_RUN_WEST);

		auto pleasant = AREA->get_player(PLEASANT);
		auto tom = AREA->get_player(TOM);
		auto tik = AREA->get_player(TIK);

		// pre-empt wedge on_tile for these
		if (br_east == false && br_west == false && entity == AREA->get_player(0) && entity->get_position().x == 8) {
			ending = false;

			wedge::pause_presses(true);
			set_following((void *)0);

			INSTANCE->set_milestone_complete(MS_BOULDER_RUN_EAST, true);
			tom->set_solid(false);
			tik->set_solid(false);
			pleasant->face(tik, false);
			tom->face(tik, false);
			NEW_SYSTEM_AND_TASK(AREA)
			if (INSTANCE->is_milestone_complete(MS_TALKED_ABOUT_BOULDERS) == false) {
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(326)/* Originally: Watch out here! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			}
			else {
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(331)/* Originally: You know the drill... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			}

			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(9, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			
			if (INSTANCE->is_milestone_complete(MS_TALKED_ABOUT_BOULDERS) == false) {
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(327)/* Originally: Tom, stand here beside me. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			}

			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(9, 28), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(8, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))

			if (INSTANCE->is_milestone_complete(MS_TALKED_ABOUT_BOULDERS) == false) {
				INSTANCE->set_milestone_complete(MS_TALKED_ABOUT_BOULDERS, true);
				ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(328)/* Originally: Pleasant, follow right behind when we go! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			}

			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Set_Integer_Step(&go, 0, new_task))
			ADD_STEP(new wedge::Wait_For_Integer_Step(&go, 1, new_task))
			auto ds = new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(329)/* Originally: Now! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task);
			ADD_STEP(ds)
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(18, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_W, true, false, new_task))
			ADD_TASK(new_task)
			ANOTHER_TASK
			wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
			ds->add_monitor(wait_step);
			ADD_STEP(wait_step)
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(17, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
			ADD_TASK(new_task)
			ANOTHER_TASK
			wedge::Wait_Step *wait_step2 = new wedge::Wait_Step(new_task);
			ds->add_monitor(wait_step2);
			ADD_STEP(wait_step2)
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
	
			return true;
		}
		else if (br_west == false && br_east == false && entity == AREA->get_player(0) && entity->get_position().x == 14) {
			ending = false;

			wedge::pause_presses(true);
			set_following((void *)0);

			INSTANCE->set_milestone_complete(MS_BOULDER_RUN_WEST, true);
			tom->set_solid(false);
			tik->set_solid(false);
			pleasant->face(tik, false);
			tom->face(tik, false);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(331)/* Originally: You know the drill... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(13, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(13, 28), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(14, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Integer_Step(&go, 0, new_task))
			ADD_STEP(new wedge::Wait_For_Integer_Step(&go, 1, new_task))
			auto ds = new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(329)/* Originally: Now! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task);
			ADD_STEP(ds)
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(4, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
			ADD_TASK(new_task)
			ANOTHER_TASK
			wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
			ds->add_monitor(wait_step);
			ADD_STEP(wait_step)
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(5, 29), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_TASK(new_task)
			ANOTHER_TASK
			wedge::Wait_Step *wait_step2 = new wedge::Wait_Step(new_task);
			ds->add_monitor(wait_step2);
			ADD_STEP(wait_step2)
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
	
			return true;
		}
		if (ending == false && br_east == true && entity == AREA->get_player(0) && entity->get_position().x == 13 && entity->get_position().y >= 28) {
			ending = true;
			wedge::pause_presses(true);
			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(16, 29), new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(330)/* Originally: Well done! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task))
			ADD_STEP(new wedge::Set_Solid_Step(tom, true, new_task))
			ADD_STEP(new wedge::Set_Solid_Step(tik, true, new_task))
			ADD_STEP(new wedge::Set_Milestone_Complete_Step(MS_BOULDER_RUN_EAST, false, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
			return true;
		}
		if (ending == false && br_west == true && entity == AREA->get_player(0) && entity->get_position().x == 9) {
			ending = true;
			wedge::pause_presses(true);
			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(6, 29), new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(330)/* Originally: Well done! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task))
			ADD_STEP(new wedge::Set_Solid_Step(tom, true, new_task))
			ADD_STEP(new wedge::Set_Solid_Step(tik, true, new_task))
			ADD_STEP(new wedge::Set_Milestone_Complete_Step(MS_BOULDER_RUN_WEST, false, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
			return true;
		}

		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		return false;
	}
	
	bool try_tile(wedge::Map_Entity *entity, util::Point<int> tile_pos)
	{
		bool br_east = INSTANCE->is_milestone_complete(MS_BOULDER_RUN_EAST);
		bool br_west = INSTANCE->is_milestone_complete(MS_BOULDER_RUN_WEST);

		if (br_east && entity == AREA->get_player(0) && tile_pos.x == 7) {
			GLOBALS->do_dialogue(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(325)/* Originally: I can't leave the gang behind! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
			return true;
		}
		else if (br_west && entity == AREA->get_player(0) && tile_pos.x == 15) {
			GLOBALS->do_dialogue(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(325)/* Originally: I can't leave the gang behind! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
			return true;
		}

		if (Area_Hooks::try_tile(entity, tile_pos)) {
			return true;
		}

		return false;
	}

	void started()
	{
		Area_Hooks::started();

		rock_sfx = new audio::MML("sfx/rolling_rock.mml");
		rock_sfx->set_master_volume(0.0f);
		rock_sfx->play(true);

		add_rock();
	}

	void add_rock()
	{
		wedge::Map_Entity *rock = new wedge::Map_Entity("rock");
		rock->start(area);
		rock->set_position(util::Point<int>(10, 2));
		rock->set_size(util::Size<int>(3, 3));
		rock->set_sprite(new gfx::Sprite("rolling_rock"));
		rock->set_solid(false);
		rock->set_speed(0.1f);
		rock->set_do_tilt(false);
		rock->set_shadow(true, {36, 18});
		area->add_entity(rock);

		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new wedge::Slide_Entity_Step(rock, {10, 48}, 0.1f*shim::tile_size/*speed is in pixels per tick*/, new_task))
		/*
		auto as = new wedge::A_Star_Step(rock, util::Point<int>(10, 40), new_task);
		as->set_allow_out_of_bounds(true);
		as->set_check_solids(false);
		ADD_STEP(as)
		*/
		ADD_STEP(new wedge::Delete_Map_Entity_Step(rock, new_task))
		ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_a_rock, this, new_task))
		slide_task = new_task;
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)
	}

	void restart_rock_sfx()
	{
		rock_sfx->set_master_volume(0.0f);
		rock_sfx->play(true);
	}

	void pause(bool onoff) {
		if (onoff) {
			rock_sfx->stop();
		}
		else {
			rock_sfx->play(true);
		}
	}

protected:
	bool do_heatwave;
	int go;
	bool ending;
	audio::MML *rock_sfx;
	bool have_dialogues;
	float rock_vol_adj;
	wedge::Task *slide_task;
};

static void add_a_rock(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave2_4 *>(data);
	hooks->add_rock();
}

class Area_Hooks_Cave2_5 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_5(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(7, 0), util::Size<int>(1, 1));
		z.area_name = "cave2_4";
		z.topleft_dest = util::Point<int>(26, 45);
		z.direction = wedge::DIR_N;
		scroll_zones.push_back(z);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(9, 17), util::Size<int>(1, 1));
		z2.area_name = "cave2_6";
		z2.topleft_dest = util::Point<int>(10, 0);
		z2.direction = wedge::DIR_S;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave2_5()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		if (loaded == false) {
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_DAGGER, 3));
			chest->start(area);
			chest->set_sprite(new gfx::Sprite("chest"));
			chest->set_position(util::Point<int>(5, 13));
			area->add_entity(chest);
		}
		
		if (area->find_entity("chest2") == nullptr) {
			wedge::Chest *chest2 = new wedge::Chest("chest2", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BLUE_POTION, 5));
			chest2->start(area);
			chest2->set_sprite(new gfx::Sprite("chest"));
			chest2->set_position(util::Point<int>(13, 6));
			area->add_entity(chest2);
		}

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		util::Point<int> pos = AREA->get_player(0)->get_position();
		std::string bgname;
		int delay;
		if (pos.y >= 3 && pos.y <= 14) {
			bgname = "lavaboss-";
			delay = 500;
		}
		else {
			bgname = "cave2-";
			delay = 0;
		}

		if (type == 3) {
			return new Battle_2RedWasp(bgname, delay);
		}
		else if (type == 2) {
			return new Battle_4Goblin(bgname, delay);
		}
		else if (type == 1) {
			return new Battle_1FlameFrog1Bot(bgname, delay);
		}
		else {
			return new Battle_2Bot(bgname, delay);
		}
	}

protected:
	bool do_heatwave;
};

static void delete_sensei(void *data)
{
	wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");
	AREA->get_current_area()->remove_entity(sensei, true);
}

static void add_sensei_cave2(void *data)
{
	auto area = static_cast<wedge::Area *>(data);

	wedge::Map_Entity *sensei = new wedge::Map_Entity("sensei");
	sensei->start(area);
	sensei->set_position(util::Point<int>(10, 7));
	sensei->set_sprite(new gfx::Sprite("sensei"));
	sensei->set_solid(false);
	sensei->set_offset({0.0f, -6.0f});
	sensei->set_shadow(true, util::Size<int>(12, 6));
	sensei->set_sub_offset_from_shadow_pos(true);
	area->add_entity(sensei);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(sensei, util::Point<float>(0.0f, -6.0f), util::Point<float>(0.0f, 0.0f), 3.0f, SENSEI_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(SENSEI_DELAY, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_s", new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void sensei_walk_s_cave2(void *data)
{
	auto area = static_cast<wedge::Area *>(data);

	auto sensei = area->find_entity("sensei");
	auto pleasant = AREA->get_player(PLEASANT);
	auto tom = AREA->get_player(TOM);
	auto tik = AREA->get_player(TIK);

	auto pos = sensei->get_position();

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::A_Star_Step(sensei, pos+util::Point<int>(0, 1), new_task))
	//ADD_STEP(new wedge::Achieve_Step((void *)ACH_GREENBELT, new_task))
	ADD_STEP(new wedge::Give_Object_Step(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_GREEN, 3), wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(346)/* Originally: Say, you're a good fighter. Why don't you help us find our dog? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(347)/* Originally: I'm a teacher, not a figher. I will see you later */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::A_Star_Step(sensei, pos, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(250, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "jump", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->sensei_jump, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(sensei, util::Point<float>(0.0f, 0.0f), util::Point<float>(0.0f, -6.0f), 0.0f, 500, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(delete_sensei, nullptr, new_task))
	auto pms = new wedge::Play_Music_Step("music/cave2.mml", new_task);
	ADD_STEP(pms)
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(348)/* Originally: What a weirdo. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::Set_Solid_Step(tom, true, new_task))
	ADD_STEP(new wedge::Set_Solid_Step(tik, true, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task))
	ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
	ADD_STEP(new Autosave_Step(new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	auto wait_step = new wedge::Wait_Step(new_task);
	pms->add_monitor(wait_step);
	ADD_STEP(wait_step)
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	auto wait_step2 = new wedge::Wait_Step(new_task);
	pms->add_monitor(wait_step2);
	ADD_STEP(wait_step2)
	ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Cave2_6 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_6(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(10, 0), util::Size<int>(1, 1));
		z.area_name = "cave2_5";
		z.topleft_dest = util::Point<int>(9, 17);
		z.direction = wedge::DIR_N;
		scroll_zones.push_back(z);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(20, 9), util::Size<int>(1, 3));
		z2.area_name = "cave2_7";
		z2.topleft_dest = util::Point<int>(0, 13);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave2_6()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		if (loaded == false) {
		}

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_2RedWasp("cave2-", 0);
		}
		else if (type == 2) {
			return new Battle_4Goblin("cave2-", 0);
		}
		else if (type == 1) {
			return new Battle_1FlameFrog1Bot("cave2-", 0);
		}
		else {
			return new Battle_3FlameFrog("cave2-", 0);
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		// pre-empty Area_Hooks::on_tile to avoid battles starting with scene

		auto pleasant = AREA->get_player(PLEASANT);
		auto tom = AREA->get_player(TOM);
		auto tik = AREA->get_player(TIK);

		auto pleasant_pos = pleasant->get_position();

		if (entity == pleasant && pleasant_pos.y == 8 && INSTANCE->is_milestone_complete(MS_GREEN_BELTS) == false) {
			INSTANCE->set_milestone_complete(MS_GREEN_BELTS, true);
			
			wedge::pause_presses(true);
			set_following((void *)0);
			
			tom->set_solid(false);
			tik->set_solid(false);

			std::vector<wedge::Map_Entity *> ve;
			std::vector<util::Point<int>> vp;

			ve.push_back(pleasant);
			ve.push_back(tom);
			ve.push_back(tik);

			vp.push_back({11, 9});
			vp.push_back({10, 9});
			vp.push_back({9, 9});

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, vp[0], new_task))
			ADD_STEP(new wedge::Check_Positions_Step(ve, vp, true, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(339)/* Originally: I'm worried about Jellybean! What if we can't find him? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(340)/* Originally: Don't worry, there's only one way he could have gone. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			auto gics = new wedge::Generic_Immediate_Callback_Step(add_sensei_cave2, area, new_task);
			ADD_STEP(gics)
			ADD_STEP(new wedge::Delay_Step(250, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new wedge::Delay_Step(SENSEI_FALL_TIME+SENSEI_DELAY-500, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Play_Music_Step("music/sensei.mml", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_n_exclamation", new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_n", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(341)/* Originally: Hello again! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(342)/* Originally: You again! Are you following us or something? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(343)/* Originally: Yes! And I have something for you! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(344)/* Originally: Please say it's my dog... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(345)/* Originally: Unfortunately not. I have been watching your battles, and I'm impressed.^You earned these. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(sensei_walk_s_cave2, area, new_task)) // When this code executes, Sensei doesn't exist yet, so need to postpone this with a callback
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tom, vp[1], new_task))
			ADD_STEP(new wedge::Check_Positions_Step(ve, vp, true, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
			gics->add_monitor(wait_step);
			ADD_STEP(wait_step)
			ADD_STEP(new wedge::Delay_Step(250, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new wedge::Delay_Step(SENSEI_FALL_TIME+SENSEI_DELAY-500, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "stand_n_exclamation", new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "stand_n", new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tik, vp[2], new_task))
			ADD_STEP(new wedge::Check_Positions_Step(ve, vp, true, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
			wedge::Wait_Step *wait_step2 = new wedge::Wait_Step(new_task);
			gics->add_monitor(wait_step2);
			ADD_STEP(wait_step2)
			ADD_STEP(new wedge::Delay_Step(250, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new wedge::Delay_Step(SENSEI_FALL_TIME+SENSEI_DELAY-500, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tik, "stand_n_exclamation", new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tik, "stand_n", new_task))
			ADD_TASK(new_task)

			FINISH_SYSTEM(AREA)

			return true;
		}

		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		return false;
	}

protected:
	bool do_heatwave;
};

class Area_Hooks_Cave2_7 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_7(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 13), util::Size<int>(1, 3));
		z.area_name = "cave2_6";
		z.topleft_dest = util::Point<int>(20, 9);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(24, 13), util::Size<int>(1, 3));
		z2.area_name = "cave2_8";
		z2.topleft_dest = util::Point<int>(0, 6);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave2_7()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		util::Point<int> chest_pos(12, 5);
		util::Point<int> chest2_pos(13, 5);

		if (loaded == false) {
			wedge::Chest *chest = new wedge::Chest("chest", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_RED_POTION, 1));
			chest->start(area);
			chest->set_sprite(new gfx::Sprite("chest"));
			chest->set_position(chest_pos);
			area->add_entity(chest);
			
			wedge::Chest *chest2 = new wedge::Chest("chest2", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 3));
			chest2->start(area);
			chest2->set_sprite(new gfx::Sprite("chest"));
			chest2->set_position(chest2_pos);
			area->add_entity(chest2);
		}

		auto chest = area->find_entity("chest");
		auto chest2 = area->find_entity("chest2");
		chest->set_position(chest_pos);
		chest2->set_position(chest2_pos);

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	bool has_battles()
	{
		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(4);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 3) {
			return new Battle_2RedWasp("cave2-", 0);
		}
		else if (type == 2) {
			return new Battle_4Goblin("cave2-", 0);
		}
		else if (type == 1) {
			return new Battle_1FlameFrog1Bot("cave2-", 0);
		}
		else {
			return new Battle_3FlameFrog("cave2-", 0);
		}
	}

protected:
	bool do_heatwave;
};

class Area_Hooks_Cave2_8 : public Area_Hooks
{
public:
	Area_Hooks_Cave2_8(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 3));
		z.area_name = "cave2_7";
		z.topleft_dest = util::Point<int>(24, 13);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);

		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(17, 6), util::Size<int>(1, 3));
		z2.area_name = "cave2_boss";
		z2.topleft_dest = util::Point<int>(0, 9);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave2_8()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		if (loaded == false) {
		}

		return true;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(3);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 3) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	bool has_battles()
	{
		return false;
	}

protected:
	bool do_heatwave;
};

static void chilly_rage_rumble(void *data)
{
	gfx::screen_shake(1.0f, 500);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void chilly_rage_nonbattle(void *data)
{
	auto chilly_thud = static_cast<audio::Sample *>(data);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->jump, false, false, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(chilly_thud, false, false, new_task))
	ADD_TASK(new_task)
	ANOTHER_TASK
	ADD_STEP(new wedge::Delay_Step(550, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_rumble, nullptr, new_task))
	ADD_STEP(new wedge::Delay_Step(550, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_rumble, nullptr, new_task))
	ADD_STEP(new wedge::Delay_Step(550, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_rumble, nullptr, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void add_tok(void *data);

static void change_to_cave3(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	std::vector< util::Point<int> > pos;
	std::vector<wedge::Direction> dir;
	pos.push_back(util::Point<int>(0, 10));
	pos.push_back(util::Point<int>(0, 10));
	pos.push_back(util::Point<int>(0, 10));
	dir.push_back(wedge::DIR_S);
	dir.push_back(wedge::DIR_S);
	dir.push_back(wedge::DIR_S);
	area->set_next_area("cave3_1", pos, dir);
}

class Area_Hooks_Cave2_Boss : public Area_Hooks
{
public:
	Area_Hooks_Cave2_Boss(wedge::Area *area) :
		Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 9), util::Size<int>(1, 3));
		z.area_name = "cave2_8";
		z.topleft_dest = util::Point<int>(17, 6);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);

		chilly_huh = new audio::Sample("chilly_huh.flac");
		chilly_groan = new audio::Sample("chilly_groan.flac");
		chilly_thud = new audio::Sample("chilly_thud.flac");

		angle = 0;
	}
	
	virtual ~Area_Hooks_Cave2_Boss()
	{
		delete chilly_huh;
		delete chilly_groan;
		delete chilly_thud;
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave2.mml");

		if (loaded == false) {
			wedge::Map_Entity *chilly = new wedge::Map_Entity("chilly");
			chilly->start(area);
			chilly->set_position(util::Point<int>(19, 10));
			auto chilly_sprite = new gfx::Sprite("chilly");
			chilly_sprite->set_animation("idle");
			chilly->set_sprite(chilly_sprite);
			chilly->set_solid(false);
			chilly->set_size({3, 3});
			chilly->set_layer(3);
			chilly->set_shadow(true, {48, 24});
			chilly->set_shadow_offset({19.0f, 0.0f});
			chilly->set_shadow_layer(2);
			area->add_entity(chilly);
			
			wedge::Map_Entity *elevator = new wedge::Map_Entity("elevator");
			elevator->start(area);
			elevator->set_position(util::Point<int>(31, 11));
			elevator->set_sprite(new gfx::Sprite("elevator_ground"));
			elevator->set_solid(false);
			elevator->set_size({3, 3});
			elevator->set_layer(2);
			area->add_entity(elevator);
			
			wedge::Map_Entity *elevator_edges = new wedge::Map_Entity("elevator_edges");
			elevator_edges->start(area);
			elevator_edges->set_position(util::Point<int>(31, 24));
			elevator_edges->set_sprite(new gfx::Sprite("elevator_edges"));
			elevator_edges->set_solid(false);
			elevator_edges->set_size({3, 16});
			elevator_edges->set_layer(2);
			area->add_entity(elevator_edges);
		}

		return true;
	}

	void end()
	{
		auto s = dynamic_cast<audio::Sample *>(DOGO_GLOBALS->elevator_raising);

		if (s && s->is_playing() && AREA->changing_areas() == false) {
			s->stop();
		}
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}

	std::vector<int> get_post_draw_layers()
	{
		std::vector<int> v;
		v.push_back(5);
		return v;
	}

	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			if (AREA->changing_areas() || AREA->is_pausing() || AREA->has_pause_ended()) {// || SHOP != nullptr) {
				do_heatwave = false;
			}
			else {
#ifdef ANDROID
				if (GLOBALS->have_highp_fragment_shaders) {
#endif
					do_heatwave = gfx::get_target_image() != GLOBALS->work_image;
#ifdef ANDROID
				}
				else {
					do_heatwave = false;
				}
#endif
			}

			if (do_heatwave) {
				auto systems = AREA->get_systems();
				for (auto sys : systems) {
					auto tasks = sys->get_tasks();
					for (auto t : tasks) {
						auto steps = t->get_steps();
						for (auto s : steps) {
							if (
								dynamic_cast<Battle_Transition_In_Step *>(s) ||
								dynamic_cast<Battle_Transition_In2_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out_Step *>(s) ||
								dynamic_cast<Battle_Transition_Out2_Step *>(s)
							) {
								do_heatwave = false;
								break;
							}
						}
						if (do_heatwave == false) {
							break;
						}
					}
				}
			}

			if (do_heatwave) {
				gfx::set_target_image(GLOBALS->work_image);
				gfx::clear(shim::black);
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();
			}
		}
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 5) {
			if (do_heatwave) {
				Uint32 elapsed = GET_TICKS();
				float t = elapsed / 2000.0f;

				gfx::Shader *prev_shader = shim::current_shader;
				shim::current_shader = DOGO_GLOBALS->heat_wave_shader;
				shim::current_shader->use();
				gfx::update_projection();
				shim::current_shader->set_float("screen_offset_x", shim::screen_offset.x);
				shim::current_shader->set_float("screen_offset_y", shim::screen_offset.y);
				shim::current_shader->set_float("real_screen_h", shim::real_screen_size.h);
				shim::current_shader->set_float("scale", shim::scale);
				shim::current_shader->set_float("screen_x", 0.0f);
				shim::current_shader->set_float("screen_y", 0.0f);
				shim::current_shader->set_float("screen_w", shim::screen_size.w);
				shim::current_shader->set_float("screen_h", shim::screen_size.h);
				shim::current_shader->set_float("heat_w", shim::screen_size.w);
				shim::current_shader->set_float("heat_h", shim::screen_size.h);
				shim::current_shader->set_float("t", t);
				shim::current_shader->set_float("inv_tex_w", 1.0f/GLOBALS->work_image->size.w);
				shim::current_shader->set_float("inv_tex_h", 1.0f/GLOBALS->work_image->size.h);
				shim::current_shader->set_float("wave_size", 2.5f);

				gfx::set_target_backbuffer();
				gfx::set_default_projection(shim::real_screen_size, {0, 0}, 1.0f);
				gfx::update_projection();

				GLOBALS->work_image->draw({0, 0});
				
				gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
				gfx::apply_screen_shake();
				gfx::update_projection();

				shim::current_shader = prev_shader;
				shim::current_shader->use();
				gfx::update_projection();
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data lava_anim;
		lava_anim.topleft = util::Point<int>(16, 17);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.delay = 1000;
		lava_anim.frames.clear();
		lava_anim.frames.push_back(util::Point<int>(19, 17));
		lava_anim.frames.push_back(util::Point<int>(22, 17));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 20);
		lava_anim.size = util::Size<int>(3, 3);
		lava_anim.frames.push_back(util::Point<int>(19, 20));
		lava_anim.frames.push_back(util::Point<int>(22, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 20);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 20));
		lava_anim.frames.push_back(util::Point<int>(27, 20));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(16, 23);
		lava_anim.size = util::Size<int>(4, 1);
		lava_anim.frames.push_back(util::Point<int>(16, 24));
		lava_anim.frames.push_back(util::Point<int>(16, 25));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(13, 18);
		lava_anim.size = util::Size<int>(1, 14);
		lava_anim.frames.push_back(util::Point<int>(14, 18));
		lava_anim.frames.push_back(util::Point<int>(15, 18));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(10, 26);
		lava_anim.size = util::Size<int>(1, 1);
		lava_anim.frames.push_back(util::Point<int>(11, 26));
		lava_anim.frames.push_back(util::Point<int>(12, 26));
		tilemap->add_animation_data(lava_anim);
		lava_anim.frames.clear();
		lava_anim.topleft = util::Point<int>(25, 22);
		lava_anim.size = util::Size<int>(1, 2);
		lava_anim.frames.push_back(util::Point<int>(26, 22));
		lava_anim.frames.push_back(util::Point<int>(27, 22));
		tilemap->add_animation_data(lava_anim);
	}
	
	bool has_battles()
	{
		return false;
	}
	
	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && entity->get_position().x == 26 && INSTANCE->is_milestone_complete(MS_ELEVATOR_SCENE) == false) {
			INSTANCE->set_milestone_complete(MS_ELEVATOR_SCENE, true);
			
			auto pleasant = AREA->get_player(0);
			auto tom = AREA->get_player(1);
			auto tik = AREA->get_player(2);

			pleasant->set_layer(5);
			tom->set_layer(5);
			tik->set_layer(5);

			wedge::pause_presses(true);
			set_following((void *)0);

			tom->set_solid(false);
			tik->set_solid(false);
		
			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(30, 11), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			std::vector<wedge::Map_Entity *> entities;
			entities.push_back(pleasant);
			entities.push_back(tom);
			entities.push_back(tik);
			std::vector<util::Point<int>> positions;
			positions.push_back({30, 11});
			positions.push_back({30, 10});
			positions.push_back({30, 9});
			ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(424)/* Originally: That's an elevator. Your little friend must have gone up. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_N, true, false, new_task))
			auto ds1 = new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(425)/* Originally: OK... then up we go! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task);
			ADD_STEP(ds1)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(32, 10), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, new_task))
			ADD_TASK(new_task)

		
			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(30, 10), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			wedge::Wait_Step *ws6 = new wedge::Wait_Step(new_task);
			ds1->add_monitor(ws6);
			ADD_STEP(ws6)
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(31, 10), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_S, true, false, new_task))
			ADD_TASK(new_task)
	
			
			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(30, 9), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
			wedge::Wait_Step *ws2 = new wedge::Wait_Step(new_task);
			ds1->add_monitor(ws2);
			ADD_STEP(ws2)
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(33, 10), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_tok, this, new_task))
			ADD_TASK(new_task)


			FINISH_SYSTEM(AREA)
			
			return true;
		}
		else if (entity == AREA->get_player(PLEASANT) && entity->get_position().x == 14 && INSTANCE->is_milestone_complete(MS_BEAT_CHILLY) == false) {
			INSTANCE->set_milestone_complete(MS_BEAT_CHILLY, true);

			auto pleasant = AREA->get_player(0);
			//auto tom = AREA->get_player(1);
			//auto tik = AREA->get_player(2);
			auto chilly = area->find_entity("chilly");
			
			wedge::pause_presses(true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(14, 10), new_task))
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(17, 10), new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(410)/* Originally: Excuse me? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(chilly_huh, false, false, new_task))
			ADD_STEP(new wedge::Play_Animation_Step(chilly->get_sprite(), "turn_w", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(chilly, "stand_w", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(419)/* Originally: Chilly */ + TAG_END, GLOBALS->game_t->translate(411)/* Originally: Huh? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(chilly, "slant_w", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(419)/* Originally: Chilly */ + TAG_END, GLOBALS->game_t->translate(412)/* Originally: Uh, elves. Too bad... I ain't goin' back! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(413)/* Originally: Elves? No, sorry, we're just looking for our dog. Have you seen him? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(chilly, "annoyed", new_task))
			ADD_STEP(new wedge::Play_Sound_Step(chilly_groan, false, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(419)/* Originally: Chilly */ + TAG_END, GLOBALS->game_t->translate(414)/* Originally: North Pole's too cold for Chilly! Ain't goin' back! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(415)/* Originally: We're actually just looking for our dog...^I'm sure he came this way. You must have seen him? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(chilly, "idle", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(419)/* Originally: Chilly */ + TAG_END, GLOBALS->game_t->translate(416)/* Originally: Uh, the reindeer. He went thataway. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(417)/* Originally: OK, we'll just scoot by you here and... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(chilly, "annoyed", new_task))
			ADD_STEP(new wedge::Play_Sound_Step(chilly_groan, false, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(419)/* Originally: Chilly */ + TAG_END, GLOBALS->game_t->translate(418)/* Originally: NOOO!!! Chilly ain't goin' back!!! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(chilly_rage_nonbattle, chilly_thud, new_task))
			ADD_STEP(new wedge::Play_Animation_Step(chilly->get_sprite(), "rage", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(chilly, "idle", new_task))
			ADD_STEP(new Start_Battle_Step(new Battle_Chilly(), new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	void inc_angle()
	{
		angle++;
		angle %= 4;
			
		auto pleasant = AREA->get_player(0);
		auto tom = AREA->get_player(1);
		auto tik = area->find_entity("Tik");
		auto tok = area->find_entity("tok");
		auto elevator = area->find_entity("elevator");

		auto pleasant_s = pleasant->get_sprite();
		auto tom_s = tom->get_sprite();
		auto tik_s = tik->get_sprite();
		auto tok_s = tok->get_sprite();

		int a;

		if (angle == 0) {
			a = 0;
		}
		else if (angle == 1) {
			a = 3;
		}
		else if (angle == 2) {
			a = 2;
		}
		else {
			a = 1;
		}

		std::string frame = util::itos(a+1);

		elevator->get_sprite()->set_animation(frame);

		if (a == 0) {
			pleasant_s->set_animation("stand_s");
			tom_s->set_animation("stand_s");
			tik_s->set_animation("stand_s");
			tok_s->set_animation("stand_s");

			tom->set_position({31, 10});
			tik->set_position({33, 10});
			tok->set_position({32, 9});
		}
		else if (a == 1) {
			pleasant_s->set_animation("stand_w");
			tom_s->set_animation("stand_w");
			tik_s->set_animation("stand_w");
			tok_s->set_animation("stand_w");

			tom->set_position({32, 9});
			tik->set_position({32, 11});
			tok->set_position({33, 10});
		}
		else if (a == 2) {
			pleasant_s->set_animation("stand_n");
			tom_s->set_animation("stand_n");
			tik_s->set_animation("stand_n");
			tok_s->set_animation("stand_n");

			tom->set_position({33, 10});
			tik->set_position({31, 10});
			tok->set_position({32, 11});
		}
		else {
			pleasant_s->set_animation("stand_e");
			tom_s->set_animation("stand_e");
			tik_s->set_animation("stand_e");
			tok_s->set_animation("stand_e");

			tom->set_position({32, 11});
			tik->set_position({32, 9});
			tok->set_position({31, 10});
		}
	}

protected:
	bool do_heatwave;
	audio::Sample *chilly_huh;
	audio::Sample *chilly_groan;
	audio::Sample *chilly_thud;

	int angle; // 0-3, 0 = start angle
};

static void cave2_boss_inc_angle(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave2_Boss *>(data);
	hooks->inc_angle();
}

static void cave2_boss_inc_angle_and_move(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave2_Boss *>(data);
	cave2_boss_inc_angle(hooks); // do this here too

	auto area = AREA->get_current_area();

	auto pleasant = AREA->get_player(0);
	auto tom = AREA->get_player(1);
	auto tik = area->find_entity("Tik");
	auto tok = area->find_entity("tok");
	auto elevator = area->find_entity("elevator");
	auto elevator_edges = area->find_entity("elevator_edges");

	NEW_SYSTEM_AND_TASK(AREA)

	ADD_STEP(new wedge::Slide_Entity_Offset_Step(pleasant, {0.0f, -16.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(tom, {0.0f, -16.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(tik, {0.0f, -16.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(tok, {0.0f, -16.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(elevator, {0.0f, -16.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(elevator_edges, {0.0f, -16.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	FINISH_SYSTEM(AREA)
}

void add_tok(void *data)
{
	auto area = AREA->get_current_area();
	auto hooks = static_cast<Area_Hooks_Cave2_Boss *>(data);

	wedge::Map_Entity *tok = new wedge::Map_Entity("tok");
	tok->start(area);
	tok->set_position(util::Point<int>(19, 11));
	tok->set_sprite(new gfx::Sprite("tok"));
	tok->set_solid(false);
	tok->set_layer(5);
	tok->set_shadow(true, util::Size<int>(12, 6));
	tok->set_auto_shadow_pos(true);
	area->add_entity(tok);
			
	auto pleasant = AREA->get_player(0);
	auto tom = AREA->get_player(1);
	auto tik = AREA->get_player(2);

	wedge::Task *task1;
	wedge::Task *task2;
	wedge::Task *task3;

	NEW_SYSTEM_AND_TASK(AREA)
	task1 = new_task;
	ANOTHER_TASK
	task2 = new_task;
	ANOTHER_TASK
	task3 = new_task;

	auto ds2 = new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(426)/* Originally: Wait! I'm coming too! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, task3);
	auto ds3 = new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(427)/* Originally: Great timing, Tok! C'mon, let's go! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, task3);

	wedge::Wait_Step *ws3 = new wedge::Wait_Step(task1);
	ds2->add_monitor(ws3);
	ADD_STEP_TO(ws3, task1);
	ADD_STEP_TO(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, task1), task1)
	wedge::Wait_Step *ws5 = new wedge::Wait_Step(task1);
	ds3->add_monitor(ws5);
	ADD_STEP_TO(ws5, task1);
	ADD_STEP_TO(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, task1), task1)

	wedge::Wait_Step *ws1 = new wedge::Wait_Step(task2);
	ds2->add_monitor(ws1);
	ADD_STEP_TO(ws1, task2);
	ADD_STEP_TO(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, task2), task2)
	wedge::Wait_Step *ws4 = new wedge::Wait_Step(task2);
	ds3->add_monitor(ws4);
	ADD_STEP_TO(ws4, task2);
	ADD_STEP_TO(new wedge::Set_Direction_Step(tom, wedge::DIR_S, true, false, task2), task2)

	ADD_STEP_TO(new wedge::A_Star_Step(tok, util::Point<int>(26, 11), task3), task3)
	ADD_STEP_TO(ds2, task3)
	ADD_STEP_TO(new wedge::Set_Direction_Step(tik, wedge::DIR_W, true, false, task3), task3)
	ADD_STEP_TO(ds3, task3)
	ADD_STEP_TO(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, task3), task3)
	ADD_STEP_TO(new wedge::A_Star_Step(tok, util::Point<int>(32, 9), task3), task3)
	ADD_STEP_TO(new wedge::Set_Direction_Step(tok, wedge::DIR_S, true, false, task3), task3)
	ADD_STEP_TO(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(428)/* Originally: So, how do we start this thing? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, task3), task3)
	ADD_STEP_TO(new wedge::Play_Sound_Step(DOGO_GLOBALS->elevator_start, true, false, task3), task3)
	ADD_STEP_TO(new wedge::Play_Sound_Step(DOGO_GLOBALS->elevator_raising, false, true, task3), task3)
	ADD_STEP_TO(new wedge::Generic_Immediate_Callback_Step(cave2_boss_inc_angle_and_move, hooks, task3), task3)
	ADD_STEP_TO(new wedge::Delay_Step(125, task3), task3)
	for (int i = 0; i < 32-1; i++) {
		ADD_STEP_TO(new wedge::Generic_Immediate_Callback_Step(cave2_boss_inc_angle, hooks, task3), task3)
		ADD_STEP_TO(new wedge::Delay_Step(125, task3), task3)
	}
	ADD_STEP_TO(new wedge::Generic_Immediate_Callback_Step(change_to_cave3, area, task3), task3)

	ADD_TASK(task1)
	ADD_TASK(task2)
	ADD_TASK(task3)
	FINISH_SYSTEM(AREA)
}

static void cave3_1_inc_angle(void *data);
static void cave3_1_inc_angle_and_move(void *data);
static void cave3_1_dec_angle(void *data);
static void cave3_1_dec_angle_and_move(void *data);
static void cave3_set_hole_solid(void *data);

static void end_elevator_sample(void *data)
{
	DOGO_GLOBALS->elevator_raising->stop();
}

static void pop_tik(void *data)
{
	pop_player(); // get rid of Tik from party
}

static void cave3_pleasant_tom_face_n(void *data)
{
	auto pleasant = AREA->get_player(0);
	auto tom = AREA->get_player(1);

	//pleasant->get_input_step()->end_movement();

	pleasant->set_direction(wedge::DIR_N, true, false);
	tom->set_direction(wedge::DIR_N, true, false);
}

static void remove_elevator(void *data)
{
	auto area = AREA->get_current_area();
	auto e1 = area->find_entity("elevator");
	auto e2 = area->find_entity("elevator_edges");

	area->remove_entity(e1, true);
	area->remove_entity(e2, true);
}

static void cave3_set_solid(void *data)
{
	auto area = AREA->get_current_area();

	auto pleasant = AREA->get_player(0);
	auto tom = AREA->get_player(1);
	auto tik = area->find_entity("Tik");
	auto tok = area->find_entity("tok");

	pleasant->set_solid(true);
	tom->set_solid(true);
	tik->set_solid(true);
	tok->set_solid(true);
}

static void cave3_set_layer_6(void *data)
{
	AREA->get_player(0)->set_layer(6);
	AREA->get_player(1)->set_layer(6);
}

static void pleasant_tom_face_each_other(void *data)
{
	auto pleasant = AREA->get_player(0);
	auto tom = AREA->get_player(1);

	pleasant->face(tom, false);
	tom->face(pleasant, false);
}

static void remove_jb(void *data)
{
	auto area = AREA->get_current_area();
	auto jb = area->find_entity("jellybean");
	area->remove_entity(jb, true);
}

class Area_Hooks_Cave3_1 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_1(wedge::Area *area) :
		Area_Hooks(area),
		angle(2),
		dripping(false)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(24, 4), util::Size<int>(1, 5));
		z.area_name = "cave3_2";
		z.player_positions.push_back(util::Point<int>(0, 22));
		z.player_positions.push_back(util::Point<int>(0, 22));
		z.player_positions.push_back(util::Point<int>(0, 22));
		z.directions.push_back(wedge::DIR_E);
		z.directions.push_back(wedge::DIR_E);
		z.directions.push_back(wedge::DIR_E);
		fade_zones.push_back(z);

		next_drip = GET_TICKS() + 5000;
	}
	
	virtual ~Area_Hooks_Cave3_1()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
			wedge::Map_Entity *elevator = new wedge::Map_Entity("elevator");
			elevator->start(area);
			elevator->set_position(util::Point<int>(3, 7));
			elevator->set_sprite(new gfx::Sprite("elevator_ground"));
			elevator->set_solid(false);
			elevator->set_size({3, 3});
			elevator->set_layer(2);
			area->add_entity(elevator);
			
			wedge::Map_Entity *elevator_edges = new wedge::Map_Entity("elevator_edges");
			elevator_edges->start(area);
			elevator_edges->set_position(util::Point<int>(3, 7));
			elevator_edges->set_sprite(new gfx::Sprite("elevator_edges"));
			elevator_edges->get_sprite()->set_animation("short");
			elevator_edges->set_solid(false);
			elevator_edges->set_size({3, 3});
			elevator_edges->set_layer(2);
			area->add_entity(elevator_edges);
	
			wedge::Map_Entity *tok = new wedge::Map_Entity("tok");
			tok->start(area);
			tok->set_position(util::Point<int>(0, 10));
			tok->set_sprite(new gfx::Sprite("tok"));
			tok->set_solid(false);
			tok->set_layer(3);
			tok->set_shadow(true, util::Size<int>(12, 6));
			tok->set_auto_shadow_pos(true);
			area->add_entity(tok);

			tok->set_offset({0.0f, 3.0f});
			elevator->set_offset({0.0f, 3.0f});
			elevator_edges->set_offset({0.0f, 3.0f});
			
			wedge::Map_Entity *jellybean = new wedge::Map_Entity("jellybean");
			jellybean->start(area);
			jellybean->set_position(util::Point<int>(22, 6));
			jellybean->set_sprite(new gfx::Sprite("jellybean"));
			jellybean->get_sprite()->set_animation("stand_w");
			jellybean->set_layer(3);
			area->add_entity(jellybean);
		}
		else {
			set_hole_solid();
		}

		return true;
	}

	void started()
	{
		if (INSTANCE->is_milestone_complete(MS_CAVE3_SCENE) == false) {
			wedge::pause_presses(true);

			auto pleasant = AREA->get_player(0);
			auto tom = AREA->get_player(1);
			auto tik = AREA->get_player(2);
			auto tok = area->find_entity("tok");

			pleasant->set_solid(false);
			pleasant->set_position({4, 6});
			pleasant->set_offset({0.0f, 3.0f});
			tom->set_offset({0.0f, 3.0f});
			tik->set_offset({0.0f, 3.0f});

			tom->set_solid(false);
			tik->set_solid(false);
			tok->set_solid(false);

			INSTANCE->set_milestone_complete(MS_CAVE3_SCENE, true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_1_inc_angle_and_move, this, new_task))
				ADD_STEP(new wedge::Delay_Step(125, new_task))
			for (int i = 0; i < 6-1; i++) {
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_1_inc_angle, this, new_task))
				ADD_STEP(new wedge::Delay_Step(125, new_task))
			}
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(end_elevator_sample, nullptr, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(pop_tik, nullptr, new_task))
			auto gics = new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task);
			ADD_STEP(gics);
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_set_layer_6, nullptr, new_task))
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(4, 8), new_task))
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(5, 8), new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_pleasant_tom_face_n, nullptr, new_task))
			std::vector<wedge::Map_Entity *> entities;
			entities.push_back(pleasant);
			entities.push_back(tom);
			entities.push_back(tik);
			entities.push_back(tok);
			std::vector<util::Point<int>> positions;
			positions.push_back({5, 8});
			positions.push_back({4, 8});
			positions.push_back({5, 4});
			positions.push_back({6, 4});
			ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(429)/* Originally: Oof. I hope we don't have to come back this way. I might be sick! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->elevator_start, true, false, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->elevator_raising, false, true, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_1_dec_angle_and_move, this, new_task))
			ADD_STEP(new wedge::Delay_Step(125, new_task))
			for (int i = 0; i < 6-1; i++) {
				ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_1_dec_angle, this, new_task))
				ADD_STEP(new wedge::Delay_Step(125, new_task))
			}
			ADD_STEP(new wedge::Delay_Step(3000, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(end_elevator_sample, nullptr, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_elevator, this, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_set_hole_solid, this, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(430)/* Originally: Looks like now we can't. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(cave3_set_solid, this, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(432)/* Originally: Would you look at that!!! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(tok, util::Point<int>(12, 4), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(433)/* Originally: They're beautiful! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(8, 4), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(434)/* Originally: Uh, guys. I'm going to stay here with Tok. He's going to need help digging out these gems.^Good luck finding your dog! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(11, 4), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(435)/* Originally: Well, we should get looking... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(436)/* Originally: *nods* */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_STEP(new Autosave_Step(new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *ws2 = new wedge::Wait_Step(new_task);
			gics->add_monitor(ws2);
			ADD_STEP(ws2)
			auto ass = new wedge::A_Star_Step(tok, util::Point<int>(5, 5), new_task);
			ADD_STEP(ass)
			ADD_STEP(new wedge::A_Star_Step(tok, util::Point<int>(6, 5), new_task))
			ADD_STEP(new wedge::A_Star_Step(tok, util::Point<int>(6, 4), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_S, true, false, new_task))
			ADD_TASK(new_task);

			ANOTHER_TASK
			wedge::Wait_Step *ws1 = new wedge::Wait_Step(new_task);
			gics->add_monitor(ws1);
			ADD_STEP(ws1)
			wedge::Wait_Step *ws3 = new wedge::Wait_Step(new_task);
			ass->add_monitor(ws3);
			ADD_STEP(ws3)
			ADD_STEP(new wedge::Delay_Step(1, new_task))
			ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(5, 4), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
			ADD_TASK(new_task);
			
			FINISH_SYSTEM(AREA)
		}
		
		Area_Hooks::started();
	}

	void end()
	{
		auto s = dynamic_cast<audio::Sample *>(DOGO_GLOBALS->elevator_raising);

		if (s && s->is_playing()) {
			s->stop();
		}
	}

	bool has_battles()
	{
		return false;
	}
	
	void inc_angle()
	{
		angle++;
		angle %= 4;
			
		auto pleasant = AREA->get_player(0);
		auto tom = AREA->get_player(1);
		auto tik = area->find_entity("Tik");
		auto tok = area->find_entity("tok");
		auto elevator = area->find_entity("elevator");

		auto pleasant_s = pleasant->get_sprite();
		auto tom_s = tom->get_sprite();
		auto tik_s = tik->get_sprite();
		auto tok_s = tok->get_sprite();

		int a;

		if (angle == 0) {
			a = 0;
		}
		else if (angle == 1) {
			a = 3;
		}
		else if (angle == 2) {
			a = 2;
		}
		else {
			a = 1;
		}

		std::string frame = util::itos(a+1);

		elevator->get_sprite()->set_animation(frame);

		if (a == 0) {
			pleasant_s->set_animation("stand_s");
			tom_s->set_animation("stand_s");
			tik_s->set_animation("stand_s");
			tok_s->set_animation("stand_s");

			tom->set_position({3, 6});
			tik->set_position({5, 6});
			tok->set_position({4, 5});
		}
		else if (a == 1) {
			pleasant_s->set_animation("stand_w");
			tom_s->set_animation("stand_w");
			tik_s->set_animation("stand_w");
			tok_s->set_animation("stand_w");

			tom->set_position({4, 5});
			tik->set_position({4, 7});
			tok->set_position({5, 6});
		}
		else if (a == 2) {
			pleasant_s->set_animation("stand_n");
			tom_s->set_animation("stand_n");
			tik_s->set_animation("stand_n");
			tok_s->set_animation("stand_n");

			tom->set_position({5, 6});
			tik->set_position({3, 6});
			tok->set_position({4, 7});
		}
		else {
			pleasant_s->set_animation("stand_e");
			tom_s->set_animation("stand_e");
			tik_s->set_animation("stand_e");
			tok_s->set_animation("stand_e");

			tom->set_position({4, 7});
			tik->set_position({4, 5});
			tok->set_position({3, 6});
		}
	}

	void dec_angle()
	{
		angle++;
		angle %= 4;
			
		auto elevator = area->find_entity("elevator");

		std::string frame = util::itos(angle+1);

		elevator->get_sprite()->set_animation(frame);
	}

	void set_hole_solid()
	{
		AREA->get_player(0)->set_layer(3);
		AREA->get_player(1)->set_layer(3);

		auto tilemap = area->get_tilemap();

		tilemap->set_solid(0, {3, 5}, {3, 3}, true);

		tilemap->swap_tiles(1, 5, {3, 7}, {3, 3});
	}

	bool activate(wedge::Map_Entity *activator, wedge::Map_Entity *activated)
	{
		if (activated->get_name() == "tok") {
			std::vector<wedge::Object> items;
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 30));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 50));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 50));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_TURNIP, 75));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_HOT_PEPPER, 125));

			NEW_SYSTEM_AND_TASK(AREA)
			Barter_Step *barter_step = new Barter_Step(items, new_task);
			ADD_STEP(new wedge::Pause_Presses_Step(true, false, new_task))
			ADD_STEP(barter_step)
			ADD_STEP(new wedge::Pause_Presses_Step(false, false, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			std::vector<std::string> choices;
			choices.push_back(GLOBALS->game_t->translate(293)/* Originally: Buy */);
			choices.push_back(GLOBALS->game_t->translate(294)/* Originally: Sell */);
			choices.push_back(GLOBALS->game_t->translate(295)/* Originally: Nevermind */);
			do_question(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(296)/* Originally: Fancy any of the merch? Or perhaps you want to lighten your load? */, wedge::DIALOGUE_SPEECH, choices, barter_step, 2);
		}
		else if (activated->get_name() == "Tik") {
			GLOBALS->do_dialogue(GLOBALS->game_t->translate(289)/* Originally: Tik */ + TAG_END, GLOBALS->game_t->translate(437)/* Originally: Finally, Tok's going to be RICH! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
		}
		return false;
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (INSTANCE->is_milestone_complete(MS_CAVE3_JB) == false && entity->get_position().x == 16) {
			INSTANCE->set_milestone_complete(MS_CAVE3_JB, true);
			
			wedge::pause_presses(true);

			auto pleasant = AREA->get_player(0);
			//auto tom = AREA->get_player(1);
			auto jb = area->find_entity("jellybean");

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::A_Star_Step(jb, util::Point<int>(24, 6), new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_jb, nullptr, new_task))
			ADD_STEP(new wedge::Delay_Step(250, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(pleasant_tom_face_each_other, nullptr, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(438)/* Originally: Did you see that? I think I saw JB! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(439)/* Originally: Are you sure? I didn't see anything. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(440)/* Originally: I don't... these caves are getting in my head.^No, I'm sure I saw him. This way! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

private:
	int angle;
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

void cave3_1_inc_angle(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave3_1 *>(data);
	hooks->inc_angle();
}

void cave3_1_inc_angle_and_move(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave3_1 *>(data);
	cave3_1_inc_angle(hooks); // do this here too

	auto area = AREA->get_current_area();

	auto pleasant = AREA->get_player(0);
	auto tom = AREA->get_player(1);
	auto tik = area->find_entity("Tik");
	auto tok = area->find_entity("tok");
	auto elevator = area->find_entity("elevator");
	auto elevator_edges = area->find_entity("elevator_edges");

	NEW_SYSTEM_AND_TASK(AREA)

	ADD_STEP(new wedge::Slide_Entity_Offset_Step(pleasant, {0.0f, 0.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(tom, {0.0f, 0.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(tik, {0.0f, 0.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(tok, {0.0f, 0.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(elevator, {0.0f, 0.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(elevator_edges, {0.0f, 0.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	FINISH_SYSTEM(AREA)
}

void cave3_1_dec_angle(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave3_1 *>(data);
	hooks->dec_angle();
}

void cave3_1_dec_angle_and_move(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave3_1 *>(data);
	cave3_1_dec_angle(hooks); // do this here too

	auto area = AREA->get_current_area();

	auto elevator = area->find_entity("elevator");
	auto elevator_edges = area->find_entity("elevator_edges");

	NEW_SYSTEM_AND_TASK(AREA)

	ADD_STEP(new wedge::Slide_Entity_Offset_Step(elevator, {0.0f, 3.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Slide_Entity_Offset_Step(elevator_edges, {0.0f, 3.0f}, 0.0667f, new_task))
	ADD_TASK(new_task)

	FINISH_SYSTEM(AREA)
}

void cave3_set_hole_solid(void *data)
{
	auto hooks = static_cast<Area_Hooks_Cave3_1 *>(data);
	hooks->set_hole_solid();
}

static void wit_join(void *data)
{
	wedge::Area *area = AREA->get_current_area();

	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);
	wedge::Map_Entity *wit = area->find_entity("Wit");

	INSTANCE->stats.push_back(wedge::Player_Stats());
	INSTANCE->stats[2].sprite = new gfx::Sprite("wit");
	INSTANCE->stats[2].level = 1;
	INSTANCE->stats[2].experience = 0;
	INSTANCE->stats[2].base.fixed.max_hp = 150;
	INSTANCE->stats[2].base.fixed.max_mp = 100;
	INSTANCE->stats[2].base.fixed.attack = 50;
	INSTANCE->stats[2].base.fixed.defence = 25;
	INSTANCE->stats[2].base.fixed.set_extra(LUCK, 30);
	INSTANCE->stats[2].base.hp = INSTANCE->stats[2].base.fixed.max_hp;
	INSTANCE->stats[2].base.mp = INSTANCE->stats[2].base.fixed.max_mp;
	INSTANCE->stats[2].base.set_name("Wit");
	
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_WHITE, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_YELLOW, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_GOLD, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_ORANGE, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_GREEN, 1));
	INSTANCE->inventory.add(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_BLUE, 1));
	
	INSTANCE->stats[2].armour = wedge::Armour_Stats(BELT_BLUE);
	
	std::vector<std::string> v;
	v.push_back("Karate Chop");
	v.push_back("Thunderclap");
	v.push_back("Cartwheel");
	v.push_back("Beatdown");
	v.push_back("Windmill");
	v.push_back("The Claw");
	v.push_back("Heal");
	DOGO_INSTANCE->combos.push_back(v);

	AREA->set_player(WIT, wit);
	
	AREA->get_current_area()->order_player_input_steps();

	pleasant->set_solid(true);
	tom->set_solid(true);
	wit->set_solid(true);
}

static void add_wit(void *data)
{
	wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");
	wedge::Map_Entity *wit = AREA->get_current_area()->find_entity("Wit");
	
	wit->set_shadow(true, util::Size<int>(12, 6));
	wit->set_sub_offset_from_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_e", new_task))
	ADD_STEP(new wedge::Set_Animation_Step(wit, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(wit, util::Point<float>(0.0f, -6.0f), util::Point<float>(0.0f, 0.0f), 3.0f, SENSEI_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(wit, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(SENSEI_DELAY, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(wit, "stand_w", new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(461)/* Originally: Aha! I caught you! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->laugh, false, false, new_task))
	ADD_STEP(new wedge::Play_Animation_Step(sensei->get_sprite(), "laugh", new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_e", new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(462)/* Originally: Caught me? Haha. I've been watching you out of the corner of my eye this whole time. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(463)/* Originally: Explain yourself mister! Who are you? Why are you here? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(464)/* Originally: Perhaps you should figure out why you are here before asking me that question. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(465)/* Originally: Wha... well I, I'm Wit and I'm a ninja! And you're being very suspicious! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(466)/* Originally: Ninja? I don't recall ever seeing you before. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(467)/* Originally: Well it's not official... yet... but wait, this isn't about me! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(468)/* Originally: I have to leave now friends. Perhaps this young lady can help you two on your quest. Adios! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(250, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "jump", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->sensei_jump, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(sensei, util::Point<float>(0.0f, 0.0f), util::Point<float>(0.0f, -6.0f), 0.0f, 500, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(delete_sensei, nullptr, new_task))
	auto pms = new wedge::Play_Music_Step("music/cave3.mml", new_task);
	ADD_STEP(pms)
	ADD_STEP(new wedge::A_Star_Step(wit, util::Point<int>(15, 8), new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(469)/* Originally: So you're a ninja eh? Have you seen a little white dog around?^He's lost and we need to find him. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(470)/* Originally: Actually, yes, he went up that way not long ago.^I can help you track him down... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::A_Star_Step(wit, util::Point<int>(16, 8), new_task))
	ADD_STEP(new wedge::Delay_Step(500, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(478)/* Originally: Aha! Look! These are his footprints! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::A_Star_Step(wit, util::Point<int>(15, 8), new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(479)/* Originally: Actually yes, he went up that way not long ago.^I can help you track him down. Aha! Look! These are his footprints!^Oh, wait... those are mine... nevermind. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(471)/* Originally: Anyhow, we could use an extra pair of eyes... and fists, if you're up for it. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(472)/* Originally: Sure. But to be honest... I've never trained as a ninja. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(473)/* Originally: That's OK, we don't know what we're doing either. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(476)/* Originally: Wit */ + TAG_END, GLOBALS->game_t->translate(474)/* Originally: *laughs* */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	auto ds = new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(475)/* Originally: OK, Jellybean must be up this way. Let's go. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task);
	ADD_STEP(ds)
	ADD_STEP(new wedge::A_Star_Step(wit, util::Point<int>(12, 9), new_task))
	ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_N, true, false, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(wit_join, AREA->get_current_area(), new_task))
	ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
	ADD_STEP(new Autosave_Step(new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void add_sensei_cave3(void *data)
{
	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);
	wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");

	pleasant->set_solid(false);
	tom->set_solid(false);
	
	sensei->set_shadow(true, util::Size<int>(12, 6));
	sensei->set_sub_offset_from_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(sensei, util::Point<float>(0.0f, -6.0f), util::Point<float>(0.0f, 0.0f), 3.0f, SENSEI_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Play_Music_Step("music/sensei.mml", new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(SENSEI_DELAY, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "stand_w", new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(454)/* Originally: I see you've acquired the Blue Belts! Those are special, you know... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(455)/* Originally: Special? How so? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(456)/* Originally: Some moves can be targeted at more than one enemy at a time. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(457)/* Originally: And how would we do that? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(458)/* Originally: Simple. Just make a mental checkmark beside each enemy you want to attack...^Then go for it! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(459)/* Originally: Sounds easy enough. But why would we want to? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(460)/* Originally: While the damage will be spread out when you target more than one enemy...^The total damage will be higher. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_wit, nullptr, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Cave3_2 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_2(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 20), util::Size<int>(1, 5));
		z.area_name = "cave3_1";
		z.player_positions.push_back(util::Point<int>(24, 6));
		z.player_positions.push_back(util::Point<int>(24, 6));
		z.player_positions.push_back(util::Point<int>(24, 6));
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		z.directions.push_back(wedge::DIR_W);
		fade_zones.push_back(z);

		next_drip = GET_TICKS() + 5000;

		Slope s;

		s.position = { 20, 8 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 21, 7 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 20, 6 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 19, 7 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);
		
		Scroll_Zone sz;
		sz.zone = util::Rectangle<int>(util::Point<int>(30, 6), util::Size<int>(1, 1));
		sz.area_name = "cave3_3";
		sz.topleft_dest = util::Point<int>(0, 18);
		sz.direction = wedge::DIR_E;
		scroll_zones.push_back(sz);
	}
	
	virtual ~Area_Hooks_Cave3_2()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
		}

		return true;
	}

	bool has_battles()
	{
		return false;
	}
	
	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (INSTANCE->is_milestone_complete(MS_MET_WIT) == false && entity->get_position().y == 10) {
			INSTANCE->set_milestone_complete(MS_MET_WIT, true);
			
			wedge::pause_presses(true);

			auto pleasant = AREA->get_player(0);
			auto tom = AREA->get_player(1);

			wedge::Map_Entity *sensei = new wedge::Map_Entity("sensei");
			sensei->start(area);
			sensei->set_position(util::Point<int>(15, 8));
			sensei->set_offset({0.0f, -6.0f});
			sensei->set_sprite(new gfx::Sprite("sensei"));
			sensei->set_solid(false);
			area->add_entity(sensei);

			wedge::Map_Entity *wit = new wedge::Map_Entity("Wit");
			wit->start(area);
			wit->set_position(util::Point<int>(17, 8));
			wit->set_offset({0.0f, -6.0f});
			wit->set_sprite(new gfx::Sprite("wit"));
			wit->set_solid(false);
			area->add_entity(wit);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(13, 8), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(453)/* Originally: We're getting close, I can feel it. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			auto gics = new wedge::Generic_Immediate_Callback_Step(add_sensei_cave3, nullptr, new_task);
			ADD_STEP(gics)
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_TASK(new_task)

			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}
	
	std::vector<util::A_Star::Way_Point> get_way_points(util::Point<int> from)
	{
		auto v = wedge::Area_Hooks::get_way_points(from);

		// Some just to make clicks easier
		util::A_Star::Way_Point wp;
		std::pair<util::Point<int>, util::Point<int>> pair;
		
		pair.first = {20, 8};
		pair.second = {20, 7};
		wp.click_swaps.push_back(pair);

		for (int i = 21; i <= 30; i++) {
			pair.first = {i, 6};
			pair.second = {i, 7};
			wp.click_swaps.push_back(pair);
		}

		v.push_back(wp);

		return v;
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

static void remove_dog_tag(void *data)
{
	wedge::Area *area = static_cast<wedge::Area *>(data);
	area->remove_entity(area->find_entity("dog_tag"), true);
	auto o = OBJECT->make_object(wedge::OBJECT_SPECIAL, SPECIAL_PET_TAG, 1);
	INSTANCE->inventory.add(o);
}

class Area_Hooks_Cave3_3 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_3(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false)
	{
		next_drip = GET_TICKS() + 5000;

		Slope s;

		s.position = { 19, 11 };
		s.direction = wedge::DIR_W;
		s.up = true;
		slopes.push_back(s);

		s.position = { 20, 10 };
		s.direction = wedge::DIR_E;
		s.up = false;
		slopes.push_back(s);

		s.position = { 38, 7 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 37, 6 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		std::vector< util::Rectangle<int> > water;
		util::Rectangle<int> r;
		r.pos.x = 28;
		r.pos.y = 7;
		r.size.w = 10;
		r.size.h = 5;
		water.push_back(r);
		r.pos.x = 20;
		r.pos.y = 11;
		r.size.w = 8;
		r.size.h = 5;
		water.push_back(r);
		r.pos.x = 28;
		r.pos.y = 12;
		r.size.w = 2;
		r.size.h = 4;
		water.push_back(r);
		area->set_water(water);

		std::vector< util::Point<int> > no_ripple_tiles;
		no_ripple_tiles.push_back({20, 11});
		no_ripple_tiles.push_back({37, 7});
		area->set_no_ripple_tiles(no_ripple_tiles);

		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(47, 6), util::Size<int>(1, 4));
		z.area_name = "cave3_4";
		z.topleft_dest = util::Point<int>(0, 7);
		z.direction = wedge::DIR_E;
		scroll_zones.push_back(z);

		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(0, 18), util::Size<int>(1, 1));
		z2.area_name = "cave3_2";
		z2.topleft_dest = util::Point<int>(30, 6);
		z2.direction = wedge::DIR_W;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave3_3()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
			wedge::Map_Entity *dog_tag = new wedge::Map_Entity("dog_tag");
			dog_tag->start(area);
			dog_tag->set_position(util::Point<int>(16, 18));
			dog_tag->set_sprite(new gfx::Sprite("dog_tag"));
			dog_tag->set_solid(false);
			area->add_entity(dog_tag);
		}

		if (area->find_entity("chest1") == nullptr) {
			wedge::Chest *chest1 = new wedge::Chest("chest1", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_HOT_PEPPER, 2));
			chest1->start(area);
			chest1->set_position(util::Point<int>(28, 7));
			chest1->set_sprite(new gfx::Sprite("chest"));
			chest1->set_extra_offset(util::Point<int>(0, -2));
			area->add_entity(chest1);
		}

		return true;
	}

	bool has_battles()
	{
		return true;
	}
	
	wedge::Battle_Game *get_random_battle()
	{
		std::vector< util::Rectangle<int> > v = area->get_water();
		bool is_water = false;
		util::Point<int> ppos = AREA->get_player(0)->get_position();

		for (auto &r : v) {
			if (r.contains(ppos)) {
				is_water = true;
				break;
			}
		}

		DOGO_GLOBALS->waterfall_sfx->stop();

		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(2);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (is_water) {
			if (type == 0) {
				return new Battle_4Snapper();
			}
			else {
				return new Battle_1Aura2Snapper();
			}
		}
		else {
			if (type == 0) {
				return new Battle_2Bone();
			}
			else {
				return new Battle_1Aura1Bone();
			}
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (INSTANCE->is_milestone_complete(MS_FOUND_PET_TAG) == false && entity->get_position().x == 14) {
			INSTANCE->set_milestone_complete(MS_FOUND_PET_TAG, true);
			
			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			
			wedge::pause_presses(true);
		
			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(15, 18), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->pickup, false, false, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_dog_tag, area, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(495)/* Originally: It's Jellybean's pet tag!^*sigh* Where are you JB? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			return true;
		}

		return false;
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

	std::vector<util::A_Star::Way_Point> get_way_points(util::Point<int> from)
	{
		auto v = wedge::Area_Hooks::get_way_points(from);

		// Some just to make clicks easier
		util::A_Star::Way_Point wp;
		std::pair<util::Point<int>, util::Point<int>> pair;
		
		for (int i = 0; i <= 7; i++) {
			pair.first = {i, 18};
			pair.second = {i, 19};
			wp.click_swaps.push_back(pair);
		}

		v.push_back(wp);

		return v;
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

class Area_Hooks_Cave3_4 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_4(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false),
		keep_playing_waterfall_sfx(false)
	{
		next_drip = GET_TICKS() + 5000;

		Slope s;

		s.position = { 7, 8 };
		s.direction = wedge::DIR_W;
		s.up = true;
		slopes.push_back(s);

		s.position = { 8, 7 };
		s.direction = wedge::DIR_E;
		s.up = false;
		slopes.push_back(s);

		std::vector< util::Rectangle<int> > water;
		util::Rectangle<int> r;
		r.pos.x = 8;
		r.pos.y = 8;
		r.size.w = 27;
		r.size.h = 2;
		water.push_back(r);
		r.pos.x = 13;
		r.pos.y = 6;
		r.size.w = 2;
		r.size.h = 2;
		water.push_back(r);
		r.pos.x = 22;
		r.pos.y = 6;
		r.size.w = 2;
		r.size.h = 2;
		water.push_back(r);
		area->set_water(water);

		std::vector< util::Point<int> > no_ripple_tiles;
		no_ripple_tiles.push_back({8, 8});
		area->set_no_ripple_tiles(no_ripple_tiles);

		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 7), util::Size<int>(1, 4));
		z.area_name = "cave3_3";
		z.topleft_dest = util::Point<int>(47, 6);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
			
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(34, 8), util::Size<int>(1, 2));
		z2.area_name = "cave3_5";
		z2.topleft_dest = util::Point<int>(0, 17);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);

		Fade_Zone z3;
		z3.zone = util::Rectangle<int>(util::Point<int>(22, 6), util::Size<int>(2, 1));
		z3.area_name = "cave3_falls";
		z3.player_positions.push_back(util::Point<int>(4, 10));
		z3.player_positions.push_back(util::Point<int>(4, 10));
		z3.player_positions.push_back(util::Point<int>(4, 10));
		z3.directions.push_back(wedge::DIR_N);
		z3.directions.push_back(wedge::DIR_N);
		z3.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(z3);
	}
	
	virtual ~Area_Hooks_Cave3_4()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
		}

		return true;
	}

	void started()
	{
		Area_Hooks::started();

		DOGO_GLOBALS->waterfall_sfx->set_master_volume(0.0f);
		DOGO_GLOBALS->waterfall_sfx->play(true);
	}

	void end()
	{
		if (keep_playing_waterfall_sfx == false) {
			DOGO_GLOBALS->waterfall_sfx->stop();
		}
	}

	void run()
	{
		auto pleasant = AREA->get_player(PLEASANT);

		auto ppos = pleasant->get_position();
		auto po = pleasant->get_offset();

		util::Point<float> waterfall1_pos(14*shim::tile_size, 7*shim::tile_size);
		util::Point<float> waterfall2_pos(23*shim::tile_size, 7*shim::tile_size);
		util::Point<float> p_abs;

		wedge::tile_to_abs(ppos, po, p_abs);

		p_abs += util::Point<float>(0.5f, 0.5f);

		float volume;
		float dist1 = (waterfall1_pos-p_abs).length();
		float dist2 = (waterfall2_pos-p_abs).length();

		float dist = MIN(dist1, dist2);

		float cutoff = 20.0f*shim::tile_size;

		if (dist > cutoff) {
			volume = 0.0f;
		}
		else {
			dist = cutoff - dist;
			volume = dist / cutoff;
		}
		
		DOGO_GLOBALS->waterfall_sfx->set_master_volume(volume);
	}

	bool has_battles()
	{
		return true;
	}
	
	wedge::Battle_Game *get_random_battle()
	{
		std::vector< util::Rectangle<int> > v = area->get_water();
		bool is_water = false;
		util::Point<int> ppos = AREA->get_player(0)->get_position();

		for (auto &r : v) {
			if (r.contains(ppos)) {
				is_water = true;
				break;
			}
		}

		DOGO_GLOBALS->waterfall_sfx->stop();

		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(2);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (is_water) {
			if (type == 0) {
				return new Battle_6Snapper();
			}
			else {
				return new Battle_2Aura("cave3water-", 0);
			}
		}
		else {
			if (type == 0) {
				return new Battle_3Bone();
			}
			else {
				return new Battle_2Aura("cave3-", 0);
			}
		}
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

	void pause(bool onoff) {
		if (onoff) {
			DOGO_GLOBALS->waterfall_sfx->stop();
		}
		else {
			DOGO_GLOBALS->waterfall_sfx->play(true);
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			std::string an = area->get_next_area_name();
			if (an == "cave3_falls" || an == "cave3_5") {
				keep_playing_waterfall_sfx = true;
			}
			return true;
		}

		return false;
	}

	void restart_waterfall_sfx()
	{
		DOGO_GLOBALS->waterfall_sfx->set_master_volume(0.0f);
		DOGO_GLOBALS->waterfall_sfx->play(true);
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
	bool keep_playing_waterfall_sfx;
};

class Area_Hooks_Cave3_Falls : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_Falls(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false),
		keep_playing_waterfall_sfx(false)
	{
		next_drip = GET_TICKS() + 5000;
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(4, 10), util::Size<int>(1, 1));
		z.area_name = "cave3_4";
		z.player_positions.push_back(util::Point<int>(22, 7));
		z.player_positions.push_back(util::Point<int>(22, 7));
		z.player_positions.push_back(util::Point<int>(22, 7));
		z.directions.push_back(wedge::DIR_S);
		z.directions.push_back(wedge::DIR_S);
		z.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(z);
	}
	
	virtual ~Area_Hooks_Cave3_Falls()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
		}

		if (area->find_entity("chest1") == nullptr) {
			wedge::Chest *chest1 = new wedge::Chest("chest1", "", OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_PURPLE, 3));
			chest1->start(area);
			chest1->set_position(util::Point<int>(4, 5));
			chest1->set_sprite(new gfx::Sprite("chest"));
			//chest1->set_achievement((void *)ACH_PURPLEBELT);
			area->add_entity(chest1);
		}

		return true;
	}

	void started()
	{
		Area_Hooks::started();

		DOGO_GLOBALS->waterfall_sfx->set_master_volume(0.0f);
		DOGO_GLOBALS->waterfall_sfx->play(true);
	}

	void end()
	{
		if (keep_playing_waterfall_sfx == false) {
			DOGO_GLOBALS->waterfall_sfx->stop();
		}
	}

	bool has_battles()
	{
		return true;
	}
	
	wedge::Battle_Game *get_random_battle()
	{
		DOGO_GLOBALS->waterfall_sfx->stop();

		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(2);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 0) {
			return new Battle_3Bone();
		}
		else {
			return new Battle_2Aura("cave3-", 0);
		}
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

	void pause(bool onoff) {
		if (onoff) {
			DOGO_GLOBALS->waterfall_sfx->stop();
		}
		else {
			DOGO_GLOBALS->waterfall_sfx->play(true);
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			if (area->get_next_area_name() == "cave3_4") {
				keep_playing_waterfall_sfx = true;
			}
			return true;
		}

		return false;
	}

	void restart_waterfall_sfx()
	{
		DOGO_GLOBALS->waterfall_sfx->set_master_volume(0.0f);
		DOGO_GLOBALS->waterfall_sfx->play(true);
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
	bool keep_playing_waterfall_sfx;
};

class Area_Hooks_Cave3_5 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_5(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false),
		keep_playing_waterfall_sfx(false)
	{
		next_drip = GET_TICKS() + 5000;

		Slope s;

		s.position = { 22, 6 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 23, 7 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		std::vector< util::Rectangle<int> > water;
		util::Rectangle<int> r;
		r.pos.x = 0;
		r.pos.y = 17;
		r.size.w = 15;
		r.size.h = 2;
		water.push_back(r);
		r.pos.x = 15;
		r.pos.y = 7;
		r.size.w = 3;
		r.size.h = 12;
		water.push_back(r);
		r.pos.x = 18;
		r.pos.y = 7;
		r.size.w = 5;
		r.size.h = 3;
		water.push_back(r);
		r.pos.x = 9;
		r.pos.y = 19;
		r.size.w = 2;
		r.size.h = 1;
		water.push_back(r);
		area->set_water(water);

		std::vector< util::Point<int> > no_ripple_tiles;
		no_ripple_tiles.push_back({22, 7});
		area->set_no_ripple_tiles(no_ripple_tiles);

		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 17), util::Size<int>(1, 2));
		z.area_name = "cave3_4";
		z.topleft_dest = util::Point<int>(34, 8);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(31, 6), util::Size<int>(1, 5));
		z2.area_name = "cave3_6";
		z2.topleft_dest = util::Point<int>(0, 15);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave3_5()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
		}

		return true;
	}

	void started()
	{
		Area_Hooks::started();

		DOGO_GLOBALS->waterfall_sfx->set_master_volume(0.0f);
		DOGO_GLOBALS->waterfall_sfx->play(true);
	}

	void end()
	{
		if (keep_playing_waterfall_sfx == false) {
			DOGO_GLOBALS->waterfall_sfx->stop();
		}
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			if (area->get_next_area_name() == "cave3_4") {
				keep_playing_waterfall_sfx = true;
			}
			return true;
		}

		return false;
	}

	void run()
	{
		auto pleasant = AREA->get_player(PLEASANT);

		auto ppos = pleasant->get_position();
		auto po = pleasant->get_offset();

		util::Point<float> waterfall1_pos(10*shim::tile_size, 21*shim::tile_size);
		util::Point<float> p_abs;

		wedge::tile_to_abs(ppos, po, p_abs);

		p_abs += util::Point<float>(0.5f, 0.5f);

		float volume;
		float dist = (waterfall1_pos-p_abs).length();

		float cutoff = 20.0f*shim::tile_size;

		if (dist > cutoff) {
			volume = 0.0f;
		}
		else {
			dist = cutoff - dist;
			volume = dist / cutoff;
		}
		
		DOGO_GLOBALS->waterfall_sfx->set_master_volume(volume);
	}

	bool has_battles()
	{
		return true;
	}
	
	wedge::Battle_Game *get_random_battle()
	{
		std::vector< util::Rectangle<int> > v = area->get_water();
		bool is_water = false;
		util::Point<int> ppos = AREA->get_player(0)->get_position();

		for (auto &r : v) {
			if (r.contains(ppos)) {
				is_water = true;
				break;
			}
		}

		DOGO_GLOBALS->waterfall_sfx->stop();

		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(3);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (is_water) {
			if (type == 0) {
				return new Battle_6Snapper();
			}
			else if (type == 1) {
				return new Battle_1Thirsty();
			}
			else {
				return new Battle_2Aura("cave3water-", 0);
			}
		}
		else {
			if (type == 0) {
				return new Battle_3Bone();
			}
			else if (type == 1) {
				return new Battle_1CutiePie1Bone();
			}
			else {
				return new Battle_2Aura("cave3-", 0);
			}
		}
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

	void pause(bool onoff) {
		if (onoff) {
			DOGO_GLOBALS->waterfall_sfx->stop();
		}
		else {
			DOGO_GLOBALS->waterfall_sfx->play(true);
		}
	}

	void restart_waterfall_sfx()
	{
		DOGO_GLOBALS->waterfall_sfx->set_master_volume(0.0f);
		DOGO_GLOBALS->waterfall_sfx->play(true);
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
	bool keep_playing_waterfall_sfx;
};

class Area_Hooks_Cave3_6 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_6(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false)
	{
		next_drip = GET_TICKS() + 5000;
		
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 15), util::Size<int>(1, 5));
		z.area_name = "cave3_5";
		z.topleft_dest = util::Point<int>(31, 6);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(31, 11), util::Size<int>(1, 1));
		z2.area_name = "cave3_7";
		z2.topleft_dest = util::Point<int>(0, 10);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave3_6()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
			wedge::Chest *chest1 = new wedge::Chest("chest1", "", OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_BROWN, 3));
			chest1->start(area);
			chest1->set_position(util::Point<int>(15, 16));
			chest1->set_sprite(new gfx::Sprite("chest"));
			area->add_entity(chest1);
			
			wedge::Chest *chest2 = new wedge::Chest("chest2", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_GREEN_POTION, 5));
			chest2->start(area);
			chest2->set_position(util::Point<int>(21, 13));
			chest2->set_sprite(new gfx::Sprite("chest"));
			area->add_entity(chest2);
			
			wedge::Chest *chest3 = new wedge::Chest("chest3", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_HOT_PEPPER, 2));
			chest3->start(area);
			chest3->set_position(util::Point<int>(18, 26));
			chest3->set_sprite(new gfx::Sprite("chest"));
			area->add_entity(chest3);
			
			wedge::Chest *chest4 = new wedge::Chest("chest4", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_RANDOMAXE, 3));
			chest4->start(area);
			chest4->set_position(util::Point<int>(19, 26));
			chest4->set_sprite(new gfx::Sprite("chest"));
			area->add_entity(chest4);
		}

		return true;
	}

	bool has_battles()
	{
		return true;
	}
	
	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(3);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 2) {
			return new Battle_1Clone2Bone();
		}
		else if (type == 1) {
			return new Battle_1CutiePie1Bone();
		}
		else {
			return new Battle_3Bone();
		}
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

class Area_Hooks_Cave3_7 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_7(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false)
	{
		next_drip = GET_TICKS() + 5000;
		
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 10), util::Size<int>(1, 1));
		z.area_name = "cave3_6";
		z.topleft_dest = util::Point<int>(31, 11);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
		
		Slope s;

		s.position = { 8, 10 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 9, 9 };
		s.direction = wedge::DIR_E;
		s.up = true;
		slopes.push_back(s);

		s.position = { 8, 8 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		s.position = { 7, 9 };
		s.direction = wedge::DIR_W;
		s.up = false;
		slopes.push_back(s);

		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(27, 6), util::Size<int>(1, 5));
		z2.area_name = "cave3_8";
		z2.topleft_dest = util::Point<int>(0, 6);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave3_7()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
			wedge::Chest *chest1 = new wedge::Chest("chest1", "", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_TURNIP, 3));
			chest1->start(area);
			chest1->set_position(util::Point<int>(20, 6));
			chest1->set_sprite(new gfx::Sprite("chest"));
			area->add_entity(chest1);
		}

		return true;
	}

	bool has_battles()
	{
		return true;
	}
	
	wedge::Battle_Game *get_random_battle()
	{
		if (rand_battle_table.size() == 0) {
			gen_rand_battle_table(3);
		}

		int type = rand_battle_table[rand_battle_table.size()-1];
		rand_battle_table.pop_back();

		if (type == 2) {
			return new Battle_1Clone2Bone();
		}
		else if (type == 1) {
			return new Battle_2CutiePie();
		}
		else {
			return new Battle_1CutiePie1Clone();
		}
	}

	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

	std::vector<util::A_Star::Way_Point> get_way_points(util::Point<int> from)
	{
		auto v = wedge::Area_Hooks::get_way_points(from);

		// Some just to make clicks easier
		util::A_Star::Way_Point wp;
		std::pair<util::Point<int>, util::Point<int>> pair;
		
		pair.first = {8, 8};
		pair.second = {8, 7};
		wp.click_swaps.push_back(pair);

		for (int i = 10; i <= 19; i++) {
			pair.first = {i, 6};
			pair.second = {i, 7};
			wp.click_swaps.push_back(pair);
		}

		v.push_back(wp);
	
		// Slopes were designed poorly so you could only use mouse/touch if player is on the exact y as the bottom of the slope
		// This is a hack that makes it work
		if (from.x <= 7 && from.y < 8) {
			util::A_Star::Way_Point wp2;
			for (int y = 6; y <= 8; y++) {
				for (int x = 8; x <= 19; x++) {
					wp2.to.push_back({x, y});
				}
			}
			wp2.by.push_back({7, 8});
			wp2.by_as_is.push_back(false);
			v.push_back(wp2);
		}

		return v;
	}
	
private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

class Area_Hooks_Cave3_8 : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_8(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false)
	{
		next_drip = GET_TICKS() + 5000;
		
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 5));
		z.area_name = "cave3_7";
		z.topleft_dest = util::Point<int>(27, 6);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
		
		Scroll_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(17, 6), util::Size<int>(1, 5));
		z2.area_name = "cave3_boss";
		z2.topleft_dest = util::Point<int>(0, 6);
		z2.direction = wedge::DIR_E;
		scroll_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave3_8()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
		}

		return true;
	}

	bool has_battles()
	{
		return false;
	}
	
	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

const int KIDDIE_FALL_TIME = 1100;

static void add_kiddies(void *data)
{
	wedge::Map_Entity *kiddie1 = AREA->get_current_area()->find_entity("kiddie1");
	wedge::Map_Entity *kiddie2 = AREA->get_current_area()->find_entity("kiddie2");
	wedge::Map_Entity *kiddie3 = AREA->get_current_area()->find_entity("kiddie3");
	
	kiddie1->set_shadow(true, util::Size<int>(20, 10));
	kiddie1->set_sub_offset_from_shadow_pos(true);
	kiddie1->set_auto_shadow_pos(true);

	kiddie2->set_shadow(true, util::Size<int>(20, 10));
	kiddie2->set_sub_offset_from_shadow_pos(true);
	kiddie2->set_auto_shadow_pos(true);

	kiddie3->set_shadow(true, util::Size<int>(20, 10));
	kiddie3->set_sub_offset_from_shadow_pos(true);
	kiddie3->set_auto_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(kiddie1, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(kiddie1, util::Point<float>(0.0f, -10.0f), util::Point<float>(0.0f, 0.0f), 3.0f, KIDDIE_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(kiddie1, "idle", new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Set_Animation_Step(kiddie2, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(kiddie2, util::Point<float>(0.0f, -10.0f), util::Point<float>(0.0f, 0.0f), 3.0f, KIDDIE_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(kiddie2, "idle", new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Set_Animation_Step(kiddie3, "fall", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(kiddie3, util::Point<float>(0.0f, -10.0f), util::Point<float>(0.0f, 0.0f), 3.0f, KIDDIE_FALL_TIME, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->land, false, false, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(kiddie3, "idle", new_task))
	ADD_TASK(new_task)

	FINISH_SYSTEM(AREA)
}

const int STALACTITE_FALL_TIME = 1100;

static void remove_stalactite1_shadow(void *data)
{
	wedge::Map_Entity *stalactite1 = AREA->get_current_area()->find_entity("stalactite1");
	stalactite1->set_shadow(false, util::Size<int>(0, 0));
}

static void add_stalactite1(void *data)
{
	wedge::Map_Entity *stalactite1 = AREA->get_current_area()->find_entity("stalactite1");
	
	stalactite1->set_shadow(true, util::Size<int>(20, 10));
	stalactite1->set_sub_offset_from_shadow_pos(true);
	stalactite1->set_auto_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(stalactite1, "full", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(stalactite1, util::Point<float>(0.0f, -10.0f), util::Point<float>(0.0f, 0.0f), 3.0f, STALACTITE_FALL_TIME, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_stalactite1_shadow, nullptr, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->crash, false, false, new_task))
	ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(stalactite1, "ground", new_task))
	ADD_STEP(new wedge::Set_Solid_Step(stalactite1, true, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void remove_stalactite2_shadow(void *data)
{
	wedge::Map_Entity *stalactite2 = AREA->get_current_area()->find_entity("stalactite2");
	stalactite2->set_shadow(false, util::Size<int>(0, 0));
}

static void add_stalactite2(void *data)
{
	wedge::Map_Entity *stalactite2 = AREA->get_current_area()->find_entity("stalactite2");
	
	stalactite2->set_shadow(true, util::Size<int>(20, 10));
	stalactite2->set_sub_offset_from_shadow_pos(true);
	stalactite2->set_auto_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(stalactite2, "full", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(stalactite2, util::Point<float>(0.0f, -10.0f), util::Point<float>(0.0f, 0.0f), 3.0f, STALACTITE_FALL_TIME, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_stalactite2_shadow, nullptr, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->crash, false, false, new_task))
	ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(stalactite2, "ground", new_task))
	ADD_STEP(new wedge::Set_Solid_Step(stalactite2, true, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void remove_stalactite3_shadow(void *data)
{
	wedge::Map_Entity *stalactite3 = AREA->get_current_area()->find_entity("stalactite3");
	stalactite3->set_shadow(false, util::Size<int>(0, 0));
}

static void add_stalactite3(void *data)
{
	wedge::Map_Entity *stalactite3 = AREA->get_current_area()->find_entity("stalactite3");
	
	stalactite3->set_shadow(true, util::Size<int>(20, 10));
	stalactite3->set_sub_offset_from_shadow_pos(true);
	stalactite3->set_auto_shadow_pos(true);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(stalactite3, "full", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->fall, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(stalactite3, util::Point<float>(0.0f, -10.0f), util::Point<float>(0.0f, 0.0f), 3.0f, STALACTITE_FALL_TIME, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(remove_stalactite3_shadow, nullptr, new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->crash, false, false, new_task))
	ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(stalactite3, "ground", new_task))
	ADD_STEP(new wedge::Set_Solid_Step(stalactite3, true, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_Cave3_Boss : public Area_Hooks
{
public:
	static const int DRIP_TIME = 1200;
	static const int DRIP_SPLASH = 250;
	static const int MIN_DELAY = 1000;
	static const int MAX_DELAY = 3000;

	Area_Hooks_Cave3_Boss(wedge::Area *area) :
		Area_Hooks(area),
		dripping(false)
	{
		next_drip = GET_TICKS() + 5000;
		
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(0, 6), util::Size<int>(1, 5));
		z.area_name = "cave3_8";
		z.topleft_dest = util::Point<int>(17, 6);
		z.direction = wedge::DIR_W;
		scroll_zones.push_back(z);
		
		Fade_Zone z2;
		z2.zone = util::Rectangle<int>(util::Point<int>(24, 4), util::Size<int>(2, 1));
		z2.area_name = "end";
		z2.player_positions.push_back(util::Point<int>(8, 30));
		z2.player_positions.push_back(util::Point<int>(8, 31));
		z2.player_positions.push_back(util::Point<int>(8, 31));
		z2.directions.push_back(wedge::DIR_N);
		z2.directions.push_back(wedge::DIR_N);
		z2.directions.push_back(wedge::DIR_N);
		fade_zones.push_back(z2);
	}
	
	virtual ~Area_Hooks_Cave3_Boss()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/cave3.mml");

		if (loaded == false) {
			wedge::Map_Entity *bumper = new wedge::Map_Entity("bumper");
			bumper->start(area);
			bumper->set_position(util::Point<int>(14, 8));
			bumper->set_sprite(new gfx::Sprite("bumper"));
			bumper->set_direction(wedge::DIR_S, true, false);
			bumper->set_shadow(true, util::Size<int>(36, 18));
			bumper->set_draw_flags(gfx::Image::FLIP_H);
			bumper->set_auto_shadow_pos(true);
			area->add_entity(bumper);
			
			wedge::Map_Entity *stalactite1 = new wedge::Map_Entity("stalactite1");
			stalactite1->start(area);
			stalactite1->set_position(util::Point<int>(7, 8));
			stalactite1->set_sprite(new gfx::Sprite("stalactite"));
			stalactite1->set_offset({0.0f, -10.0f});
			stalactite1->set_solid(false);
			stalactite1->set_layer(4);
			stalactite1->set_size(util::Size<int>(2, 1));
			area->add_entity(stalactite1);
			
			wedge::Map_Entity *stalactite2 = new wedge::Map_Entity("stalactite2");
			stalactite2->start(area);
			stalactite2->set_position(util::Point<int>(7, 9));
			stalactite2->set_sprite(new gfx::Sprite("stalactite"));
			stalactite2->set_offset({0.0f, -10.0f});
			stalactite2->set_solid(false);
			stalactite2->set_layer(4);
			stalactite2->set_size(util::Size<int>(2, 1));
			area->add_entity(stalactite2);
			
			wedge::Map_Entity *stalactite3 = new wedge::Map_Entity("stalactite3");
			stalactite3->start(area);
			stalactite3->set_position(util::Point<int>(7, 10));
			stalactite3->set_sprite(new gfx::Sprite("stalactite"));
			stalactite3->set_offset({0.0f, -10.0f});
			stalactite3->set_solid(false);
			stalactite3->set_layer(4);
			stalactite3->set_size(util::Size<int>(2, 1));
			area->add_entity(stalactite3);
			
			wedge::Map_Entity *kiddie1 = new wedge::Map_Entity("kiddie1");
			kiddie1->start(area);
			kiddie1->set_position(util::Point<int>(12, 7));
			kiddie1->set_sprite(new gfx::Sprite("kiddie"));
			kiddie1->set_offset({0.0f, -10.0f});
			kiddie1->set_solid(false);
			kiddie1->set_draw_flags(gfx::Image::FLIP_H);
			area->add_entity(kiddie1);
			
			wedge::Map_Entity *kiddie2 = new wedge::Map_Entity("kiddie2");
			kiddie2->start(area);
			kiddie2->set_position(util::Point<int>(12, 9));
			kiddie2->set_sprite(new gfx::Sprite("kiddie"));
			kiddie2->set_offset({0.0f, -10.0f});
			kiddie2->set_solid(false);
			kiddie2->set_draw_flags(gfx::Image::FLIP_H);
			area->add_entity(kiddie2);
			
			wedge::Map_Entity *kiddie3 = new wedge::Map_Entity("kiddie3");
			kiddie3->start(area);
			kiddie3->set_position(util::Point<int>(14, 10));
			kiddie3->set_sprite(new gfx::Sprite("kiddie"));
			kiddie3->set_offset({0.0f, -10.0f});
			kiddie3->set_solid(false);
			kiddie3->set_draw_flags(gfx::Image::FLIP_H);
			kiddie3->set_layer(4);
			area->add_entity(kiddie3);
		}

		if (INSTANCE->is_milestone_complete(MS_BUMPER)) {
			wedge::Map_Entity *b = area->find_entity("bumper");
			b->get_sprite()->set_animation("sit_s");
			// FIXME: stalactites
		}

		return true;
	}

	bool has_battles()
	{
		return false;
	}
	
	std::vector<int> get_pre_draw_layers()
	{
		std::vector<int> v;
		v.push_back(0);
		return v;
	}
	
	void pre_draw(int layer, util::Point<float> map_offset)
	{
		if (layer == 0) {
			util::Point<int> p(0, 20);
			auto p2 = area->get_tilemap()->get_animated_tile(p);
			if (p2 == p) {
				area->set_water_height(4.0f);
			}
			else {
				area->set_water_height(3.0f);
			}
		}
	}

	std::vector<int> get_post_draw_layers()
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		std::vector<int> v;
		v.push_back(layers-1);
		return v;
	}

	void post_draw(int layer, util::Point<float> map_offset)
	{
		auto tilemap = area->get_tilemap();
		int layers = tilemap->get_num_layers();
		auto sz = tilemap->get_size();
		if (layer == layers-1) {
			Uint32 t = GET_TICKS();
			if (dripping == false && next_drip <= t) {
				std::vector<wedge::Map_Entity *> ignore;
				std::vector< util::Rectangle<int> > rects = area->get_entity_solids(ignore, true);

				util::Point<int> ppos = AREA->get_player(0)->get_position();

				util::Point<int> pos;
				bool solid;

				bool good = false;

				for (int i = 0; i < 100; i++) {
					int x = util::rand(0, 32);
					int y = util::rand(0, 18);

					x = (x - 16) + ppos.x;
					y = (y - 9) + ppos.y;

					if (x < 0 || y < 0 || x >= sz.w || y >= sz.h) {
						continue;
					}
				
					int l = layers-1;
					for (; l >= 0; l--) {
						bool res = tilemap->get_tile(l, {x, y}, pos, solid);
						if (res && pos.x >= 0) {
							break;
						}
					}
					if (l >= 0) {
						// already have pos
						if (pos.x >= 16 && pos.x <= 17 && pos.y >= 26 && pos.y <= 28) {
							bool has_entity = false;
							for (auto &r : rects) {
								if (r.contains({x, y})) {
									has_entity = true;
									break;
								}
							}
							if (has_entity == false) {
								good = true;
								drip_pos = util::Point<int>(x, y);
								break;
							}
						}
					}
				}

				if (good) {
					dripping = true;

				}
			}
			else if (dripping == true && next_drip+DRIP_TIME+DRIP_SPLASH < t) {
				dripping = false;
				int dec = ((sz.w*sz.h)/200.0f) * ((MAX_DELAY-MIN_DELAY)/5);
				int max = MAX(MIN_DELAY, MAX_DELAY - dec);
				next_drip = t + util::rand(MIN_DELAY, max);
			}

			if (dripping) {
				int x = drip_pos.x * shim::tile_size + shim::tile_size/2 + map_offset.x;
				int y = drip_pos.y * shim::tile_size + shim::tile_size/2 + map_offset.y;

				int e = t - next_drip;

				SDL_Colour colour = shim::palette[28]/*blue*/;

				if (e >= DRIP_TIME) {
					float xx = x;
					float yy = y;

					float p = (e-DRIP_TIME) / (float)DRIP_SPLASH;

					float a1 = M_PI + p * M_PI;
					float a2 = M_PI*2 - p * M_PI;

					const float r = 2.5f;

					util::Point<float> a(xx+r, yy);
					util::Point<float> b(xx-r, yy);
					util::Point<float> c(xx, yy+r);
					util::Point<float> d(xx, yy-r);

					a.x += cos(a1) * r;
					a.y += sin(a1) * r;

					b.x += cos(a2) * r;
					b.y += sin(a2) * r;

					c.y += cos(a1) * r;
					c.y += sin(a1) * r;

					d.y += cos(a2) * r;
					d.y += sin(a2) * r;

					p = 1.0f - (p*p); // slow fade

					SDL_Colour col = colour;
					col.r *= p;
					col.g *= p;
					col.b *= p;
					col.a *= p;

					gfx::draw_primitives_start();
					gfx::draw_filled_rectangle(col, {a.x, a.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {b.x, b.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {c.x, c.y}, {1.0f, 1.0f});
					gfx::draw_filled_rectangle(col, {d.x, d.y}, {1.0f, 1.0f});
					gfx::draw_primitives_end();
				}
				else {
					float p = e / (float)DRIP_TIME;

					y = y - (shim::screen_size.h*2) + p * (shim::screen_size.h*2);

					gfx::draw_filled_rectangle(colour, util::Point<float>{(float)x, y-3.0f}, {1.0f, 4.0f});
				}
			}
		}
	}

	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(0, 20);
		water_anim.size = util::Size<int>(13, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 21));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 32);
		water_anim.size = util::Size<int>(4, 2);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 34));
		water_anim.frames.push_back(util::Point<int>(0, 36));
		water_anim.frames.push_back(util::Point<int>(0, 38));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 32);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 33));
		water_anim.frames.push_back(util::Point<int>(4, 34));
		water_anim.frames.push_back(util::Point<int>(4, 35));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 36);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 100;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 37));
		water_anim.frames.push_back(util::Point<int>(4, 38));
		water_anim.frames.push_back(util::Point<int>(4, 39));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(4, 22);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(4, 23));
		tilemap->add_animation_data(water_anim);

		water_anim.topleft = util::Point<int>(0, 25);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(0, 26));
		tilemap->add_animation_data(water_anim);
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && INSTANCE->is_milestone_complete(MS_BUMPER) == false && entity->get_position().x == 9) {
			wedge::pause_presses(true);
			
			set_following((void *)0);

			INSTANCE->set_milestone_complete(MS_BUMPER, true);

			wedge::Map_Entity *pleasant = AREA->get_player(0);
			wedge::Map_Entity *tom = AREA->get_player(1);
			wedge::Map_Entity *wit = AREA->get_player(2);
			wedge::Map_Entity *bumper = area->find_entity("bumper");
			
			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(10, 9), new_task))
			std::vector<wedge::Map_Entity *> entities;
			entities.push_back(pleasant);
			entities.push_back(tom);
			entities.push_back(wit);
			std::vector<util::Point<int>> positions;
			positions.push_back({10, 9});
			positions.push_back({10, 8});
			positions.push_back({10, 7});
			ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(pleasant, "stand_e", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(tom, "stand_e", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(wit, "stand_e", new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->hoohoo, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bumper, "mouth_open", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(546)/* Originally: Bumper */ + TAG_END, GLOBALS->game_t->translate(539)/* Originally: Who's that? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bumper, "idle", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(540)/* Originally: No need to worry, we're just looking for our dog. Have you seen him? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->bumper_growl, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bumper, "scream", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(546)/* Originally: Bumper */ + TAG_END, GLOBALS->game_t->translate(541)/* Originally: You want to take Momma's babies! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bumper, "idle", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(542)/* Originally: Not at all! We just want our dog! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->bumper_growl, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bumper, "scream", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(546)/* Originally: Bumper */ + TAG_END, GLOBALS->game_t->translate(543)/* Originally: CHILDRENNN! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_kiddies, nullptr, new_task))
			ADD_STEP(new wedge::Delay_Step(1200, new_task))
			wedge::Step *ps1 = new wedge::Play_Sound_Step(DOGO_GLOBALS->bumper_growl, false, false, new_task);
			ADD_STEP(ps1);
			wedge::Step *attack1 = new wedge::Play_Animation_Step(bumper->get_sprite(), "attack", new_task);
			ADD_STEP(attack1);
			wedge::Step *attack2 = new wedge::Play_Animation_Step(bumper->get_sprite(), "attack", new_task);
			ADD_STEP(attack2);
			ADD_STEP(new wedge::Play_Animation_Step(bumper->get_sprite(), "attack", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bumper, "idle", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(546)/* Originally: Bumper */ + TAG_END, GLOBALS->game_t->translate(544)/* Originally: MAMA WON'T LET THEM HURT YOU!!! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Start_Battle_Step(new Battle_Bumper(), new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(10, 8), new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(wit, util::Point<int>(10, 7), new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step1 = new wedge::Wait_Step(new_task);
			ps1->add_monitor(wait_step1);
			ADD_STEP(wait_step1)
			ADD_STEP(new wedge::Delay_Step(200, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->loud_thud, false, false, new_task))
			ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
			ADD_STEP(new wedge::Delay_Step(100, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_stalactite1, nullptr, new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step2 = new wedge::Wait_Step(new_task);
			attack1->add_monitor(wait_step2);
			ADD_STEP(wait_step2)
			ADD_STEP(new wedge::Delay_Step(200, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->loud_thud, false, false, new_task))
			ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
			ADD_STEP(new wedge::Delay_Step(100, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_stalactite2, nullptr, new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			wedge::Wait_Step *wait_step3 = new wedge::Wait_Step(new_task);
			attack2->add_monitor(wait_step3);
			ADD_STEP(wait_step3)
			ADD_STEP(new wedge::Delay_Step(200, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->loud_thud, false, false, new_task))
			ADD_STEP(new wedge::Rumble_Step(1.0f, 500, new_task))
			ADD_STEP(new wedge::Delay_Step(100, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_stalactite3, nullptr, new_task))
			ADD_TASK(new_task)

			FINISH_SYSTEM(AREA)
#if 0
			util::Point<int> pos = player->get_position();
			util::Point<float> offset = player->get_offset();
			util::Size<int> player_size = player->get_size();
			util::Point<float> sz(player_size.w / 2.0f, 1.0f - player_size.h / 2.0f);
			wedge::add_tiles(pos, offset, sz);
			util::Point<float> map_offset = area->get_centred_offset(pos, offset, true);

			util::Point<float> start_offset = map_offset;
			util::Point<float> end_offset = map_offset + util::Point<float>(-shim::tile_size*2.0f, 0.0f);

			draw_offset = end_offset;

			static_cast<Area_Game *>(AREA)->set_use_camera(true);

			wedge::Map_Entity *jellybean = area->find_entity("jellybean");

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new Pan_Camera_Step(start_offset, end_offset, 2000, new_task));
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shock, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(player, "stand_e_exclamation", new_task))
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->shake, false, false, new_task))
			ADD_STEP(new wedge::Set_Animation_Step(jellybean, "shake", new_task))
			ADD_STEP(new wedge::Set_Direction_Step(player, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(start_spray_cb, this, new_task))
			ADD_STEP(new wedge::Delay_Step(500, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(jellybean, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(81)/* Originally: JB! There you are! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(jellybean, util::Point<int>(22, 12), new_task))
			ADD_STEP(new wedge::Delete_Map_Entity_Step(jellybean, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(82)/* Originally: Wait! Come back Jellybean! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Pan_Camera_Step(end_offset, start_offset, 1000, new_task));
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(stop_using_camera, NULL, new_task))
			ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
#endif

			return true;
		}

		return false;
	}

private:
	bool dripping;
	Uint32 next_drip;
	util::Point<int> drip_pos;
};

void add_sensei_end(void *data)
{
	wedge::Map_Entity *sensei = new wedge::Map_Entity("sensei");
	sensei->start(AREA->get_current_area());
	sensei->set_position(util::Point<int>(9, 30));
	sensei->set_sprite(new gfx::Sprite("sensei"));
	sensei->set_solid(false);
	AREA->get_current_area()->add_entity(sensei);

	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);
	wedge::Map_Entity *wit = AREA->get_player(WIT);
	wedge::Map_Entity *tok = AREA->get_current_area()->find_entity("tok");
	wedge::Map_Entity *tik = AREA->get_current_area()->find_entity("Tik");
	
	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::A_Star_Step(sensei, util::Point<int>(9, 25), new_task))
	ADD_STEP(new wedge::Set_Direction_Step(sensei, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_N, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(551)/* Originally: Why hello there! You've come far!^The three of you earned my highest reward! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::Give_Object_Step(OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_BLACK, 3), wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::A_Star_Step(sensei, util::Point<int>(9, 27), new_task))
	ADD_STEP(new wedge::Set_Direction_Step(sensei, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(552)/* Originally: Young lady, if you ever want to train with the ninja, come and find me! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::A_Star_Step(sensei, util::Point<int>(9, 30), new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new wedge::Set_Solid_Step(tok, true, new_task))
	ADD_STEP(new wedge::Set_Solid_Step(tik, true, new_task))
	std::vector<wedge::Map_Entity *> entities;
	entities.push_back(sensei);
	std::vector<util::Point<int>> positions;
	positions.push_back({9, 30});
	ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
	ADD_STEP(new wedge::Delete_Map_Entity_Step(sensei, new_task))
	ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
	ADD_STEP(new Autosave_Step(new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

void add_tik_tok_end(void *data)
{
	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);
	wedge::Map_Entity *wit = AREA->get_player(WIT);

	wedge::Map_Entity *tok = new wedge::Map_Entity("tok");
	tok->start(AREA->get_current_area());
	tok->set_position(util::Point<int>(7, 30));
	tok->set_sprite(new gfx::Sprite("tok"));
	tok->set_solid(false);
	AREA->get_current_area()->add_entity(tok);
	
	wedge::NPC *tik = new wedge::NPC("Tik", "Tik", "tik", "tik");
	tik->start(AREA->get_current_area());
	tik->set_position(util::Point<int>(7, 30));
	tik->set_sprite(new gfx::Sprite("tik"));
	tik->set_solid(false);
	AREA->get_current_area()->add_entity(tik);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::A_Star_Step(tok, util::Point<int>(7, 25), new_task))
	std::vector<wedge::Map_Entity *> entities;
	entities.push_back(tok);
	entities.push_back(tik);
	std::vector<util::Point<int>> positions;
	positions.push_back({7, 25});
	positions.push_back({7, 26});
	ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tok, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tik, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(550)/* Originally: Hey guys! I've got a bounty of supplies if you need anything! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_sensei_end, AREA->get_current_area(), new_task))
	ADD_TASK(new_task)

	ANOTHER_TASK
	ADD_STEP(new wedge::Delay_Step(250, new_task))
	ADD_STEP(new wedge::A_Star_Step(tik, util::Point<int>(7, 26), new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

class Area_Hooks_End : public Area_Hooks
{
public:
	Area_Hooks_End(wedge::Area *area) :
		Area_Hooks(area)
	{
		Fade_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(7, 31), util::Size<int>(3, 1));
		z.area_name = "cave3_boss";
		z.player_positions.push_back(util::Point<int>(24, 5));
		z.player_positions.push_back(util::Point<int>(24, 5));
		z.player_positions.push_back(util::Point<int>(24, 5));
		z.directions.push_back(wedge::DIR_S);
		z.directions.push_back(wedge::DIR_S);
		z.directions.push_back(wedge::DIR_S);
		fade_zones.push_back(z);
	}
	
	virtual ~Area_Hooks_End()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/end.mml");

		if (loaded == false) {
			wedge::Map_Entity *bigtuna = new wedge::Map_Entity("bigtuna");
			bigtuna->start(area);
			bigtuna->set_position(util::Point<int>(8, 7));
			bigtuna->set_sprite(new gfx::Sprite("bigtuna"));
			bigtuna->set_solid(false);
			area->add_entity(bigtuna);
			
			wedge::Map_Entity *jellybean = new wedge::Map_Entity("jellybean");
			jellybean->start(area);
			jellybean->set_position(util::Point<int>(7, 7));
			jellybean->set_sprite(new gfx::Sprite("jellybean"));
			jellybean->set_solid(false);
			area->add_entity(jellybean);
		}

		return true;
	}

	void started()
	{
		if (INSTANCE->is_milestone_complete(MS_END1) == false) {
			INSTANCE->set_milestone_complete(MS_END1, true);

			wedge::pause_presses(true);

			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *tom = AREA->get_player(TOM);
			wedge::Map_Entity *wit = AREA->get_player(WIT);

			util::Point<int> ppos = pleasant->get_position();

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, ppos+util::Point<int>(0, -5), new_task))
			ADD_STEP(new wedge::Delay_Step(250, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(549)/* Originally: It's windy out here! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(add_tik_tok_end, area, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}

		Area_Hooks::started();
	}
	
	bool activate(wedge::Map_Entity *activator, wedge::Map_Entity *activated)
	{
		if (activated->get_name() == "tok") {
			std::vector<wedge::Object> items;
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_PEACH, 50));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 100));
			items.push_back(OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_EGGPLANT, 150));

			NEW_SYSTEM_AND_TASK(AREA)
			Barter_Step *barter_step = new Barter_Step(items, new_task);
			ADD_STEP(new wedge::Pause_Presses_Step(true, false, new_task))
			ADD_STEP(barter_step)
			ADD_STEP(new wedge::Pause_Presses_Step(false, false, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)

			std::vector<std::string> choices;
			choices.push_back(GLOBALS->game_t->translate(293)/* Originally: Buy */);
			choices.push_back(GLOBALS->game_t->translate(294)/* Originally: Sell */);
			choices.push_back(GLOBALS->game_t->translate(295)/* Originally: Nevermind */);
			do_question(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(296)/* Originally: Fancy any of the merch? Or perhaps you want to lighten your load? */, wedge::DIALOGUE_SPEECH, choices, barter_step, 2);
		}
		return false;
	}

	bool has_battles()
	{
		return false;
	}
	
	void set_animated_tiles()
	{
		gfx::Tilemap *tilemap = area->get_tilemap();
		gfx::Tilemap::Animation_Data water_anim;
		water_anim.topleft = util::Point<int>(23, 9);
		water_anim.size = util::Size<int>(9, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 10));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(18, 7);
		water_anim.size = util::Size<int>(8, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(18, 8));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 11);
		water_anim.size = util::Size<int>(3, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 2);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 5));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(8, 35);
		water_anim.size = util::Size<int>(1, 2);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(8, 37));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(12, 35);
		water_anim.size = util::Size<int>(1, 2);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(12, 37));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(32, 11);
		water_anim.size = util::Size<int>(2, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(32, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(23, 2);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(23, 5));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(27, 11);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(27, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(31, 11);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(31, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(34, 11);
		water_anim.size = util::Size<int>(1, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(34, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(15, 33);
		water_anim.size = util::Size<int>(2, 2);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(15, 35));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(35, 11);
		water_anim.size = util::Size<int>(4, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(35, 12));
		tilemap->add_animation_data(water_anim);
		
		water_anim.topleft = util::Point<int>(32, 8);
		water_anim.size = util::Size<int>(3, 1);
		water_anim.delay = 500;
		water_anim.frames.clear();
		water_anim.frames.push_back(util::Point<int>(32, 9));
		tilemap->add_animation_data(water_anim);
	}

	bool on_tile(wedge::Map_Entity *entity)
	{
		if (Area_Hooks::on_tile(entity)) {
			return true;
		}

		if (entity == AREA->get_player(PLEASANT) && INSTANCE->is_milestone_complete(MS_END2) == false && entity->get_position().y == 19) {
			wedge::pause_presses(true);
			
			set_following((void *)0);

			INSTANCE->set_milestone_complete(MS_END2, true);
			
			wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
			wedge::Map_Entity *tom = AREA->get_player(TOM);
			wedge::Map_Entity *wit = AREA->get_player(WIT);

			pleasant->set_solid(false);
			tom->set_solid(false);
			wit->set_solid(false);
			
			wedge::Map_Entity *bigtuna = area->find_entity("bigtuna");
			wedge::Map_Entity *jellybean = area->find_entity("jellybean");

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::A_Star_Step(pleasant, util::Point<int>(8, 10), new_task))
			std::vector<wedge::Map_Entity *> entities;
			entities.push_back(pleasant);
			entities.push_back(tom);
			entities.push_back(wit);
			std::vector<util::Point<int>> positions;
			positions.push_back({8, 10});
			positions.push_back({8, 11});
			positions.push_back({8, 12});
			ADD_STEP(new wedge::Check_Positions_Step(entities, positions, true, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(wit, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(562)/* Originally: Big Tuna */ + TAG_END, GLOBALS->game_t->translate(557)/* Originally: Ah, the three stooges. Come to rescue your pooch I suppose?^Fat chance. He's coming with me to Prosper: City of Gold. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(558)/* Originally: I don't think so! Come here Jellybean, let's go home! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			wedge::Step *ass = new wedge::A_Star_Step(jellybean, util::Point<int>(7, 9), new_task);
			ADD_STEP(ass)
			ADD_STEP(new wedge::Play_Animation_Step(bigtuna->get_sprite(), "bang_staff", new_task))
			ADD_STEP(new wedge::Set_Animation_Step(bigtuna, "idle", new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(562)/* Originally: Big Tuna */ + TAG_END, GLOBALS->game_t->translate(559)/* Originally: Come with me! Eat and drink out of bowls of gold! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(jellybean, util::Point<int>(7, 7), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(jellybean, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(560)/* Originally: I love you JB! Please come home. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new wedge::A_Star_Step(jellybean, util::Point<int>(7, 10), new_task))
			ADD_STEP(new wedge::Set_Direction_Step(jellybean, wedge::DIR_N, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(562)/* Originally: Big Tuna */ + TAG_END, GLOBALS->game_t->translate(561)/* Originally: I'll find a way to convince him once I'm finished with you! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
			ADD_STEP(new Start_Battle_Step(new Battle_BigTuna(), new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(8, 11), new_task))
			ADD_TASK(new_task)

			ANOTHER_TASK
			ADD_STEP(new wedge::A_Star_Step(wit, util::Point<int>(8, 12), new_task))
			ADD_TASK(new_task)
			
			ANOTHER_TASK
			wedge::Wait_Step *wait_step = new wedge::Wait_Step(new_task);
			ass->add_monitor(wait_step);
			ADD_STEP(wait_step)
			ADD_STEP(new wedge::Delay_Step(1000, new_task))
			ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->thud, false, false, new_task))
			ADD_TASK(new_task)

			FINISH_SYSTEM(AREA)

			return true;
		}
	
		return false;
	}

private:
};

//--

Area_Game::Area_Game() :
	area_create_count(0),
	use_camera(false)
{
	fadeout_colour = shim::black;
}

Area_Game::~Area_Game()
{
	delete plasma;
}

wedge::Area_Hooks *get_area_hooks(std::string area_name, wedge::Area *area)
{
	wedge::Area_Hooks *hooks = NULL;

	if (area_name == "start") {
		hooks = new Area_Hooks_Start(area);
	}
	else if (area_name == "village") {
		hooks = new Area_Hooks_Village(area);
	}
	else if (area_name == "toms") {
		hooks = new Area_Hooks_Toms(area);
	}
	else if (area_name == "pleasants") {
		hooks = new Area_Hooks_Pleasants(area);
	}
	else if (area_name == "pleasants_u") {
		hooks = new Area_Hooks_Pleasants_U(area);
	}
	else if (area_name == "vances") {
		hooks = new Area_Hooks_Vances(area);
	}
	else if (area_name == "cave1_start") {
		hooks = new Area_Hooks_Cave1_Start(area);
	}
	else if (area_name == "cave1_battletut") {
		hooks = new Area_Hooks_Cave1_BattleTut(area);
	}
	else if (area_name == "cave1_3") {
		hooks = new Area_Hooks_Cave1_3(area);
	}
	else if (area_name == "cave1_4") {
		hooks = new Area_Hooks_Cave1_4(area);
	}
	else if (area_name == "cave1_5") {
		hooks = new Area_Hooks_Cave1_5(area);
	}
	else if (area_name == "cave1_6") {
		hooks = new Area_Hooks_Cave1_6(area);
	}
	else if (area_name == "cave1_7") {
		hooks = new Area_Hooks_Cave1_7(area);
	}
	else if (area_name == "cave1_8") {
		hooks = new Area_Hooks_Cave1_8(area);
	}
	else if (area_name == "cave1_drop") {
		hooks = new Area_Hooks_Cave1_Drop(area);
	}
	else if (area_name == "cave1_boss") {
		hooks = new Area_Hooks_Cave1_Boss(area);
	}
	else if (area_name == "cave2_1") {
		hooks = new Area_Hooks_Cave2_1(area);
	}
	else if (area_name == "cave2_2") {
		hooks = new Area_Hooks_Cave2_2(area);
	}
	else if (area_name == "cave2_3") {
		hooks = new Area_Hooks_Cave2_3(area);
	}
	else if (area_name == "cave2_4") {
		hooks = new Area_Hooks_Cave2_4(area);
	}
	else if (area_name == "cave2_5") {
		hooks = new Area_Hooks_Cave2_5(area);
	}
	else if (area_name == "cave2_6") {
		hooks = new Area_Hooks_Cave2_6(area);
	}
	else if (area_name == "cave2_7") {
		hooks = new Area_Hooks_Cave2_7(area);
	}
	else if (area_name == "cave2_8") {
		hooks = new Area_Hooks_Cave2_8(area);
	}
	else if (area_name == "cave2_boss") {
		hooks = new Area_Hooks_Cave2_Boss(area);
	}
	else if (area_name == "cave3_1") {
		hooks = new Area_Hooks_Cave3_1(area);
	}
	else if (area_name == "cave3_2") {
		hooks = new Area_Hooks_Cave3_2(area);
	}
	else if (area_name == "cave3_3") {
		hooks = new Area_Hooks_Cave3_3(area);
	}
	else if (area_name == "cave3_4") {
		hooks = new Area_Hooks_Cave3_4(area);
	}
	else if (area_name == "cave3_falls") {
		hooks = new Area_Hooks_Cave3_Falls(area);
	}
	else if (area_name == "cave3_5") {
		hooks = new Area_Hooks_Cave3_5(area);
	}
	else if (area_name == "cave3_6") {
		hooks = new Area_Hooks_Cave3_6(area);
	}
	else if (area_name == "cave3_7") {
		hooks = new Area_Hooks_Cave3_7(area);
	}
	else if (area_name == "cave3_8") {
		hooks = new Area_Hooks_Cave3_8(area);
	}
	else if (area_name == "cave3_boss") {
		hooks = new Area_Hooks_Cave3_Boss(area);
	}
	else if (area_name == "end") {
		hooks = new Area_Hooks_End(area);
	}
	else {
		util::debugmsg("No area hooks for '%s'\n", area_name.c_str());
	}

	return hooks;
}

wedge::Area_Hooks *Area_Game::get_area_hooks(std::string area_name, wedge::Area *area)
{
	return ::get_area_hooks(area_name, area);
}

void Area_Game::draw()
{
	/*
	if (skip_clear == false) {
		gfx::clear(shim::black);
	}
	else {
		skip_clear = false;
	}
	*/

	if (scrolling_in) {
		for (int i = 0; i < 2; i++) {
			wedge::Area::Layer_Spec spec;

			if (i == 0) {
				spec = wedge::Area::BELOW;
			}
			else {
				spec = wedge::Area::ABOVE;
			}

			util::Size<int> tilemap_size = current_area->get_tilemap()->get_size() * shim::tile_size;
			util::Point<float> maximum(shim::screen_size.w, shim::screen_size.h);
			maximum.x = MIN(maximum.x, tilemap_size.w);
			maximum.y = MIN(maximum.y, tilemap_size.h);
			util::Point<float> scrolled = scroll_offset * maximum;
			util::Point<int> player_pos = players[0]->get_position();
			util::Point<float> player_offset = util::Point<float>(0.0f, 0.0f);
			util::Size<int> player_size = players[0]->get_size();
			util::Point<float> sz(player_size.w / 2.0f, 1.0f - player_size.h / 2.0f);
			wedge::add_tiles(player_pos, player_offset, sz);
			//util::Point<float> curr_offset = current_area->get_centred_offset(players[0]->get_position(), util::Point<float>(0.0f, 0.0f), true);
			util::Point<float> curr_offset = current_area->get_centred_offset(player_pos, player_offset, true);
			curr_offset -= scrolled;
			current_area->draw(curr_offset, spec);
			util::Point<int> next_pos = next_area_positions[0];
			util::Point<float> next_o(0.0f, 0.0f);
			wedge::add_tiles(next_pos, next_o, sz);
			//util::Point<float> next_offset = next_area->get_centred_offset(next_area_positions[0], util::Point<float>(0.0f, 0.0f), true);
			util::Point<float> next_offset = next_area->get_centred_offset(next_pos, next_o, true);
			util::Point<float> no;
			if (scroll_increment.x < 0 || scroll_increment.y < 0) {
				util::Point<float> o = curr_offset;
				if (tilemap_size.w < shim::screen_size.w) {
					o.x -= (shim::screen_size.w-tilemap_size.w)/2.0f;
				}
				if (tilemap_size.h < shim::screen_size.h) {
					o.y -= (shim::screen_size.h-tilemap_size.h)/2.0f;
				}
				if (scroll_increment.x == 0.0f) {
					maximum.x = o.x;
				}
				else {
					maximum.y = o.y;
				}
				no = next_offset - (maximum - o);
				next_area->draw(next_offset - (maximum - o), spec);
			}
			else {
				if (scroll_increment.x == 0.0f) {
					maximum.x = scrolled.x;
				}
				else {
					maximum.y = scrolled.y;
				}
				no = next_offset + (maximum - scrolled);
				next_area->draw(next_offset + (maximum - scrolled), spec);
			}
			if (i == 0) {
				std::vector<wedge::Map_Entity *> entities = players;
				std::sort(entities.begin(), entities.end(), wedge::entity_y_compare);
				for (size_t i = 0; i < entities.size(); i++) {
					wedge::Map_Entity *p = entities[i];
					p->draw(curr_offset + (scroll_offset * shim::tile_size));
				}
			}
		}
	}
	else {
		Uint32 now = GET_TICKS();

		if (pausing) {
			gfx::set_target_image(GLOBALS->work_image);
			gfx::clear(shim::black);
			gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
			gfx::update_projection();
		}
		//else if (int(now - pause_end_time) <= pause_fade_time) {
		else if (pause_ended) {
			gfx::set_target_image(GLOBALS->work_image);
			gfx::clear(shim::black);
			gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
			gfx::update_projection();
		}
		
		if (use_camera) {
			current_area->draw(camera);
		}
		else {
			current_area->draw();
		}
		
		if (pausing) {
			Uint32 diff = now - pause_start_time;
			if ((int)diff > pause_fade_time) {
				diff = pause_fade_time;
			}
			float p = diff / (float)pause_fade_time;

			gfx::set_target_backbuffer();
			if (pause_is_for_menu == false) {
				gfx::clear(shim::black);
				gfx::draw_filled_rectangle(shim::palette[22], {0, 0}, shim::screen_size);
			}
			else {
				//gfx::clear(shim::transparent);
				//gfx::draw_filled_rectangle(shim::black, {0, 0}, shim::screen_size);
				gfx::clear(shim::black);
			}
			/*
			gfx::enable_depth_write(true);
			gfx::clear_depth_buffer(1.0f);
			gfx::enable_depth_write(false);
			*/
			glm::mat4 mv_save, proj_save, mv, proj;
			gfx::get_matrices(mv_save, proj_save);
			float x = 1.0f;
			float y = 1.0f;
			proj = glm::scale(proj, glm::vec3(10.0f, 10.0f, 1.0f));
			proj = glm::translate(proj, glm::vec3(0.0f, 0.0f, -10.0f));
			proj = glm::frustum(-x, x, y, -y, 1.0f, 1000.0f) * proj;
			proj = glm::translate(proj, glm::vec3(0.0f, y, 0.0f));
			proj = glm::rotate(proj, float(M_PI/2.0f*p), glm::vec3(1.0f, 0.0f, 0.0f));
			proj = glm::translate(proj, glm::vec3(0.0f, -y, 0.0f));
			gfx::set_matrices(mv, proj);
			gfx::update_projection();
			//gfx::set_cull_mode(gfx::NO_FACE);
			SDL_Colour tint = shim::white;
			float f = 1.0f - p;
			tint.r *= f;
			tint.g *= f;
			tint.b *= f;
			tint.a *= f;
			GLOBALS->work_image->stretch_region_tinted(tint, util::Point<float>(0.0f, 0.0f), shim::real_screen_size, util::Point<float>(-x, -y), util::Size<int>(x*2, y*2));
			gfx::set_matrices(mv_save, proj_save);
			gfx::update_projection();
			//gfx::set_cull_mode(gfx::BACK_FACE);
		}
		//else if (int(now - pause_end_time) <= pause_fade_time) {
		else if (pause_ended) {
			gfx::set_target_backbuffer();
		}

		Game::draw();
	
		if (gameover) {
			Uint32 now = GET_TICKS();
			Uint32 end = gameover_time + GLOBALS->gameover_timeout;
			Uint32 diff;
			if (now > end) {
				diff = 0;
			}
			else {
				diff = end-now;
			}
			if ((int)diff <= GLOBALS->gameover_fade_time) {
				float p = 1.0f - ((float)diff / GLOBALS->gameover_fade_time);
				p = p * p;
				SDL_Colour colour;
				if (gameover_time == 0) { // hack for credits
					colour = shim::palette[38];
				}
				else {
					colour.r = GLOBALS->gameover_fade_colour.r * p;
					colour.g = GLOBALS->gameover_fade_colour.g * p;
					colour.b = GLOBALS->gameover_fade_colour.b * p;
					colour.a = GLOBALS->gameover_fade_colour.a * p;
				}
				gfx::draw_filled_rectangle(colour, util::Point<int>(0, 0), shim::screen_size);
			}
		}
	}
	
	if (fading_in) {
		Uint32 now = GET_TICKS();
		Uint32 elapsed = now - fade_start_time;
		if ((int)elapsed > change_area_fade_duration) {
			elapsed = change_area_fade_duration;
		}

		float p;
		if ((int)elapsed >= change_area_fade_duration/2) {
			elapsed -= change_area_fade_duration/2;
			p = 1.0f - (elapsed / (change_area_fade_duration/2.0f));
		}
		else {
			p = elapsed / (change_area_fade_duration/2.0f);
		}
		p = MAX(0.0f, MIN(1.0f, p));
		SDL_Colour black;
		black.r = fadeout_colour.r * p;
		black.g = fadeout_colour.g * p;
		black.b = fadeout_colour.b * p;
		black.a = p * 255;
		gfx::draw_filled_rectangle(black, util::Point<int>(0, 0), shim::screen_size);
	}
}

void Area_Game::set_next_fadeout_colour(SDL_Colour colour)
{
	fadeout_colour = colour;
}

std::string get_friendly_name(std::string area_name)
{
	if (area_name == "start") {
		return GLOBALS->game_t->translate(53)/* Originally: The Well */;
	}
	else if (area_name == "village") {
		return GLOBALS->game_t->translate(54)/* Originally: Cozy River */;
	}
	else if (area_name == "toms") {
		return GLOBALS->game_t->translate(58)/* Originally: Tom's */;
	}
	else if (area_name == "pleasants") {
		return GLOBALS->game_t->translate(56)/* Originally: Pleasant's */;
	}
	else if (area_name == "pleasants_u") {
		return GLOBALS->game_t->translate(57)/* Originally: Pleasant's 2 */;
	}
	else if (area_name == "vances") {
		return GLOBALS->game_t->translate(55)/* Originally: Vance's */;
	}
	else if (area_name == "cave1_start") {
		return GLOBALS->game_t->translate(74)/* Originally: Cave 1-1 */;
	}
	else if (area_name == "cave1_battletut") {
		return GLOBALS->game_t->translate(168)/* Originally: Cave 1-2 */;
	}
	else if (area_name == "cave1_3") {
		return GLOBALS->game_t->translate(231)/* Originally: Cave 1-3 */;
	}
	else if (area_name == "cave1_4") {
		return GLOBALS->game_t->translate(234)/* Originally: Cave 1-4 */;
	}
	else if (area_name == "cave1_5") {
		return GLOBALS->game_t->translate(235)/* Originally: Cave 1-5 */;
	}
	else if (area_name == "cave1_6") {
		return GLOBALS->game_t->translate(236)/* Originally: Cave 1-6 */;
	}
	else if (area_name == "cave1_7") {
		return GLOBALS->game_t->translate(237)/* Originally: Cave 1-7 */;
	}
	else if (area_name == "cave1_8") {
		return GLOBALS->game_t->translate(238)/* Originally: Cave 1-8 */;
	}
	else if (area_name == "cave1_drop") {
		return GLOBALS->game_t->translate(240)/* Originally: Cave 1-Drop */;
	}
	else if (area_name == "cave1_boss") {
		return GLOBALS->game_t->translate(254)/* Originally: Cave 1-??? */;
	}
	else if (area_name == "cave2_1") {
		return GLOBALS->game_t->translate(281)/* Originally: Cave 2-1 */;
	}
	else if (area_name == "cave2_2") {
		return GLOBALS->game_t->translate(320)/* Originally: Cave 2-2 */;
	}
	else if (area_name == "cave2_3") {
		return GLOBALS->game_t->translate(321)/* Originally: Cave 2-3 */;
	}
	else if (area_name == "cave2_4") {
		return GLOBALS->game_t->translate(323)/* Originally: Cave 2-4 */;
	}
	else if (area_name == "cave2_5") {
		return GLOBALS->game_t->translate(324)/* Originally: Cave 2-5 */;
	}
	else if (area_name == "cave2_6") {
		return GLOBALS->game_t->translate(335)/* Originally: Cave 2-6 */;
	}
	else if (area_name == "cave2_7") {
		return GLOBALS->game_t->translate(336)/* Originally: Cave 2-7 */;
	}
	else if (area_name == "cave2_8") {
		return GLOBALS->game_t->translate(337)/* Originally: Cave 2-8 */;
	}
	else if (area_name == "cave2_boss") {
		return GLOBALS->game_t->translate(338)/* Originally: Cave 2-??? */;
	}
	else if (area_name == "cave3_1") {
		return GLOBALS->game_t->translate(431)/* Originally: Cave 3-1 */;
	}
	else if (area_name == "cave3_2") {
		return GLOBALS->game_t->translate(477)/* Originally: Cave 3-2 */;
	}
	else if (area_name == "cave3_3") {
		return GLOBALS->game_t->translate(492)/* Originally: Cave 3-3 */;
	}
	else if (area_name == "cave3_4") {
		return GLOBALS->game_t->translate(493)/* Originally: Cave 3-4 */;
	}
	else if (area_name == "cave3_falls") {
		return GLOBALS->game_t->translate(503)/* Originally: Cave 3-Falls */;
	}
	else if (area_name == "cave3_5") {
		return GLOBALS->game_t->translate(494)/* Originally: Cave 3-5 */;
	}
	else if (area_name == "cave3_6") {
		return GLOBALS->game_t->translate(504)/* Originally: Cave 3-6 */;
	}
	else if (area_name == "cave3_7") {
		return GLOBALS->game_t->translate(507)/* Originally: Cave 3-7 */;
	}
	else if (area_name == "cave3_8") {
		return GLOBALS->game_t->translate(532)/* Originally: Cave 3-8 */;
	}
	else if (area_name == "cave3_boss") {
		return GLOBALS->game_t->translate(533)/* Originally: Cave 3-??? */;
	}
	else if (area_name == "end") {
		return GLOBALS->game_t->translate(548)/* Originally: ??? */;
	}
	else {
		return GLOBALS->game_t->translate(0)/* Originally: Unknown... */;
	}
}

int Area_Game::get_num_areas_created()
{
	return area_create_count;
}

wedge::Area *Area_Game::create_area(std::string name)
{
	area_create_count++;
	return new wedge::Area(name);
}

wedge::Area *Area_Game::create_area(util::JSON::Node *json)
{
	area_create_count++;
	return new wedge::Area(json);
}

void Area_Game::set_use_camera(bool use_camera)
{
	this->use_camera = use_camera;
}

void Area_Game::set_camera(util::Point<float> camera)
{
	this->camera = camera;
}

wedge::Game *Area_Game::create_menu()
{
	return new Menu_Game();
}

wedge::Game *Area_Game::create_shop(std::vector<wedge::Object> items)
{
	return new Shop_Game(items);
}

bool Area_Game::run()
{
	int max = 0;
	for (auto &s : INSTANCE->stats) {
		if (s.base.fixed.max_mp > max) {
			max = s.base.fixed.max_mp;
		}
	}
	// Fill all SP after 2 minutes
	int inc = 1000000 / 120 / 60 * max;
	DOGO_INSTANCE->sp_replenish_count += inc;

	return wedge::Area_Game::run();
}

static void sensei_leave1(void *data)
{
	wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");
	wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
	wedge::Map_Entity *tom = AREA->get_player(TOM);

	NEW_SYSTEM_AND_TASK(AREA)
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "crouch_s", new_task))
	ADD_STEP(new wedge::Delay_Step(250, new_task))
	ADD_STEP(new wedge::Set_Animation_Step(sensei, "jump", new_task))
	ADD_STEP(new wedge::Play_Sound_Step(DOGO_GLOBALS->sensei_jump, false, false, new_task))
	ADD_STEP(new wedge::Offset_Arc_Step(sensei, util::Point<float>(0.0f, 0.0f), util::Point<float>(0.0f, -6.0f), 0.0f, 500, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(delete_sensei, nullptr, new_task))
	ADD_STEP(new wedge::Achieve_Step((void *)ACH_SENSEI, new_task))
	ADD_STEP(new wedge::Play_Music_Step("music/underground.mml", new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_E, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_W, true, false, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(223)/* Originally: Who was that guy? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(224)/* Originally: *shrug* */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_N, true, false, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(tom, wedge::DIR_N, true, false, new_task))
	ADD_STEP(new wedge::Delay_Step(1000, new_task))
	ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, new_task))
	ADD_STEP(new wedge::A_Star_Step(tom, util::Point<int>(1, 0)+pleasant->get_position(), new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task))
	ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
	ADD_STEP(new Autosave_Step(new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void retry_battle_tut_callback(void *data)
{
	wedge::Yes_No_GUI_Callback_Data *d = static_cast<wedge::Yes_No_GUI_Callback_Data *>(data);

	if (d->cancelled == false && d->choice == true) {
		wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");
		sensei->set_visible(false);
		auto b = new Battle_Sensei1();
		b->start_transition_in();
	}
	else {
		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(225)/* Originally: You'll have to learn the hard way, then. */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
		ADD_STEP(new wedge::Generic_Immediate_Callback_Step(sensei_leave1, nullptr, new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)
	}
}

static void run_retry(void *data)
{
	Yes_No_GUI *yn_gui = new Yes_No_GUI(GLOBALS->game_t->translate(220)/* Originally: Retry battle? */, true, retry_battle_tut_callback, NULL);
	shim::guis.push_back(yn_gui);
}

#ifdef DEMO
static void end_demo_real(void *data)
{
	GLOBALS->quit(true);
}

static void end_demo(void *data)
{
	std::string msg = GLOBALS->game_t->translate(517)/* Originally: You have reached the end of the demo! Thanks for playing! */;
	Notification_GUI *gui = new Notification_GUI(msg, end_demo_real);
	shim::guis.push_back(gui);
}
#endif

static void boss1_raise_bridge(void *data)
{
	wedge::Area *area = AREA->get_current_area();
	Area_Hooks_Cave1_Boss *hooks = dynamic_cast<Area_Hooks_Cave1_Boss *>(area->get_hooks());

	NEW_SYSTEM_AND_TASK(AREA)
	for (int i = 0; i < 13; i++) {
		ADD_STEP(new wedge::Generic_Immediate_Callback_Step(boss1_bridge_callback, hooks, new_task));
		ADD_STEP(new wedge::Delay_Step(250, new_task))
	}
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task))
#ifdef DEMO
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(end_demo, nullptr, new_task))
#else
	ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
	ADD_STEP(new Autosave_Step(new_task))
#endif
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)

	int freq = noo::audio::internal::audio_context.device_spec.freq;
	
	for (int i = 0; i < 10; i++) {
		hooks->get_thump()->play_stretched(1.0f*shim::sfx_volume, (i*250)*freq/1000, 0);
	}
}

static void boss1_done(void *data)
{
	wedge::Area *area = AREA->get_current_area();
	wedge::Map_Entity *octo = area->find_entity("octo");
	area->remove_entity(octo, true);
		
	NEW_SYSTEM_AND_TASK(AREA)
	//ADD_STEP(new wedge::Delay_Step(1000, new_task))
	ADD_STEP(new wedge::Generic_Immediate_Callback_Step(boss1_raise_bridge, nullptr, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(AREA)
}

static void octo_leave(void *data)
{
	wedge::Area *area = AREA->get_current_area();
	wedge::Map_Entity *octo = area->find_entity("octo");
	octo->get_sprite()->set_animation("dive", boss1_done, nullptr);
	DOGO_GLOBALS->octo->play(false);
}

void Area_Game::battle_ended(wedge::Battle_Game *battle)
{
	wedge::Area_Game::battle_ended(battle);

	if (dynamic_cast<Battle_BigTuna *>(battle) != NULL) {
		bool all_dead = true;
		for (auto &s : INSTANCE->stats) {
			if (s.base.hp > 0) {
				all_dead = false;
			}
			s.base.hp = s.base.fixed.max_hp;
		}

		if (all_dead == false) {
			AREA->get_current_area()->remove_entity(AREA->get_current_area()->find_entity("bigtuna"), true);

			auto area = AREA->get_current_area();
			auto jb = area->find_entity("jellybean");
			auto pleasant = AREA->get_player(0);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new wedge::Set_Direction_Step(jb, wedge::DIR_E, true, false, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_W, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(574)/* Originally: I hope you know how to drive a submarine JB... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new wedge::Set_Direction_Step(pleasant, wedge::DIR_S, true, false, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(575)/* Originally: We'll figure it out... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(576)/* Originally: Let's get going, your mom should have supper ready by now! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(577)/* Originally: I think I smell meatballs... */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_TOP, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(credits, nullptr, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}
	}
	else if (dynamic_cast<Battle_Sensei1 *>(battle) != NULL) {
		wedge::Map_Entity *sensei = AREA->get_current_area()->find_entity("sensei");
		sensei->set_visible(true);

		bool all_dead = true;
		for (auto &s : INSTANCE->stats) {
			if (s.base.hp > 0) {
				all_dead = false;
			}
			s.base.hp = s.base.fixed.max_hp;
		}

		NEW_SYSTEM_AND_TASK(AREA)

		if (all_dead) {
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(221)/* Originally: You have much to learn. Will you try again? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(run_retry, nullptr, new_task))
		}
		else {
			ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(193)/* Originally: Sensei */ + TAG_END, GLOBALS->game_t->translate(222)/* Originally: That's all I can teach you... for now. Be brave! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
			ADD_STEP(new wedge::Generic_Immediate_Callback_Step(sensei_leave1, nullptr, new_task))
		}

		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)

		return;
	}
	else if (dynamic_cast<Battle_Octo *>(battle) != NULL) {
		wedge::Area *area = AREA->get_current_area();
		wedge::Map_Entity *octo = area->find_entity("octo");

		for (int i = 0; i < 8; i++) {
			std::string name = "tentacle" + util::itos(i);
			wedge::Map_Entity *tentacle = area->find_entity(name);
			area->remove_entity(tentacle, true);
		}

		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new wedge::Set_Animation_Step(octo, "scared", new_task))
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(258)/* Originally: Octo */ + TAG_END, GLOBALS->game_t->translate(269)/* Originally: Please sirs, don't eat me! You wouldn't eat hot rubber, would you? */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(15)/* Originally: Pleasant */ + TAG_END, GLOBALS->game_t->translate(270)/* Originally: We're not going to eat you! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
		ADD_STEP(new wedge::Set_Animation_Step(octo, "alert", new_task))
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(258)/* Originally: Octo */ + TAG_END, GLOBALS->game_t->translate(271)/* Originally: Thank you! Thank you so much!^Let me help you across so you can find the pooch! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
		ADD_STEP(new wedge::Generic_Immediate_Callback_Step(octo_leave, nullptr, new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)

		// FIXME? have to test this but should be returning here I think
		return;
	}
	if (dynamic_cast<Battle_Chilly *>(battle) != NULL) {
		wedge::Area *area = AREA->get_current_area();
		wedge::Map_Entity *chilly = area->find_entity("chilly");

		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new wedge::Set_Animation_Step(chilly, "idle", new_task))
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(419)/* Originally: Chilly */ + TAG_END, GLOBALS->game_t->translate(422)/* Originally: Ugh... Chilly sorry. You go now! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(16)/* Originally: Tom */ + TAG_END, GLOBALS->game_t->translate(423)/* Originally: Thanks big guy. Sorry about the bruises! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
		ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
		ADD_STEP(new Autosave_Step(new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)

		return;
	}
	else if (dynamic_cast<Battle_Bumper *>(battle) != NULL) {
		wedge::Map_Entity *kiddie1 = AREA->get_current_area()->find_entity("kiddie1");
		wedge::Map_Entity *kiddie2 = AREA->get_current_area()->find_entity("kiddie2");
		wedge::Map_Entity *kiddie3 = AREA->get_current_area()->find_entity("kiddie3");
		AREA->get_current_area()->remove_entity(kiddie1, true);
		AREA->get_current_area()->remove_entity(kiddie2, true);
		AREA->get_current_area()->remove_entity(kiddie3, true);
		AREA->get_current_area()->find_entity("bumper")->get_sprite()->set_animation("sit_s");
		
		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(546)/* Originally: Bumper */ + TAG_END, GLOBALS->game_t->translate(545)/* Originally: Pumpkins... Measly pumpkins!^*shrug* */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_BOTTOM, new_task))
		ADD_STEP(new wedge::Generic_Immediate_Callback_Step(set_following, (void *)1, new_task))
		ADD_STEP(new wedge::Pause_Presses_Step(false, true, new_task))
		ADD_STEP(new Autosave_Step(new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)
	}

	auto cave2_4_hooks = dynamic_cast<Area_Hooks_Cave2_4 *>(current_area->get_hooks());
	if (cave2_4_hooks) {
		cave2_4_hooks->restart_rock_sfx();
	}
	
	auto cave3_4_hooks = dynamic_cast<Area_Hooks_Cave3_4 *>(current_area->get_hooks());
	if (cave3_4_hooks) {
		cave3_4_hooks->restart_waterfall_sfx();
	}
	
	auto cave3_falls_hooks = dynamic_cast<Area_Hooks_Cave3_Falls *>(current_area->get_hooks());
	if (cave3_falls_hooks) {
		cave3_falls_hooks->restart_waterfall_sfx();
	}
	
	auto cave3_5_hooks = dynamic_cast<Area_Hooks_Cave3_5 *>(current_area->get_hooks());
	if (cave3_5_hooks) {
		cave3_5_hooks->restart_waterfall_sfx();
	}
	
	if (can_autosave()) {
		autosave(true);
	}
}

void Area_Game::handle_event(TGUI_Event *event)
{
	wedge::Area_Game::handle_event(event);

#ifdef DEBUG
	if (event->type == TGUI_KEY_DOWN && event->keyboard.code == TGUIK_b) {
#else
	if (event->type == TGUI_KEY_DOWN && event->keyboard.code == TGUIK_b && util::bool_arg(false, shim::argc, shim::argv, "b-for-battle")) {
#endif
		//wedge::Battle_Game *battle_game = new Battle_3FlameFrog("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_2Bot("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_1FlameFrog1Bot("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_3Goblin("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_4Goblin("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_3FlameFrog("cave2-", 0);
		wedge::Battle_Game *battle_game = new Battle_2RedWasp("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_1RedWasp2Goblin("cave2-", 0);
		//wedge::Battle_Game *battle_game = new Battle_2Coinman();
		//auto battle_game = new Battle_6Snapper();
		//auto battle_game = new Battle_3Bone();
		//auto battle_game = new Battle_2Aura("cave3-", 0);

		//auto battle_game = new Battle_4Snapper();
		//auto battle_game = new Battle_2Bone();
		//auto battle_game = new Battle_Bumper();
		//auto battle_game = new Battle_1Aura1Bone();
		
		//auto battle_game = new Battle_1Thirsty();
		//auto battle_game = new Battle_2CutiePie();
		//auto battle_game = new Battle_1CutiePie1Clone();

		//auto battle_game = new Battle_BigTuna();

		battle_game->start_transition_in();
	}
}

