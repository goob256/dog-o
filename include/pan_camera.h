#ifndef PAN_CAMERA_H
#define PAN_CAMERA_H

#include <wedge2/main.h>
#include <wedge2/battle_entity.h>
#include <wedge2/systems.h>

class Pan_Camera_Step : public wedge::Step
{
public:
	Pan_Camera_Step(util::Point<float> start_offset, util::Point<float> end_offset, Uint32 duration, wedge::Task *task);
	virtual ~Pan_Camera_Step();
	
	void start();
	bool run();

private:
	Uint32 start_time;
	util::Point<float> start_offset;
	util::Point<float> end_offset;
	Uint32 duration;
};

#endif // PAN_CAMERA_H
