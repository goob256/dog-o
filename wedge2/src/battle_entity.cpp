#include "wedge2/battle_enemy.h"
#include "wedge2/battle_entity.h"
#include "wedge2/battle_game.h"
#include "wedge2/battle_player.h"
#include "wedge2/generic_callback.h"
#include "wedge2/globals.h"
#include "wedge2/stats.h"

using namespace wedge;

namespace wedge {

Battle_Entity::Battle_Entity(Type type, std::string name) :
	type(type),
	name(name),
	stats(0),
	sprite(0),
	defending(false),
	spell_effect_offset(0.0f, 0.0f),
	draw_shadow(false),
	auto_shadow_pos(false),
	shadow_offset(0, 0),
	buff(NONE),
	fast(false),
	inverse_x(false),
	global_flip_h(false),
	draw_as_player(false)
{
	global_tint = shim::white;
}

Battle_Entity::~Battle_Entity()
{
}

bool Battle_Entity::start()
{
	return true;
}

void Battle_Entity::handle_event(TGUI_Event *event)
{
}

void Battle_Entity::draw_back()
{
	if (draw_shadow && (dynamic_cast<Battle_Enemy *>(this) == nullptr || stats->hp > 0)) {
		util::Point<float> pos;
		Battle_Enemy *enemy = dynamic_cast<Battle_Enemy *>(this);
		if (enemy != nullptr) {
			if (draw_as_player) {
				gfx::Image *img = sprite->get_current_image();
				pos = enemy->get_position();
				pos += util::Point<int>((shim::tile_size-img->size.w)/2, 0);
				util::Point<int> topleft, bottomright;
				img->get_bounds(topleft, bottomright);
				if (global_flip_h) {
					int tl = topleft.x;
					topleft.x = img->size.w - bottomright.x;
					bottomright.x = img->size.w - tl;
				}
				pos += util::Point<float>(topleft.x + (bottomright.x-topleft.x)/2.0f, shim::tile_size);
			}
			else if (auto_shadow_pos) {
				pos = enemy->get_position();
				gfx::Image *img = sprite->get_current_image();
				util::Point<int> topleft, bottomright;
				img->get_bounds(topleft, bottomright);
				pos += util::Point<float>(topleft.x + (bottomright.x-topleft.x)/2.0f, img->size.h);
			}
			else {
				pos = enemy->get_position();
				pos += shadow_pos;
			}
		}
		else {
			Battle_Player *player = static_cast<Battle_Player *>(this);
			gfx::Image *img = sprite->get_current_image();
			pos = player->get_draw_pos() + util::Point<float>(shim::tile_size/2.0f-img->size.w/2.0f, 0.0f);
			util::Point<int> topleft, bottomright;
			img->get_bounds(topleft, bottomright);
			pos += util::Point<float>(topleft.x + (bottomright.x-topleft.x)/2.0f, shim::tile_size);
			pos += util::Point<float>{player->get_run_offset().x, 0.0f};
		}
		if (inverse_x) {
			pos.x = shim::screen_size.w-pos.x;
		}
		pos += shadow_offset;
		SDL_Colour tint = shim::black;
		tint.r *= 0.333f;
		tint.g *= 0.333f;
		tint.b *= 0.333f;
		tint.a *= 0.333f;
		gfx::Shader *tmp_shader = shim::current_shader;
		shim::current_shader = GLOBALS->pixelate_shader;
		shim::current_shader->use();
		gfx::update_projection();
		shim::current_shader->set_int("draw_w", shadow_size.w);
		shim::current_shader->set_int("draw_h", shadow_size.h);
		GLOBALS->shadow->stretch_region_tinted(tint, {0, 0}, GLOBALS->shadow->size, pos-shadow_size/2, shadow_size);
		shim::current_shader = tmp_shader;
		shim::current_shader->use();
		gfx::update_projection();
	}
}

void Battle_Entity::draw()
{
}

void Battle_Entity::draw_fore()
{
}

bool Battle_Entity::is_dead()
{
	return stats->hp <= 0;
}

Battle_Entity::Type Battle_Entity::get_type()
{
	return type;
}

Base_Stats *Battle_Entity::get_stats()
{
	// Kind of a hack to put this here but it's convenient
	stats->set_name(name);
	return stats;
}

gfx::Sprite *Battle_Entity::get_sprite()
{
	return sprite;
}

std::string Battle_Entity::get_name()
{
	return name;
}

bool Battle_Entity::is_defending()
{
	return defending;
}

util::Point<int> Battle_Entity::get_decoration_offset(int decoration_width, util::Point<int> offset, int *flags)
{
	util::Point<int> pos;

	util::Point<int> topleft;
	util::Point<int> bottomright;
	sprite->get_bounds(topleft, bottomright);

	if (type == Battle_Entity::PLAYER) {
		Battle_Player *player = static_cast<Battle_Player *>(this);
		pos = player->get_draw_pos() + util::Point<int>(-(decoration_width + offset.x), offset.y) + topleft;
		if (flags != NULL) {
			*flags = gfx::Image::FLIP_H;
		}
	}
	else {
		Battle_Enemy *enemy = static_cast<Battle_Enemy *>(this);
		pos = enemy->get_position() + offset + util::Point<int>(bottomright.x, topleft.y);
		if (flags != NULL) {
			*flags = 0;
		}
	}

	return pos;
}

float Battle_Entity::get_poison_odds()
{
	return 0.0f;
}

void Battle_Entity::run()
{
}

void Battle_Entity::lost_device()
{
}

void Battle_Entity::found_device()
{
}

void Battle_Entity::resize(util::Size<int> new_size)
{
}

util::Point<float> Battle_Entity::get_spell_effect_offset()
{
	return spell_effect_offset;
}

int Battle_Entity::get_max_turns()
{
	return 1;
}

bool Battle_Entity::take_hit(Battle_Entity *actor, int damage)
{
	stats->hp -= MIN(stats->hp, damage);
	return stats->hp <= 0;
}

Battle_Entity::Buff_Type Battle_Entity::get_buff()
{
	return buff;
}

void Battle_Entity::set_buff(Buff_Type buff)
{
	this->buff = buff;
}

bool Battle_Entity::is_fast()
{
	return fast;
}

void Battle_Entity::set_fast(bool fast, Uint32 fast_end)
{
	this->fast = fast;
	this->fast_end = fast_end;
}

Uint32 Battle_Entity::get_fast_end()
{
	return fast_end;
}

void Battle_Entity::set_inverse_x(bool inverse_x)
{
	this->inverse_x = inverse_x;
}

bool Battle_Entity::get_inverse_x()
{
	return inverse_x;
}

bool Battle_Entity::get_draw_as_player()
{
	return draw_as_player;
}

}
