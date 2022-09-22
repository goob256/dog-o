#ifndef COIN_H
#define COIN_H

#include <wedge2/main.h>
#include <wedge2/battle_entity.h>
#include <wedge2/systems.h>

class Coin_Step : public wedge::Step
{
public:
	static const int LIFETIME = 333;

	Coin_Step(util::Point<float> pos, wedge::Task *task);
	virtual ~Coin_Step();
	
	void start();
	bool run();
	void draw_fore();

private:
	Uint32 start_time;
	util::Point<float> pos;
	util::Point<float> velocity;
	gfx::Sprite *sprite;
};

#endif // COIN_H
