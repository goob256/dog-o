#include <wedge2/battle_enemy.h>
#include <wedge2/battle_game.h>
#include <wedge2/general.h>
#include <wedge2/globals.h>
#include <wedge2/special_number.h>
#include <wedge2/stats.h>

#include "achievements.h"
#include "battles.h"
#include "battle_game.h"
#include "battle_player.h"
#include "enemies.h"
#include "general.h"
#include "globals.h"
#include "hit.h"
#include "inventory.h"
#include "multi.h"
#include "stats.h"

std::map< gfx::Sprite *, Battle_Player::ripple_info > Battle_Player::water_ripples;

void Battle_Player::end_water_ripple(void *data)
{
	gfx::Sprite *sprite = static_cast<gfx::Sprite *>(data);
	auto it = water_ripples.find(sprite);
	if (it != water_ripples.end()) {
		delete sprite;
		water_ripples.erase(it);
	}
}

static void attack_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	Battle_Player *p = static_cast<Battle_Player *>(data);
	p->get_sprite()->set_animation("stand_w");
	p->set_attack_done(true);
}

static void use_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	Battle_Player *p = static_cast<Battle_Player *>(data);
	p->get_sprite()->set_animation("stand_w");
	p->set_use_done(true);
}

static void hit_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	auto p = static_cast<Battle_Player *>(data);
	gfx::Sprite *sprite = p->get_sprite();
	if (sprite->get_animation() == "hit") {
		sprite->set_animation(sprite->get_previous_animation());
	}
}

static void heal_callback(void *data)
{
	if (BATTLE == nullptr) {
		return;
	}

	auto p = static_cast<Battle_Player *>(data);
	p->set_heal_done(true);
}

Battle_Player::Battle_Player(int index) :
	wedge::Battle_Player(INSTANCE->stats[index].base.get_name(), index),
	turn_started(false),
	attack_done(false),
	use_done(false),
	tilt_angle(0.0f),
	doing_ninja_star(false),
	doing_dagger(false),
	mult_level(0),
	multiplier(1.0f),
	mult_stage(0),
	doing_heal(false),
	doing_axe(false),
	weapon_ready(false),
	last_sprite_centre(-1),
	last_sprite_bottom(-1),
	took_hit(false)
{
	if (static_cast<Battle_Game *>(BATTLE)->in_water()) {
		draw_shadow = false;
	}
	else {
		draw_shadow = true;
		shadow_size = {12, 6};
	}
}

Battle_Player::~Battle_Player()
{
	for (auto p : water_ripples) {
		delete p.first;
	}
	water_ripples.clear();
}

void Battle_Player::handle_event(TGUI_Event *event)
{
}

util::Point<float> Battle_Player::get_sprite_pos()
{
	gfx::Image *current_image = sprite->get_current_image();
	util::Size<int> offset = util::Size<int>((shim::tile_size-current_image->size.w)/2, shim::tile_size-current_image->size.h);
	util::Point<float> pos = get_draw_pos() + util::Point<int>(offset.w, offset.h);
	return pos;
}

