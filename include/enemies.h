#ifndef ENEMIES_H
#define ENEMIES_H

#include <wedge2/battle_enemy.h>
#include <wedge2/battle_player.h>

class Battle_Enemy : public wedge::Battle_Enemy
{
public:
	Battle_Enemy(std::string name, int min_delay, int max_delay);
	virtual ~Battle_Enemy();

	void set_attack_done(bool done);

	bool take_hit(wedge::Battle_Entity *actor, int damage);

	virtual util::Point<int> get_turn_order_display_offset();

protected:
	int start_attack(wedge::Battle_Game::Turn *turn);
	bool turn_attack(wedge::Battle_Game::Turn *turn);
	
	void start_dialogue(wedge::Battle_Game::Turn *turn);
	bool turn_dialogue(wedge::Battle_Game::Turn *turn);

	bool turn_started;
	bool attack_done;
	Uint32 next_move;
	int min_delay;
	int max_delay;
	std::string attack_name;

	std::vector<std::string> attack_anims;
	std::vector<std::string> attack_names;
	std::vector<audio::Sound *> attack_sounds;
	std::vector<float> attack_mults;
	int attack_num;
};

class Enemy_Puddle : public Battle_Enemy
{
public:
	Enemy_Puddle();
	virtual ~Enemy_Puddle();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
};

class Enemy_Sensei1 : public Battle_Enemy
{
public:
	static const int START_HP = 1000000000;

	Enemy_Sensei1();
	virtual ~Enemy_Sensei1();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	void end_hadouken();
	void adjust_next_damage_reminder();
	void adjust_next_item_reminder();
	void adjust_next_both_reminder();
	
	int get_max_turns();
	
	bool take_hit(wedge::Battle_Entity *actor, int damage);

private:
	void set_button_strings();
	void add_apples();
	void add_caramels();

	bool switch_touch;
	std::string pleasant_button;
	std::string tom_button;
	std::string switch_button;

	bool did_damage;
	bool doing_hadouken;
	bool hadouken_done;
	Uint32 next_damage_reminder;
	Uint32 next_item_reminder;
	bool done_attacks;

	int p0_hit;
	int p1_hit;
	Uint32 next_both_reminder;

	bool did_both;

	Uint32 next_caramel_before_hadouken_reminder;

	Uint32 hadouken_dialogue_appear_time;

	bool fireball_done;
};

class Enemy_Wrath : public Battle_Enemy
{
public:
	static void static_start();

	Enemy_Wrath();
	virtual ~Enemy_Wrath();

	bool start();
	void draw_fore();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

protected:
	static Battle_Enemy *doing_drain;
	bool draining;
	Uint32 drain_start;
	bool laughed;
	Uint32 soul_suck_start;
	int time_until_laugh;
	Battle_Player *drain_target;
};

class Enemy_Coinman : public Battle_Enemy
{
public:
	static const unsigned int BOUNCE_TIME = 500;

	Enemy_Coinman();
	virtual ~Enemy_Coinman();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
	
	bool take_hit(wedge::Battle_Entity *actor, int damage);

protected:
	bool hit_ground;
	Uint32 hit_ground_time;
	audio::Sound *coin_thud;
	audio::Sound *coin_jump;
};

class Enemy_Unicorn : public Battle_Enemy
{
public:
	static const int RAINBOW_TIME = 1500;
	static const float MIN_TRI_SZ;
	static const float MAX_TRI_SZ;

	static void static_start();

	Enemy_Unicorn();
	virtual ~Enemy_Unicorn();

	bool start();

	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	void draw_fore();

protected:
	static int count;
	static gfx::Image *rainbow;

	struct Tri {
		float ox;
		float oy;
		float sz;
		int sz_dir;
		float a;
		int a_dir;
		SDL_Colour colour;
	};

	void init_tris(std::vector<Tri> &v);
	void update_tris(std::vector<Tri> &v);
	void draw_tris(std::vector<Tri> &v, util::Point<float> pos, float angle, float alpha);

	Uint32 rainbow_start;
	bool doing_rainbow;
	wedge::Battle_Entity *rainbow_target;

	std::vector<Tri> tris1;
	std::vector<Tri> tris2;
	
	audio::Sound *sfx;
};

class Enemy_Tentacle : public Battle_Enemy
{
public:
	Enemy_Tentacle();
	virtual ~Enemy_Tentacle();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	bool take_hit(wedge::Battle_Entity *actor, int damage);
};

class Enemy_Octo : public Battle_Enemy
{
public:
	Enemy_Octo();
	virtual ~Enemy_Octo();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
};

