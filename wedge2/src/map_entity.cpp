#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/input.h"
#include "wedge2/look_around_input.h"
#include "wedge2/map_entity.h"
#include "wedge2/stats.h"
#include "wedge2/wander_input.h"

using namespace wedge;

namespace wedge {

Map_Entity::Map_Entity(std::string name) :
	name(name),
	offset(0.0f, 0.0f),
	speed(DEFAULT_WALK_SPEED),
	moving(false),
	sprite(NULL),
	area(NULL),
	input_step(NULL),
	direction(DIR_NONE),
	solid(true),
	size(1, 1),
	destroy_sprite(true),
	layer(-1),
	visible(true),
	was_visible(true),
	jumping(false),
	movement_type(MOVEMENT_DEFAULT),
	extra_offset(0, 0),
	angle(0.0f),
	pivot_offset(0.0f, 0.0f),
	do_tilt(true),
	tilt_degrees(10),
	_draw_shadow(false),
	shadow_size(0, 0),
	shadow_offset(0.0f, 0.0f),
	auto_shadow_pos(false), // get image bounds for shadow pos
	sub_offset_from_shadow_pos(false),
	draw_flags(0),
	on_slope(false),
	shadow_layer(-1)
{
}

Map_Entity::Map_Entity(util::JSON::Node *json) :
	offset(0.0f, 0.0f),
	speed(DEFAULT_WALK_SPEED),
	moving(false),
	sprite(NULL),
	area(NULL),
	input_step(NULL),
	direction(DIR_NONE),
	solid(true),
	size(1, 1),
	destroy_sprite(true),
	layer(-1),
	visible(true),
	was_visible(true),
	jumping(false),
	movement_type(MOVEMENT_DEFAULT),
	extra_offset(0, 0),
	angle(0.0f),
	pivot_offset(0.0f, 0.0f),
	do_tilt(true),
	tilt_degrees(10),
	_draw_shadow(false),
	shadow_size(0, 0),
	shadow_offset(0.0f, 0.0f),
	auto_shadow_pos(false),
	sub_offset_from_shadow_pos(false),
	draw_flags(0),
	on_slope(false),
	shadow_layer(-1)
{
	name = json->key;

	for (size_t i = 0; i < json->children.size(); i++) {
		util::JSON::Node *n = json->children[i];

		if (n->key == "position") {
			pos = json_to_integer_point(n);
		}
		else if (n->key == "speed") {
			if (INSTANCE->get_version() <= 0) {
				speed = DEFAULT_WALK_SPEED; // fixes saves saved with speed = 0.05f (the old default speed)
			}
			else {
				speed = atof(n->as_string().c_str());
			}
		}
		else if (n->key == "sprite") {
			sprite = json_to_sprite(n);
		}
		else if (n->key == "direction") {
			direction = (Direction)n->as_int();
		}
		else if (n->key == "solid") {
			solid = n->as_bool();
		}
		else if (n->key == "size") {
			size = json_to_integer_size(n);
		}
		else if (n->key == "movement_type") {
			std::string v = n->as_string();
			if (v == "wander") {
				movement_type = MOVEMENT_WANDER;
			}
			else if (v == "look_around") {
				movement_type = MOVEMENT_LOOK_AROUND;
			}
		}
		else if (n->key == "can_wander_l") {
			can_wander_l = n->as_bool();
		}
		else if (n->key == "can_wander_r") {
			can_wander_r = n->as_bool();
		}
		else if (n->key == "can_wander_u") {
			can_wander_u = n->as_bool();
		}
		else if (n->key == "can_wander_d") {
			can_wander_d = n->as_bool();
		}
		else if (n->key == "home_position") {
			home_position = json_to_integer_point(n);
		}
		else if (n->key == "max_dist_from_home") {
			max_dist_from_home = n->as_int();
		}
		else if (n->key == "can_look_e") {
			if (n->as_bool() == false) {
				dont_look.push_back(DIR_E);
			}
		}
		else if (n->key == "can_look_w") {
			if (n->as_bool() == false) {
				dont_look.push_back(DIR_W);
			}
		}
		else if (n->key == "can_look_s") {
			if (n->as_bool() == false) {
				dont_look.push_back(DIR_S);
			}
		}
		else if (n->key == "can_look_n") {
			if (n->as_bool() == false) {
				dont_look.push_back(DIR_N);
			}
		}
		else if (n->key == "layer") {
			layer = n->as_int();
		}
		else if (n->key == "visible") {
			was_visible = visible;
			visible = n->as_bool();
		}
		else if (n->key == "extra_offset") {
			extra_offset = json_to_integer_point(n);
		}
		else if (n->key == "angle") {
			angle = n->as_float();
		}
		else if (n->key == "pivot_offset") {
			pivot_offset = json_to_float_point(n);
		}
		else if (n->key == "do_tilt") {
			do_tilt = n->as_bool();
		}
		else if (n->key == "tilt_degrees") {
			tilt_degrees = n->as_int();
		}
		else if (n->key == "draw_shadow") {
			_draw_shadow = n->as_bool();
		}
		else if (n->key == "shadow_size") {
			shadow_size = json_to_integer_size(n);
		}
		else if (n->key == "shadow_offset") {
			shadow_offset = json_to_float_point(n);
		}
		else if (n->key == "auto_shadow_pos") {
			auto_shadow_pos = n->as_bool();
		}
		else if (n->key == "sub_offset_from_shadow_pos") {
			sub_offset_from_shadow_pos = n->as_bool();
		}
		else if (n->key == "draw_flags") {
			draw_flags = n->as_int();
		}
		else if (n->key == "shadow_layer") {
			shadow_layer = n->as_int();
		}
	}
}

Map_Entity::~Map_Entity()
{
	if (destroy_sprite) {
		delete sprite;
	}
	if (input_step) {
		input_step->die();
	}
}

void Map_Entity::set_wanders(bool can_wander_l, bool can_wander_r, bool can_wander_u, bool can_wander_d, util::Point<int> home_position, int max_dist_from_home)
{
	movement_type = MOVEMENT_WANDER;
	this->can_wander_l = can_wander_l;
	this->can_wander_r = can_wander_r;
	this->can_wander_u = can_wander_u;
	this->can_wander_d = can_wander_d;
	this->home_position = home_position;
	this->max_dist_from_home = max_dist_from_home;
}

void Map_Entity::set_looks_around(std::vector<Direction> dont_look)
{
	movement_type = MOVEMENT_LOOK_AROUND;
	this->dont_look = dont_look;
}

bool Map_Entity::start(Area *area)
{
	this->area = area;

	if (movement_type != MOVEMENT_NONE) {
		System *new_system = area->get_entity_movement_system();
		Task *new_task = new Task(new_system);

		switch (movement_type) {
			case MOVEMENT_DEFAULT:
				input_step = new Map_Entity_Input_Step(this, new_task);
				break;
			case MOVEMENT_WANDER:
				input_step = new Wander_Input_Step(this, can_wander_l, can_wander_r, can_wander_u, can_wander_d, home_position, max_dist_from_home, new_task);
				break;
			case MOVEMENT_LOOK_AROUND:
				input_step = new Look_Around_Input_Step(this, dont_look, new_task);
				break;
			default:
				break;
		}

		ADD_STEP(input_step)
		ADD_TASK(new_task)
	}

	if (layer == -1) {
		put_on_middle_layer();
	}

	return true;
}

std::string Map_Entity::get_name()
{
	return name;
}

void Map_Entity::set_name(std::string name)
{
	this->name = name;
}

util::Point<int> Map_Entity::get_position()
{
	return pos;
}

void Map_Entity::set_position(util::Point<int> position)
{
	pos = position;
}

util::Point<float> Map_Entity::get_offset()
{
	return offset;
}

void Map_Entity::set_offset(util::Point<float> offset)
{
	this->offset = offset;
}

float Map_Entity::get_speed()
{
	if (jumping) {
		std::vector<Uint32> delays = sprite->get_frame_times();
		std::string anim = sprite->get_animation();
		if (delays.size() < 3 || anim.substr(0, 4) != "jump") {
			return speed;
		}
		Uint32 now = GET_TICKS();
		Uint32 elapsed = now - jump_time;
		Uint32 motion = delays[0] + delays[1]; // first two frames are jump motion
		if (elapsed < motion) {
			return 0.0f;
		}
		Uint32 leap = 0; // length of all frames except first 2
		for (size_t i = 2; i < delays.size(); i++) {
			leap += delays[i];
		}
		return 16.0f/*16 ms per frame at 60fps*/ / leap;
	}
	return speed;
}

void Map_Entity::set_speed(float speed)
{
	this->speed = speed;
}

bool Map_Entity::is_moving()
{
	return moving;
}

void Map_Entity::set_moving(bool moving)
{
	this->moving = moving;
}

gfx::Sprite *Map_Entity::get_sprite()
{
	return sprite;
}

void Map_Entity::set_sprite(gfx::Sprite *sprite)
{
	this->sprite = sprite;
}

void Map_Entity::set_destroy_sprite(bool destroy_sprite)
{
	this->destroy_sprite = destroy_sprite;
}

Area *Map_Entity::get_area()
{
	return area;
}

void Map_Entity::draw(util::Point<float> draw_offset)
{
	if (visible == false || sprite == NULL) {
		return;
	}

	draw_offset += extra_offset;

	util::Point<float> p = (offset + pos) * shim::tile_size;
	gfx::Image *image = sprite->get_current_image();

	p.y -= (image->size.h - shim::tile_size);

	bool in_water = false;
	std::vector< util::Rectangle<int> > water = area->get_water();
	for (auto &r : water) {
		if (r.contains(pos)) {
			in_water = true;
			break;
		}
	}

	if (in_water) {
		float wh = area->get_water_height();

		if (shim::opengl) {
			glEnable_ptr(GL_SCISSOR_TEST);
			int h = (draw_offset.y+p.y+shim::tile_size-wh-extra_offset.y)*shim::scale+shim::screen_offset.y;
			glScissor_ptr(0, shim::real_screen_size.h-h, shim::screen_size.w*shim::scale+shim::screen_offset.x, h);
			PRINT_GL_ERROR("glScissor");
		}
#ifdef _WIN32
		else {
			shim::d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			RECT scissor = { 0, 0, (LONG)(shim::screen_size.w*shim::scale+shim::screen_offset.x), (LONG)((draw_offset.y+p.y+shim::tile_size-wh-extra_offset.y)*shim::scale+shim::screen_offset.y) };
			shim::d3d_device->SetScissorRect(&scissor);
		}
#endif
	}

	util::Point<float> pivot = util::Point<float>(shim::tile_size/2.0f, shim::tile_size/2.0f)+pivot_offset;
	image->draw_rotated(pivot, draw_offset+p+pivot, angle, draw_flags);

	if (in_water) {
		gfx::unset_scissor();
	}

	int index = -1;
	std::vector<Map_Entity *> players = AREA->get_players();
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i] == this) {
			index = (int)i;
			break;
		}
	}
	if (index >= 0 && INSTANCE->stats[index].base.status != STATUS_OK) {
		if (INSTANCE->stats[index].base.status == STATUS_POISONED) {
			gfx::Image *poison_image = globals->poison_sprite->get_current_image();
			util::Point<int> topleft, bottomright;
			sprite->get_bounds(topleft, bottomright);
			util::Point<int> sz = bottomright - topleft;
			poison_image->draw(draw_offset+p+topleft+sz/2-poison_image->size/2);
		}
		else {
			GLOBALS->draw_custom_status(this, INSTANCE->stats[index].base.status, draw_offset+p);
		}
	}
}

