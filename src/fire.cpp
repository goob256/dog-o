#include <wedge2/general.h>

#include "fire.h"
#include "general.h"
#include "globals.h"

#define SPEED 2.0f
#define STRAIGHTEN1 125
#define STRAIGHTEN2 100
#define SAVE_ANGLES 25

struct Draw_Info
{
	int colour;
	util::Point<float> pos;
	Fire_Step::Fire_Particle f;
};

static bool compare_draw_info(const Draw_Info &a, const Draw_Info &b)
{
	//return b.pos.y < a.pos.y; // reverse sort
	return b.pos.y > a.pos.y;
}

Fire_Step::Fire_Step(util::Point<int> draw_pos_bottom_middle, wedge::Battle_Entity *target, SDL_Colour c1, SDL_Colour c2, SDL_Colour c3, wedge::Task *task, bool is_fatal) :
	wedge::Step(task),
	draw_pos(draw_pos_bottom_middle),
	changed(false),
	ended(false),
	target(target),
	is_fatal(is_fatal)
{
	colour1 = c1;
	colour2 = c2;
	colour3 = c3;

	speed = SPEED;

	start_angle = (float)M_PI/2.0f;
	curr_angle = start_angle;
	for (int i = 0; i < SAVE_ANGLES; i++) {
		last_angles.push_back(curr_angle);
	}
	end_pos = draw_pos;
		
	curr_pos = draw_pos;
	move_end = 0;
	start_time = GET_TICKS();

	total_fire_particles = 2000;
	fire_size = util::Size<int>(6, 32);
	num_fire_particles = fire_size.w * fire_size.h * 2.0f;
	added_fire_particles = 0;
	veers = util::rand(0, 5);
	fire_to_add = num_fire_particles;

	/*
	for (int i = 33; i >= 22; i--) {
		fire_colours.push_back(shim::palette[i]);
	}
	for (int i = 8; i <= 17; i++) {
		fire_colours.push_back(shim::palette[i]);
	}
	*/
	fire_colours.push_back(colour1);
	fire_colours.push_back(colour1);
	fire_colours.push_back(colour1);
	fire_colours.push_back(colour1);
	fire_colours.push_back(colour1);
	int nc = 15;
	for (int i = 0; i < nc; i++) {
		SDL_Colour c = blend_colours(colour2, colour1, i/(float)nc);
		fire_colours.push_back(c);
	}
	nc = 10;
	for (int i = 0; i < nc; i++) {
		SDL_Colour c = blend_colours(colour3, colour2, i/(float)nc);
		fire_colours.push_back(c);
	}
	

	for (int i = 0; i < total_fire_particles; i++) {
		add_fire_particle();
	}

	bucket = fire;
	fire.clear();

	fire_it = bucket.begin();

	added_fire_particles = 0;
}

Fire_Step::~Fire_Step()
{
}

