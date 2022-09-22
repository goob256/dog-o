#ifndef WEDGE2_BATTLE_ENTITY_H
#define WEDGE2_BATTLE_ENTITY_H

#include "wedge2/main.h"
#include "wedge2/battle_game.h"
#include "wedge2/stats.h"

namespace wedge {

class WEDGE2_EXPORT Battle_Entity
{
public:
	enum Type
	{
		PLAYER = 0,
		ENEMY
	};

	enum Buff_Type {
		NONE = 0,
		FAST
	};
			
	Battle_Entity(Type type, std::string name);
	virtual ~Battle_Entity();

	virtual bool start();
	virtual void handle_event(TGUI_Event *event);
	virtual void draw_back();
	virtual void draw();
	virtual void draw_fore();
	virtual void run();
	virtual void lost_device();
	virtual void found_device();
	virtual void resize(util::Size<int> new_size); // screen size change

	virtual bool is_dead();

	Type get_type();
	std::string get_name();
	Base_Stats *get_stats();
	gfx::Sprite *get_sprite();

	bool is_defending();

	util::Point<int> get_decoration_offset(int decoration_width, util::Point<int> offset, int *flags);

	virtual float get_poison_odds(); // 0-1

	util::Point<float> get_spell_effect_offset();

	virtual int get_max_turns();
	virtual Battle_Game::Turn *get_turn() = 0;
	virtual bool take_turn(Battle_Game::Turn *turn) = 0; // return false when done

	virtual bool take_hit(Battle_Entity *actor, int damage);

	Buff_Type get_buff();
	void set_buff(Buff_Type buff);
	bool is_fast();
	void set_fast(bool fast, Uint32 fast_end = 0);
	Uint32 get_fast_end();

	void set_inverse_x(bool inverse_x);
	bool get_inverse_x();

	bool get_draw_as_player();

protected:
	Type type;
	std::string name;
	Base_Stats *stats;
	gfx::Sprite *sprite;
	bool defending;
	util::Point<float> spell_effect_offset;

	bool draw_shadow;
	util::Point<int> shadow_pos;
	util::Size<int> shadow_size;
	bool auto_shadow_pos;
	util::Point<int> shadow_offset;

	Buff_Type buff;
	bool fast; // can't just use a Buff_Type alone: buff can change but fast can stick around until end of turn
	Uint32 fast_end;

	bool inverse_x;
	
	SDL_Colour global_tint;
	bool global_flip_h;
	bool draw_as_player;

};

}

#endif // WEDGE2_BATTLE_ENTITY_H
