// FF4 GBA style battle transition

#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/map_entity.h"
#include "wedge2/omnipresent.h"

#include "general.h"
#include "globals.h"
#include "transition.h"

//const float back = 0.175f;
//const float back = 0.16667f;
//const float back = 0.25f;
const float back = 0.125f;

static util::Point<float> get_pos(bool out)
{
	util::Point<float> pos;
	if (BATTLE == nullptr) {
		wedge::Area *current_area = AREA->get_current_area();
		util::Size<int> tilemap_size = current_area->get_tilemap()->get_size() * shim::tile_size;
		util::Point<float> maximum(shim::screen_size.w, shim::screen_size.h);
		maximum.x = MIN(maximum.x, tilemap_size.w);
		maximum.y = MIN(maximum.y, tilemap_size.h);
		std::vector<wedge::Map_Entity *> players = AREA->get_players();
		util::Point<int> player_pos = players[0]->get_position();
		util::Point<float> player_offset = players[0]->get_offset();//util::Point<float>(0.0f, 0.0f);
		util::Size<int> player_size = players[0]->get_size();
		util::Point<float> sz(player_size.w / 2.0f, 1.0f - player_size.h / 2.0f);
		wedge::add_tiles(player_pos, player_offset, sz);
		//util::Point<float> curr_offset = current_area->get_centred_offset(player_pos, player_offset, true);
		util::Point<float> p = player_pos;
		p += player_offset;
		p *= shim::tile_size;
		// Use this one to zoom to player
		//pos = p + curr_offset;
		// This one zooms to centre
		pos = util::Point<float>(shim::screen_size.w/2.0f, shim::screen_size.h/2.0f);
	}
	else {
		pos = util::Point<float>(shim::screen_size.w/2.0f, shim::screen_size.h/2.0f);
	}
	return pos;
}

Transition_Step::Transition_Step(bool out, wedge::Task *task) :
	wedge::Step(task),
	out(out),
	count(0)
{
	gfx::get_matrices(mv_save, proj_save);

	if (out == true && BATTLE == nullptr) {
		length = LENGTH;
	}
	else {
		length = LENGTH / 3;
	}
}

Transition_Step::~Transition_Step()
{
	// Double check in case Alt-F4 pressed
	gfx::set_matrices(mv_save, proj_save);
	gfx::update_projection();
}

bool Transition_Step::run()
{
	Uint32 now = SDL_GetTicks();
	Uint32 elapsed = now - start_time;

	bool ret = elapsed < (Uint32)length;

	if (out && ret == false) {
		SDL_Colour colour;
		if (out ^ (BATTLE != nullptr)) {
			colour = shim::white;
		}
		else {
			colour = shim::black;
		}
		gfx::clear(shim::black);
		gfx::draw_filled_rectangle(colour, {0, 0}, shim::screen_size);
		OMNIPRESENT->draw_fore();
		gfx::flip();
		SDL_Delay(LENGTH/3);

		/*
		auto v = AREA->get_players();
		for (auto p : v) {
			p->set_visible(true);
		}
		*/

		auto v = AREA->get_current_area()->get_entities();
		for  (auto e : v) {
			e->set_was_visible();
		}
	}

	if (ret == false) {
		gfx::set_matrices(mv_save, proj_save);
		gfx::update_projection();
	}

	return ret;
}