class Enemy_Flame_Frog : public Battle_Enemy
{
public:
	static const int MIN_PARTICLE_LIFETIME = 512;
	static const int MAX_PARTICLE_LIFETIME = 1024;
	static const int NUM_PARTICLES = 256;
	static const int FLAME_FRAME = 3;

	static void static_start();

	Enemy_Flame_Frog();
	virtual ~Enemy_Flame_Frog();

	bool start();
	void draw_fore();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	void set_hop_done(bool done);

protected:
	static int count;
	static SDL_Colour palette[256];
	static int palette_size;
	static gfx::Sprite *particle;

	void add_particle();

	bool hop_done;

	struct Particle {
		util::Point<float> pos;
		util::Point<float> velocity;
		Uint32 start_time;
		Uint32 lifetime;
	};

	Uint32 croak_start_time;
	std::vector<Particle> particles;
	int added_particles;
	bool done_regular_attack;
	int delay_before_flame;
	bool started_adding_particles;
	
	audio::Sound *sfx_attack;
	audio::Sound *sfx_hop;
};

class Enemy_Bot : public Battle_Enemy
{
public:
	static void static_start();

	Enemy_Bot();
	virtual ~Enemy_Bot();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	bool take_hit(wedge::Battle_Entity *actor, int damage);

protected:
	static int count;
	static audio::Sound *bot;

	audio::Sound *machine_gun;
};

class Enemy_Goblin : public Battle_Enemy
{
public:
	static void static_start();

	static const int THROW_TIME = 500;
	static const int MIN_ARCH_TIME = 750;
	static const int MAX_ARCH_TIME = 1250;
	static const int EXPLOSION_TIME = 1000;

	Enemy_Goblin();
	virtual ~Enemy_Goblin();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
	
	void draw_fore();

protected:
	static int count;
	static gfx::Sprite *bomb;
	static SDL_Colour palette[256];
	static int palette_size;
	static gfx::Image *particles[3];

	util::Point<float> calc_arch(float p);

	// Bomb is copied from Rainbow/Unicorn
	Uint32 bomb_start;
	bool doing_bomb;
	wedge::Battle_Entity *bomb_target;
	bool thrown;
	Uint32 arch_time;
	bool played_explosion;
	audio::Sound *explosion;
};

class Enemy_Red_Wasp : public Battle_Enemy
{
public:
	static void static_start();

	Enemy_Red_Wasp();
	virtual ~Enemy_Red_Wasp();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

private:
	static int count;
	static audio::Sound *red_wasp;
};

class Enemy_Chilly : public Battle_Enemy
{
public:
	static const int SPIT_START = 1225;
	static const int SPIT_TIME = 150;
	static const int BOUNCE_TIME = 500;
	static const int IMPACT_FADE = 250;
	static const int ICICLE_DELAY = 1900;
	static const int ICICLE_FALL = 500;

	Enemy_Chilly();
	virtual ~Enemy_Chilly();

	bool start();
	void draw_fore();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
	
	bool take_hit(wedge::Battle_Entity *actor, int damage);

	void play_spit_sound();
	void set_impact_done();

private:
	enum Move {
		SPIT,
		BOUNCE,
		RAGE
	};
	Move move;

	int spit_start;
	bool goblin_added;
	int spit_len;
	int spit_num;

	audio::Sample *chilly_thud;
	audio::Sample *chilly_spit;
	audio::Sample *chilly_groan;
	audio::MML *icicle_impact;
	gfx::Sprite *icicle;
	gfx::Image *goblin_img;

	util::Point<int> goblin0_pos;
	util::Point<int> goblin1_pos;
	
	bool hit_ground;
	Uint32 hit_ground_time;

	int impact_len;
	bool impact_done;
	Uint32 impact_done_time;
	Uint32 rage_start;
	bool drop_sound_done;
	std::vector<wedge::Battle_Entity *> icicle_targets;
	bool smash_sound_played;
};

class Enemy_Snapper : public Battle_Enemy
{
public:
	static void static_start();
	static void end_water_ripple(void *data);

	Enemy_Snapper();
	virtual ~Enemy_Snapper();

	bool start();
	void draw();
	void run();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
	
	util::Point<int> get_turn_order_display_offset();

private:
	int last_sprite_centre;
	int last_sprite_bottom;
	struct ripple_info {
		util::Point<int> pos;
		Enemy_Snapper *snapper;
	};
	bool played_chomp;
	bool played_splash2;
	audio::Sample *chomp;
	audio::Sound *splash_short;
	static std::map< gfx::Sprite *, ripple_info > water_ripples;
};

class Enemy_Bone1 : public Battle_Enemy
{
public:
	Enemy_Bone1();
	virtual ~Enemy_Bone1();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

private:
	audio::Sound *sound;
};

