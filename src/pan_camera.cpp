#include "area_game.h"
#include "pan_camera.h"

Pan_Camera_Step::Pan_Camera_Step(util::Point<float> start_offset, util::Point<float> end_offset, Uint32 duration, wedge::Task *task) :
	wedge::Step(task),
	start_offset(start_offset),
	end_offset(end_offset),
	duration(duration)
{
}

Pan_Camera_Step::~Pan_Camera_Step()
{
}
	
void Pan_Camera_Step::start()
{
	start_time = GET_TICKS();
}

bool Pan_Camera_Step::run()
{
	Area_Game *area_game = static_cast<Area_Game *>(AREA);

	Uint32 now = GET_TICKS();
	Uint32 elapsed = now - start_time;

	if (elapsed >= duration) {
		area_game->set_camera(end_offset);
		return false;
	}

	float p = elapsed / (float)duration;

	util::Point<float> camera = start_offset;
	camera.x += (end_offset.x - start_offset.x) * p;
	camera.y += (end_offset.y - start_offset.y) * p;

	area_game->set_camera(camera);

	return true;
}