void Battle_Player::draw()
{
	gfx::Image *current_image = sprite->get_current_image();
	util::Size<int> offset = util::Size<int>((shim::tile_size-current_image->size.w)/2, shim::tile_size-current_image->size.h);
	util::Point<float> pos = get_draw_pos() + util::Point<int>(offset.w, offset.h);
	pos += run_offset;
	util::Point<float> pivot = util::Point<float>(shim::tile_size/2.0f, shim::tile_size/2.0f);
	bool running = false;
	for (auto p : BATTLE->get_players()) {
		auto player = dynamic_cast<Battle_Player *>(p);
		if (player->is_running()) {
			running = true;
			break;
		}
	}
	
	auto b = static_cast<Battle_Game *>(BATTLE);
	bool in_water = b->in_water();
	
	if (in_water) {
		float wh = b->get_water_height();

		for (auto p : water_ripples) {
			if (p.second.player == this) {
				gfx::Image *img = p.first->get_current_image();
				util::Point<int> pos = p.second.pos;
				if (inverse_x) {
					pos.x = shim::screen_size.w-pos.x;
				}
				img->draw({ float(pos.x-img->size.w/2.0f), float(pos.y-img->size.h/2.0f-wh) });
			}
		}

		if (shim::opengl) {
			glEnable_ptr(GL_SCISSOR_TEST);
			int h = (pos.y+current_image->size.h-wh-run_offset.y)*shim::scale+shim::screen_offset.y;
			glScissor_ptr(0, shim::real_screen_size.h-h, shim::screen_size.w*shim::scale+shim::screen_offset.x, h);
			PRINT_GL_ERROR("glScissor");
		}
#ifdef _WIN32
		else {
			shim::d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			RECT scissor = { 0, 0, (LONG)(shim::screen_size.w*shim::scale+shim::screen_offset.x), (LONG)((pos.y+current_image->size.h-wh-run_offset.y)*shim::scale+shim::screen_offset.y) };
			shim::d3d_device->SetScissorRect(&scissor);
		}
#endif
	}

	if (mult_stage > 0) {
		gfx::Sprite *flames;
		switch (mult_stage) {
			case 1:
				flames = DOGO_GLOBALS->mult1;
				break;
			case 2:
				flames = DOGO_GLOBALS->mult2;
				break;
			default:
				flames = DOGO_GLOBALS->mult3;
				break;
		}
		// Getting pos ripped from wedge::Battle_Entity
		gfx::Image *img = sprite->get_current_image();
		util::Point<float> pos = get_draw_pos() + util::Point<float>(shim::tile_size/2.0f-img->size.w/2.0f, 0.0f);
		util::Point<int> topleft, bottomright;
		img->get_bounds(topleft, bottomright);
		pos += util::Point<float>(topleft.x + (bottomright.x-topleft.x)/2.0f, shim::tile_size);
		pos += util::Point<float>{get_run_offset().x, 0.0f};
		if (inverse_x) {
			pos.x = shim::screen_size.w-pos.x;
		}
		gfx::Image *flame_img = flames->get_current_image();
		flame_img->draw({pos.x-flame_img->size.w/2, pos.y-flame_img->size.h});
	}

	int flags;
	if (inverse_x) {
		pos.x = shim::screen_size.w - pos.x - current_image->size.w;
		flags = gfx::Image::FLIP_H;
	}
	else {
		flags = 0;
	}

	if (running) {
		current_image->draw_rotated(pivot, pos+pivot, tilt_angle, flags);
	}
	else {
		current_image->draw(pos, flags);
	}
	
	if (in_water) {
		gfx::unset_scissor();
	}

	int turns = static_cast<Battle_Game *>(BATTLE)->get_num_turns(this);
	int max = get_max_turns();

	if (is_fast()) {
		max += 3;
	}

	util::Point<int> topleft, bottomright;
	sprite->get_bounds(topleft, bottomright);

	if (turns >= max) {
		util::Point<int> p = pos;
		if (inverse_x) {
			p.x -= (11+DOGO_GLOBALS->hourglass->size.w);
		}
		else {
			p.x += (bottomright.x+11);
		}
		p.y += current_image->size.h-shim::tile_size-shim::tile_size/4;
		DOGO_GLOBALS->hourglass->draw(p, flags);
	}
	else if (is_fast()) {
		util::Point<int> p = pos;
		if (inverse_x) {
			p.x -= (11+DOGO_GLOBALS->fast_sprite->get_current_image()->size.w);
		}
		else {
			p.x += (bottomright.x+11);
		}
		p.y += current_image->size.h-shim::tile_size-shim::tile_size/8;
		DOGO_GLOBALS->fast_sprite->get_current_image()->draw(p, flags);
	}

	if (sprite->get_animation() == "attack_defend") {
		util::Point<int> p = pos;
		if (inverse_x) {
			p.x -= (11+DOGO_GLOBALS->defend_img->size.w);
		}
		else {
			p.x += (bottomright.x+11);
		}
		p.y += current_image->size.h-shim::tile_size-shim::tile_size/4;
		DOGO_GLOBALS->defend_img->draw(p, flags);
	}

	if (ninja_star_turn != nullptr || dagger_turn != nullptr || axe_turn != nullptr) {
		if ((sprite->get_animation() == "throw_item" || sprite->get_animation() == "throw_axe") && sprite->get_current_frame() == 0) {
			if (throw_id == ITEM_RANDOMAXE) {
				int f;
				util::Point<float> p = get_draw_pos();
				if (inverse_x) {
					f = gfx::Image::FLIP_H;
					p.x = shim::screen_size.w - p.x;
					p += util::Point<float>(-current_image->size.w+2, current_image->size.h*0.5f);
				}
				else {
					f = 0;
					p += util::Point<float>(current_image->size.w-2, current_image->size.h*0.5f);
				}
				DOGO_GLOBALS->randomaxe->draw_rotated(util::Point<float>(DOGO_GLOBALS->randomaxe->size.w/2, DOGO_GLOBALS->randomaxe->size.h-1), p, inverse_x ? -M_PI/2 : M_PI/2, f);
			}
			else if (throw_id == ITEM_NINJA_STAR) {
				util::Point<float> p = get_draw_pos();
				float a;
				if (inverse_x) {
					p.x = shim::screen_size.w - p.x - current_image->size.w;
					p += util::Point<float>(-2, current_image->size.h*0.75f);
					a = -float(M_PI / 4.0f);
				}
				else {
					p += util::Point<float>(current_image->size.w+2, current_image->size.h*0.75f);
					a = float(M_PI / 4.0f);
				}
				DOGO_GLOBALS->ninja_star->draw_rotated(util::Point<float>(DOGO_GLOBALS->ninja_star->size.w/2.0f, DOGO_GLOBALS->ninja_star->size.h/2.0f), p, a);
			}
			else {
				int f;
				util::Point<float> p = get_draw_pos();
				if (inverse_x) {
					f = 0;
					p.x = shim::screen_size.w - p.x;
					p += util::Point<float>(-current_image->size.w+2, current_image->size.h*0.75f);
				}
				else {
					f = gfx::Image::FLIP_H;
					p += util::Point<float>(current_image->size.w-2, current_image->size.h*0.75f);
				}
				auto image = DOGO_GLOBALS->dagger->get_image(0);
				image->draw_rotated(inverse_x ? util::Point<float>(image->size.w-1, image->size.h/2.0f) : util::Point<float>(1.0f, image->size.h/2.0f), p, 0.0f, f);
			}
		}
		else if (turn_started && weapon_ready == false) {
			weapon_ready = true;
			wedge::Battle_Game::Turn *turn = nullptr;
			if (throw_id == ITEM_NINJA_STAR) {
				turn = ninja_star_turn;
			}
			else if (throw_id == ITEM_DAGGER) {
				turn = dagger_turn;
			}
			else if (throw_id == ITEM_RANDOMAXE) {
				turn = axe_turn;
			}
			int index = atoi(turn->turn_name.c_str());
			INSTANCE->inventory.remove(index, 1);
			if (throw_id == ITEM_RANDOMAXE) {
				axe_start = GET_TICKS();
				doing_axe = true;
				axe_hit = false;
				axe_target = turn->targets[0];
				thrown = false;
				util::Point<float> pos = calc_arch(0.5f);
				auto ent = static_cast<Battle_Enemy *>(turn->targets[0]);
				util::Point<float> position = ent->get_position();
				util::Point<int> topleft, bottomright;
				ent->get_sprite()->get_bounds(topleft, bottomright);
				position.x += topleft.x;
				position.x += (bottomright.x - topleft.x) / 2;
				position.y += ent->get_sprite()->get_current_image()->size.h;
				position.y += AXE_DIG;
				float dy = fabsf(pos.y - position.y);
				float dx = fabsf(pos.x - position.x);
				float d = sqrtf(dy*dy + dx*dx);
				float lo = 75.0f;
				float hi = 175.0f;
				if (d < lo) {
					d = lo;
				}
				if (d > hi) {
					d = hi;
				}
				float p = (d-lo)/(hi-lo);
				arch_time = MIN_ARCH_TIME + p * (MAX_ARCH_TIME-MIN_ARCH_TIME);
				played_axe_impact = false;
				arch_anim = "";
			}
			else if (throw_id == ITEM_NINJA_STAR) {
				doing_ninja_star = true;
				DOGO_GLOBALS->ninja_star_loop->play(true);
				DOGO_GLOBALS->throw_->play(false);
				ninja_star_targets_done = 0;
				calc_ninja_star_pos();
			}
			else if (throw_id == ITEM_DAGGER) {
				doing_dagger = true;
				dagger_hit = false;
				DOGO_GLOBALS->throw_->play(false);

				dagger_start_pos = get_draw_pos() + util::Point<int>(-16, shim::tile_size/2-3);
					
				auto enemy = static_cast<Battle_Enemy *>(dagger_turn->targets[0]);
				gfx::Sprite *sprite = enemy->get_sprite();
				util::Point<int> topleft, bottomright;
				sprite->get_bounds(topleft, bottomright);
				dagger_end_pos = enemy->get_position() + topleft + (bottomright-topleft)/2;
				
				dagger_start = GET_TICKS();
				
				float len = (dagger_start_pos-dagger_end_pos).length();

				dagger_impact = dagger_start + len/100.0f * PROJECTILE_TIME_PER_100;

				float angle = (dagger_end_pos-dagger_start_pos).angle();

				dagger_end_pos.x += cos(angle) * 150;
				dagger_end_pos.y += sin(angle) * 150;

				len = (dagger_start_pos-dagger_end_pos).length();

				dagger_end = dagger_start + len/100.0f * PROJECTILE_TIME_PER_100;
			}
		}
	}
}

