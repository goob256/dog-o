#include <wedge2/globals.h>

#include "globals.h"
#include "multi.h"

Multi_Step::Multi_Step(int level, wedge::Task *task) :
	wedge::Step(task),
	level(level)
{
}

Multi_Step::~Multi_Step()
{
}

void Multi_Step::start()
{
	start_time = GET_TICKS();
}

bool Multi_Step::run()
{
	if (GET_TICKS() >= start_time + LENGTH) {
		return false;
	}
	return true;
}

void Multi_Step::draw_fore()
{
	gfx::Font::end_batches();

	gfx::set_target_image(GLOBALS->work_image);
	gfx::clear(shim::transparent);

	std::string text = GLOBALS->game_t->translate(392 + level/* Originally: MULTIPLIER+ */ /* or ++/+++ */);

	int w = DOGO_GLOBALS->multi_font->get_text_width(text);

	SDL_Colour c;
	float extra = 0.0f;;

	switch (level) {
		case 1:
			c = shim::palette[38];
			extra = 0.75f;
			break;
		case 2:
			c = shim::palette[17];
			extra = 1.0f;
			break;
		case 3:
			c = shim::palette[23];
			extra = 1.5f;
			break;
	}
	
	float p0 = ((GET_TICKS() - start_time) / (float)LENGTH);
	if (p0 > 1.0f) {
		p0 = 1.0f;
	}

	float p = 0.5f + extra * p0;

	int h = DOGO_GLOBALS->multi_font->get_height();

	int dw = (w+2) * p;
	int dh = (h+2) * p;

	util::Point<float> dest(float(shim::screen_size.w/2-dw/2), float(shim::screen_size.h*0.05f+DOGO_GLOBALS->mini_pics["pleasant"]->size.h+2));

	DOGO_GLOBALS->multi_font->enable_shadow(c, gfx::Font::FULL_SHADOW);
	DOGO_GLOBALS->multi_font->draw(shim::white, text, {1, 1});
	DOGO_GLOBALS->multi_font->disable_shadow();

	gfx::Font::end_batches();

	gfx::set_target_backbuffer();

	p0 *= p0;
	float p_inv = 1.0f - p0;

	SDL_Colour tint = shim::white;
	tint.r *= p_inv;
	tint.g *= p_inv;
	tint.b *= p_inv;
	tint.a *= p_inv;

	GLOBALS->work_image->stretch_region_tinted(tint, {0.0f, 0.0f}, {w+2, h+2}, dest, {dw, dh});
}
