#ifndef GUI_H
#define GUI_H

#include <wedge2/main.h>

#include "globals.h"

class GUI_Drawing_Hook_Step;
class Widget;
class Widget_Checkbox;
class Widget_Controls_List;
class Widget_Label;
class Widget_List;
class Widget_Main_Menu_Text_Button;
class Widget_Save_Slot;
class Widget_Settings_List;
class Widget_Slider;
class Widget_Text_Button;
class Widget_Vertical_Slider;
class Widget_Window;

class Dogo_GUI : public gui::GUI {
public:
	Dogo_GUI();
	virtual ~Dogo_GUI();
};

class Title_GUI : public Dogo_GUI {
public:
	Title_GUI();
	virtual ~Title_GUI();

	virtual void draw();
	virtual void update();
	virtual void resize(util::Size<int> new_size);
	void handle_event(TGUI_Event *event);

	void focus_button(int n);
	
private:
	void set_container_pad();

	TGUI_Widget *container;
	Widget_Text_Button *play_button;
	Widget_Text_Button *settings_button;
	Widget_Text_Button *quit_button;

	gfx::Image *logo;
#ifdef DEMO
	gfx::Image *logo_demo;
#endif
	gfx::Image *title_wall_l;
	gfx::Image *title_wall_r;
};

class Gems_GUI : public Dogo_GUI
{
public:
	Gems_GUI();
	virtual ~Gems_GUI();

	void draw();
	void draw_fore();
	void handle_event(TGUI_Event *event);

	void lost_device();
	void found_device();

private:

	static gfx::Image *work1;
	static gfx::Image *work2;
	static gfx::Image *work_src;
	static gfx::Image *work_dest;
	static gfx::Image *image_l;
	static gfx::Image *image_r;
	static int count;

	Uint32 start;
	Uint32 next;
};

class Save_Slot_GUI : public Gems_GUI
{
public:
	static const int NUM_SLOTS = 3;
	static const int NUM_AUTOSAVES = 5;

	Save_Slot_GUI(bool loading = true, int start_slot = 0);
	virtual ~Save_Slot_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void draw();

	void erase();
	void load(util::JSON *json);
	void load();
	void load_autosave(int i);
	void show_autosaves();
	void select_difficulty(Difficulty difficulty);
	void new_game(int answer);
	void show_sub_menu(int selected);

	void saved();
	void save_failed();
	
	void set_text();

private:
	int get_selected_number();

	Widget_Save_Slot *slots[NUM_SLOTS];

	int selected;

	Difficulty difficulty;

	bool loading;
	int prev_slot;
};

class Yes_No_GUI : public Dogo_GUI
{
public:
	Yes_No_GUI(std::string text, bool escape_cancels, util::Callback callback = 0, void *callback_data = 0, bool shrink_to_fit = true);
	virtual ~Yes_No_GUI();

	void update();
	void handle_event(TGUI_Event *event);
	void draw_back();
	void draw();
	void draw_fore();

	void set_selected(bool yes_no);
	void hook_omnipresent(bool hook, bool last = false);

	void set_b1_text(std::string b1_text);
	void set_b2_text(std::string b2_text);

	bool get_escape_cancels();

private:
	Widget_Text_Button *yes_button;
	Widget_Text_Button *no_button;

	bool escape_cancels;

	util::Callback callback;
	void *callback_data;

	bool _hook_omnipresent;
	bool hook_draw_last;
	int count;
	GUI_Drawing_Hook_Step *drawing_hook;
};

// NOTE: *_Multiple_Choice_GUIs need to be resized, then layed out right after creating them!!!

class Multiple_Choice_GUI : public Dogo_GUI {
public:
	struct Callback_Data {
		int choice;
		void *userdata;
	};

