#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/player_input.h"
#include "wedge2/map_entity.h"
#include "wedge2/omnipresent.h"

#include "dialogue.h"
#include "general.h"
#include "globals.h"

static util::Size<int> get_pad()
{
	util::Size<int> pad;
	pad.w = shim::screen_size.w * 0.03f;
	pad.h = shim::screen_size.h * 0.03f;
	return pad;
}

void Dialogue_Step::get_positions(int *HEIGHT, int *indicator_height, int *y, util::Size<int> *PAD, util::Point<int> *text_pos, wedge::Dialogue_Position position)
{
	if (HEIGHT) {
		*HEIGHT = shim::font->get_height() * 2 + Dialogue_Step::BORDER * 2 + 2;
	}
	if (indicator_height) {
		*indicator_height = 5;
	}
	if (PAD) {
		*PAD = get_pad();
	}
	if (y) {
		if (position == wedge::DIALOGUE_TOP) {
			*y = PAD->h;
		}
		else {
			*y = shim::screen_size.h - 1 - PAD->h - *HEIGHT;
		}
	}
	if (text_pos) {
		*text_pos = util::Point<int>(PAD->w + Dialogue_Step::BORDER, *y + Dialogue_Step::BORDER);
	}
}

Dialogue_Step::Dialogue_Step(std::string tag, std::string text, wedge::Dialogue_Type type, wedge::Dialogue_Position position, wedge::Task *task, bool darken_screen) :
	wedge::Step(task),
	tag(tag),
	text(text),
	type(type),
	position(position),
	start_character(0),
	done(false),
	fade_out_start_time(0),
	dismissable(true),
	count(0),
	sent_done(false),
	darken_screen(darken_screen)
{
}

void Dialogue_Step::start()
{
	if (type == wedge::DIALOGUE_MESSAGE) {
		started_time_set = false;
	}
	else {
		started_time_set = true;
	}
	started_time = GET_TICKS();
	started_time_transition = GET_TICKS();

	tag_width = MAX(0, DOGO_GLOBALS->bold_font->get_text_width(tag));

	in_battle = BATTLE != NULL;
	wedge::Area *area = AREA->get_current_area();

	if (position == wedge::DIALOGUE_AUTO) {
		if (in_battle) {
			position = wedge::DIALOGUE_BOTTOM;
		}
		else {
			wedge::Map_Entity *player = AREA->get_player(0);
			util::Point<float> entity_position = player->get_position();
			util::Point<float> entity_offset = player->get_offset();

			util::Point<float> entity_pos;
			util::Point<float> centre;

			entity_pos = (entity_offset + entity_position) * shim::tile_size + shim::tile_size / 2;

			util::Size<float> half_screen = shim::screen_size;
			half_screen /= 2.0f;

			centre = -entity_pos + half_screen;

			gfx::Tilemap *tilemap = area->get_tilemap();

			util::Size<int> tilemap_size = tilemap->get_size() * shim::tile_size;

			if (tilemap_size.h < shim::screen_size.h) {
				util::Point<float> pos = player->get_position();
				util::Size<float> sz = tilemap->get_size();
				float o = pos.y - ((sz.h-1)/2);
				if (o == 0) {
					if (player->get_direction() == wedge::DIR_S) {
						position = wedge::DIALOGUE_TOP;
					}
					else {
						position = wedge::DIALOGUE_BOTTOM;
					}
				}
				else if (o < 0) {
					position = wedge::DIALOGUE_BOTTOM;
				}
				else {
					position = wedge::DIALOGUE_TOP;
				}
			}
			else {
				if (centre.y > 0.0f) {
					position = wedge::DIALOGUE_BOTTOM;
				}
				else if (entity_pos.y + half_screen.h > tilemap_size.h) {
					position = wedge::DIALOGUE_TOP;
				}
				else {
					if (player->get_direction() == wedge::DIR_S) {
						position = wedge::DIALOGUE_TOP;
					}
					else {
						position = wedge::DIALOGUE_BOTTOM;
					}
				}
			}
		}
	}

	if (in_battle == false) {
		wedge::pause_player_input(true);

		entity_movement_system = area->get_entity_movement_system();
		entity_movement_was_paused = entity_movement_system->is_paused();

		std::vector<Dialogue_Step *> dialogues = active_dialogues(task->get_system()->get_game());
		for (size_t i = 0; i < dialogues.size(); i++) {
			Dialogue_Step *d = dialogues[i];
			if (d != this && d->is_initialised()) {
				entity_movement_was_paused = d->get_entity_movement_was_paused();
				break;
			}
		}

		entity_movement_system->set_paused(true);

		std::list<wedge::Map_Entity *> &entities = area->get_entities();
		for (std::list<wedge::Map_Entity *>::iterator it = entities.begin(); it != entities.end(); it++) {
			wedge::Map_Entity *entity = *it;
			gfx::Sprite *sprite = entity->get_sprite();
			if (sprite && entity->is_moving()) {
				if (sprite->is_started()) {
					sprite->stop();
					unpause.push_back(sprite);
				}
			}
		}
		wedge::Map_Entity_Input_Step *input_step = AREA->get_player(0)->get_input_step();
		if (input_step != NULL) {
			input_step->end_movement();
		}
	}
}