class Enemy_Bone2 : public Battle_Enemy
{
public:
	Enemy_Bone2();
	virtual ~Enemy_Bone2();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

private:
	audio::Sound *sound;
};

class Enemy_Aura : public Battle_Enemy
{
public:
	static const int RING_MIN = 12;
	static const int RING_MAX = 24;
	static const int HEX_MIN = 12;
	static const int HEX_MAX = 32;
	static const int RING_TIME = 25;
	static const int HEX_TIME = 40;
	static const int SPAWN_PRE_TIME = 300;
	static const int SPAWN_TIME = 667;
	static const int SPAWN_MOVE_TIME = 1000;

	static void static_start();
	static void end_spawn_anim(void *data);

	Enemy_Aura();
	virtual ~Enemy_Aura();

	bool start();
	void draw_back();
	void draw();
	void lost_device();
	void found_device();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

private:
	struct glow {
		float glow_alpha;
		int glow_dir;
		float hex_angle;
		int ring_dir;
		int hex_dir;
		math::Interpolator *ring_interp;
		math::Interpolator *hex_interp;
		float ring_prev;
		float hex_prev;
		float ring_dest;
		float hex_dest;
		float ring_sz;
		float hex_sz;
		float ring_thickness;
	};

	void init_glow(glow &g, int RING_MIN, int RING_MAX, int RING_TIME, int HEX_MIN, int HEX_MAX, int HEX_TIME);
	void update_glow(glow &g, int RING_MIN, int RING_MAX, int RING_TIME, int HEX_MIN, int HEX_MAX, int HEX_TIME);
	void draw_glow(float alpha, glow &g, util::Point<float> pos);

	glow main_g;

	Uint32 attack_start;
	int spawn_full_len;
	gfx::Sprite *auraball;
	bool spawning;
	std::vector<glow> ball_g;
	wedge::Battle_Game::Turn *spawn_turn;
	bool did_die;

	audio::Sound *auraball_appear;
	audio::Sound *auraball_hit;

	bool appear_played;

	static int count;
	static gfx::Image *work1;
	static gfx::Image *work2;
	static gfx::Sprite *main_auraball; // this is just to cache the sprite
	static audio::Sound *loop_sfx;
};

class Enemy_Clone : public Battle_Enemy
{
public:
	Enemy_Clone();
	virtual ~Enemy_Clone();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
};

class Enemy_CutiePie : public Battle_Enemy
{
public:
	static const int HEART_TIME = 2000;

	Enemy_CutiePie();
	virtual ~Enemy_CutiePie();

	bool start();
	void draw_fore();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

private:
	static int count;
	static audio::Sound *snub_sfx;
	static audio::Sound *heart_sfx;
	static audio::Sound *pop_sfx;

	enum TurnType {
		SNUB,
		HEART
	};
	TurnType tt;
	Uint32 turn_start;
	gfx::Sprite *heart;
	audio::Sound *heart_loop;
	wedge::Battle_Game::Turn *heart_turn;
	int pop_time;
	bool popped;
};

class Enemy_Thirsty : public Battle_Enemy
{
public:
	static const int TSUNAMI_DELAY = 500;
	static const int TSUNAMI_TIME = 2000;

	Enemy_Thirsty();
	virtual ~Enemy_Thirsty();

	bool start();
	void draw_fore();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

private:
	Uint32 turn_start;
	gfx::Sprite *tsunami;
	audio::MML *tsunami_sfx;
	audio::MML *splash;
	bool played_sfx;
};

class Enemy_Bumper : public Battle_Enemy
{
public:
	static const unsigned int BOUNCE_TIME = 500;

	Enemy_Bumper();
	virtual ~Enemy_Bumper();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	void set_eat_done(bool done);

	bool take_hit(wedge::Battle_Entity *actor, int damage);

protected:
	bool hit_ground;
	Uint32 hit_ground_time;
	int hit_num;
	audio::Sound *eat_sfx;
	bool eat_done;
	bool eat_sound_played;
	Uint32 eat_start;
};

class Enemy_Kiddie : public Battle_Enemy
{
public:
	static int count;
	static audio::Sound *pop_sfx;

	Enemy_Kiddie();
	virtual ~Enemy_Kiddie();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);
};

class Enemy_BigTuna : public Battle_Enemy
{
public:
	Enemy_BigTuna();
	virtual ~Enemy_BigTuna();

	bool start();
	
	wedge::Battle_Game::Turn *get_turn();
	bool take_turn(wedge::Battle_Game::Turn *turn);

	void end_fireball();

protected:
	audio::Sound *attack_sfx;
	audio::Sound *attack2_sfx;
	int attack_number;
	Uint32 fireball_start;
	bool bang_sound_done;
	bool fireball_ended;
	bool did_fireball;
};

#endif // ENEMIES_H