	Multiple_Choice_GUI(bool tint_screen, std::string caption, std::vector<std::string> choices, int escape_choice, util::Callback callback, void *callback_data, int lines_to_show = 2, int width = 100, bool shrink_to_fit = true, Widget_List *custom_list = NULL, bool draw_window = true, int win_border = WIN_BORDER, bool show_combo_tip = false);
	virtual ~Multiple_Choice_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void get_height(int &w, int &h, int &num_lines);

	virtual void resize(util::Size<int> new_size); // must override this

	void set_selected(int index);

	void set_wrap(bool wrap);

	Widget_List *get_list();

	int get_escape_choice();

protected:
	Widget *window;
	Widget_Label *caption_label;
	Widget_List *list;

	std::string caption;

	util::Callback callback;
	void *callback_data;

	bool exit_menu;

	int escape_choice; // escape activates this choice. If it's -1, escape does nothing. If it's -2, escape dismisses the dialog with no action. -3 can be used to close the dialog but still pass -3 to the callback

	int lines_to_show;
	bool shrink_to_fit;

	int win_border;
};

class Positioned_Multiple_Choice_GUI : public Multiple_Choice_GUI {
public:
	Positioned_Multiple_Choice_GUI(bool tint_screen, std::string caption, std::vector<std::string> choices, int escape_choice, int horiz_pos/*-1, 0, 1=left, center, right*/, int vert_pos/*same as horiz_pos*/, int padding_left, int padding_right, int padding_top, int padding_bottom, float screen_margin_x, float screen_margin_y, util::Callback callback, void *callback_data, int lines_to_show = 2, int width = 100, bool shrink_to_fit = true, float width_ratio = 0.571f/*4/7ths*/, Widget_List *custom_list = NULL, bool draw_window = true, int win_border = WIN_BORDER, bool show_combo_tip = false);
	virtual ~Positioned_Multiple_Choice_GUI();

	void resize(util::Size<int> new_size);

	void set_padding(int padding_left, int padding_right, int padding_top, int padding_bottom);

private:
	int horiz_pos;
	int vert_pos;
	int padding_left;
	int padding_right;
	int padding_top;
	int padding_bottom;
	float screen_margin_x;
	float screen_margin_y;
	float width_ratio;
	int width;
};

class Battle_Multiple_Choice_GUI : public Multiple_Choice_GUI {
public:
	Battle_Multiple_Choice_GUI(std::string caption, std::vector<std::string> choices, int escape_choice, util::Callback callback, void *callback_data);
	virtual ~Battle_Multiple_Choice_GUI();

	void resize(util::Size<int> new_size);

	int get_selected();

private:
};

class Notification_GUI : public Dogo_GUI
{
public:
	struct Callback_Data {
		void *userdata;
	};

	Notification_GUI(std::string text, util::Callback callback = 0, void *callback_data = 0, bool shrink_to_fit = true);
	virtual ~Notification_GUI();

	void update();
	void handle_event(TGUI_Event *event);

	void draw_back();
	void draw();
	void draw_fore();
	
	void hook_omnipresent(bool hook, bool last = false);

private:
	Widget_Text_Button *ok_button;

	util::Callback callback;
	void *callback_data;

	bool _hook_omnipresent;
	bool hook_draw_last;
	int count;
	GUI_Drawing_Hook_Step *drawing_hook;
};

class Settings_GUI : public Gems_GUI
{
public:
	static void static_start();

	Settings_GUI(bool disable_language);
	virtual ~Settings_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void draw();
	
	void set_top_selected(int top, int selected);

	Widget_Settings_List *get_list();

private:

	Widget_Settings_List *list;
};

class Language_GUI : public Gems_GUI
{
public:
	Language_GUI();
	virtual ~Language_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void draw();

	void done();

private:

	Widget_Settings_List *list;
};

class Video_Settings_GUI : public Gems_GUI {
public:
	Video_Settings_GUI(bool fullscreen, int settings_top, int settings_selected);
	virtual ~Video_Settings_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void draw();
	void found_device();

private:
	void set_text();
	void set_selected();

	Widget_Settings_List *list;

	std::vector< util::Size<int> > modes;