Dialogue_Step::~Dialogue_Step()
{
}

bool Dialogue_Step::run()
{
	bool ret = !(done && GET_TICKS() >= fade_out_start_time+FADE_TIME);
	return ret;
}

void Dialogue_Step::handle_event(TGUI_Event *event)
{
	if (done) {
		return;
	}

	if (BATTLE != nullptr && type != wedge::DIALOGUE_MESSAGE) {
		Uint32 now = GET_TICKS();
		float p = (now - started_time_transition) / (float)(FADE_TIME+1000);
		if (p < 1.0f) {
			return;
		}
	}

	std::string t = text.substr(util::utf8_len_bytes(text, start_character));
	bool full;
	int num_lines;
	int width;
	util::Size<int> PAD = get_pad();
	int num_chars = shim::font->draw_wrapped(shim::black/*colour doesn't matter here*/, t, text_pos, shim::screen_size.w-PAD.w*2-BORDER*2-1/*-1 for shadow*/, shim::font->get_height()+2, 2, (started_time_set == false ? -1 : GET_TICKS()-started_time), DELAY, true, full, num_lines, width, true, false, tag_width);

	bool next = false;

	if (event->type == TGUI_KEY_DOWN && event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action && event->keyboard.is_repeat == false) {
		next = true;
	}
	else if (event->type == TGUI_MOUSE_DOWN && event->mouse.is_repeat == false) {
		next = true;
	}
	else if (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_action && event->joystick.is_repeat == false) {
		next = true;
	}

	if (next && sent_done == false) {
		if (full) {
			if (num_chars+start_character == (int)util::utf8_len(text)) {
				DOGO_GLOBALS->button->play(false);
				if (dismissable) {
					dismiss();
				}
				send_done_signal();
				sent_done = true;
			}
			else {
				DOGO_GLOBALS->button->play(false);
				start_character += num_chars;
				if (type == wedge::DIALOGUE_MESSAGE) {
					started_time_set = false;
				}
				else {
					started_time_set = true;
					started_time = GET_TICKS();
				}
			}
		}
		else {
			started_time_set = false;
		}
	}
	
	if (done == false && BATTLE == NULL) {
		wedge::Map_Entity *pleasant = AREA->get_player(PLEASANT);
		wedge::Player_Input_Step *pis = static_cast<wedge::Player_Input_Step *>(pleasant->get_input_step());
		pis->handle_event(event);
	}
}

