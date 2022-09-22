#ifndef AREA_GAME_H
#define AREA_GAME_H

#include <wedge2/area.h>
#include <wedge2/area_game.h>
#include <wedge2/inventory.h>

class Battle_Game;

class Area_Game : public wedge::Area_Game
{
public:
	Area_Game();
	virtual ~Area_Game();

	wedge::Area_Hooks *get_area_hooks(std::string area_name, wedge::Area *area);
	void draw();
	bool run();
	void handle_event(TGUI_Event *event);
	
	void set_next_fadeout_colour(SDL_Colour colour);
	
	int get_num_areas_created();

	wedge::Area *create_area(std::string name);
	wedge::Area *create_area(util::JSON::Node *json);

	void set_use_camera(bool use_camera);
	void set_camera(util::Point<float> camera);

	wedge::Game *create_menu();
	wedge::Game *create_shop(std::vector<wedge::Object> items);
	
	void battle_ended(wedge::Battle_Game *battle);

private:
	int area_create_count;
	
	bool use_camera;
	util::Point<float> camera;
};

wedge::Area_Hooks *get_area_hooks(std::string area_name, wedge::Area *area);
std::string get_friendly_name(std::string area_name);

#endif // AREA_GAME_H