	bool fs;
	int settings_top;
	int settings_selected;
};

class Audio_Settings_GUI : public Gems_GUI {
public:
	Audio_Settings_GUI(int settings_top, int settings_selected);
	virtual ~Audio_Settings_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void draw();

private:
	Widget_Vertical_Slider *sfx_slider;
	Widget_Vertical_Slider *music_slider;
	int settings_top;
	int settings_selected;
};

class Controls_Settings_GUI : public Gems_GUI {
public:
	Controls_Settings_GUI(bool keyboard, int settings_top, int settings_selected);
	virtual ~Controls_Settings_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void draw();
	void draw_fore();

	void quit(bool apply);
	void quit2(bool apply);
	void quit0(bool apply, bool tab);

private:
	void set_text();
	std::string get_key_name(int code);
	int index_of(int i);

	Widget_Controls_List *list;

	bool keyboard;

	enum Control {
		ACTION = 0, // keyboard only, joystick uses joy_b1
		BACK,
		CONFIRM,
		PAUSE,
		BMENU,
		SWITCH,
		CANCEL,
		B1,
		B2,
		B3,
		B4,
		FS,
		L,
		R,
		U,
		D,
		CONTROL_SIZE
	};

	int controls[CONTROL_SIZE];

	bool assigning;
	Control which_assigning;
	
	int settings_top;
	int settings_selected;
};

class Miscellaneous_Settings_GUI : public Gems_GUI {
public:
	Miscellaneous_Settings_GUI(int settings_top, int settings_selected);
	virtual ~Miscellaneous_Settings_GUI();

	void handle_event(TGUI_Event *event);
	void draw();
	void update();

	void bail();

private:
	Widget_Checkbox *easy_combos_checkbox;
	Widget_Checkbox *safe_mode_checkbox;
	Widget_Checkbox *onscreen_controller_checkbox;
	Widget_Checkbox *rumble_enabled_checkbox;
	Widget_Checkbox *simple_turn_display_checkbox;
	int settings_top;
	int settings_selected;
	bool have_content;
};

class Player_Stats_GUI : public Dogo_GUI {
public:
	struct Callback_Data {
		bool cancelled;
		void *userdata;
	};

	Player_Stats_GUI(int character_index, util::Callback callback, void *callback_data);
	virtual ~Player_Stats_GUI();

	void update();
	void handle_event(TGUI_Event *event);
	void draw_back();
	void draw();
	void draw_fore();

private:
	void set_labels();

	Widget_Label *hp_label;
	Widget_Label *mp_label;
	Widget_Text_Button *button;

	util::Callback callback;
	void *callback_data;

	int character_index;

	int count;
	GUI_Drawing_Hook_Step *drawing_hook;
};

class Get_Number_GUI : public gui::GUI
{
public:
	struct Callback_Data {
		int number;
		void *userdata;
	};

	Get_Number_GUI(std::string text, int stops, int initial_value, util::Callback callback, void *callback_data);
	virtual ~Get_Number_GUI();

	void handle_event(TGUI_Event *event);
	void update();

private:
	Widget_Slider *slider;
	Widget_Label *value_label;
	Widget_Text_Button *ok_button;
	Widget_Text_Button *cancel_button;

	util::Callback callback;
	void *callback_data;
};

// Note: needs to be resize()'d right away like Multiple_Choice_GUI
class Battle_Multi_Confirm_GUI : public Dogo_GUI
{
public:
	struct Callback_Data {
		bool confirmed;
		void *userdata;
	};

	Battle_Multi_Confirm_GUI(util::Callback callback, void *callback_data);
	virtual ~Battle_Multi_Confirm_GUI();

	void handle_event(TGUI_Event *event);
	void update();
	void resize(util::Size<int> new_size);

private:
	void set_text();

	Widget_Label *label;
	Widget_Text_Button *confirm_button;
	Widget_Window *window;

	util::Callback callback;
	void *callback_data;
};

#endif // GUI_H