void Dialogue_Step::draw_fore()
{
	// Every even, hook, every odd, draw (causes draws to happen in Omnipresent::draw_fore)
	int mod = count % 2;
	count++;
	if (mod == 0) {
		OMNIPRESENT->hook_draw_fore(this);
		return;
	}

	gfx::Font::end_batches();

	if (darken_screen) {
		SDL_Colour colour = shim::black;
		colour.r *= 0.75;
		colour.g *= 0.75;
		colour.b *= 0.75;
		colour.a *= 0.75;
		gfx::draw_filled_rectangle(colour, util::Point<int>(0, 0), shim::screen_size);
	}

	int HEIGHT, indicator_height, y;
	util::Size<int> PAD;
	util::Point<int> text_pos;

	get_positions(&HEIGHT, &indicator_height, &y, &PAD, &text_pos, position);
	HEIGHT += indicator_height;

	if (position == wedge::DIALOGUE_BOTTOM) {
		y -= indicator_height;
		text_pos.y -= indicator_height;
	}

	float alpha;
	float offset;
	Uint32 now = GET_TICKS();
	if (done) {
		float p = (now - fade_out_start_time) / (float)FADE_TIME;
		p = p * p;
		alpha = MAX(0.0f, 1.0f - p);
		offset = 0.0f;
	}
	else {
		alpha = 1.0f;
		float max_offset;
		if (position == wedge::DIALOGUE_TOP) {
			max_offset = -(PAD.h + HEIGHT);
		}
		else {
			max_offset = PAD.h + HEIGHT;
		}
		float p;
		if (start_character != 0) {
			p = 1.0f;
		}
		else {
			p = MIN(1.0f, (now - started_time_transition) / (float)FADE_TIME);
		}
		p = p * p;
		offset = (1.0f - p) * max_offset;
	}

	// set up projection

	glm::mat4 modelview, proj;
	gfx::get_matrices(modelview, proj);
	glm::mat4 mv = glm::translate(modelview, glm::vec3(0.0f, offset, 0.0f));

	// draw window

	SDL_Colour text_colour = shim::palette[39];
	SDL_Colour arrow_colour;
	gfx::Image *nine_patch;
	if (type == wedge::DIALOGUE_SPEECH) {
		arrow_colour = shim::palette[35];
		nine_patch = DOGO_GLOBALS->speech_window;
	}
	else {
		arrow_colour = shim::palette[48];
		nine_patch = DOGO_GLOBALS->message_window;
	}
	shim::current_shader->set_float("alpha", alpha);
	gfx::set_matrices(mv, proj);
	gfx::update_projection();

	gfx::draw_9patch_tinted(make_translucent(shim::white, alpha), nine_patch, util::Point<int>(PAD.w, y), util::Size<int>(shim::screen_size.w-PAD.w*2, HEIGHT));

	shim::current_shader = DOGO_GLOBALS->alpha_shader;
	shim::current_shader->use();
	shim::current_shader->set_float("alpha", alpha);

	gfx::set_matrices(mv, proj);
	gfx::update_projection();

	DOGO_GLOBALS->bold_font->draw(text_colour, tag, text_pos);
	std::string t = text.substr(util::utf8_len_bytes(text, start_character));
	bool full;
	int num_lines;
	int width;
	int num_drawn = shim::font->draw_wrapped(text_colour, t, text_pos, shim::screen_size.w-PAD.w*2-BORDER*2-1/*-1 for shadow*/, shim::font->get_height()+2, 2, (started_time_set == false ? -1 : GET_TICKS()-started_time), DELAY, false, full, num_lines, width, true, false, tag_width);

	util::Point<float> draw_pos(shim::screen_size.w-PAD.w-DOGO_GLOBALS->down_arrow->size.w-3, y+HEIGHT-DOGO_GLOBALS->down_arrow->size.h-4);
	if (num_drawn+start_character < util::utf8_len(text)) {
		const int anim_time = 1000;
		Uint32 ticks = GET_TICKS() % anim_time;
		if (ticks < anim_time/2) {
			draw_pos += util::Point<int>(0, 1);
		}
		DOGO_GLOBALS->down_arrow->draw_tinted(arrow_colour, draw_pos);
	}
	else {
		DOGO_GLOBALS->nomore->draw_tinted(arrow_colour, draw_pos);
	}

	gfx::Font::end_batches();
	
	shim::current_shader = shim::default_shader;
	shim::current_shader->use();
	gfx::set_matrices(modelview, proj);
	gfx::update_projection();
}

void Dialogue_Step::set_dismissable(bool dismissable)
{
	this->dismissable = dismissable;
}

void Dialogue_Step::dismiss()
{
	if (in_battle == false) {
		wedge::pause_player_input(false);

		entity_movement_system->set_paused(entity_movement_was_paused);
		
		for (std::list<gfx::Sprite *>::iterator it = unpause.begin(); it != unpause.end(); it++) {
			gfx::Sprite *sprite = *it;
			sprite->start();
		}
	}
	done = true;
	fade_out_start_time = GET_TICKS();
	disown();
}

bool Dialogue_Step::get_entity_movement_was_paused()
{
	return entity_movement_was_paused;
}

bool Dialogue_Step::is_done()
{
	return done;
}
