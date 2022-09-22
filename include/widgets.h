#ifndef WIDGETS_H
#define WIDGETS_H

#include <wedge2/main.h>

class Widget : public TGUI_Widget {
public:
	static void static_start();

	static void set_default_background_colour(SDL_Colour default_colour);

	Widget(int w, int h);
	Widget(float percent_w, float percent_h);
	Widget(int w, float percent_h);
	Widget(float percent_w, int h);
	Widget(TGUI_Widget::Fit fit, int other);
	Widget(TGUI_Widget::Fit fit, float percent_other);
	Widget(); // Fit both
	virtual ~Widget();

	virtual void draw();

	bool is_focussed();

	void set_background_colour(SDL_Colour colour);

protected:
	static SDL_Colour default_background_colour;

	void start();

	SDL_Colour background_colour;
};

class Widget_Window : public Widget
{
public:
	Widget_Window(int w, int h);
	Widget_Window(float percent_w, float percent_h);
	Widget_Window(int w, float percent_h);
	Widget_Window(float percent_w, int h);
	Widget_Window(TGUI_Widget::Fit fit, int other);
	Widget_Window(TGUI_Widget::Fit fit, float percent_other);
	virtual ~Widget_Window();

	void draw();

	void set_image(gfx::Image *image);
	void set_alpha(float alpha);

protected:
	void start();

	gfx::Image *image;
	float alpha;
};

class Widget_Button : public Widget {
public:
	Widget_Button(int w, int h);
	Widget_Button(float w, float h);
	Widget_Button(int w, float h);
	Widget_Button(float w, int h);
	virtual ~Widget_Button();

	virtual void handle_event(TGUI_Event *event);

	virtual bool pressed();

	void set_sound_enabled(bool enabled);

	void set_pressed(bool pressed);

	void set_mouse_only(bool mouse_only);

protected:
	void start();

	bool _pressed;
	bool _released;
	bool _hover;
	bool gotten;
	bool sound_enabled;
	bool mouse_only;
};

class Widget_Text_Button : public Widget_Button
{
public:
	Widget_Text_Button(std::string text);
	virtual ~Widget_Text_Button();

	void draw();

	void set_enabled(bool enabled);
	bool is_enabled();
	void set_text(std::string text);

	void set_normal_shadow_colour(SDL_Colour colour);
	void set_focussed_shadow_colour(SDL_Colour colour);
	void set_disabled_text_colour(SDL_Colour colour);

protected:
	void set_size();

	std::string text;

	bool enabled;
	
	SDL_Colour disabled_text_colour;
	SDL_Colour focussed_text_colour;
	SDL_Colour normal_text_colour;
	SDL_Colour disabled_shadow_colour;
	SDL_Colour focussed_shadow_colour;
	SDL_Colour normal_shadow_colour;
};

class Widget_Save_Slot : public Widget_Button
{
public:
	Widget_Save_Slot(int number); // number = 0-2
	virtual ~Widget_Save_Slot();

	void draw();

	bool exists();
	bool is_corrupt();

	void set_text();

protected:
	std::string time_text;
	std::string place_text;

	int number;

	bool _exists;
	bool corrupt;

	gfx::Image *row_img;

	Difficulty difficulty;
};

class Widget_Label : public Widget
{
public:
	static void static_start();
	static void set_default_text_colour(SDL_Colour default_colour);
	static void set_default_shadow_colour(SDL_Colour default_colour);

	Widget_Label(std::string text, int max_w, gfx::Font *font = NULL);
	virtual ~Widget_Label();

	void draw();

	void set_text(std::string text);
	void set_max_width(int width);

	void set_text_colour(SDL_Colour colour);
	void set_shadow_colour(SDL_Colour colour);

	void set_shadow_type(gfx::Font::Shadow_Type shadow_type);

	std::string get_text();

private:
	static SDL_Colour default_text_colour;
	static SDL_Colour default_shadow_colour;

	void start();

	std::string text;
	int max_w;
	
	SDL_Colour text_colour;

	SDL_Colour shadow_colour;
	gfx::Font::Shadow_Type shadow_type;
	gfx::Font *font;
};

class Widget_List : public Widget
{
public:
	static const int LONGPRESS_TIME = 500;

	static void static_start();
	static void set_default_normal_text_colour1(SDL_Colour default_colour);
	static void set_default_normal_text_colour2(SDL_Colour default_colour);
	static void set_default_highlight_text_colour1(SDL_Colour default_colour);
	static void set_default_highlight_text_colour2(SDL_Colour default_colour);
	static void set_default_disabled_text_colour1(SDL_Colour default_colour);
	static void set_default_disabled_text_colour2(SDL_Colour default_colour);
	static void set_default_text_shadow_colour(SDL_Colour default_colour);
	static void set_default_disabled_text_shadow_colour(SDL_Colour default_colour);
	static void set_default_odd_row_colour(SDL_Colour default_colour);

