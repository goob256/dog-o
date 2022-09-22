#include "battle_game.h"
#include "coin.h"
#include "general.h"
#include "globals.h"

Coin_Step::Coin_Step(util::Point<float> pos, wedge::Task *task) :
	wedge::Step(task),
	pos(pos)
{
	bool left = util::rand(0, 1) == 0;
	velocity.x = util::rand(0, 1000)/1000.0f * 1.5f * (left ? -1 : 1);
	velocity.y = -1.0f;

	sprite = new gfx::Sprite("coin");
	sprite->set_rand_start(true);
	sprite->set_animation("only");
}

Coin_Step::~Coin_Step()
{
	delete sprite;
}
	
void Coin_Step::start()
{
	start_time = GET_TICKS();
}

bool Coin_Step::run()
{
	Uint32 now = GET_TICKS();

	pos += velocity;
	velocity.x *= 0.95f;
	velocity.y += 0.15f;

	if (now-start_time >= (unsigned)LIFETIME) {
		return false;
	}

	return true;
}

void Coin_Step::draw_fore()
{
	gfx::Image *img = sprite->get_current_image();
	float alpha;
	Uint32 now = GET_TICKS();
	Uint32 elapsed = now - start_time;
	if (elapsed < LIFETIME/3*2) {
		alpha = 1.0f;
	}
	else {
		int t = elapsed - LIFETIME/3*2;
		float p = t / (LIFETIME/3.0f);
		if (p > 1.0f) {
			p = 1.0f;
		}
		p = 1.0f - p;
		alpha = p;
	}
	util::Point<float> pos2 = pos;
	if (static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
		pos2.x = shim::screen_size.w - pos2.x - img->size.w;
	}
	img->draw_tinted(make_translucent(shim::white, alpha), pos2);
}
