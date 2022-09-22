#ifndef FIRE_H
#define FIRE_H

#include <wedge2/main.h>
#include <wedge2/battle_entity.h>
#include <wedge2/systems.h>

class Fire_Step : public wedge::Step
{
public:
	struct Fire_Particle {
		float max_h;
		float p;
		float start_heat;
		float sin_offset;
		float waviness;
		float speed;
		float waves;
		float start_x;
		float initial_x;
		float veeriness;
		util::Point<float> offset;
	};

	Fire_Step(util::Point<int> draw_pos_bottom_middle, wedge::Battle_Entity *target, SDL_Colour c1, SDL_Colour c2, SDL_Colour c3, wedge::Task *task, bool is_fatal = false);
	virtual ~Fire_Step();
	
	bool run();
	void draw_fore();

	void set_start_angle(float start_angle);
	void set_start_pos(util::Point<float> start_pos);
	void set_end_pos(util::Point<float> end_pos);
	void double_particles();

	void set_total_particles();

private:
	void draw_fire(bool draw_blurred);

	void add_fire_particle();
	void add_from_bucket();
	void get_draw_info(int i, Fire_Particle &f, int &colour, util::Point<float> &pos);

	util::Point<int> draw_pos;

	static const int MAX_WAVINESS = 7;
	static const int MAX_WAVES = 7;

	util::Size<int> fire_size;

	int num_fire_particles;
	int fire_to_add;
	int total_fire_particles;
	int added_fire_particles;

	std::vector<SDL_Colour> fire_colours;

	int veers;
	int veer_dir;
	
	std::list<Fire_Particle> fire;

	float start_angle;
	float curr_angle;
	util::Point<float> end_pos;
	util::Point<float> curr_pos;
	Uint32 move_end;

	std::vector<float> last_angles;

	bool changed;

	bool ended;

	std::list<Fire_Particle> bucket;
	std::list<Fire_Particle>::iterator fire_it;

	float speed;
	
	Uint32 start_time;

	std::string prev_anim;
	wedge::Battle_Entity *target;

	SDL_Colour colour1;
	SDL_Colour colour2;
	SDL_Colour colour3;

	bool is_fatal;
};

#endif // FIRE_H
