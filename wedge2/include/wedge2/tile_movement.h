#ifndef WEDGE2_TILE_MOVEMENT_H
#define WEDGE2_TILE_MOVEMENT_H

#include "wedge2/globals.h"
#include "wedge2/systems.h"

namespace wedge {

class Map_Entity;

class WEDGE2_EXPORT Tile_Movement_Step : public Step
{
public:
	Tile_Movement_Step(Map_Entity *entity, Direction direction, Task *task);
	virtual ~Tile_Movement_Step();
	
	bool run();

	// call this from done_signal() to keep going/change to 'direction'
	// or call it if not moving to start moving in 'direction'
	void set_next_direction(Direction direction);

	bool is_moving();
	void set_moving(bool moving);

	void set_increment(util::Point<float> increment);

	bool hit_wall();

	void delay_movement(int frames);

	bool is_waiting_for_next_direction();

	void set_try_tile(util::Point<int> tile);
	
	void set_next_path(util::Point<int> tile);

protected:
	void go(Direction direction);

	Map_Entity *entity;
	util::Point<int> start_tile;
	util::Point<float> increment;
	util::Point<float> initial_offset;
	bool moving;
	Direction next_direction;
	bool waiting_for_next_direction;
	bool _hit_wall;
	int movement_delay;

	util::Point<int> tile_to_try;
	bool do_try_tile;

	util::Point<int> next_path;
	bool do_next_path;
};

}

#endif // WEDGE2_TILE_MOVEMENT_H