	Widget_List(int w, int h);
	Widget_List(float w, float h);
	Widget_List(int w, float h);
	Widget_List(float w, int h);
	virtual ~Widget_List();

	virtual void draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip = ""); // subclass this to customize

	void handle_event(TGUI_Event *event);
	void draw();

	virtual void set_items(std::vector<std::string> new_items); // you can overload this to set extra values
	int pressed();
	int get_selected();
	void set_selected(int selected);
	int get_size();
	int get_top();
	void set_top(int top);
	std::vector<std::string> get_items();
	std::string get_item(int index);

	void set_highlight(int index, bool onoff);
	bool is_highlighted(int index);

	void set_disabled(int index, bool onoff);
	bool is_disabled(int index);

	void set_normal_text_colour1(SDL_Colour colour);
	void set_normal_text_colour2(SDL_Colour colour);
	void set_highlight_text_colour1(SDL_Colour colour);
	void set_highlight_text_colour2(SDL_Colour colour);
	void set_disabled_text_colour1(SDL_Colour colour);
	void set_disabled_text_colour2(SDL_Colour colour);
	void set_text_shadow_colour(SDL_Colour colour);
	void set_disabled_text_shadow_colour(SDL_Colour colour);
	void set_odd_row_colour(SDL_Colour colour);

	void set_arrow_colour(SDL_Colour colour);
	void set_arrow_shadow_colour(SDL_Colour colour);

	void set_reserved_size(int pixels); // for auto-sizing lists
	virtual void resize();
	
	virtual int visible_rows();

	void set_wrap(bool wrap);

	void set_row_h(int row_h);

	void set_text_offset(util::Point<float> text_offset);

	void set_arrow_offsets(util::Point<float> top_o, util::Point<float> bottom_o);

	void set_always_show_arrows(bool always);

	void set_tips(std::vector<std::string> &tips);

protected:
	static SDL_Colour default_normal_text_colour1;
	static SDL_Colour default_normal_text_colour2; // brighter when selected
	static SDL_Colour default_highlight_text_colour1;
	static SDL_Colour default_highlight_text_colour2; // brighter when selected
	static SDL_Colour default_disabled_text_colour1;
	static SDL_Colour default_disabled_text_colour2;
	static SDL_Colour default_text_shadow_colour;
	static SDL_Colour default_disabled_text_shadow_colour;
	static SDL_Colour default_odd_row_colour;

	void start();
	void up();
	void down();
	int get_click_row(int y);
	void change_top(int rows);
	int used_height();
	void draw_scrollbar();
	int scrollbar_height();
	int scrollbar_pos();
	virtual void show_description();

	util::Point<int> top_arrow_pos();
	util::Point<int> bottom_arrow_pos();

	std::vector<std::string> items; // this generic class expects this to be filled, or at least filled with the number of empty strings in your list
	int top;
	int selected;
	int row_h;

	int pressed_item;

	bool mouse_down;
	bool scrollbar_down;
	bool clicked;
	util::Point<int> mouse_down_point;
	int mouse_down_row;
	int orig_mouse_down_row;
	int scrollbar_pos_mouse_down;

	std::vector<int> highlight;
	std::vector<int> disabled;
	
	SDL_Colour normal_text_colour1;
	SDL_Colour normal_text_colour2;
	SDL_Colour highlight_text_colour1;
	SDL_Colour highlight_text_colour2;
	SDL_Colour disabled_text_colour1;
	SDL_Colour disabled_text_colour2;
	SDL_Colour text_shadow_colour;
	SDL_Colour disabled_text_shadow_colour;
	SDL_Colour odd_row_colour;

	util::Size<int> arrow_size;

	SDL_Colour arrow_colour;
	SDL_Colour arrow_shadow_colour;

	bool has_scrollbar;

	Uint32 down_time;
	int down_selected;

	int reserved_size;

	Uint32 selected_time;

	bool wrap;

	util::Point<float> text_offset;
	
	util::Point<float> top_o;
	util::Point<float> bottom_o;

	bool always_show_arrows;

	std::vector<std::string> tips;
};

class Widget_Quantity_List : public Widget_List
{
public:
	Widget_Quantity_List(int w, int h);
	Widget_Quantity_List(float w, float h);
	Widget_Quantity_List(int w, float h);
	Widget_Quantity_List(float w, int h);
	virtual ~Widget_Quantity_List();
	
