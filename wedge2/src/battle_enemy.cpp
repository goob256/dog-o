#include "wedge2/battle_enemy.h"
#include "wedge2/battle_game.h"
#include "wedge2/battle_player.h"
#include "wedge2/globals.h"
#include "wedge2/stats.h"

using namespace wedge;

namespace wedge {

Battle_Enemy::Battle_Enemy(std::string name) :
	Battle_Entity(ENEMY, name),
	die_time(1000),
	experience(0),
	attack_sound(NULL),
	die_sound(NULL),
	started_attack(false),
	done_sprite(false),
	attack_hits_all(false),
	boss(false),
	use_death_shader(true),
	_use_death_sound(true),
	_remove_when_dead(true),
	play_attack_sound(true),
	click_zone_inc(0)
{
}

Battle_Enemy::~Battle_Enemy()
{
	delete stats;
	delete sprite;
	delete attack_sound;
	delete die_sound;
}

bool Battle_Enemy::start()
{
	return true;
}

void Battle_Enemy::draw_enemy(SDL_Colour tint, gfx::Image *image, util::Point<float> draw_pos, float scale, int flags)
{
	bool dead = stats->hp <= 0;
	if (dead && use_death_shader) {
		shim::current_shader = globals->enemy_die_shader;
		shim::current_shader->use();
		gfx::update_projection();
		shim::current_shader->set_float("p", MIN(1.0f, (GET_TICKS()-dead_start)/(float)die_time));
		shim::current_shader->set_float("tint_p", 0.25f);
		shim::current_shader->set_colour("tint", GLOBALS->enemy_death_tint);
		shim::current_shader->set_int("current_frame", sprite->get_current_frame());
		shim::current_shader->set_int("num_frames", sprite->get_num_frames());
		shim::current_shader->set_int("sprite_w", image->size.w);
		shim::current_shader->set_int("sprite_h", image->size.h);
		shim::current_shader->set_float("scale", shim::scale);
		shim::current_shader->set_bool("inverse_x", global_flip_h);
	}
	image->draw_tinted(tint, draw_pos, flags);
	if (dead && use_death_shader) {
		shim::current_shader = shim::default_shader;
		shim::current_shader->use();
		gfx::update_projection();
	}
}

void Battle_Enemy::draw_boss(SDL_Colour tint, gfx::Image *image, util::Point<float> draw_pos, float scale, int curr_frame, int num_frames, int flags)
{
	bool dead = stats->hp <= 0;
	if (dead && use_death_shader) {
		image->stretch_region(util::Point<float>(0.0f, 0.0f), image->size, util::Point<float>(0.0f, 0.0f), shim::screen_size, flags);
	}
	else {
		image->draw_tinted(tint, draw_pos, flags);
	}
}

void Battle_Enemy::draw()
{
	if (sprite) {
		gfx::Image *image = sprite->get_current_image();
		int flags;
		if (inverse_x) {
			flags = gfx::Image::FLIP_H;
		}
		else {
			flags = 0;
		}
		util::Point<int> pos;
		if (draw_as_player) {
			pos = get_sprite_pos();
			if (inverse_x) {
				pos.x = shim::screen_size.w - pos.x - image->size.w;
			}
		}
		else {
			if (inverse_x) {
				pos = util::Point<int>(shim::screen_size.w-position.x-image->size.w, position.y);
			}
			else {
				pos = position;
			}
		}
		if (global_flip_h) {
			if (flags == 0) {
				flags = gfx::Image::FLIP_H;
			}
			else {
				flags = 0;
			}
		}
#ifdef ANDROID
		if (boss && GLOBALS->have_highp_fragment_shaders) {
#else
		if (boss) {
#endif
			draw_boss(global_tint, image, pos, 1.0f, sprite->get_current_frame(), sprite->get_num_frames(), flags);
		}
		else {
			draw_enemy(global_tint, image, pos, 1.0f, flags);
		}
	}
}

bool Battle_Enemy::is_dead()
{
	return stats->hp <= 0 && int(GET_TICKS()-dead_start) >= die_time;
}

int Battle_Enemy::get_experience()
{
	return experience;
}

int Battle_Enemy::get_gold()
{
	return gold;
}

void Battle_Enemy::sprite_done()
{
	done_sprite = true;
}

void Battle_Enemy::set_position(util::Point<int> position)
{
	this->position = position;
}

util::Point<int> Battle_Enemy::get_position()
{
	return position;
}

void Battle_Enemy::play_die_sound()
{
	if (die_sound != NULL) {
		die_sound->play(false);
	}
	else if (_use_death_sound) {
		if (boss) {
			globals->boss_die->play(false);
		}
		else {
			globals->enemy_die->play(false);
		}
	}
}

Battle_Player *Battle_Enemy::rand_living_player()
{
	std::vector<Battle_Entity *> players = BATTLE->get_players();
	std::vector<Battle_Entity *> living;
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i]->get_stats()->hp > 0) {
			living.push_back(players[i]);
		}
	}
	if (living.size() == 0) {
		return NULL;
	}
	return static_cast<Battle_Player *>(living[util::rand()%living.size()]);
}

void Battle_Enemy::set_dead()
{
	dead_start = GET_TICKS();
}

bool Battle_Enemy::remove_when_dead()
{
	return _remove_when_dead;
}

bool Battle_Enemy::use_death_sound()
{
	return _use_death_sound;
}

bool Battle_Enemy::take_hit(Battle_Entity *actor, int damage)
{
	bool dead = Battle_Entity::take_hit(actor, damage);
	if (dead) {
		play_die_sound();
		set_dead();
	}
	return dead;
}

int Battle_Enemy::get_click_zone_inc()
{
	return click_zone_inc;
}

util::Point<float> Battle_Enemy::get_sprite_pos()
{
       gfx::Image *current_image = sprite->get_current_image();
       util::Size<int> offset = util::Size<int>((shim::tile_size-current_image->size.w)/2, shim::tile_size-current_image->size.h);
       return position + util::Point<int>(offset.w, offset.h);
}

}
