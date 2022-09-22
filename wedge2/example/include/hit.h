#ifndef HIT_H
#define HIT_H

#include <wedge2/main.h>
#include <wedge2/battle_entity.h>
#include <wedge2/systems.h>

class Hit_Step : public wedge::Step
{
public:
	Hit_Step(wedge::Battle_Entity *entity, int delay, wedge::Task *task);
	virtual ~Hit_Step();
	
	bool run();
	void draw();

private:
	Uint32 start_time;
	wedge::Battle_Entity *entity;
	gfx::Sprite *sprite;
	util::Size<int> hit_size;
	bool done;
	bool started;
	std::string prev_anim;
	int flags;
	gfx::Sprite *entity_sprite;
	util::Point<int> draw_pos;
};

#endif // HIT_H
