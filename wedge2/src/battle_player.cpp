#include "wedge2/battle_game.h"
#include "wedge2/battle_player.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/omnipresent.h"

namespace wedge {

Battle_Player::Battle_Player(std::string name, int index) :
	Battle_Entity(Battle_Entity::PLAYER, name),
	index(index),
	run_success(false),
	run_offset(0.0f, 0.0f)
{
	stats = &INSTANCE->stats[index].base;
	player_stats = &INSTANCE->stats[index];
	sprite = INSTANCE->stats[index].sprite;
	
	if (stats->hp > 0) {
		sprite->set_animation("stand_w");
	}
	else {
		sprite->set_animation("dead");
	}
}

Battle_Player::~Battle_Player()
{
}

bool Battle_Player::start()
{
	return Battle_Entity::start();
}

bool Battle_Player::is_dead()
{
	return stats->hp <= 0;
}

wedge::Player_Stats *Battle_Player::get_player_stats()
{
	return player_stats;
}

int Battle_Player::get_index()
{
	return index;
}

util::Point<float> Battle_Player::get_draw_pos()
{
	util::Point<int> pos;
	pos.x = shim::screen_size.w - shim::screen_size.w*0.05f - shim::tile_size*2 - shim::tile_size*(INSTANCE->stats.size()-index-1);
	pos.y = (shim::screen_size.h * 0.05f) + (shim::tile_size+shim::tile_size/8)*index;
	if (INSTANCE->stats.size() <= 2) {
		pos.y += shim::tile_size*2.0f;
	}
	else {
		pos.y += shim::tile_size*1.5f;
	}
	return pos;
}

void Battle_Player::draw_fore()
{
	Battle_Entity::draw_fore();
}

bool Battle_Player::is_running()
{
	return run_success;
}

bool Battle_Player::take_hit(Battle_Entity *actor, int damage)
{
	bool ret = Battle_Entity::take_hit(actor, damage);
	if (stats->hp <= 0) {
		rumble(1.0f, 1000);
		gfx::screen_shake(1.0f, 500);
		sprite->set_animation("dead");
	}
	else {
		rumble(0.5f, 333);
	}
	return ret;
}

util::Point<float> Battle_Player::get_run_offset()
{
	return run_offset;
}

void Battle_Player::set_run_offset(util::Point<float> run_offset)
{
	this->run_offset = run_offset;
}

}