bool Fire_Step::run()
{
	if (changed) {
		if (ended == false) {
			util::Point<float> diff = (end_pos-curr_pos);
			float len = diff.length();
			if (len <= speed) {
				curr_pos = end_pos;
				move_end = GET_TICKS();
				ended = true;
				auto v = BATTLE->get_enemies();
				add_special_number(v[0], target, target->get_stats()->hp-(is_fatal ? 0 : 1), false, true);
				wedge::rumble(1.0f, 500);
				gfx::screen_shake(0.75f, 350);
			}
			else {
				float angle = diff.angle();
				curr_pos.x += cos(angle) * speed;
				curr_pos.y += sin(angle) * speed;
			}
		}
		else {
			util::Point<float> diff = (end_pos-draw_pos);
			float angle = diff.angle();
			if (diff.x > 0) {
				angle -= float(M_PI / 4.0f);
			}
			else if (diff.x < 0) {
				angle += float(M_PI / 4.0f);
			}
			curr_pos.x += cos(angle) * speed * 0.1f;
			curr_pos.y += sin(angle) * speed * 0.1f;
			float minx = 1000000.0f;
			float maxx = -1000000.0f;
			for (auto &f : fire) {
				if (f.initial_x < minx) {
					minx = f.initial_x;
				}
				if (f.initial_x > maxx) {
					maxx = f.initial_x;
				}
			}
			for (auto &f : fire) {
				float p = (f.initial_x - minx) / (maxx-minx);
				p -= 0.5f;
				if (p > 0.0f) {
					p *= 2.0f;
				}
				//p = 1.0f - ((1.0f - p) * (1.0f - p));
				f.offset.x += cos(angle) * (p < 0.0f ? -p : p) * 2.0f;
				f.offset.y += sin(angle) * p * 4.0f;
			}
		}
		/*
		else if (now-move_end < STRAIGHTEN1+STRAIGHTEN2) {
			Uint32 e = now-move_end;
			if (e < STRAIGHTEN1) {
				float diff = (start_angle+(M_PI*5/6*(end_pos.x<draw_pos.x?-1:1))) - start_angle;
				float p = e / (float)STRAIGHTEN1;
				p += util::rand(0, 100) / 750.0f;
				curr_angle = start_angle + sin(p*M_PI/2) * diff;
			}
			else {
				e -= STRAIGHTEN1;
				float diff = ((float)M_PI/2.0f) - (start_angle+(M_PI*5/6*(end_pos.x<draw_pos.x?-1:1)));
				float p = e / (float)STRAIGHTEN2;
				p += util::rand(0, 100) / 500.0f;
				curr_angle = (start_angle+(M_PI*5/6*(end_pos.x<draw_pos.x?-1:1))) + sin(p*M_PI/2) * diff;
			}
		}
		else {
			curr_angle = (float)M_PI/2.0f;
		}
		*/
	}

	last_angles.erase(last_angles.begin());
	last_angles.push_back(curr_angle);
			
	for (std::list<Fire_Particle>::iterator it = fire.begin(); it != fire.end();) {
		Fire_Particle &f = *it;
		float mul = ended ? 5.0f : 1.0f;
		f.p += f.speed * mul;
		f.start_x += f.veeriness;
		if (f.p >= 1.0f) {
			it = fire.erase(it);
		}
		else {
			it++;
		}
	}

	int add = MIN(25, fire_to_add);

	if (add > 0) {
		for (int i = 0; i < add; i++) {
			add_from_bucket();
		}

		fire_to_add -= add;
	}
	else {
		int to_add = num_fire_particles - (int)fire.size();

		for (int i = 0; i < to_add; i++) {
			add_from_bucket();
		}
	}

	bool done = added_fire_particles >= total_fire_particles && fire.size() == 0;
	
	if (done) {
		send_done_signal();
	}

	return !done;
}

