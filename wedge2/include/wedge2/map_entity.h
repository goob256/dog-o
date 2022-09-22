#ifndef WEDGE2_MAP_ENTITY_H
#define WEDGE2_MAP_ENTITY_H

#include "wedge2/main.h"
#include "wedge2/globals.h"
#include "wedge2/systems.h"

namespace wedge {

class Area;
class Map_Entity;
class Map_Entity_Input_Step;

class WEDGE2_EXPORT Map_Entity
{
public:
	enum Movement_Type {
		MOVEMENT_NONE = 0, // none or custom
		MOVEMENT_DEFAULT,
		MOVEMENT_WANDER,
		MOVEMENT_LOOK_AROUND
	};

	Map_Entity(std::string name);
	Map_Entity(util::JSON::Node *json);
	virtual ~Map_Entity();

	// call before start
	//--
	void set_wanders(bool can_wander_l, bool can_wander_r, bool can_wander_u, bool can_wander_d, util::Point<int> home_position, int max_dist_from_home);
	void set_looks_around(std::vector<Direction> dont_look);
	//--

	virtual bool start(Area *area);
	virtual void draw(util::Point<float> draw_offset);
	virtual bool activate(Map_Entity *activator);
	
	std::string get_name();
	void set_name(std::string name);

	util::Point<int> get_position();
	void set_position(util::Point<int> position);

	util::Point<float> get_offset();
	void set_offset(util::Point<float> offset);

	float get_speed();
	void set_speed(float speed);

	bool is_moving();
	void set_moving(bool moving);

	gfx::Sprite *get_sprite();
	void set_sprite(gfx::Sprite *sprite);
	void set_destroy_sprite(bool destroy_sprite);

	Area *get_area();
	void set_area(Area *area);

	Map_Entity_Input_Step *get_input_step();
	void set_input_step(Map_Entity_Input_Step *input_step);

	Direction get_direction();
	void set_direction(Direction direction, bool update_sprite, bool moving);

	bool is_solid();
	void set_solid(bool solid);

	util::Size<int> get_size();
	void set_size(util::Size<int> size);

	virtual std::string save();

	int get_layer();
	void set_layer(int layer);
	void put_on_middle_layer();

	bool is_visible();
	void set_visible(bool visible);
	void set_was_visible();

	bool is_jumping();
	void set_jumping(bool jumping);

	void face(Map_Entity *entity, bool moving); // turn and face entity

	void set_movement_type(Movement_Type type); // can be called before start so an input step is never created

	void set_extra_offset(util::Point<int> extra_offset);

	float get_angle();
	void set_angle(float angle);
	util::Point<float> get_pivot_offset();
	void set_pivot_offset(util::Point<float> pivot_offset);

	bool get_do_tilt();
	void set_do_tilt(bool do_tilt);
	int get_tilt_degrees();
	void set_tilt_degrees(int tilt_degrees);

	void set_shadow(bool enable, util::Size<int> size);
	void set_shadow_offset(util::Point<float> offset);
	void set_auto_shadow_pos(bool auto_shadow_pos);
	void set_sub_offset_from_shadow_pos(bool sub);

	void set_draw_flags(int draw_flags);

	void set_on_slope(bool on_slope);
	bool is_on_slope();
	
	void draw_shadow(util::Point<float> draw_offset);
	void set_shadow_layer(int layer);
	int get_shadow_layer();

protected:
	std::string name;
	util::Point<int> pos;
	util::Point<float> offset; // -1 -> 1
	float speed;
	bool moving;
	gfx::Sprite *sprite;
	Area *area;
	Map_Entity_Input_Step *input_step;
	Direction direction;
	bool solid;
	util::Size<int> size;
	bool destroy_sprite;
	int layer;
	bool visible;
	bool was_visible;
	bool jumping;
	Uint32 jump_time;
	Movement_Type movement_type;
	// wander
	bool can_wander_l;
	bool can_wander_r;
	bool can_wander_u;
	bool can_wander_d;
	util::Point<int> home_position;
	int max_dist_from_home;
	// end wander
	// look_around
	std::vector<Direction> dont_look;
	// end look_around
	util::Point<int> extra_offset;
	float angle;
	util::Point<float> pivot_offset; // offset from centre
	bool do_tilt;
	int tilt_degrees;
	bool _draw_shadow;
	util::Size<int> shadow_size;
	util::Point<float> shadow_offset;
	bool auto_shadow_pos;
	bool sub_offset_from_shadow_pos;
	int draw_flags;
	bool on_slope;
	int shadow_layer;
};

}

#endif // WEDGE2_MAP_ENTITY_H