void Map_Entity::draw_shadow(util::Point<float> draw_offset)
{
	if (visible == false || sprite == NULL) {
		return;
	}

	draw_offset += extra_offset;

	util::Point<float> p = (offset + pos) * shim::tile_size;
	gfx::Image *image = sprite->get_current_image();

	p.y -= (image->size.h - shim::tile_size);

	bool in_water = false;
	std::vector< util::Rectangle<int> > water = area->get_water();
	for (auto &r : water) {
		if (r.contains(pos)) {
			in_water = true;
			break;
		}
	}

	if (in_water == false && _draw_shadow) {
		util::Point<float> shadow_pos = draw_offset + p;
		if (auto_shadow_pos) {
			util::Point<int> topleft, bottomright;
			image->get_bounds(topleft, bottomright);
			util::Point<float> p(0.0f, 0.0f);
			p += topleft;
			p += util::Point<float>((bottomright.x-topleft.x)/2.0f, bottomright.y-topleft.y/*sub topleft.y because it's added above*/);
			if (draw_flags & gfx::Image::FLIP_H) {
				p.x = image->size.w - p.x;
			}
			shadow_pos += p;
		}
		else {
			shadow_pos += util::Point<float>(shim::tile_size/2.0f, image->size.h) + shadow_offset;
		}
		if (sub_offset_from_shadow_pos) {
			shadow_pos.y -= offset.y * shim::tile_size;
		}
		SDL_Colour tint = shim::black;
		tint.r *= 0.333f;
		tint.g *= 0.333f;
		tint.b *= 0.333f;
		tint.a *= 0.333f;
		int w = size.w * shim::tile_size - shim::tile_size;
		shadow_pos.x += w/2;
		gfx::Shader *tmp_shader = shim::current_shader;
		shim::current_shader = GLOBALS->pixelate_shader;
		shim::current_shader->use();
		gfx::update_projection();
		shim::current_shader->set_int("draw_w", shadow_size.w);
		shim::current_shader->set_int("draw_h", shadow_size.h);
		GLOBALS->shadow->stretch_region_tinted(tint, {0, 0}, GLOBALS->shadow->size, shadow_pos-shadow_size/2, shadow_size);
		shim::current_shader = tmp_shader;
		shim::current_shader->use();
		gfx::update_projection();
	}
}

