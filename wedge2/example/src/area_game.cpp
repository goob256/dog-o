#include <wedge2/area.h>
#include <wedge2/area_game.h>
#include <wedge2/chest.h>
#include <wedge2/general.h>

#include "area_game.h"
#include "battles.h"
#include "inventory.h"

class Area_Hooks_Example : public wedge::Area_Hooks
{
public:
	Area_Hooks_Example(wedge::Area *area) :
		wedge::Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(5, 0), util::Size<int>(2, 1));
		z.area_name = "example2";
		z.topleft_dest = util::Point<int>(5, 6);
		z.direction = wedge::DIR_N;
		scroll_zones.push_back(z);
	}
	
	virtual ~Area_Hooks_Example()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/example.mml");

		if (new_game) {
			wedge::Chest *chest = new wedge::Chest("chest", "chest", OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_POTION, 1));
			chest->start(area);
			chest->set_position(util::Point<int>(10, 5));
			area->get_entities().push_back(chest);
		}

		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		util::Point<int> player_pos = AREA->get_player(0)->get_position();
		if (get_scroll_zones(player_pos).size() > 0 || get_fade_zone(player_pos) != NULL) {
			return NULL;
		}

		return new Battle_2Slimes();
	}
};

class Area_Hooks_Example2 : public wedge::Area_Hooks
{
public:
	Area_Hooks_Example2(wedge::Area *area) :
		wedge::Area_Hooks(area)
	{
		Scroll_Zone z;
		z.zone = util::Rectangle<int>(util::Point<int>(5, 6), util::Size<int>(2, 1));
		z.area_name = "start";
		z.topleft_dest = util::Point<int>(5, 0);
		z.direction = wedge::DIR_S;
		scroll_zones.push_back(z);
	}
	
	virtual ~Area_Hooks_Example2()
	{
	}

	bool start(bool new_game, bool loaded, std::string save)
	{
		audio::play_music("music/example.mml");

		return true;
	}

	wedge::Battle_Game *get_random_battle()
	{
		util::Point<int> player_pos = AREA->get_player(0)->get_position();
		if (get_scroll_zones(player_pos).size() > 0 || get_fade_zone(player_pos) != NULL) {
			return NULL;
		}

		return new Battle_2Slimes();
	}
};

//--

Area_Game::~Area_Game()
{
}

wedge::Area_Hooks *Area_Game::get_area_hooks(std::string area_name, wedge::Area *area)
{
	wedge::Area_Hooks *hooks = NULL;

	if (area_name == "start") {
		hooks = new Area_Hooks_Example(area);
	}
	else if (area_name == "example2") {
		hooks = new Area_Hooks_Example2(area);
	}
	else {
		util::debugmsg("No area hooks for '%s'\n", area_name.c_str());
	}

	return hooks;
}

void Area_Game::draw()
{
	gfx::clear(shim::black);

	if (scrolling_in) {
		for (int i = 0; i < 2; i++) {
			wedge::Area::Layer_Spec spec;

			if (i == 0) {
				spec = wedge::Area::BELOW;
			}
			else {
				spec = wedge::Area::ABOVE;
			}

			util::Size<int> tilemap_size = current_area->get_tilemap()->get_size() * shim::tile_size;
			util::Point<float> maximum(shim::screen_size.w, shim::screen_size.h);
			maximum.x = MIN(maximum.x, tilemap_size.w);
			maximum.y = MIN(maximum.y, tilemap_size.h);
			util::Point<float> scrolled = scroll_offset * maximum;
			util::Point<int> player_pos = players[0]->get_position();
			util::Point<float> player_offset = util::Point<float>(0.0f, 0.0f);
			util::Size<int> player_size = players[0]->get_size();
			util::Point<float> sz(player_size.w / 2.0f, 1.0f - player_size.h / 2.0f);
			wedge::add_tiles(player_pos, player_offset, sz);
			//util::Point<float> curr_offset = current_area->get_centred_offset(players[0]->get_position(), util::Point<float>(0.0f, 0.0f), true);
			util::Point<float> curr_offset = current_area->get_centred_offset(player_pos, player_offset, true);
			curr_offset -= scrolled;
			current_area->draw(curr_offset, spec);
			util::Point<int> next_pos = next_area_positions[0];
			util::Point<float> next_o(0.0f, 0.0f);
			wedge::add_tiles(next_pos, next_o, sz);
			//util::Point<float> next_offset = next_area->get_centred_offset(next_area_positions[0], util::Point<float>(0.0f, 0.0f), true);
			util::Point<float> next_offset = next_area->get_centred_offset(next_pos, next_o, true);
			util::Point<float> no;
			if (scroll_increment.x < 0 || scroll_increment.y < 0) {
				util::Point<float> o = curr_offset;
				if (tilemap_size.w < shim::screen_size.w) {
					o.x -= (shim::screen_size.w-tilemap_size.w)/2.0f;
				}
				if (tilemap_size.h < shim::screen_size.h) {
					o.y -= (shim::screen_size.h-tilemap_size.h)/2.0f;
				}
				if (scroll_increment.x == 0.0f) {
					maximum.x = o.x;
				}
				else {
					maximum.y = o.y;
				}
				no = next_offset - (maximum - o);
				next_area->draw(next_offset - (maximum - o), spec);
			}
			else {
				if (scroll_increment.x == 0.0f) {
					maximum.x = scrolled.x;
				}
				else {
					maximum.y = scrolled.y;
				}
				no = next_offset + (maximum - scrolled);
				next_area->draw(next_offset + (maximum - scrolled), spec);
			}
			if (i == 0) {
				std::vector<wedge::Map_Entity *> entities = players;
				std::sort(entities.begin(), entities.end(), wedge::entity_y_compare);
				for (size_t i = 0; i < entities.size(); i++) {
					wedge::Map_Entity *p = entities[i];
					p->draw(curr_offset + (scroll_offset * shim::tile_size));
				}
			}
		}
	}
	else {
		current_area->draw();
		Game::draw();
	}
}
