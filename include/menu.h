#ifndef MENU_H
#define MENU_H

#include <wedge2/main.h>

#include "combo.h"
#include "gui.h"

class Widget_Image_Button;
class Widget_List;
class Widget_Quantity_List;
class Widget_Map;

class Menu_Game : public wedge::Game
{
public:
	Menu_Game();
	virtual ~Menu_Game();

	bool start();
	bool run();
	void draw();
	void draw_fore();
};

class Menu_GUI : public Dogo_GUI
{
public:
	static const int BORDER = 3;
	static const int EDGE_X = 10;
	static const int EDGE_Y = 8;

	Menu_GUI();
	virtual ~Menu_GUI();

	void draw();
	void handle_event(TGUI_Event *event);

protected:
	std::string menu_name;
};

class Multiplayer_Menu_GUI : public Menu_GUI
{
public:
	Multiplayer_Menu_GUI();
	virtual ~Multiplayer_Menu_GUI();

	virtual void draw();
	virtual void draw_back();
	void handle_event(TGUI_Event *event);

protected:
	virtual void set_text() = 0;
	std::string get_switch_text();
	virtual void switch_chars();
	int player_index;
	bool draw_name;
};

class Practice_Menu_GUI : public Multiplayer_Menu_GUI
{
public:
	Practice_Menu_GUI();
	virtual ~Practice_Menu_GUI();

	void handle_event(TGUI_Event *event);
	void draw();
	void resize(util::Size<int> new_size);
	void update();

	bool is_practicing();

private:
	void switch_chars();
	int get_list_height(util::Size<int> screen_size);
	void get_konami();
	void set_text();

	Widget_Quantity_List *list;

	bool practicing;

	std::vector<Combo> combos;
	std::vector<std::string> combo_names;

	Combo_Detector *detector;

	Uint32 start_time;

	Uint32 good_end;

	int last_good;

	std::string konami;

	std::string orig_anim;

	bool osc_enabled;

	bool started_with_osc;

	audio::Sound *sfx;
};

class Main_Menu_GUI : public Menu_GUI
{
public:
	Main_Menu_GUI(int selected);
	virtual ~Main_Menu_GUI();

	void handle_event(TGUI_Event *event);
	void draw_back();
	void draw();
	void update();

private:
	
	TGUI_Widget *buttons;
	Widget_Text_Button *items_button;
	Widget_Text_Button *armour_button;
	Widget_Text_Button *stats_button;
	Widget_Text_Button *practice_button;
	Widget_Text_Button *progress_button;
	Widget_Text_Button *settings_button;
	Widget_Text_Button *save_button;
	Widget_Text_Button *quit_button;
};

class Stats_Menu_GUI : public Multiplayer_Menu_GUI
{
public:
	Stats_Menu_GUI();
	virtual ~Stats_Menu_GUI();

	void handle_event(TGUI_Event *event);
	void draw();

	void set_text();

private:
	void draw_gauge(int x, int y, int width, int full);
};

class Items_Menu_GUI : public Multiplayer_Menu_GUI
{
public:
	Items_Menu_GUI();
	virtual ~Items_Menu_GUI();

	void handle_event(TGUI_Event *event);
	void draw();
	void resize(util::Size<int> new_size);
	void update();

private:
	int get_list_height(util::Size<int> screen_size);
	void set_text();

	TGUI_Widget *container;
	Widget_Quantity_List *list;

	std::vector<int> inventory_indices;
};

class Map_Menu_GUI : public Menu_GUI
{
public:
	Map_Menu_GUI();
	virtual ~Map_Menu_GUI();

	void handle_event(TGUI_Event *event);
	void draw_back();

	void lost_device();
	void found_device();

	void draw_map(int w, int h, bool draw_portals);

private:
	
	Widget_Map *map_widget;
	gfx::Image *map;
	gfx::Image *map2;
};

class Armour_Menu_GUI : public Multiplayer_Menu_GUI
{
public:
	Armour_Menu_GUI();
	virtual ~Armour_Menu_GUI();

	void handle_event(TGUI_Event *event);
	void draw();
	void resize(util::Size<int> new_size);
	void update();

private:
	void push_belt(std::string belt, std::vector<int> &i, std::vector<int> &q, std::vector<std::string> &n, std::vector<int> i2, std::vector<int> q2, std::vector<std::string> n2);
	int get_list_height(util::Size<int> screen_size);
	void set_text();
	void equip_belt(std::vector<std::string> moves);

	TGUI_Widget *container;
	Widget_Quantity_List *list;

	std::vector<int> inventory_indices;
};

#endif // MENU_H