	void handle_event(TGUI_Event *event);

	void draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip = "");

	void set_items_extra(std::vector<int> quantities, std::vector<std::string> new_items, std::vector<std::string> descriptions);

	void set_hide_quantities(bool hide);

protected:
	void show_description();

	bool hide_quantities;
	std::vector<int> quantities;
	std::vector<std::string> descriptions;
	int max_q_len;
	Uint32 longpress_started;
	util::Point<int> longpress_pos;
};

class Widget_Combo_List : public Widget_Quantity_List
{
public:
	Widget_Combo_List(int w, int h);
	Widget_Combo_List(float w, float h);
	Widget_Combo_List(int w, float h);
	Widget_Combo_List(float w, int h);
	virtual ~Widget_Combo_List();

	void draw();
};

class Widget_Autosaves_List : public Widget_List
{
public:
	Widget_Autosaves_List(int w, int h);
	Widget_Autosaves_List(float w, float h);
	Widget_Autosaves_List(int w, float h);
	Widget_Autosaves_List(float w, int h);
	virtual ~Widget_Autosaves_List();

	void draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip = "");

	void set_items_extra(std::vector<std::string> times, std::vector<std::string> places);

protected:
	std::vector<std::string> places;
};

class Widget_Settings_List : public Widget_List
{
public:
	Widget_Settings_List(int visible_rows, std::string image_filename);
	virtual ~Widget_Settings_List();

	void draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip = "");
	int visible_rows();
	void set_items(std::vector<std::string> new_items);

protected:
	gfx::Image *row_img;
};

class Widget_Vertical_Slider : public Widget
{
public:
	static const int WIDTH = 4;

	Widget_Vertical_Slider(int height, int stops, int initial_value);
	virtual ~Widget_Vertical_Slider();

	void handle_event(TGUI_Event *event);
	void draw();

	int get_value();

protected:
	int stops;
	int value;
	bool mouse_down;
	math::Interpolator *interp;
};

class Widget_Slider : public Widget
{
public:
	static const int WIDTH = 4;

	Widget_Slider(int width, int stops, int initial_value);
	virtual ~Widget_Slider();

	void handle_event(TGUI_Event *event);
	void draw();

	int get_value();

	void set_action_button(Widget_Button *button);

protected:
	int stops;
	int value;
	bool mouse_down;
	math::Interpolator *interp;
	Widget_Button *action_button;
};

class Widget_Controls_List : public Widget_Settings_List
{
public:
	Widget_Controls_List(int visible_rows);
	virtual ~Widget_Controls_List();

	void draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip = "");

	void set_items_extra(std::vector<std::string> names, std::vector<std::string> assignments);

protected:
	std::vector<std::string> assignments;
};

class Widget_Checkbox : public Widget
{
public:
	static const int WIDTH = 5;

	Widget_Checkbox(std::string text, bool checked);
	virtual ~Widget_Checkbox();

	void handle_event(TGUI_Event *event);
	void draw();

	bool is_checked();

protected:
	std::string text;
	bool checked;
	bool mouse_down;
	math::Interpolator *interp;
};

class Widget_Map : public Widget
{
public:
	Widget_Map();
	virtual ~Widget_Map();

	void draw();

	void set_images(gfx::Image *map, gfx::Image *map2); // with/without portals for blinking

private:
	gfx::Image *image;
	gfx::Image *image2;
};

class Widget_Image : public Widget
{
public:
	static void static_start();
	static void set_default_shadow_colour(SDL_Colour default_colour);

	Widget_Image(gfx::Image *image, bool destroy = true);
	virtual ~Widget_Image();

	void draw();

	void set_shadow_colour(SDL_Colour colour);
	void set_shadow_type(gfx::Font::Shadow_Type type);

private:
	static SDL_Colour default_shadow_colour;

	gfx::Image *image;
	bool destroy;
	SDL_Colour shadow_colour;
	gfx::Font::Shadow_Type shadow_type;
};

class Widget_Image_Button : public Widget_Button
{
public:
	static void static_start();
	static void set_default_shadow_colour(SDL_Colour default_colour);

	Widget_Image_Button(gfx::Image *image, bool destroy = true);
	virtual ~Widget_Image_Button();

	void draw();

	void set_shadow_colour(SDL_Colour colour);
	void set_shadow_type(gfx::Font::Shadow_Type type);

private:
	static SDL_Colour default_shadow_colour;

	gfx::Image *image;
	bool destroy;
	SDL_Colour shadow_colour;
	gfx::Font::Shadow_Type shadow_type;
	math::Interpolator *interp;
};

#endif // WIDGETS_H