Map_Entity_Input_Step *Map_Entity::get_input_step()
{
	return input_step;
}

void Map_Entity::set_input_step(Map_Entity_Input_Step *input_step)
{
	this->input_step = input_step;
}

Direction Map_Entity::get_direction()
{
	return direction;
}

void Map_Entity::set_direction(Direction direction, bool update_sprite, bool moving)
{
	this->direction = direction;

	std::string new_anim;

	if (update_sprite) {
		if (moving) {
			switch (direction) {
				case DIR_N:
					new_anim = "walk_n";
					break;
				case DIR_E:
					if (jumping) {
						new_anim = "jump_e";
					}
					else {
						new_anim = "walk_e";
					}
					break;
				case DIR_S:
					new_anim = "walk_s";
					break;
				case DIR_W:
					if (jumping) {
						new_anim = "jump_w";
					}
					else {
						new_anim = "walk_w";
					}
					break;
				default:
					break;
			}
		}
		else {
			switch (direction) {
				case DIR_N:
					new_anim = "stand_n";
					break;
				case DIR_E:
					new_anim = "stand_e";
					break;
				case DIR_S:
					new_anim = "stand_s";
					break;
				case DIR_W:
					new_anim = "stand_w";
					break;
				default:
					break;
			}
		}
	}

	if (new_anim != "") {
		if (sprite->get_animation() != new_anim) {
			sprite->set_animation(new_anim);
		}
	}
}