void Battle_Player::draw_fore()
{
	if (doing_axe) {
		Uint32 now = GET_TICKS();
		Uint32 elapsed = now - axe_start;

		if (thrown == false) {
			DOGO_GLOBALS->throw_->play(false);
		}

		thrown = true;
		
		if (played_axe_impact == false && elapsed >= arch_time) {
			DOGO_GLOBALS->thud->play(5.0f*shim::sfx_volume, false);
			wedge::rumble(1.0f, 333);
			gfx::screen_shake(2.0f, 333);
			played_axe_impact = true;
		}

		if (elapsed < arch_time+AXE_STAY_TIME) {
			float p = elapsed / (float)arch_time;

			if (p > 1.0f) {
				p = 1.0f;
			}
	
			bool dig = p >= 0.5f ? true : false;

			util::Point<float> pos = calc_arch(p);

			float angle = -(p * (M_PI * 2 + M_PI / 2 + M_PI / 4));

			if (inverse_x) {
				pos.x = shim::screen_size.w - pos.x;
				angle = -angle;
			}

			float p2;
			if (elapsed >= arch_time+AXE_STAY_TIME/2) {
				p2 = 1.0f- ((elapsed-arch_time-AXE_STAY_TIME/2) / (float)(AXE_STAY_TIME/2.0f));
			}
			else {
				p2 = 1.0f;
			}
			SDL_Colour tint = shim::white;
			tint.r *= p2;
			tint.g *= p2;
			tint.b *= p2;
			tint.a *= p2;

			if (dig) {
				auto enemy = static_cast<Battle_Enemy *>(axe_target);
				float add_y;
				if (shim::using_screen_shake) {
					add_y = shim::screen_shake_save.y*shim::scale;
				}
				else {
					add_y = 0.0f;
				}
				if (shim::opengl) {
					glEnable_ptr(GL_SCISSOR_TEST);
					int h = axe_bottom*shim::scale+shim::screen_offset.y+1+add_y;
					//int h = (enemy->get_position().y + enemy->get_sprite()->get_current_image()->size.h)*shim::scale+shim::screen_offset.y+1+add_y;
					glScissor_ptr(0, shim::real_screen_size.h-h, shim::screen_size.w*shim::scale+shim::screen_offset.x, h);
					PRINT_GL_ERROR("glScissor");
				}
#ifdef _WIN32
				else {
					shim::d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
					//RECT scissor = { 0, 0, (LONG)(shim::screen_size.w*shim::scale+shim::screen_offset.x), (LONG)((enemy->get_position().y + enemy->get_sprite()->get_current_image()->size.h)*shim::scale+shim::screen_offset.y+1+add_y) };
					RECT scissor = { 0, 0, (LONG)(shim::screen_size.w*shim::scale+shim::screen_offset.x), (LONG)(axe_bottom*shim::scale+shim::screen_offset.y+1+add_y) };
					shim::d3d_device->SetScissorRect(&scissor);
				}
#endif
			}

			DOGO_GLOBALS->randomaxe->draw_tinted_rotated(tint, util::Point<float>(7, 7), pos, angle, inverse_x ? gfx::Image::FLIP_H : 0);
	
			if (dig) {
				gfx::unset_scissor();
			}

		}
	}
	else if (doing_ninja_star) {
		Uint32 now = GET_TICKS();
		float p = (now-ninja_star_start) / (float)(ninja_star_end-ninja_star_start);
		p = MIN(1.0f, p);

		util::Point<float> pos = ninja_star_start_pos;

		pos += (ninja_star_end_pos-ninja_star_start_pos) * p;

		int t = GET_TICKS() % 250;
		float angle = -(t / 250.0f * M_PI * 2.0f);

		if (inverse_x) {
			pos.x = shim::screen_size.w - pos.x;
			angle = -angle;
		}

		DOGO_GLOBALS->ninja_star->draw_rotated(util::Point<float>(DOGO_GLOBALS->ninja_star->size.w/2.0f, DOGO_GLOBALS->ninja_star->size.h/2.0f), pos, angle);
	}
	else if (doing_dagger) {
		Uint32 now = GET_TICKS();
		float p = (now-dagger_start) / (float)(dagger_end-dagger_start);
		p = MIN(1.0f, p);

		util::Point<float> pos = dagger_start_pos;
		
		pos += (dagger_end_pos-dagger_start_pos) * p;
	
		int flags;
		if (inverse_x) {
			pos.x = shim::screen_size.w - pos.x - DOGO_GLOBALS->dagger->get_current_image()->size.w;
			flags = gfx::Image::FLIP_H;
		}
		else {
			flags = 0;
		}

		DOGO_GLOBALS->dagger->get_current_image()->draw(pos, flags);
	}
	else if (doing_heal && heal_done == false) {
		Uint32 now = GET_TICKS();
		float p = (now-turn_start_time) / (float)heal_time;
		p = MIN(1.0f, p);

		float h = shim::tile_size*1.5f*(1.0f-p);

		auto img = DOGO_GLOBALS->heal->get_current_image();

		for (auto t : heal_turn->targets) {
			util::Point<float> pos = static_cast<Battle_Player *>(t)->get_draw_pos();

			pos.y -= h;
			pos.y -= (img->size.h-shim::tile_size);
			pos.y += shim::tile_size/4;

			pos.x -= (img->size.w-shim::tile_size)/2;
						
			if (static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
				pos.x = shim::screen_size.w - pos.x - img->size.w;
			}

			img->draw(pos);
		}
	}
}