void Transition_Step::draw_back()
{
	Uint32 now = SDL_GetTicks();
	Uint32 elapsed = now - start_time;

	float p = MIN(1.0f, elapsed / (float)length);
	bool rotate = false;
	bool scale = false;
	if (out && BATTLE == nullptr) {
		scale = true;
		if (p < back) {
			p = p / back;
		}
		else {
			if (p >= back && p < back*2) {
				p = (p - back) / back;
				p = p * 0.5f;
				p = 1.0f - p;
			}
			else if (p >= back*2) {
				//p = (p - back*2) / (1.0f - back*2);
				rotate = true;
				scale = true;
				p = 0.5f;
			}
		}
	}
	if (out == false) {
		p = 1.0f - p;
	}
	float max_scale = 4;//shim::screen_size.h / (float)shim::tile_size; // makes it zoom exactly to Pleasant's size
	util::Point<float> pos = get_pos(out);
	glm::mat4 m = mv_save;
	if (scale || rotate) {
		m = glm::translate(m, glm::vec3(pos.x, pos.y, 0.0f));
	}
	if (scale) {
		m = glm::scale(m, glm::vec3(1.0f+p*max_scale, 1.0f+p*max_scale, 1.0f));
	}
	if (rotate) {
		float p = MIN(1.0f, elapsed / (float)length);
		p = p - back*2;
		p = p / (1.0f - (back*2));
		m = glm::rotate(m, float(-p*M_PI*2*5), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (scale || rotate) {
		m = glm::translate(m, glm::vec3(-pos.x, -pos.y, 0.0f));
		gfx::set_matrices(m, proj_save);
		gfx::update_projection();
	}
}

void Transition_Step::draw_fore()
{
	// Every even, hook, every odd, draw (causes draws to happen in Omnipresent::draw_fore)
	int mod = count % 2;
	count++;
	if (mod == 0) {
		OMNIPRESENT->hook_draw_last(this);
		return;
	}

	if (out && BATTLE == nullptr && count == 2) { // count == 2 the first time it comes here because of the if above (count=0 in constructor)
		/*
		auto v = AREA->get_players();
		for (auto p : v) {
			p->set_visible(false);
		}
		*/

		auto v = AREA->get_current_area()->get_entities();
		for  (auto e : v) {
			e->set_visible(false);
		}
	}

	Uint32 now = SDL_GetTicks();
	Uint32 elapsed = now - start_time;
	float p = MIN(1.0f, elapsed / (float)length);

#if 0
	// shapes

	gfx::enable_colour_write(true);

	// set_target_image changes the projection... back it up
	glm::mat4 mv_bak, proj_bak;
	gfx::get_matrices(mv_bak, proj_bak);
	
	// Using work_image makes it pixelated
	gfx::set_target_image(GLOBALS->work_image);

	gfx::clear(shim::transparent);

	gfx::enable_depth_test(true);
	gfx::enable_depth_write(true);
	gfx::clear_depth_buffer(1.0f);
	gfx::set_depth_mode(gfx::COMPARE_NOTEQUAL);

	util::Point<float> pos = get_pos(out);

	float x = shim::screen_size.w/2.0f;
	float y = shim::screen_size.h/2.0f;
	float max = sqrt(x*x + y*y);
	// max above is dist from center of screen to corner. The modifications below increase that value based on shortest part of each shape.
	// use this for a square
	max = max / cos(M_PI/4.0f);
	// use this for a triangle
	//max = max / cos(M_PI/3.0f);
	float radius;
	if (out) {
		radius = (1.0f - p) * max;
	}
	else {
		radius = p * max;
	}

	gfx::enable_colour_write(false);

	// square
	SDL_Colour colours[3];
	colours[0] = shim::white;
	colours[1] = shim::white;
	colours[2] = shim::white;

	const int length = 333;
	Uint32 t = GET_TICKS() % length;
	/*
	if (out == false) {
		t = length - t;
	}
	*/
	float angle = (t / (float)length) * M_PI * 2.0f;
	float angle2 = angle + M_PI / 2.0f;
	float angle3 = angle2 + M_PI / 2.0f;
	float angle4 = angle3 + M_PI / 2.0f;

	float x1 = cos(angle) * radius + pos.x;
	float y1 = sin(angle) * radius + pos.y;
	float x2 = cos(angle2) * radius + pos.x;
	float y2 = sin(angle2) * radius + pos.y;
	float x3 = cos(angle3) * radius + pos.x;
	float y3 = sin(angle3) * radius + pos.y;
	float x4 = cos(angle4) * radius + pos.x;
	float y4 = sin(angle4) * radius + pos.y;

	gfx::draw_filled_triangle(colours, util::Point<float>(x1, y1), util::Point<float>(x2, y2), util::Point<float>(x4, y4));
	gfx::draw_filled_triangle(colours, util::Point<float>(x2, y2), util::Point<float>(x3, y3), util::Point<float>(x4, y4));

	// circle
	//gfx::draw_filled_circle(shim::white, pos, radius);

	// triangle
	/*
	SDL_Colour colours[3];
	colours[0] = shim::white;
	colours[1] = shim::white;
	colours[2] = shim::white;

	const int length = 333;
	Uint32 t = GET_TICKS() % length;

	float angle = (t / (float)length) * M_PI * 2.0f;
	float angle2 = angle + M_PI / 180.0f * 120.0f;
	float angle3 = angle2 + M_PI / 180.0f * 120.0f;

	float x1 = cos(angle) * radius + pos.x;
	float y1 = sin(angle) * radius + pos.y;
	float x2 = cos(angle2) * radius + pos.x;
	float y2 = sin(angle2) * radius + pos.y;
	float x3 = cos(angle3) * radius + pos.x;
	float y3 = sin(angle3) * radius + pos.y;

	gfx::draw_filled_triangle(colours, util::Point<float>(x1, y1), util::Point<float>(x2, y2), util::Point<float>(x3, y3));

	// for a second triangle (star)
	angle = ((333-t) / (float)length) * M_PI * 2.0f;
	angle2 = angle + M_PI / 180.0f * 120.0f;
	angle3 = angle2 + M_PI / 180.0f * 120.0f;

	x1 = cos(angle) * radius + pos.x;
	y1 = sin(angle) * radius + pos.y;
	x2 = cos(angle2) * radius + pos.x;
	y2 = sin(angle2) * radius + pos.y;
	x3 = cos(angle3) * radius + pos.x;
	y3 = sin(angle3) * radius + pos.y;

	gfx::draw_filled_triangle(colours, util::Point<float>(x1, y1), util::Point<float>(x2, y2), util::Point<float>(x3, y3));
	*/

	gfx::enable_colour_write(true);

	gfx::draw_filled_rectangle(shim::white, {0, 0}, shim::screen_size);

	gfx::enable_depth_test(false);
	gfx::enable_depth_write(false);
	gfx::set_depth_mode(gfx::COMPARE_LESSEQUAL);

	gfx::set_target_backbuffer();
	
	gfx::set_matrices(mv_save, proj_save);
	gfx::update_projection();
	
	GLOBALS->work_image->draw_region({0, 0}, shim::screen_size, {0, 0});
#else
	// fade
	
	gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
	gfx::update_projection();

	if (out == false) {
		p = 1.0f - p;
	}
	else if (out && BATTLE == nullptr) {
		if (p < back) {
			p = 0.0f;
		}
		else {
			p = (p - back) / (1.0f - back);
		}
	}

	// slow fade
	p = p * p;

	SDL_Colour colour;
	if (out != (BATTLE != nullptr)) {
		colour = shim::white;
	}
	else {
		colour = shim::black;
	}
	gfx::draw_filled_rectangle(make_translucent(colour, p), {0, 0}, shim::screen_size);

	gfx::set_matrices(mv_save, proj_save);
	gfx::update_projection();
#endif
}

void Transition_Step::start()
{
	start_time = SDL_GetTicks();
}

#ifdef _WIN32
void *Transition_Step::operator new(size_t i)
{
	return _mm_malloc(i,16);
}

void Transition_Step::operator delete(void* p)
{
	_mm_free(p);
}
#endif