void Map_Entity::set_area(Area *area)
{
	this->area = area;
}

bool Map_Entity::is_solid()
{
	return solid;
}

void Map_Entity::set_solid(bool solid)
{
	this->solid = solid;
}

util::Size<int> Map_Entity::get_size()
{
	return size;
}

void Map_Entity::set_size(util::Size<int> size)
{
	this->size = size;
}

bool Map_Entity::activate(Map_Entity *activator)
{
	return false;
}

std::string Map_Entity::save()
{
	std::string s;
	if (movement_type == MOVEMENT_WANDER) {
		s += util::string_printf("\"movement_type\": \"wander\",");
		s += util::string_printf("\"can_wander_l\": %s,", bool_to_string(can_wander_l).c_str());
		s += util::string_printf("\"can_wander_r\": %s,", bool_to_string(can_wander_r).c_str());
		s += util::string_printf("\"can_wander_u\": %s,", bool_to_string(can_wander_u).c_str());
		s += util::string_printf("\"can_wander_d\": %s,", bool_to_string(can_wander_d).c_str());
		s += util::string_printf("\"home_position\": %s,", integer_point_to_string(home_position).c_str());
		s += util::string_printf("\"max_dist_from_home\": %d,", max_dist_from_home);
	}
	else if (movement_type == MOVEMENT_LOOK_AROUND) {
		s += util::string_printf("\"movement_type\": \"look_around\",");
		for (size_t i = 0; i < dont_look.size(); i++) {
			std::string d;
			if (dont_look[i] == DIR_E) {
				d = "e";
			}
			else if (dont_look[i] == DIR_W) {
				d = "w";
			}
			else if (dont_look[i] == DIR_N) {
				d = "n";
			}
			else if (dont_look[i] == DIR_S) {
				d = "s";
			}
			if (d != "") {
				s += "\"can_look_" + d + "\": false,";
			}
		}
	}
	s += util::string_printf("\"position\": %s,", integer_point_to_string(pos).c_str());
	s += util::string_printf("\"speed\": %f,", speed); // FIXME: will this lose precision over time?
	if (sprite != NULL) {
		s += util::string_printf("\"sprite\": %s,", sprite_to_string(sprite).c_str());
	}
	s += util::string_printf("\"direction\": %d,", (int)direction);
	s += util::string_printf("\"solid\": %s,", bool_to_string(solid).c_str());
	s += util::string_printf("\"size\": %s,", integer_size_to_string(size).c_str());
	s += util::string_printf("\"layer\": %d,", layer);
	s += util::string_printf("\"visible\": %s,", bool_to_string(visible).c_str());
	s += util::string_printf("\"extra_offset\": %s,", integer_point_to_string(extra_offset).c_str());
	s += util::string_printf("\"angle\": %f,", angle);
	s += util::string_printf("\"pivot_offset\": %s,", float_point_to_string(pivot_offset).c_str());
	s += util::string_printf("\"do_tilt\": %s,", bool_to_string(do_tilt).c_str());
	s += util::string_printf("\"tilt_degrees\": %d,", tilt_degrees);
	s += util::string_printf("\"draw_shadow\": %s,", bool_to_string(_draw_shadow).c_str());
	s += util::string_printf("\"shadow_size\": %s,", integer_size_to_string(shadow_size).c_str());
	s += util::string_printf("\"shadow_offset\": %s,", float_point_to_string(shadow_offset).c_str());
	s += util::string_printf("\"auto_shadow_pos\": %s,", bool_to_string(auto_shadow_pos).c_str());
	s += util::string_printf("\"sub_offset_from_shadow_pos\": %s,", bool_to_string(sub_offset_from_shadow_pos).c_str());
	s += util::string_printf("\"draw_flags\": %d,", draw_flags);
	s += util::string_printf("\"shadow_layer\": %d,", shadow_layer);
	return s;
}