void Battle_Player::run()
{
	if (static_cast<Battle_Game *>(BATTLE)->in_water()) {
		gfx::Image *current_image = sprite->get_current_image();
		util::Point<int> topleft, bottomright;
		current_image->get_bounds(topleft, bottomright);
		util::Size<int> offset = util::Size<int>((shim::tile_size-current_image->size.w)/2, shim::tile_size-current_image->size.h);
		util::Point<float> pos = get_draw_pos() + util::Point<int>(offset.w, offset.h);
		pos += run_offset;
		int centre_x = pos.x + topleft.x + (bottomright.x-topleft.x)/2;
		int bottom_y = pos.y + bottomright.y;
		int abs_bottom = pos.y + current_image->size.h;
		auto b = static_cast<Battle_Game *>(BATTLE);
		float wh = b->get_water_height();
		bool was_below = last_sprite_bottom == -1 ? true : last_sprite_bottom >= (abs_bottom-wh);
		bool is_below = bottom_y >= (abs_bottom-wh);
		bool moved_x = last_sprite_centre == -1 ? false : std::abs(centre_x-last_sprite_centre) >= 3;
		bool do_ripple = (moved_x && was_below && is_below) || (was_below != is_below);
		if (last_sprite_centre == -1 || moved_x) {
			last_sprite_centre = centre_x;
		}
		if (last_sprite_bottom == -1 || was_below != is_below) {
			last_sprite_bottom = bottom_y;
		}
		if (do_ripple) {
			for (auto it = water_ripples.begin(); it != water_ripples.end(); it++) {
				auto p = *it;
				if (p.second.player == this) {
					delete p.first;
					water_ripples.erase(it);
					break;
				}
			}

			gfx::Sprite *wr = new gfx::Sprite("water_ripple2");
			ripple_info info;
			info.pos = util::Point<int>{ last_sprite_centre, int(pos.y + current_image->size.h) };
			info.player = this;
			wr->set_animation("only", end_water_ripple, wr);
			water_ripples[wr] = info;
		}
	}
}