void Fire_Step::draw_fire(bool draw_blurred)
{
	glm::mat4 mv_save, proj_save;
	gfx::get_matrices(mv_save, proj_save);

	if (draw_blurred) {
		gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, 1.0f);
		gfx::update_projection();
	}

	std::vector<Draw_Info> draw_info;

	int index = 0;
	for (std::list<Fire_Particle>::iterator it = fire.begin(); it != fire.end(); it++) {
		Fire_Particle &f = *it;
		Draw_Info i;
		get_draw_info(index, f, i.colour, i.pos);
		i.f = f;
		draw_info.push_back(i);
		index++;
	}
	
	std::sort(draw_info.begin(), draw_info.end(), compare_draw_info);
	std::reverse(draw_info.begin(), draw_info.end());
	
	float alpha;
	Uint32 now = GET_TICKS();
	const int fade_in_duration = 500;
	if (move_end != 0) {
		Uint32 diff = now - move_end;
		const int duration = 350;
		alpha = MIN(duration, (int)diff) / (float)duration;
		alpha = 1.0f - alpha;
	}
	else if (now - start_time < (Uint32)fade_in_duration) {
		alpha = (now - start_time) / (float)fade_in_duration;
	}
	else {
		alpha = 1.0f;
	}
   
   	for (int this_sprite = 0; this_sprite < 3; this_sprite++) {
		// Batching works because only the first loaded sprite has real (non-sub) images
		gfx::Sprite *sprite;
		if (this_sprite == 0) {
			sprite = DOGO_GLOBALS->fire_big;
		}
		else if (this_sprite == 1) {
			sprite = DOGO_GLOBALS->fire_medium;
		}
		else {
			sprite = DOGO_GLOBALS->fire_small;
		}
		gfx::Image *img = sprite->get_current_image();
		if (draw_blurred == false) {
			img->start_batch();
		}
		// FIXME: need batching for draw_blurred == true
		for (size_t i = 0; i < draw_info.size(); i++) {
			Draw_Info &d = draw_info[i];
			int s;
			if (d.f.p >= 0.667f) {
				s = 2;
			}
			else if (d.f.p >= 0.333f) {
				s = 1;
			}
			else {
				s = 0;
			}
			if (s != this_sprite) {
				continue;
			}
			if (draw_blurred) {
				gfx::Image *img = DOGO_GLOBALS->fire_blurred[this_sprite][sprite->get_current_frame()];
				//img->draw_tinted_rotated(make_translucent(fire_colours[d.colour], alpha), {img->size.w/2.0f, img->size.h/2.0f}, d.pos * shim::scale, curr_angle-float(M_PI/2.0f));
				img->draw_tinted_rotated(make_translucent(fire_colours[d.colour], alpha), {img->size.w/2.0f, img->size.h/2.0f}, d.pos * shim::scale, curr_angle);
			}
			else {
				gfx::Image *img = sprite->get_current_image();
				img->draw_tinted_rotated(fire_colours[d.colour], {img->size.w/2.0f, img->size.h/2.0f}, d.pos, curr_angle-float(M_PI/2.0f));
			}
		}
		if (draw_blurred == false) {
			img->end_batch();
		}
	}
	
	if (draw_blurred) {
		gfx::set_matrices(mv_save, proj_save);
		gfx::update_projection();
	}
}

void Fire_Step::draw_fore()
{
	draw_fire(true);
	//draw_fire(false);
}

void Fire_Step::add_fire_particle()
{
	if (added_fire_particles >= total_fire_particles) {
		return;
	}

	if (veers > 0) {
		if (added_fire_particles % (total_fire_particles / veers) == 0) {
			veer_dir = util::rand(0, 2) - 1;
		}
	}
	else {
		veer_dir = 0;
	}

	added_fire_particles++;

	Fire_Particle f;

	f.offset = {0.0f, 0.0f};
	f.max_h = util::rand(fire_size.h/2, fire_size.h);
	//f.p = 0.25f * util::rand(0, 1000) / 1000.0f * (1.0f - total_p);
	f.p = 0.0f;//0.1f * util::rand(0, 1000) / 1000.0f * (1.0f - total_p);
	//f.start_heat = (util::rand(0, 1000) / 1000.0f) * 0.5f + 0.5f;
	f.sin_offset = (util::rand(0, 1000) / 1000.0f) * 2 * M_PI;
	f.waviness = util::rand(0, 1000) / 1000.0f;
	f.waves = (1.0f - f.waviness) * MAX_WAVES;
	const float max_speed_pps = 40;
	const float min_speed_pps = 15;
	float speed_pps = min_speed_pps + (util::rand(0, 1000) / 1000.0f) * (max_speed_pps - min_speed_pps);
	float speed_ppf = speed_pps / shim::logic_rate;
	f.speed = speed_ppf / f.max_h;
	float x_p = util::rand(0, 1000) / 1000.0f;
	x_p *= x_p;
	x_p = (0.5f * x_p) + util::rand(0, 1000) / 1000.0f * 0.5f;
	f.start_x = fire_size.w * x_p;
	f.initial_x = f.start_x;
	f.start_heat = util::rand(0, 1000) / 1000.0f * 0.2f;
	f.veeriness = veer_dir * (util::rand(0, 1000) / 1000.0f) * 0.1f;

	fire.push_back(f);
}