int Map_Entity::get_layer()
{
	return layer;
}

void Map_Entity::set_layer(int layer)
{
	this->layer = layer;
}

void Map_Entity::put_on_middle_layer()
{
	layer = area->get_middle_layer();
}

bool Map_Entity::is_visible()
{
	return visible;
}

void Map_Entity::set_visible(bool visible)
{
	this->was_visible = this->visible;
	this->visible = visible;
}

void Map_Entity::set_was_visible()
{
	this->visible = this->was_visible;
}

bool Map_Entity::is_jumping()
{
	return jumping;
}

void Map_Entity::set_jumping(bool jumping)
{
	this->jumping = jumping;
	if (jumping) {
		jump_time = GET_TICKS();
	}
}

void Map_Entity::face(Map_Entity *entity, bool moving)
{
	util::Point<int> diff = entity->get_position() - get_position();
	Direction dir = direction_from_offset(diff);
	set_direction(dir, true, moving);
}

void Map_Entity::set_movement_type(Movement_Type type)
{
	movement_type = type;
}

void Map_Entity::set_extra_offset(util::Point<int> extra_offset)
{
	this->extra_offset = extra_offset;
}

float Map_Entity::get_angle()
{
	return angle;
}

void Map_Entity::set_angle(float angle)
{
	this->angle = angle;
}

util::Point<float> Map_Entity::get_pivot_offset()
{
	return pivot_offset;
}

void Map_Entity::set_pivot_offset(util::Point<float> pivot_offset)
{
	this->pivot_offset = pivot_offset;
}

bool Map_Entity::get_do_tilt()
{
	return do_tilt;
}

void Map_Entity::set_do_tilt(bool do_tilt)
{
	this->do_tilt = do_tilt;
}

int Map_Entity::get_tilt_degrees()
{
	return tilt_degrees;
}

void Map_Entity::set_tilt_degrees(int tilt_degrees)
{
	this->tilt_degrees = tilt_degrees;
}

void Map_Entity::set_shadow(bool enable, util::Size<int> size)
{
	_draw_shadow = enable;
	shadow_size = size;
}

void Map_Entity::set_shadow_offset(util::Point<float> offset)
{
	shadow_offset = offset;
}
	
void Map_Entity::set_auto_shadow_pos(bool auto_shadow_pos)
{
	this->auto_shadow_pos = auto_shadow_pos;
}
	
void Map_Entity::set_sub_offset_from_shadow_pos(bool sub)
{
	sub_offset_from_shadow_pos = sub;
}

void Map_Entity::set_draw_flags(int draw_flags)
{
	this->draw_flags = draw_flags;
}

void Map_Entity::set_on_slope(bool on_slope)
{
	this->on_slope = on_slope;
}

bool Map_Entity::is_on_slope()
{
	return on_slope;
}

void Map_Entity::set_shadow_layer(int layer)
{
	shadow_layer = layer;
}

int Map_Entity::get_shadow_layer()
{
	return shadow_layer;
}

}