wedge::Battle_Game::Turn *Battle_Player::get_turn()
{
	took_hit = false;
	return static_cast<Battle_Game *>(BATTLE)->get_turn(this);
}

bool Battle_Player::take_turn(wedge::Battle_Game::Turn *turn)
{
	if (turn_started == false) {
		turn_started = true;
		turn_start_time = GET_TICKS();
		heal_turn = nullptr;
		ninja_star_turn = nullptr;
		dagger_turn = nullptr;
		axe_turn = nullptr;
		if (turn->turn_type == wedge::Battle_Game::ATTACK && turn->turn_name == "attack_defend") {
		}
		else if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			if (turn->targets.size() == 0) {
				turn_started = false;
				return false; // do nothing, we were attacking dead enemy or something and there are no more enemies
			}
			attack_done = false;
			std::string attack_name = turn->turn_name.substr(7);/* Remove attack_ */
			audio::Sound *sfx = get_combo_sfx(attack_name);
			if (sfx) {
				sfx->play(false);
			}
			do_combo_screen_shake(attack_name);
			sprite->set_animation(turn->turn_name, attack_callback, this);
			if (attack_name == "heal") {
				doing_heal = true;
				heal_turn = turn;
				heal_time = DOGO_GLOBALS->heal->get_length();
				heal_done = false;
				DOGO_GLOBALS->heal->set_animation("only", heal_callback, this);
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::ITEM) {
			if (turn->targets.size() == 0) {
				turn_started = false;
				return false; // do nothing, we were attacking dead enemy or something and there are no more enemies
			}
			use_done = false;
			weapon_ready = false;
			int index = atoi(turn->turn_name.c_str());
			wedge::Object *inv = INSTANCE->inventory.get_all();
			int id = inv[index].id;
			if ((id == ITEM_RANDOMAXE || id == ITEM_NINJA_STAR) && turn->targets[0]->get_stats()->fixed.max_hp >= 2000 && dynamic_cast<Battle_BigTuna *>(BATTLE) == nullptr) {
				turn_started = false;
				return false;
			}
			if (id == ITEM_NINJA_STAR || id == ITEM_DAGGER || id == ITEM_RANDOMAXE) {
				throw_id = id;
				if (id == ITEM_RANDOMAXE) {
					sprite->set_animation("throw_axe", use_callback, this);
					axe_turn = turn;
				}
				else {
					sprite->set_animation("throw_item", use_callback, this);
					if (id == ITEM_NINJA_STAR) {
						ninja_star_turn = turn;
					}
					else {
						dagger_turn = turn;
					}
				}
			}
			else {
				sprite->set_animation("use", use_callback, this);
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::RUN) {
			run_start = GET_TICKS();
			run_success = util::rand(1, (int)INSTANCE->stats.size()) == 1;
			if (run_success == false) {
				gfx::add_notification(GLOBALS->game_t->translate(175)/* Originally: Couldn't run... */);
			}
			else {
				DOGO_GLOBALS->run_away->play(false);
				for (auto p : BATTLE->get_players()) {
					p->get_sprite()->set_animation("walk_e");
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			GLOBALS->do_dialogue(turn->dialogue.tag, turn->dialogue.text, turn->dialogue.type, turn->dialogue.position, turn->dialogue.monitor);
		}
	}
	else {
		if (turn->turn_name == "attack_defend") {
			if (took_hit) {
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::ATTACK) {
			std::string cmbo = turn->turn_name.substr(7);
			if (cmbo == "heal") {
				if (heal_done) {
					int inc_mult = 0;
					float multi_mul = turn->targets.size() > 1 ? 1.1f : 1.0f;
					int inc = (HEAL_AMOUNT / turn->targets.size()) * multi_mul;

					doing_heal = false;

					for (size_t i = 0; i < turn->targets.size(); i++) {
						auto p = turn->targets[i];

						auto s = p->get_stats();

						if (s->hp <= 0) {
							continue;
						}

						s->hp = MIN(s->fixed.max_hp, s->hp + inc);
						
						SDL_Colour colour;
						SDL_Colour shadow_colour;
						std::string text;
						get_use_item_info(inc, ITEM_APPLE, colour, shadow_colour, text);
						util::Point<int> pos = static_cast<Battle_Player *>(p)->get_draw_pos();
						pos.x -= 5 + shim::font->get_text_width(text);
						pos.y -= shim::tile_size/4;
						if (static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
							pos.x = shim::screen_size.w - pos.x - shim::font->get_text_width(text);
						}
						NEW_SYSTEM_AND_TASK(BATTLE)
						wedge::Special_Number_Step *step = new wedge::Special_Number_Step(colour, shadow_colour, text, pos, wedge::Special_Number_Step::RISE, new_task, false);
						ADD_STEP(step)
						ADD_TASK(new_task)
						FINISH_SYSTEM(BATTLE)

						inc_mult++;
					}

					Combo c = get_combo(cmbo);

					if (inc_mult != 0) {
						int n;
						if (easy_combos) {
							n = 2;
						}
						else {
							n = (int)c.size();
						}
						for (int i = 0; i < n; i++) {
							inc_mult_level();
						}
					}

					turn_started = false;
					return false;
				}
			}
			else if (attack_done) {
				int inc_mult = 0;
				float multi_mul = turn->targets.size() > 1 ? 1.1f : 1.0f;

				for (size_t i = 0; i < turn->targets.size(); i++) {
					auto p = turn->targets[i];

					if (cmbo == "fast") {
						p->set_buff(wedge::Battle_Entity::FAST);
						p->set_fast(true, GET_TICKS() + 30000);
						inc_mult++;
					}
					else {
						int damage = MAX(1, stats->fixed.attack-p->get_stats()->fixed.defence);
						damage *= get_combo_multiplier(cmbo) * multiplier;
						damage = int(damage / (float)turn->targets.size() * multi_mul);
						bool dodged = add_special_number(this, p, damage, true, true);
						if (damage != 0 && dodged == false) {
							inc_mult++;
						}
					}
				}

				Combo c = get_combo(cmbo);

				if (inc_mult != 0) {
					int n;
					if (easy_combos) {
						n = 2;
					}
					else {
						n = (int)c.size();
					}
					for (int i = 0; i < n; i++) {
						inc_mult_level();
					}
				}

				turn_started = false;
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::ITEM) {
			if (doing_axe) {
				Uint32 now = GET_TICKS();
				if (now >= axe_start+arch_time && axe_hit == false) {
					axe_hit = true;

					auto t = axe_turn->targets[0];
					
					int damage = MAX(1, stats->fixed.attack-t->get_stats()->fixed.defence) * 100;
		
					if (dynamic_cast<Enemy_BigTuna *>(t)) {
						damage = 1;
					}
				
					add_special_number(this, t, damage, true, true);
				}
				if (now >= axe_start+arch_time+AXE_STAY_TIME) {
					turn_started = false;
					doing_axe = false;
					return false;
				}
			}
			else if (doing_ninja_star) {
				Uint32 now = GET_TICKS();
				if (now >= ninja_star_end) {
					ninja_star_targets_done++;
					if (ninja_star_targets_done == (int)ninja_star_turn->targets.size()+1) {
						DOGO_GLOBALS->ninja_star_loop->stop();
						turn_started = false;
						doing_ninja_star = false;
						return false;
					}
					else {
						if (ninja_star_targets_done <= (int)ninja_star_turn->targets.size()) {
							auto t = ninja_star_turn->targets[ninja_star_targets_done-1];
							
							int damage = t->get_stats()->fixed.max_hp * 0.25f;

							if (dynamic_cast<Enemy_BigTuna *>(t)) {
								damage = 1;
							}
				
							add_special_number(this, t, damage, true, true);
						}
					}
					calc_ninja_star_pos();
				}
			}
			else if (doing_dagger) {
				Uint32 now = GET_TICKS();
				if (now >= dagger_impact && dagger_hit == false) {
					dagger_hit = true;
					auto t = dagger_turn->targets[0];
					
					int damage = MAX(1, stats->fixed.attack-t->get_stats()->fixed.defence) * 50;
		
					if (dynamic_cast<Enemy_BigTuna *>(t)) {
						damage = 1;
					}
				
					add_special_number(this, t, damage, true, true);
				}
				else if (now >= dagger_end) {
					turn_started = false;
					doing_dagger = false;
					return false;
				}
			}
			else if (use_done) {
				int index = atoi(turn->turn_name.c_str());
				wedge::Object *inv = INSTANCE->inventory.get_all();
				int id = inv[index].id;
				if (id != ITEM_NINJA_STAR && id != ITEM_DAGGER && id != ITEM_RANDOMAXE) {
					for (auto e : turn->targets) {
						if (id == ITEM_CARAMEL) {
							e->get_sprite()->set_animation("stand_w");
						}
						int amount = INSTANCE->inventory.use(index, e->get_stats());
						SDL_Colour colour;
						SDL_Colour shadow_colour;
						std::string text;
						get_use_item_info(amount, id, colour, shadow_colour, text);
						util::Point<int> pos = static_cast<Battle_Player *>(e)->get_draw_pos();
						pos.x -= 5 + shim::font->get_text_width(text);
						pos.y -= shim::tile_size/4;
						if (static_cast<Battle_Game *>(BATTLE)->is_sneak_attack()) {
							pos.x = shim::screen_size.w - pos.x - shim::font->get_text_width(text);
						}
						NEW_SYSTEM_AND_TASK(BATTLE)
						wedge::Special_Number_Step *step = new wedge::Special_Number_Step(colour, shadow_colour, text, pos, wedge::Special_Number_Step::RISE, new_task, false);
						ADD_STEP(step)
						ADD_TASK(new_task)
						FINISH_SYSTEM(BATTLE)
					}
					turn_started = false;
					return false;
				}
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::RUN) {
			if (run_success) {
				for (auto p : BATTLE->get_players()) {
					auto player = dynamic_cast<Battle_Player *>(p);
					player->set_run_offset(player->get_run_offset() + util::Point<float>(DEFAULT_WALK_SPEED*(inverse_x ? -1 : 1)*shim::tile_size, 0.0f));
					gfx::Sprite *sprite = p->get_sprite();
					if (sprite) {
						std::vector<Uint32> v = sprite->get_frame_times();
						Uint32 total = 0;
						for (size_t i = 0; i < v.size(); i++) {
							total += v[i];
						}
						Uint32 elapsed = sprite->get_elapsed();
						Uint32 t = elapsed % total;
						float p = t / (float)total;
						float dest_angle;
						int tilt_degrees = 10.0f;
						if (p < 0.5f) {
							p /= 0.5f;
							dest_angle = tilt_degrees * M_PI / 360.0f;
						}
						else {
							p -= 0.5f;
							p /= 0.5f;
							dest_angle = -tilt_degrees * M_PI / 360.0f;
						}
						float a;
						if (p < 0.5f) {
							p /= 0.5f;
							a = p * dest_angle;
						}
						else {
							p -= 0.5f;
							p /= 0.5f;
							a = (1.0f - p) * dest_angle;
						}
						wedge::Direction dir = wedge::DIR_E;
						if (dir == wedge::DIR_E || dir == wedge::DIR_N) {
							a = -a;
						}
						tilt_angle = a;
					}
				}
			}
			if (GET_TICKS() >= run_start+2000) {
				if (run_success) {
					BATTLE->set_done(true);
				}
				turn_started = false;
				return false;
			}
		}
		else if (turn->turn_type == wedge::Battle_Game::DIALOGUE) {
			if (GLOBALS->dialogue_active(BATTLE) == false) {
				turn_started = false;
				return false;
			}
		}
	}
	return true;
}

void Battle_Player::set_attack_done(bool done)
{
	attack_done = done;
}
	
void Battle_Player::set_use_done(bool done)
{
	use_done = done;
}
	
int Battle_Player::get_max_turns()
{
	int max = 2;
	int extra_turns = stats->fixed.get_extra(EXTRA_TURNS);
	if (extra_turns > 0) {
		max += extra_turns;
	}
	return max;
}
	
bool Battle_Player::take_hit(wedge::Battle_Entity *actor, int damage)
{
	took_hit = true;
	if (sprite->get_animation() == "attack_defend") {
		damage = 1;
		sprite->set_animation("idle");
	}
	if (damage > 0) {
		reset_mult_level();
		sprite->set_animation("hit", hit_callback, this);
	}
	return wedge::Battle_Player::take_hit(actor, damage);
}

void Battle_Player::calc_ninja_star_pos()
{
	util::Point<float> start, end;
	if (ninja_star_targets_done == 0) {
		start = get_draw_pos() + util::Point<int>(-5, shim::tile_size/2);
	}
	else {
		auto enemy = static_cast<Battle_Enemy *>(ninja_star_turn->targets[ninja_star_targets_done-1]);
		gfx::Sprite *sprite = enemy->get_sprite();
		util::Point<int> topleft, bottomright;
		sprite->get_bounds(topleft, bottomright);
		start = enemy->get_position() + topleft + (bottomright-topleft)/2;
	}
	if (ninja_star_targets_done >= (int)ninja_star_turn->targets.size()) {
		util::Point<float> prev;
		if (ninja_star_turn->targets.size() == 1) {
			prev = get_draw_pos() + util::Point<int>(-5, shim::tile_size/2);
		}
		else {
			auto enemy = static_cast<Battle_Enemy *>(ninja_star_turn->targets[ninja_star_targets_done-2]);
			gfx::Sprite *sprite = enemy->get_sprite();
			util::Point<int> topleft, bottomright;
			sprite->get_bounds(topleft, bottomright);
			prev = enemy->get_position() + topleft + (bottomright-topleft)/2;
		}
		float a = (start-prev).angle();
		end.x = start.x + cos(a) * 100;
		end.y = start.y + sin(a) * 100;
	}
	else {
		auto enemy = static_cast<Battle_Enemy *>(ninja_star_turn->targets[ninja_star_targets_done]);
		gfx::Sprite *sprite = enemy->get_sprite();
		util::Point<int> topleft, bottomright;
		sprite->get_bounds(topleft, bottomright);
		end = enemy->get_position() + topleft + (bottomright-topleft)/2;
	}

	ninja_star_start_pos = start;
	ninja_star_end_pos = end;

	float l = (ninja_star_end_pos-ninja_star_start_pos).length();
	
	ninja_star_start = GET_TICKS();

	ninja_star_end = ninja_star_start + (l / 100) * PROJECTILE_TIME_PER_100;
}

void Battle_Player::inc_mult_level()
{
	mult_level++;
	if (mult_level == 5) {
		multiplier = 1.25f;
		show_mult_inc(1);
		util::achieve((void *)ACH_PLUS);
	}
	else if (mult_level == 10) {
		multiplier = 1.5f;
		show_mult_inc(2);
		util::achieve((void *)ACH_PLUSPLUS);
	}
	else if (mult_level == 15) {
		multiplier = 2.0f;
		show_mult_inc(3);
		util::achieve((void *)ACH_PLUSPLUSPLUS);
	}
}

void Battle_Player::reset_mult_level()
{
	mult_level = 0;
	multiplier = 1.0f;
	mult_stage = 0;
}

int Battle_Player::get_mult_level()
{
	return mult_level;
}

void Battle_Player::show_mult_inc(int level)
{
	mult_stage = level;

	switch (level) {
		case 1:
			DOGO_GLOBALS->mult1_s->play(false);
			break;
		case 2:
			DOGO_GLOBALS->mult2_s->play(false);
			break;
		case 3:
			DOGO_GLOBALS->mult3_s->play(false);
			break;
	}
	
	NEW_SYSTEM_AND_TASK(BATTLE)
	ADD_STEP(new Multi_Step(level, new_task))
	ADD_TASK(new_task)
	FINISH_SYSTEM(BATTLE)
}

void Battle_Player::set_heal_done(bool done)
{
	heal_done = done;
}

util::Point<float> Battle_Player::calc_arch(float p)
{
	p = 1.0f - p;

	util::Point<int> hand_exit = {-4, -10};

	util::Point<float> p2 = get_draw_pos() + hand_exit;
	
	Battle_Enemy *enemy = static_cast<Battle_Enemy *>(axe_target);
	if (arch_anim == "") {
		arch_anim = enemy->get_sprite()->get_animation();
	}
	std::string old_anim = enemy->get_sprite()->get_animation();
	if (old_anim != arch_anim) {
		enemy->get_sprite()->set_animation(arch_anim);
	}

	util::Point<int> topleft, bottomright;
	enemy->get_sprite()->get_bounds(topleft, bottomright);

	if (old_anim != arch_anim) {
		enemy->get_sprite()->set_animation(old_anim);
	}

	util::Point<float> p1;
	if (enemy->get_draw_as_player()) {
		gfx::Image *current_image = enemy->get_sprite()->get_current_image();
		util::Size<int> offset = util::Size<int>((shim::tile_size-current_image->size.w)/2, shim::tile_size-current_image->size.h);
		p1 = enemy->get_position() + util::Point<int>(offset.w, offset.h);
		p1 += util::Point<float>(float(topleft.x + (bottomright.x-topleft.x)/2.0f), bottomright.y + AXE_DIG);
	}
	else {
		p1 = enemy->get_position() + util::Point<float>(float(topleft.x + (bottomright.x-topleft.x)/2.0f), bottomright.y + AXE_DIG);
	}
	axe_bottom = p1.y - AXE_DIG;

	float dx = std::abs(p2.x-p1.x);
	float dy = std::abs(p2.y-p1.y);
	float radius = (dx+dy/2.0f)/2.0f; // guess

	// The loops guesses these
	util::Point<float> centre;
	float start_a;
	float end_a;
	float diff_y = 0.0f;

	while (true) {
		float a = atan(dy/radius);
		bool done = false;
		if (p1.y > p2.y) {
			centre = {p1.x+radius, MAX(p1.y, p2.y)};
			start_a = float(M_PI);
			end_a = M_PI*2 - a;
			float x = cos(end_a) * radius + centre.x;
			float y = sin(end_a) * radius + centre.y;
			float l = std::abs(x-p2.x);
			if (l < 1.0f) {
				done = true;
				diff_y = y - p2.y;
				end_a -= (diff_y/radius/2.0f) * (M_PI/2.0f);
			}
			else {
				if (x > p2.x) {
					radius -= 0.5f;
				}
				else {
					radius += 0.5f;
				}
			}
		}
		else {
			centre = {p2.x-radius, MAX(p1.y, p2.y)};
			start_a = M_PI + a;
			end_a = float(M_PI*2);
			float x = cos(start_a) * radius + centre.x;
			float y = sin(start_a) * radius + centre.y;
			float l = std::abs(x-p1.x);
			if (l < 1.0f) {
				done = true;
				diff_y = y - p1.y;
				start_a += (diff_y/radius/2.0f) * (M_PI/2.0f);
			}
			else {
				if (x > p1.x) {
					radius += 0.5f;
				}
				else {
					radius -= 0.5f;
				}
			}
		}
		if (done) {
			break;
		}
	}

	util::Point<float> pos = centre;
	float a = start_a + p * (end_a-start_a);
	float d_max = diff_y / radius;
	float m;
	if (p1.y < p2.y) {
		if (a < M_PI*1.5f) { // left half of arch
			/*
			float m1 = 1.0f + d_max;
			float m2 = 1.0f + (d_max/2.0f);
			*/
			float m1 = 1.0f + d_max;
			float m2 = 1.0f + d_max;
			float f = a - M_PI;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
		else { // right half
			/*
			float m1 = 1.0f + (d_max/2.0f);
			float m2 = 1.0f;
			*/
			float m1 = 1.0f;
			float m2 = 1.0f;
			float f = a - M_PI*1.5f;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
	}
	else {
		if (a < M_PI*1.5f) {
			/*
			float m1 = 1.0f;
			float m2 = 1.0f + (d_max/2.0f);
			*/
			float m1 = 1.0f;
			float m2 = 1.0f;
			float f = a - M_PI;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
		else {
			/*
			float m1 = 1.0f + (d_max/2.0f);
			float m2 = 1.0f + d_max;
			*/
			float m1 = 1.0f + d_max;
			float m2 = 1.0f + d_max;
			float f = a - M_PI*1.5f;
			float p3 = f / (M_PI/2.0f);
			m = m1 + (m2-m1) * p3;
		}
	}

	// FIXME?
	m = 1.0f;

	pos.x += cos(a) * radius;
	pos.y += sin(a) * radius * m;

	return pos;
}