void Fire_Step::get_draw_info(int i, Fire_Particle &f, int &colour, util::Point<float> &pos)
{
	float p = 1.0f - sin((1.0f-f.p) * M_PI/2.0f);
	int index = last_angles.size() - (p * last_angles.size());
	if (index < 0) {
		index = 0;
	}
	if (index >= (int)last_angles.size()) {
		index = (int)last_angles.size() - 1;
	}
	float ca = last_angles[index];
	float a2 = ca + (float)M_PI/2.0f;
	pos = util::Point<float>(curr_pos.x-cos(a2)*fire_size.w/2, curr_pos.y-sin(a2)*fire_size.w/2) + util::Point<float>(cos(a2) * f.start_x, sin(a2) * f.start_x);
	pos.x -= cos(ca) * p * f.max_h;
	pos.y -= sin(ca) * p * f.max_h;
	float waviness = f.waviness * (1.0f - p);
	pos.x += cos(ca) * sin(p * f.waves * M_PI * 2 + f.sin_offset) * waviness * MAX_WAVINESS;
	pos.x += sin(ca) * sin(p * f.waves * M_PI * 2 + f.sin_offset) * waviness * MAX_WAVINESS;
	float start_colour = fire_colours.size() * f.start_heat;
	float diff = fire_colours.size() - start_colour;
	float p2 = i / (float)fire.size();
	p2 = 1.0f - p2;
	p2 = MAX(0.0f, MIN(1.0f, p2));
	colour = start_colour + p * diff;
	//colour = start_colour + p2 * diff;
	if (colour > (int)fire_colours.size()-1) {
		colour = (int)fire_colours.size()-1;
	}
	pos += f.offset;
}
	
void Fire_Step::set_start_angle(float start_angle)
{
	this->start_angle = start_angle;
	curr_angle = start_angle;
	last_angles.clear();
	for (int i = 0; i < SAVE_ANGLES; i++) {
		last_angles.push_back(curr_angle);
	}
	changed = true;
}

void Fire_Step::set_start_pos(util::Point<float> start_pos)
{
	draw_pos = start_pos;
	curr_pos = draw_pos;
	changed = true;
}

void Fire_Step::set_end_pos(util::Point<float> end_pos)
{
	this->end_pos = end_pos;
	changed = true;
}

void Fire_Step::double_particles()
{
	total_fire_particles *= 2;
}

void Fire_Step::set_total_particles()
{
	int total = 0;

	util::Point<float> curr_pos = this->curr_pos;
	
	while (true) {
		if (ended == false) {
			util::Point<float> diff = (end_pos-curr_pos);
			float len = diff.length();
			if (len <= speed) {
				curr_pos = end_pos;
				ended = true;
				total += fire.size();
			}
			else {
				float angle = diff.angle();
				curr_pos.x += cos(angle) * speed;
				curr_pos.y += sin(angle) * speed;
			}
		}
				
		for (std::list<Fire_Particle>::iterator it = fire.begin(); it != fire.end();) {
			Fire_Particle &f = *it;
			f.p += f.speed;
			if (f.p >= 1.0f) {
				it = fire.erase(it);
				if (ended == false) {
					total++;
				}
			}
			else {
				it++;
			}
		}

		int add = MIN(25, fire_to_add);

		if (add > 0) {
			for (int i = 0; i < add; i++) {
				add_from_bucket();
			}

			fire_to_add -= add;
		}
		else {
			int to_add = num_fire_particles - (int)fire.size();

			for (int i = 0; i < to_add; i++) {
				add_from_bucket();
			}
		}

		bool done = added_fire_particles >= total_fire_particles && fire.size() == 0;
		
		if (done) {
			total_fire_particles = total + 50;
			added_fire_particles = 0;
			fire_it = bucket.begin();
			ended = false;
			return;
		}
	}
}

void Fire_Step::add_from_bucket()
{
	if (added_fire_particles >= total_fire_particles) {
		return;
	}

	Fire_Particle p = *fire_it;
	fire.push_back(p);
	fire_it++;
	added_fire_particles++;
}
