#include <wedge2/area.h>
#include <wedge2/general.h>

#include "area_game.h"
#include "battle_game.h"
#include "general.h"
#include "globals.h"
#include "gui.h"
#include "widgets.h"

SDL_Colour Widget::default_background_colour;

void Widget::static_start()
{
	default_background_colour.r = 0;
	default_background_colour.g = 0;
	default_background_colour.b = 0;
	default_background_colour.a = 0;
	
	Widget_Image::static_start();
	Widget_Image_Button::static_start();
	Widget_Label::static_start();
	Widget_List::static_start();
}

void Widget::set_default_background_colour(SDL_Colour colour)
{
	default_background_colour = colour;
}

void Widget::set_background_colour(SDL_Colour colour)
{
	background_colour = colour;
}

void Widget::start()
{
	background_colour = default_background_colour;
}

Widget::Widget(int w, int h) :
	TGUI_Widget(w, h)
{
	start();
}

Widget::Widget(float percent_w, float percent_h) :
	TGUI_Widget(percent_w, percent_h)
{
	start();
}

Widget::Widget(int w, float percent_h) :
	TGUI_Widget(w, percent_h)
{
	start();
}

Widget::Widget(float percent_w, int h) :
	TGUI_Widget(percent_w, h)
{
	start();
}

Widget::Widget(TGUI_Widget::Fit fit, int other) :
	TGUI_Widget(fit, other)
{
	start();
}

Widget::Widget(TGUI_Widget::Fit fit, float percent_other) :
	TGUI_Widget(fit, percent_other)
{
	start();
}

Widget::Widget() :
	TGUI_Widget()
{
	start();
}

Widget::~Widget()
{
}

void Widget::draw()
{
	// This is used to clear the background to a darker colour, so don't do it unless this widget is part of
	// the topmost gui because it could happen twice giving a darker colour

	if (shim::guis.size() > 0) {
		TGUI_Widget *root = shim::guis.back()->gui->get_main_widget();
		TGUI_Widget *w = this;
		TGUI_Widget *parent;
		while ((parent = w->get_parent()) != NULL) {
			w = parent;
		}
		if (root != w) {
			return;
		}
	}

	if (background_colour.a != 0) {
		// Need to clear transforms temporarily because it might be part of a transition
		glm::mat4 old_mv, old_p;
		gfx::get_matrices(old_mv, old_p);
		gfx::set_default_projection(shim::real_screen_size, shim::screen_offset, shim::scale);
		gfx::update_projection();
		gfx::draw_filled_rectangle(background_colour, util::Point<int>(calculated_x, calculated_y), util::Size<int>(calculated_w, calculated_h));
		gfx::set_matrices(old_mv, old_p);
		gfx::update_projection();
	}
}

bool Widget::is_focussed()
{
	if (shim::guis.size() == 0) {
		return false;
	}
	if (gui != shim::guis.back()->gui) {
		return false;
	}
	return gui->get_focus() == this;
}

//--

Widget_Window::Widget_Window(int w, int h) :
	Widget(w, h)
{
	start();
}

Widget_Window::Widget_Window(float percent_w, float percent_h) :
	Widget(percent_w, percent_h)
{
	start();
}

Widget_Window::Widget_Window(int w, float percent_h) :
	Widget(w, percent_h)
{
	start();
}

Widget_Window::Widget_Window(float percent_w, int h) :
	Widget(percent_w, h)
{
	start();
}

Widget_Window::Widget_Window(TGUI_Widget::Fit fit, int other) :
	Widget(fit, other)
{
	start();
}

Widget_Window::Widget_Window(TGUI_Widget::Fit fit, float percent_other) :
	Widget(fit, percent_other)
{
	start();
}

void Widget_Window::start()
{
	image = nullptr;
	alpha = 1.0f;
}

Widget_Window::~Widget_Window()
{
}

void Widget_Window::draw()
{
	gfx::Image *i;

	if (image == nullptr) {
		i = DOGO_GLOBALS->gui_window;
	}
	else {
		i = image;
	}

	gfx::draw_9patch_tinted(make_translucent(shim::white, alpha), i, util::Point<int>(calculated_x, calculated_y), util::Size<int>(calculated_w, calculated_h));
}
	
void Widget_Window::set_image(gfx::Image *image)
{
	this->image = image;
}

void Widget_Window::set_alpha(float alpha)
{
	this->alpha = alpha;
}

//--

Widget_Button::Widget_Button(int w, int h) :
	Widget(w, h)
{
	start();
}

Widget_Button::Widget_Button(float w, float h) :
	Widget(w, h)
{
	start();
}

Widget_Button::Widget_Button(int w, float h) :
	Widget(w, h)
{
	start();
}

Widget_Button::Widget_Button(float w, int h) :
	Widget(w, h)
{
	start();
}

void Widget_Button::start()
{
	_pressed = false;
	_released = false;
	_hover = false;
	gotten = true;
	sound_enabled = true;
	accepts_focus = true;
	mouse_only = false;
}

Widget_Button::~Widget_Button()
{
}

void Widget_Button::handle_event(TGUI_Event *event)
{
	if (mouse_only && (event->type == TGUI_KEY_DOWN || event->type == TGUI_KEY_UP || event->type == TGUI_JOY_DOWN || event->type == TGUI_JOY_UP)) {
		return;
	}

	int x, y;

	if (use_relative_position) {
		x = relative_x;
		y = relative_y;
	}
	else {
		x = calculated_x;
		y = calculated_y;
	}

	if (event->type == TGUI_MOUSE_AXIS) {
		if (event->mouse.x >= x && event->mouse.x < x+calculated_w && event->mouse.y >= y && event->mouse.y < y+calculated_h) {
			_hover = true;
		}
		else {
			_hover = false;
		}
	}
	if (accepts_focus && gui->get_event_owner(event) == this) {
		if (event->type == TGUI_KEY_DOWN && event->keyboard.is_repeat == false) {
			if (event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) {
				if (gotten) {
					_pressed = true;
					_hover = true;
				}
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_JOY_DOWN && event->joystick.is_repeat == false) {
			if (event->joystick.button == GLOBALS->joy_action) {
				if (gotten) {
					_pressed = true;
					_hover = true;
				}
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_MOUSE_DOWN && event->mouse.is_repeat == false) {
			if (event->mouse.button == 1) {
				if (gotten) {
					_pressed = true;
				}
			}
			else {
				_pressed = false;
			}
			_hover = true;
		}
		else if (event->type == TGUI_KEY_UP && event->keyboard.is_repeat == false) {
			if (_pressed && event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) {
				if (gotten) {
					gotten = false;
					_released = true;
					_hover = false;
					if (sound_enabled) {
						if (DOGO_GLOBALS->button != 0) {
							DOGO_GLOBALS->button->play(false);
						}
					}
				}
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_JOY_UP && event->joystick.is_repeat == false) {
			if (_pressed && (event->joystick.button == GLOBALS->joy_action)) {
				if (gotten) {
					gotten = false;
					_released = true;
					_hover = false;
					if (sound_enabled) {
						if (DOGO_GLOBALS->button != 0) {
							DOGO_GLOBALS->button->play(false);
						}
					}
				}
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
			if (_pressed && (event->mouse.button == 1)) {
				if (gotten) {
					gotten = false;
					_released = true;
					if (sound_enabled) {
						if (DOGO_GLOBALS->button != 0) {
							DOGO_GLOBALS->button->play(false);
						}
					}
				}
			}
			else {
				_pressed = false;
			}
		}
	}
	else {
		if (event->type == TGUI_KEY_UP) {
			_pressed = false;
			_hover = false;
		}
		else if (event->type == TGUI_JOY_UP) {
			_pressed = false;
			_hover = false;
		}
		else if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
			_pressed = false;
			_hover = false;
		}
		else if ((event->type == TGUI_MOUSE_DOWN || event->type == TGUI_MOUSE_AXIS) && event->mouse.is_repeat == false) {
			_hover = false;
		}
	}
}

bool Widget_Button::pressed()
{
	bool r = _released;
	if (_released) {
		_pressed = _released = false;
	}
	gotten = true;
	return r;
}

void Widget_Button::set_sound_enabled(bool enabled)
{
	sound_enabled = enabled;
}

void Widget_Button::set_pressed(bool pressed)
{
	this->_pressed = this->_released = pressed;
}

void Widget_Button::set_mouse_only(bool mouse_only)
{
	this->mouse_only = mouse_only;
}

//--

Widget_Text_Button::Widget_Text_Button(std::string text) :
	Widget_Button(0, 0),
	text(text)
{
	enabled = true;
	set_size();

	disabled_text_colour = shim::palette[36];
	normal_text_colour = shim::palette[34];
	disabled_shadow_colour = shim::transparent;
	normal_shadow_colour = shim::transparent;
}

Widget_Text_Button::~Widget_Text_Button()
{
}

void Widget_Text_Button::draw()
{
	bool focussed = is_focussed();

	int x, y;

	if (use_relative_position) {
		x = relative_x;
		y = relative_y;
	}
	else {
		x = calculated_x;
		y = calculated_y;
	}

	SDL_Colour colour;

	if (enabled == false) {
		colour = disabled_text_colour;
	}
	else {
		if (focussed) {
			SDL_Colour c1 = shim::white;
			SDL_Colour c2 = normal_text_colour;
			int dr = int(c2.r) - int(c1.r);
			int dg = int(c2.g) - int(c1.g);
			int db = int(c2.b) - int(c1.b);
			int da = int(c2.a) - int(c1.a);
			Uint32 t = GET_TICKS() % 1000;
			float p;
			if (t < 500) {
				p = t / 500.0f;
			}
			else {
				p = 1.0f - ((t - 500) / 500.0f);
			}
			p = sin(p * M_PI / 2.0f);
			colour.r = c1.r + dr * p;
			colour.g = c1.g + dg * p;
			colour.b = c1.b + db * p;
			colour.a = c1.a + da * p;
		}
		else{
			colour = normal_text_colour;
		}
	}

	if (enabled) {
		shim::font->enable_shadow(normal_shadow_colour, gfx::Font::DROP_SHADOW);
	}
	else {
		shim::font->enable_shadow(disabled_shadow_colour, gfx::Font::DROP_SHADOW);
	}

	shim::font->draw(colour, text, util::Point<float>(x, y) + (_pressed && _hover ? util::Point<int>(1, 1) : util::Point<int>(0, 0)));

	shim::font->disable_shadow();
	
	if (focussed && gui == shim::guis.back()->gui) {
		gfx::Image *img = GLOBALS->cursor->get_current_image();
		img->draw(util::Point<float>(x-img->size.w-1, y+h/2-img->size.h/2-1));
	}
}

void Widget_Text_Button::set_size()
{
	w = shim::font->get_text_width(text);
	h = shim::font->get_height();
}

void Widget_Text_Button::set_enabled(bool enabled)
{
	this->enabled = enabled;
	if (enabled == true) {
		accepts_focus = true;
	}
	else {
		accepts_focus = false;
	}
}

bool Widget_Text_Button::is_enabled()
{
	return enabled;
}

void Widget_Text_Button::set_text(std::string text)
{
	this->text = text;
	set_size();
}

void Widget_Text_Button::set_normal_shadow_colour(SDL_Colour colour)
{
	normal_shadow_colour = colour;
}

void Widget_Text_Button::set_focussed_shadow_colour(SDL_Colour colour)
{
	focussed_shadow_colour = colour;
}

void Widget_Text_Button::set_disabled_text_colour(SDL_Colour colour)
{
	disabled_text_colour = colour;
}

//--

Widget_Save_Slot::Widget_Save_Slot(int number) :
	Widget_Button(0, 0),
	number(number)
{
	row_img = new gfx::Image("ui/settings_row_wide.tga");

	accepts_focus = true;
	w = row_img->size.w;
	h = row_img->size.h + 1;

	set_text();
}
	
Widget_Save_Slot::~Widget_Save_Slot()
{
	delete row_img;
}

void Widget_Save_Slot::draw()
{
	bool focussed = is_focussed();

	util::Point<int> down_o;
	if (_pressed && _hover) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	int x, y;

	if (use_relative_position) {
		x = relative_x;
		y = relative_y;
	}
	else {
		x = calculated_x;
		y = calculated_y;
	}

	row_img->draw(util::Point<int>(x, y));
	//gfx::draw_9patch(DOGO_GLOBALS->gui_window, util::Point<int>(x, y), util::Size<int>(w, h));

	SDL_Colour col;
	if (focussed) {
		SDL_Colour c1 = shim::white;
		SDL_Colour c2 = shim::palette[34];
		int dr = int(c2.r) - int(c1.r);
		int dg = int(c2.g) - int(c1.g);
		int db = int(c2.b) - int(c1.b);
		int da = int(c2.a) - int(c1.a);
		Uint32 t = GET_TICKS() % 1000;
		float p;
		if (t < 500) {
			p = t / 500.0f;
		}
		else {
			p = 1.0f - ((t - 500) / 500.0f);
		}
		p = sin(p * M_PI / 2.0f);
		col.r = c1.r + dr * p;
		col.g = c1.g + dg * p;
		col.b = c1.b + db * p;
		col.a = c1.a + da * p;
	}
	else{
		col = shim::palette[34];
	}

	shim::font->enable_shadow(shim::palette[18], gfx::Font::DROP_SHADOW);
	shim::font->draw(col, time_text, util::Point<int>(x+WIN_BORDER, y+h/2-shim::font->get_height()/2)+down_o);
	shim::font->draw(col, place_text, util::Point<int>(x+w-shim::font->get_text_width(place_text)-WIN_BORDER-1, y+h/2-shim::font->get_height()/2)+down_o);
	std::string d;
	int d_o = 0; // hack - underscore is used for easy because minus isn't wide enough
	SDL_Colour d_colour;
	if (_exists) {
		if (difficulty == DIFFICULTY_EASY) {
			d = "_";
			d_o = -3;
			d_colour = shim::palette[30];
		}
		else if (difficulty == DIFFICULTY_HARD) {
			d = "+";
			d_o = 0;
			d_colour = shim::palette[13];
		}
		if (d != "") {
			shim::font->draw(d_colour, d, util::Point<int>(x+w+2, y+h/2-shim::font->get_height()/2+d_o));
		}
	}
	shim::font->disable_shadow();

	if (focussed && gui == shim::guis.back()->gui) {
		gfx::Image *img = GLOBALS->cursor->get_current_image();
		img->draw(util::Point<float>(x-img->size.w+1, y+h/2-img->size.h/2-1));
	}
}

bool Widget_Save_Slot::exists()
{
	return _exists;
}

bool Widget_Save_Slot::is_corrupt()
{
	return corrupt;
}

void Widget_Save_Slot::set_text()
{
	int d;
	get_save_info(number, _exists, corrupt, time_text, place_text, d);
	difficulty = (Difficulty)d;
}

//--

SDL_Colour Widget_Label::default_text_colour;
SDL_Colour Widget_Label::default_shadow_colour;

void Widget_Label::static_start()
{
	default_text_colour.r = 0;
	default_text_colour.g = 0;
	default_text_colour.b = 0;
	default_text_colour.a = 255;
	default_shadow_colour.r = 0;
	default_shadow_colour.g = 0;
	default_shadow_colour.b = 0;
	default_shadow_colour.a = 0;
}

void Widget_Label::set_default_text_colour(SDL_Colour colour)
{
	default_text_colour = colour;
}

void Widget_Label::set_default_shadow_colour(SDL_Colour colour)
{
	default_shadow_colour = colour;
}

void Widget_Label::set_text_colour(SDL_Colour colour)
{
	text_colour = colour;
}

void Widget_Label::set_shadow_colour(SDL_Colour colour)
{
	shadow_colour = colour;
}

void Widget_Label::set_shadow_type(gfx::Font::Shadow_Type shadow_type)
{
	this->shadow_type = shadow_type;
}

void Widget_Label::start()
{
	text_colour = default_text_colour;
	shadow_colour = default_shadow_colour;
	shadow_type = gfx::Font::DROP_SHADOW;
}

Widget_Label::Widget_Label(std::string text, int max_w, gfx::Font *font) :
	Widget(0, 0)
{
	this->font = (font == NULL) ? shim::font : font;

	start();

	if (max_w < 0) {
		this->max_w = INT_MAX;
	}
	else {
		this->max_w = max_w;
	}

	set_text(text);
}

Widget_Label::~Widget_Label()
{
}

void Widget_Label::draw()
{
	bool full;
	int num_lines, width;

	if (shadow_type != gfx::Font::NO_SHADOW) {
		font->enable_shadow(shadow_colour, shadow_type);
	}

	font->draw_wrapped(text_colour, text, util::Point<int>(calculated_x, calculated_y), max_w, font->get_height()+1, -1, -1, 0, false, full, num_lines, width);

	if (shadow_type != gfx::Font::NO_SHADOW) {
		font->disable_shadow();
	}
}

void Widget_Label::set_text(std::string text)
{
	this->text = text;
	bool full;
	int num_lines, width;
	int line_height = font->get_height()-1;
	font->draw_wrapped(text_colour, text, util::Point<int>(calculated_x, calculated_y), max_w, line_height, -1, -1, 0, true, full, num_lines, width);
	w = width;
	h = line_height * num_lines;
}

void Widget_Label::set_max_width(int width)
{
	max_w = width;
	set_text(text); // readjust w
}

std::string Widget_Label::get_text()
{
	return text;
}

//--

SDL_Colour Widget_List::default_normal_text_colour1;
SDL_Colour Widget_List::default_normal_text_colour2;
SDL_Colour Widget_List::default_highlight_text_colour1;
SDL_Colour Widget_List::default_highlight_text_colour2;
SDL_Colour Widget_List::default_disabled_text_colour1;
SDL_Colour Widget_List::default_disabled_text_colour2;
SDL_Colour Widget_List::default_text_shadow_colour;
SDL_Colour Widget_List::default_disabled_text_shadow_colour;
SDL_Colour Widget_List::default_odd_row_colour;

void Widget_List::static_start()
{
	default_normal_text_colour1.r = 192;
	default_normal_text_colour1.g = 192;
	default_normal_text_colour1.b = 192;
	default_normal_text_colour1.a = 255;
	default_normal_text_colour2.r = 192;
	default_normal_text_colour2.g = 192;
	default_normal_text_colour2.b = 192;
	default_normal_text_colour2.a = 255;
	default_highlight_text_colour1.r = 115;
	default_highlight_text_colour1.g = 216;
	default_highlight_text_colour1.b = 245;
	default_highlight_text_colour1.a = 255;
	default_highlight_text_colour2.r = 162;
	default_highlight_text_colour2.g = 229;
	default_highlight_text_colour2.b = 248;
	default_highlight_text_colour2.a = 255;
	default_disabled_text_colour1.r = 0;
	default_disabled_text_colour1.g = 0;
	default_disabled_text_colour1.b = 0;
	default_disabled_text_colour1.a = 255;
	default_disabled_text_colour2.r = 0;
	default_disabled_text_colour2.g = 0;
	default_disabled_text_colour2.b = 0;
	default_disabled_text_colour2.a = 255;
	default_text_shadow_colour.r = 0;
	default_text_shadow_colour.g = 0;
	default_text_shadow_colour.b = 0;
	default_text_shadow_colour.a = 0;
	default_disabled_text_shadow_colour.r = 0;
	default_disabled_text_shadow_colour.g = 0;
	default_disabled_text_shadow_colour.b = 0;
	default_disabled_text_shadow_colour.a = 0;
	default_odd_row_colour.r = 0;
	default_odd_row_colour.g = 0;
	default_odd_row_colour.b = 0;
	default_odd_row_colour.a = 0;
}

void Widget_List::set_default_normal_text_colour1(SDL_Colour colour)
{
	default_normal_text_colour1 = colour;
}

void Widget_List::set_default_normal_text_colour2(SDL_Colour colour)
{
	default_normal_text_colour2 = colour;
}

void Widget_List::set_default_highlight_text_colour1(SDL_Colour colour)
{
	default_highlight_text_colour1 = colour;
}

void Widget_List::set_default_highlight_text_colour2(SDL_Colour colour)
{
	default_highlight_text_colour2 = colour;
}

void Widget_List::set_default_disabled_text_colour1(SDL_Colour colour)
{
	default_disabled_text_colour1 = colour;
}

void Widget_List::set_default_disabled_text_colour2(SDL_Colour colour)
{
	default_disabled_text_colour2 = colour;
}

void Widget_List::set_default_text_shadow_colour(SDL_Colour colour)
{
	default_text_shadow_colour = colour;
}

void Widget_List::set_default_disabled_text_shadow_colour(SDL_Colour colour)
{
	default_disabled_text_shadow_colour = colour;
}

void Widget_List::set_default_odd_row_colour(SDL_Colour colour)
{
	default_odd_row_colour = colour;
}

void Widget_List::set_normal_text_colour1(SDL_Colour colour)
{
	normal_text_colour1 = colour;
}

void Widget_List::set_normal_text_colour2(SDL_Colour colour)
{
	normal_text_colour2 = colour;
}

void Widget_List::set_highlight_text_colour1(SDL_Colour colour)
{
	highlight_text_colour1 = colour;
}

void Widget_List::set_highlight_text_colour2(SDL_Colour colour)
{
	highlight_text_colour2 = colour;
}

void Widget_List::set_disabled_text_colour1(SDL_Colour colour)
{
	disabled_text_colour1 = colour;
}

void Widget_List::set_disabled_text_colour2(SDL_Colour colour)
{
	disabled_text_colour2 = colour;
}

void Widget_List::set_text_shadow_colour(SDL_Colour colour)
{
	text_shadow_colour = colour;
}

void Widget_List::set_disabled_text_shadow_colour(SDL_Colour colour)
{
	disabled_text_shadow_colour = colour;
}

void Widget_List::set_odd_row_colour(SDL_Colour colour)
{
	odd_row_colour = colour;
}

void Widget_List::start()
{
	accepts_focus = true;
	top = 0;
	selected = -1;
	row_h = shim::font->get_height();
	pressed_item = -1;
	mouse_down = false;
	scrollbar_down = false;
	arrow_size = util::Size<int>(5, 3);
	has_scrollbar = false;
	always_show_arrows = false;

	normal_text_colour1 = default_normal_text_colour1;
	normal_text_colour2 = default_normal_text_colour2;
	highlight_text_colour1 = default_highlight_text_colour1;
	highlight_text_colour2 = default_highlight_text_colour2;
	disabled_text_colour1 = default_disabled_text_colour1;
	disabled_text_colour2 = default_disabled_text_colour2;
	text_shadow_colour = default_text_shadow_colour;
	disabled_text_shadow_colour = default_disabled_text_shadow_colour;
	odd_row_colour = default_odd_row_colour;

	arrow_colour = shim::white;
	arrow_shadow_colour = shim::palette[22];
	
	down_selected = -1;

	reserved_size = -1;

	selected_time = GET_TICKS();

	wrap = false;

	text_offset = util::Point<float>(0.0f, 0.0f);

	top_o = util::Point<float>(0.0f, 0.0f);
	bottom_o = util::Point<float>(0.0f, 0.0f);
}

Widget_List::Widget_List(int w, int h) :
	Widget(w, h)
{
	start();
}

Widget_List::Widget_List(float w, float h) :
	Widget(w, h)
{
	start();
}

Widget_List::Widget_List(int w, float h) :
	Widget(w, h)
{
	start();
}

Widget_List::Widget_List(float w, int h) :
	Widget(w, h)
{
	start();
}

Widget_List::~Widget_List()
{
}

void Widget_List::handle_event(TGUI_Event *event)
{
	bool focussed = is_focussed();

	int scrollbar_x = 0, scrollbar_x2 = 0;

	if (has_scrollbar && visible_rows() < (int)items.size()) {
		scrollbar_x = calculated_x + calculated_w - arrow_size.w - 2;
		scrollbar_x2 = calculated_x + calculated_w;
	}
	else {
		scrollbar_x = scrollbar_x2 = calculated_x + calculated_w;
	}

	int scrollbar_y = calculated_y+scrollbar_pos()+DOGO_GLOBALS->up_arrow->size.h+1;

	if (focussed && event->type == TGUI_FOCUS) {
		if (event->focus.type == TGUI_FOCUS_UP) {
			up();
			if (items[selected].substr(0, 5) == "-----") {
				if (selected == 0) {
					selected = 1;
				}
				else {
					up();
				}
			}
		}
		else if (event->focus.type == TGUI_FOCUS_DOWN) {
			down();
			if (items[selected].substr(0, 5) == "-----") {
				down();
			}
		}
	}
	else if (focussed && event->type == TGUI_KEY_DOWN && event->keyboard.is_repeat == false) {
		if (event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) {
			if (is_disabled(selected) == false) {
				down_selected = selected;
				down_time = GET_TICKS();
			}
		}
	}
	else if (focussed && event->type == TGUI_JOY_DOWN && event->joystick.is_repeat == false) {
		if (event->joystick.button == GLOBALS->joy_action) {
			if (is_disabled(selected) == false) {
				down_selected = selected;
				down_time = GET_TICKS();
			}
		}
	}
	else if (focussed && event->type == TGUI_KEY_UP && event->keyboard.is_repeat == false) {
		if (event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) {
			if (is_disabled(selected) == false) {
				if (selected == down_selected) {
					pressed_item = selected;
					if (DOGO_GLOBALS->button != 0) {
						DOGO_GLOBALS->button->play(false);
					}
				}
			}
			down_selected = -1;
		}
	}
	else if (focussed && event->type == TGUI_JOY_UP && is_disabled(selected) == false && event->joystick.is_repeat == false) {
		if (event->joystick.button == GLOBALS->joy_action) {
			if (selected == down_selected) {
				pressed_item = selected;
				if (DOGO_GLOBALS->button != 0) {
					DOGO_GLOBALS->button->play(false);
				}
			}
			down_selected = -1;
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN) {
		int mx = (int)event->mouse.x;
		int my = (int)event->mouse.y;
		// Check for clicks on arrows  first
		bool top_arrow = top > 0;
		bool bottom_arrow;
		int vr = visible_rows();
		if ((int)items.size() > vr && top < (int)items.size() - vr) {
			bottom_arrow = true;
		}
		else {
			bottom_arrow = false;
		}
		int height = used_height();
		util::Point<int> tap = top_arrow_pos();
		util::Point<int> bap = bottom_arrow_pos();
		util::Size<int> asize = DOGO_GLOBALS->up_arrow->size;
		// +1px on arrows to make easier to press them
		if (((always_show_arrows && selected > 0) || top > 0) && mx >= tap.x-1 && my >= tap.y-1 && mx <= tap.x+asize.w && my <= tap.y+asize.h) {
			up();
		}
		else if (((always_show_arrows && selected < (int)items.size()-1) || top+visible_rows() < (int)items.size()) && mx >= bap.x-1 && my >= bap.y-1 && mx <= bap.x+asize.w && my <= bap.y+asize.h) {
			down();
		}
		else if (scrollbar_down == false && has_scrollbar && visible_rows() < (int)items.size() && top_arrow && mx >= scrollbar_x && mx < scrollbar_x2 && my >= calculated_y && my <= calculated_y+arrow_size.h) {
			change_top(-1);
		}
		else if (scrollbar_down == false && has_scrollbar && visible_rows() < (int)items.size() && bottom_arrow && mx >= scrollbar_x && mx < scrollbar_x2 && my >= calculated_y+height-arrow_size.h && my <= calculated_y+height) {
			change_top(1);
		}
		else if (event->mouse.is_repeat == false && has_scrollbar && visible_rows() < (int)items.size() && (top_arrow || bottom_arrow) && mx >= scrollbar_x && mx < scrollbar_x2 && my >= scrollbar_y && my < scrollbar_y+scrollbar_height()) {
			scrollbar_down = true;
			mouse_down_point = util::Point<int>(mx, my);
			scrollbar_pos_mouse_down = calculated_y+scrollbar_pos()+arrow_size.h+1;
		}
		else if (scrollbar_down == false && has_scrollbar && visible_rows() < (int)items.size() && (top_arrow || bottom_arrow) && mx >= scrollbar_x && mx < scrollbar_x2 && ((my >= calculated_y+DOGO_GLOBALS->up_arrow->size.h && my < calculated_y+calculated_h-DOGO_GLOBALS->down_arrow->size.h) || (my >= scrollbar_y+scrollbar_height() && my < calculated_y+calculated_h-DOGO_GLOBALS->down_arrow->size.h))) {
			if (my < calculated_y+scrollbar_pos()+arrow_size.h+1) {
				change_top(-visible_rows());
			}
			else if (my >= calculated_y+scrollbar_pos()+scrollbar_height()+arrow_size.h+1) {
				change_top(visible_rows());
			}
		}
		else if (mx-text_offset.x >= calculated_x && mx-text_offset.x < scrollbar_x && my >= calculated_y && my < calculated_y+calculated_h) {
			if (event->mouse.is_repeat == false) {
				int r = get_click_row(my);
				if (r >= top && r < (int)items.size() && r < top+visible_rows()) {
					if (items[r].substr(0, 5) != "-----") {
						selected = r;
						selected_time = GET_TICKS();
						if (is_disabled(selected) == false) {
							mouse_down_row = selected;
							orig_mouse_down_row = selected;
						}
						else {
							show_description();
							mouse_down_row = -1;
							orig_mouse_down_row = -1;
						}
					}
					else {
						mouse_down_row = -1;
						orig_mouse_down_row = -1;
					}
					mouse_down = true;
					clicked = true;
					mouse_down_point = util::Point<int>(mx, my);
				}
			}
		}
	}
	else if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
		if (mouse_down && clicked && event->mouse.x-text_offset.x >= calculated_x && event->mouse.x-text_offset.x < calculated_x+calculated_w && event->mouse.y >= calculated_y && event->mouse.y < calculated_y+calculated_h) {
			int row = get_click_row((int)event->mouse.y);
			if (row >= top && row < (int)items.size() && row < top+visible_rows()) {
				if (row == orig_mouse_down_row && is_disabled(selected) == false) {
					pressed_item = selected;
					if (DOGO_GLOBALS->button != 0) {
						DOGO_GLOBALS->button->play(false);
					}
				}
			}
		}
		mouse_down = false;
		scrollbar_down = false;
	}
	else if (event->type == TGUI_MOUSE_AXIS) {
		int mx = (int)event->mouse.x;
		int my = (int)event->mouse.y;
		if (mouse_down && clicked && event->mouse.x-text_offset.x >= calculated_x && event->mouse.x-text_offset.x < calculated_x+calculated_w && event->mouse.y >= calculated_y && event->mouse.y < calculated_y+calculated_h) {
			int row = get_click_row((int)event->mouse.y);
			if (row >= top && row < (int)items.size() && row < top+visible_rows()) {
				if (row == orig_mouse_down_row && is_disabled(selected) == false) {
					mouse_down_row = row;
				}
				else {
					mouse_down_row = -1;
				}
			}
			else {
				mouse_down_row = -1;
			}
		}
		else {
			mouse_down_row = -1;
		}
		if (mouse_down) {
			util::Point<int> p(mx, my);
			util::Point<int> d = p - mouse_down_point;
			if (abs(d.y) >= row_h) {
				clicked = false;
			}
			if (clicked == false) {
				int rows = -d.y / row_h;
				if (rows != 0) {
					change_top(rows);
					mouse_down_point.y -= rows * row_h;
				}
			}
		}
		else if (scrollbar_down) {
			int diffy = scrollbar_pos_mouse_down - mouse_down_point.y;
			float _my = my + diffy;
			int total_rows = (int)items.size();
			int displayed_rows = visible_rows();
			int extra_rows = total_rows - displayed_rows;
			float half = 0.5f / extra_rows;
			_my -= half;
			int extra_h = used_height() - (arrow_size.h * 2 + 2) - scrollbar_height();
			int off_y = _my - (calculated_y + arrow_size.h + 1);
			float p = off_y / (float)extra_h;
			int old_top = top;
			top = p * extra_rows;
			if (top < 0) {
				top = 0;
			}
			if (top > extra_rows) {
				top = extra_rows;
			}
			if (selected < top) {
				selected = top;
				selected_time = GET_TICKS();
			}
			if (selected > top+displayed_rows-1) {
				selected = top+displayed_rows-1;
				selected_time = GET_TICKS();
			}
			if (old_top != top) {
				shim::widget_mml->play(false);
			}
		}
	}
	else if (event->type == TGUI_MOUSE_WHEEL) {
		util::Point<int> mouse_pos = wedge::get_mouse_position();
		if (mouse_pos.x >= calculated_x && mouse_pos.x < calculated_x+calculated_w && mouse_pos.y >= calculated_y && mouse_pos.y < calculated_y+used_height()) {
			change_top(-event->mouse.y);
		}
	}
}

void Widget_List::draw()
{
	for (int i = top, odd = 0; i < (int)items.size() && i < top+visible_rows(); i++, odd++) {
		int y = calculated_y+row_h/2-shim::font->get_height()/2+((i-top)*row_h);
		SDL_Colour colour1;
		SDL_Colour colour2;
		SDL_Colour shadow_colour;
		if (is_disabled(i)) {
			colour1 = disabled_text_colour1;
			colour2 = disabled_text_colour2;
			shadow_colour = disabled_text_shadow_colour;
		}
		else if (is_highlighted(i)) {
			colour1 = highlight_text_colour1;
			colour2 = highlight_text_colour2;
			shadow_colour = text_shadow_colour;
		}
		else {
			colour1 = normal_text_colour1;
			colour2 = normal_text_colour2;
			shadow_colour = text_shadow_colour;
		}

		bool use_shadow;
		if (shadow_colour.r != 0 || shadow_colour.g != 0 || shadow_colour.b != 0 || shadow_colour.a != 0) {
			use_shadow = true;
		}
		else {
			use_shadow = false;
		}

		if (odd % 2 == 1 && odd_row_colour.a > 0) {
			int yy = calculated_y+row_h/2-shim::font->get_height()/2+((i-top)*row_h);
			gfx::draw_filled_rectangle(odd_row_colour, util::Point<int>{calculated_x, yy}, util::Size<int>{calculated_w, row_h}); 
		}

		if (use_shadow) {
			shim::font->enable_shadow(shadow_colour, gfx::Font::DROP_SHADOW);
		}

		draw_row(i, colour1, colour2, shadow_colour, y, i < (int)tips.size() ? tips[i] : "");

		if (use_shadow) {
			shim::font->disable_shadow();
		}
	
		bool draw_cursor = true;

		if (shim::guis.size() > 0) {
			TGUI_Widget *root = shim::guis.back()->gui->get_main_widget();
			TGUI_Widget *w = this;
			TGUI_Widget *parent;
			while ((parent = w->get_parent()) != NULL) {
				w = parent;
			}
			if (root != w) {
				draw_cursor = false;
			}
			else {
				if (shim::guis.back()->is_transitioning_in() || shim::guis.back()->is_transitioning_out()) {
					draw_cursor = false;
				}
			}
		}
		else {
			draw_cursor = false;
		}
		
		if (draw_cursor && i == selected && gui == shim::guis.back()->gui && is_focussed()) {
			gfx::Image *img = GLOBALS->cursor->get_current_image();
			img->draw(util::Point<int>(calculated_x-img->size.w+1, y)+text_offset);
		}
	}

	if (has_scrollbar && visible_rows() < (int)items.size()) {
		draw_scrollbar();
	}
	
	if (gui == shim::guis.back()->gui && is_focussed()) {
		Uint32 t = GET_TICKS() % 500;
		if (t < 250) {
			if ((always_show_arrows && selected > 0) || top > 0) {
				draw_tinted_shadowed_image(arrow_colour, arrow_shadow_colour, DOGO_GLOBALS->up_arrow, top_arrow_pos(), gfx::Font::DROP_SHADOW);
			}
			if ((always_show_arrows && selected < (int)items.size()-1) || top+visible_rows() < (int)items.size()) {
				draw_tinted_shadowed_image(arrow_colour, arrow_shadow_colour, DOGO_GLOBALS->down_arrow, bottom_arrow_pos(), gfx::Font::DROP_SHADOW);
			}
		}
	}
}

util::Point<int> Widget_List::top_arrow_pos()
{
	return util::Point<int>(
		calculated_x+calculated_w/2-DOGO_GLOBALS->up_arrow->size.w/2,
		calculated_y
	)+top_o;
}

util::Point<int> Widget_List::bottom_arrow_pos()
{
	return util::Point<int>(
		calculated_x+calculated_w/2-DOGO_GLOBALS->down_arrow->size.w/2,
		calculated_y+calculated_h-DOGO_GLOBALS->down_arrow->size.h
	)+bottom_o;
}

void Widget_List::draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip)
{
	SDL_Colour col;
	if (index == selected && gui == shim::guis.back()->gui) {
		col = selected_colour(colour1, colour2);
	}
	else {
		col = colour1;
	}

	util::Point<int> down_o;
	if ((mouse_down && index == selected && selected == mouse_down_row) || (index == selected && selected == down_selected)) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	int xx = calculated_x+WIN_BORDER;
	int have = calculated_w - WIN_BORDER*2;
	int totlen = shim::font->get_text_width(items[index]);
	int scroll = totlen-have + 2;
	if (totlen > have) {
		const int wait = 1000;
		Uint32 ticks = GET_TICKS() - selected_time;
		int m = scroll * 50;
		Uint32 t = ticks % (wait*2 + m);
		float o;
		if (t < (Uint32)wait || index != selected) {
			o = 0.0f;
		}
		else if ((int)t >= wait+m) {
			o = -scroll;
		}
		else {
			o = -((t - wait) / (float)m * scroll);
		}
		gfx::Font::end_batches();
		// These lists can be transformed, scissor doesn't understand transforms though so we have to calculate
		// However scissor already accounts for black bars, so reverse that
		glm::mat4 mv, _p;
		gfx::get_matrices(mv, _p);
		glm::vec3 zero(0.0f, 0.0f, 0.0f);
		zero = glm::project(zero, mv, _p, glm::vec4(0.0f, 0.0f, (float)shim::real_screen_size.w, (float)shim::real_screen_size.h));
		// don't just use shim::screen_size above as it has to account for black bars
		zero.x -= shim::screen_offset.x;
		zero.y -= shim::screen_offset.y;
		zero.x /= shim::scale;
		zero.y /= shim::scale;
		zero.y = shim::screen_size.h - zero.y;
		gfx::set_scissor(int(zero.x+xx), 0, have, shim::screen_size.h-1);
		shim::font->draw(col, items[index], util::Point<float>(xx+o, y)+text_offset+down_o);
		gfx::Font::end_batches();
		gfx::unset_scissor();
	}
	else {
		shim::font->draw(col, items[index], util::Point<int>(xx, y)+text_offset+down_o);
		if (tip != "") {
			int tw = shim::font->get_text_width(items[index]);
			int t = GET_TICKS() % 1000;
			int tip_xo = t < 500 ? 1 : 0;
			gfx::draw_9patch(DOGO_GLOBALS->tip_window, util::Point<int>(xx+tw+10+tip_xo, y-2), util::Size<int>(shim::font->get_text_width(tip) + 10, shim::font->get_height() + 3));
			DOGO_GLOBALS->tip_tri->draw({float(xx+tw+10-DOGO_GLOBALS->tip_tri->size.w+2+tip_xo), float(y+shim::font->get_height()/2-DOGO_GLOBALS->tip_tri->size.h/2-1)});
			shim::font->disable_shadow();
			shim::font->draw(shim::black, tip, util::Point<int>(xx+tw+10+5+tip_xo, y));
			shim::font->enable_shadow(shadow_colour, gfx::Font::DROP_SHADOW);
		}
	}
}

void Widget_List::draw_scrollbar()
{
	if (visible_rows() >= (int)items.size()) {
		return;
	}
	
	int arrow_w = DOGO_GLOBALS->up_arrow->size.w;
	int height = used_height();

	int xx = calculated_x;

	if (arrow_shadow_colour.a != 0) {
		DOGO_GLOBALS->up_arrow->start_batch();
		/*
		DOGO_GLOBALS->up_arrow->draw_tinted(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y)+util::Point<int>(1, 1));
		DOGO_GLOBALS->up_arrow->draw_tinted(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y)+util::Point<int>(0, 1));
		DOGO_GLOBALS->up_arrow->draw_tinted(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y)+util::Point<int>(1, 0));
		*/
		DOGO_GLOBALS->up_arrow->draw(util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y));
		DOGO_GLOBALS->up_arrow->end_batch();

		DOGO_GLOBALS->down_arrow->start_batch();
		/*
		DOGO_GLOBALS->down_arrow->draw_tinted(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+height-DOGO_GLOBALS->down_arrow->size.h)+util::Point<int>(1, 1));
		DOGO_GLOBALS->down_arrow->draw_tinted(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+height-DOGO_GLOBALS->down_arrow->size.h)+util::Point<int>(0, 1));
		DOGO_GLOBALS->down_arrow->draw_tinted(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+height-DOGO_GLOBALS->down_arrow->size.h)+util::Point<int>(1, 0));
		*/
		DOGO_GLOBALS->down_arrow->draw(util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+height-DOGO_GLOBALS->down_arrow->size.h));
		DOGO_GLOBALS->down_arrow->end_batch();

		gfx::draw_primitives_start();
		//gfx::draw_filled_rectangle(arrow_shadow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+scrollbar_pos()+DOGO_GLOBALS->up_arrow->size.h+1), util::Size<int>(DOGO_GLOBALS->up_arrow->size.w+1, scrollbar_height()+1));
	}
	else {
		DOGO_GLOBALS->up_arrow->draw_tinted(arrow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y));
		DOGO_GLOBALS->down_arrow->draw_tinted(arrow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+height-DOGO_GLOBALS->down_arrow->size.h));
		gfx::draw_primitives_start();
	}

	gfx::draw_filled_rectangle(arrow_colour, util::Point<int>(xx+calculated_w-arrow_w-1, calculated_y+scrollbar_pos()+DOGO_GLOBALS->up_arrow->size.h+1), util::Size<int>(DOGO_GLOBALS->up_arrow->size.w, scrollbar_height()));
	gfx::draw_primitives_end();
}

int Widget_List::scrollbar_height()
{
	int total_rows = (int)items.size();
	int displayed_rows = visible_rows();
	int extra_rows = total_rows - displayed_rows;
	float p = 1.0f - ((float)extra_rows / total_rows);
	int arrow_h = DOGO_GLOBALS->up_arrow->size.h + DOGO_GLOBALS->down_arrow->size.h + 2;
	return (used_height() - arrow_h) * p;
}

int Widget_List::scrollbar_pos()
{
	int arrow_h = DOGO_GLOBALS->up_arrow->size.h + DOGO_GLOBALS->down_arrow->size.h + 2;
	int extra_h = used_height() - arrow_h - scrollbar_height();
	int total_rows = (int)items.size();
	int displayed_rows = visible_rows();
	int extra_rows = total_rows - displayed_rows;
	float p = (float)top / extra_rows;
	return p * extra_h;
}

void Widget_List::set_items(std::vector<std::string> new_items)
{
	items = new_items;

	accepts_focus = items.size() != 0;

	if (items.size() == 0) {
		selected = -1;
		if (gui != 0 && gui->get_focus() == this) {
			gui->focus_something();
		}
	}
	else {
		if (selected < 0) {
			selected = 0;
			selected_time = GET_TICKS();
		}
	}
}

int Widget_List::pressed()
{
	int ret = pressed_item;
	pressed_item = -1;
	return ret;
}

int Widget_List::get_selected()
{
	return selected;
}

void Widget_List::set_selected(int selected)
{
	this->selected = selected;
	selected_time = GET_TICKS();
}

int Widget_List::get_size()
{
	return (int)items.size();
}

int Widget_List::get_top()
{
	return top;
}

void Widget_List::set_top(int top)
{
	this->top = top;
}

void Widget_List::set_highlight(int index, bool onoff)
{
	std::vector<int>::iterator it = std::find(highlight.begin(), highlight.end(), index);
	if (it != highlight.end()) {
		highlight.erase(it);
	}
	if (onoff) {
		highlight.push_back(index);
	}
}

bool Widget_List::is_highlighted(int index)
{
	return std::find(highlight.begin(), highlight.end(), index) != highlight.end();
}

void Widget_List::up()
{
	if (selected > 0) {
		if (shim::widget_mml != 0) {
			shim::widget_mml->play(false);
		}
		selected--;
		if (selected < top) {
			top--;
		}
		selected_time = GET_TICKS();
	}
	else if (wrap) {
		if (shim::widget_mml != 0) {
			shim::widget_mml->play(false);
		}
		selected = int(items.size())-1;
		top = int(items.size() - visible_rows());
	}
}

void Widget_List::down()
{
	if (selected < (int)items.size()-1) {
		if (shim::widget_mml != 0) {
			shim::widget_mml->play(false);
		}
		selected++;
		if (top + visible_rows() <= selected) {
			top++;
		}
		selected_time = GET_TICKS();
	}
	else if (wrap) {
		if (shim::widget_mml != 0) {
			shim::widget_mml->play(false);
		}
		selected = 0;
		while (items[selected].substr(0, 5) == "-----") {
			selected++;
		}
		top = 0;
	}
}

int Widget_List::get_click_row(int y)
{
	y -= text_offset.y;
	int row = (y - calculated_y) / row_h + top;
	/*
	if (row < 0) {
		row = 0;
	}
	else if (row >= (int)items.size()) {
		row = (int)items.size()-1;
	}
	*/
	return row;
}

void Widget_List::change_top(int rows)
{
	int vr = visible_rows();
	int old = top;
	top += rows;
	if (top < 0) {
		top = 0;
	}
	else if ((int)items.size() <= vr) {
		top = 0;
	}
	else if (top > (int)items.size() - vr) {
		top = (int)items.size() - vr;
	}
	if (selected < top) {
		selected = top;
		selected_time = GET_TICKS();
	}
	else if (selected >= top + vr) {
		selected = MIN((int)items.size()-1, top + vr - 1);
		selected_time = GET_TICKS();
	}
	if (top != old) {
		if (shim::widget_mml != 0) {
			shim::widget_mml->play(false);
		}
	}

	if (items[selected].substr(0, 5) == "-----") {
		if (rows > 1) {
			for (int i = selected+1; i < top+visible_rows(); i++) {
				selected = i;
				if (items[selected].substr(0, 5) != "-----") {
					break;
				}
			}
			if (items[selected].substr(0, 5) == "-----") {
				for (int i = selected-1; i >= top; i--) {
					selected = i;
					if (items[selected].substr(0, 5) != "-----") {
						break;
					}
				}
			}
		}
		else {
			for (int i = selected-1; i >= top; i--) {
				selected = i;
				if (items[selected].substr(0, 5) != "-----") {
					break;
				}
			}
			if (items[selected].substr(0, 5) == "-----") {
				for (int i = selected+1; i < top+visible_rows(); i++) {
					selected = i;
					if (items[selected].substr(0, 5) != "-----") {
						break;
					}
				}
			}
		}
	}
}

int Widget_List::visible_rows()
{
	return calculated_h / row_h;
}

int Widget_List::used_height()
{
	return calculated_h - 1;
}

void Widget_List::set_disabled(int index, bool onoff)
{
	std::vector<int>::iterator it = std::find(disabled.begin(), disabled.end(), index);
	if (it != disabled.end()) {
		disabled.erase(it);
	}
	if (onoff) {
		disabled.push_back(index);
	}
}

bool Widget_List::is_disabled(int index)
{
	return std::find(disabled.begin(), disabled.end(), index) != disabled.end();
}

void Widget_List::set_arrow_colour(SDL_Colour colour)
{
	arrow_colour = colour;
}

void Widget_List::set_arrow_shadow_colour(SDL_Colour colour)
{
	arrow_shadow_colour = colour;
}

std::vector<std::string> Widget_List::get_items()
{
	return items;
}

std::string Widget_List::get_item(int index)
{
	return items[index];
}

void Widget_List::set_reserved_size(int pixels)
{
	reserved_size = pixels;
	resize();
}

void Widget_List::resize()
{
	if (reserved_size >= 0) {
		calculated_h = (((shim::screen_size.h - reserved_size) - 4) / row_h) * row_h + 4;
	}
	TGUI_Widget::resize();
	if (items.size() == 0) {
		top = selected = 0;
		selected_time = GET_TICKS();
	}
	else if (reserved_size >= 0) {
		int vr = visible_rows();
		if (top + vr <= selected) {
			top = selected - (vr-1);
		}
		if (top + vr > (int)items.size()) {
			top = (int)items.size() - vr;
		}
		if (top < 0) {
			top = 0;
		}
	}
}

void Widget_List::set_wrap(bool wrap)
{
	this->wrap = wrap;
}

void Widget_List::set_row_h(int row_h)
{
	this->row_h = row_h;
}

void Widget_List::set_text_offset(util::Point<float> text_offset)
{
	this->text_offset = text_offset;
}

void Widget_List::set_arrow_offsets(util::Point<float> top_o, util::Point<float> bottom_o)
{
	this->top_o = top_o;
	this->bottom_o = bottom_o;
}

void Widget_List::set_always_show_arrows(bool always)
{
	always_show_arrows = always;
}

void Widget_List::set_tips(std::vector<std::string> &tips)
{
	this->tips = tips;
}

void Widget_List::show_description()
{
}

//--

Widget_Autosaves_List::Widget_Autosaves_List(int w, int h) :
	Widget_List(w, h)
{
}

Widget_Autosaves_List::Widget_Autosaves_List(float w, float h) :
	Widget_List(w, h)
{
}

Widget_Autosaves_List::Widget_Autosaves_List(int w, float h) :
	Widget_List(w, h)
{
}

Widget_Autosaves_List::Widget_Autosaves_List(float w, int h) :
	Widget_List(w, h)
{
}

Widget_Autosaves_List::~Widget_Autosaves_List()
{
}

void Widget_Autosaves_List::draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip)
{
	SDL_Colour col;
	if (index == selected && gui == shim::guis.back()->gui) {
		col = selected_colour(colour1, colour2);
	}
	else {
		col = colour1;
	}

	util::Point<int> down_o;
	if ((mouse_down && index == selected && selected == mouse_down_row) || (index == selected && selected == down_selected)) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	int max = 0;
	for (auto s : items) {
		int w = shim::font->get_text_width(s);
		max = MAX(max, w);
	}

	int arrow_w = DOGO_GLOBALS->up_arrow->size.w;
	shim::font->draw(col, items[index], util::Point<int>(calculated_x+WIN_BORDER+max-shim::font->get_text_width(items[index]), y)+text_offset+down_o);
	int w = shim::font->get_text_width(places[index]);
	int xo;
	if (has_scrollbar && visible_rows() < (int)items.size()) {
		xo = arrow_w+6;
	}
	else {
		xo = 4;
	}
	shim::font->draw(col, places[index], util::Point<int>(calculated_x+calculated_w-w-xo, y)+text_offset+down_o);
}

void Widget_Autosaves_List::set_items_extra(std::vector<std::string> names, std::vector<std::string> places)
{
	items = names;
	this->places = places;
	selected = 0;
	selected_time = GET_TICKS();
}

//--

Widget_Settings_List::Widget_Settings_List(int visible_rows, std::string image_filename) :
	Widget_List(1, 1)
{
	row_img = new gfx::Image(image_filename);
	row_h = row_img->size.h + 1;
	this->w = row_img->size.w;
	this->h = visible_rows * row_h;
	text_shadow_colour = shim::palette[18];
	arrow_shadow_colour = shim::palette[18];
}

Widget_Settings_List::~Widget_Settings_List()
{
	delete row_img;
}

void Widget_Settings_List::draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip)
{
	SDL_Colour col;
	if (index == selected && gui == shim::guis.back()->gui) {
		col = selected_colour(colour1, colour2);
	}
	else {
		col = colour1;
	}

	util::Point<int> down_o;
	if ((mouse_down && index == selected && selected == mouse_down_row) || (index == selected && selected == down_selected)) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	row_img->draw(util::Point<int>(calculated_x, y-(row_h/2-shim::font->get_height()/2)));

	shim::font->enable_shadow(shadow_colour, gfx::Font::DROP_SHADOW);
	shim::font->draw(col, items[index], util::Point<int>(calculated_x+WIN_BORDER, y)+text_offset+down_o);
	shim::font->disable_shadow();
}

int Widget_Settings_List::visible_rows()
{
	return h / row_h;
}

void Widget_Settings_List::set_items(std::vector<std::string> new_items)
{
	Widget_List::set_items(new_items);

	if (has_scrollbar) {
		w += arrow_size.w + 2;
	}
}

//--

Widget_Vertical_Slider::Widget_Vertical_Slider(int height, int stops, int initial_value) :
	Widget(0, height),
	stops(stops),
	value(initial_value),
	mouse_down(false)
{
	w = WIDTH + 2;

	accepts_focus = true;

	interp = new math::I_Sin();
}

Widget_Vertical_Slider::~Widget_Vertical_Slider()
{
	delete interp;
}

void Widget_Vertical_Slider::handle_event(TGUI_Event *event)
{
	bool focussed = is_focussed();

	if (focussed && event->type == TGUI_FOCUS) {
		if (event->focus.type == TGUI_FOCUS_DOWN) {
			if (value > 0) {
				if (shim::widget_mml != 0) {
					shim::widget_mml->play(false);
				}
				value--;
			}
		}
		else if (event->focus.type == TGUI_FOCUS_UP) {
			if (value < stops-1) {
				if (shim::widget_mml != 0) {
					shim::widget_mml->play(false);
				}
				value++;
			}
		}
	}
	else if (focussed && ((event->type == TGUI_MOUSE_DOWN && event->mouse.is_repeat == false) || (mouse_down && event->type == TGUI_MOUSE_AXIS))) {
		int old_value = value;
		if (mouse_down) {
			float p = (event->mouse.y - calculated_y) / calculated_h;
			p = MAX(0.0f, MIN(0.999f, p));
			p *= stops;
			p = (stops-1) - p;
			value = (int)p;
		}
		else {
			TGUI_Event e = tgui_get_relative_event(this, event);
			if (e.mouse.y >= 0) {
				mouse_down = true;
				float p = (event->mouse.y - calculated_y) / calculated_h;
				p = MAX(0.0f, MIN(0.999f, p));
				p *= stops;
				p = (stops-1) - p;
				value = (int)p;
			}
		}
		if (value != old_value) {
			if (shim::widget_mml != 0) {
				shim::widget_mml->play(false);
			}
		}
	}
	else if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
		mouse_down = false;
	}
}

void Widget_Vertical_Slider::draw()
{
	bool focussed = is_focussed();

	SDL_Colour colour1;
	SDL_Colour colour2;
	SDL_Colour dark;

	if (focussed) {
		const int STEP = 2000;
		Uint32 t = GET_TICKS() % STEP;
		float p;
		if (t < STEP/2) {
			p = t / (float)(STEP/2);
		}
		else {
			p = 1.0f - ((t - (STEP/2)) / (float)(STEP/2));
		}

		interp->start(0.0f, 0.0f, 0.5f, 0.5f, 1000);
		interp->interpolate(p * 1000);
		p = interp->get_value();

		colour1 = brighten(shim::palette[28], p);
		colour2 = brighten(shim::palette[27], p);
		dark = shim::palette[38];
	}
	else {
		colour1 = shim::palette[28];
		colour2 = shim::palette[27];
		dark = shim::black;
	}


	gfx::draw_line(dark, util::Point<float>(calculated_x+0.5f-1, calculated_y), util::Point<float>(calculated_x+0.5f-1, calculated_y+calculated_h));
	gfx::draw_filled_rectangle(dark, util::Point<float>(calculated_x, calculated_y-1), util::Size<int>(WIDTH, calculated_h+2));
	gfx::draw_line(dark, util::Point<float>(calculated_x+0.5f+WIDTH, calculated_y), util::Point<float>(calculated_x+0.5f+WIDTH, calculated_y+calculated_h));

	int y = int((float)value / (stops-1) * calculated_h);

	if (y != 0) {
		gfx::draw_filled_rectangle(colour1, util::Point<float>(calculated_x, calculated_y+calculated_h-y), util::Size<float>(WIDTH/2, y));
		gfx::draw_filled_rectangle(colour2, util::Point<float>(calculated_x+WIDTH/2, calculated_y+calculated_h-y), util::Size<float>(WIDTH/2, y));
	}
}

int Widget_Vertical_Slider::get_value()
{
	return value;
}

//--

Widget_Controls_List::Widget_Controls_List(int visible_rows) :
	Widget_Settings_List(visible_rows, "ui/settings_row_wide.tga")
{
	arrow_shadow_colour = shim::palette[18];
}

Widget_Controls_List::~Widget_Controls_List()
{
}

void Widget_Controls_List::draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip)
{
	SDL_Colour col;
	if (index == selected && gui == shim::guis.back()->gui) {
		col = selected_colour(colour1, colour2);
	}
	else {
		col = colour1;
	}

	util::Point<int> down_o;
	if ((mouse_down && index == selected && selected == mouse_down_row) || (index == selected && selected == down_selected)) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	std::string text = items[index];
	bool draw_bg = true;
	if (text.substr(0, 5) == "-----") {
		text = text.substr(5, text.length()-10);
		draw_bg = false;
	}

	text = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(text));

	gfx::Font *font = shim::font;

	gfx::Font::Shadow_Type shadow_type;

	if (draw_bg) {
		row_img->draw(util::Point<int>(calculated_x, y-(row_h/2-font->get_height()/2)));
		shadow_type = gfx::Font::DROP_SHADOW;
	}
	else {
		shadow_colour = shim::palette[19];
		shadow_type = gfx::Font::FULL_SHADOW;
	}

	font->enable_shadow(shadow_colour, shadow_type);
	font->draw(col, text, util::Point<int>(calculated_x+WIN_BORDER, y)+text_offset+down_o);
	font->draw(col, assignments[index], util::Point<int>(calculated_x+calculated_w-WIN_BORDER-font->get_text_width(assignments[index]), y)+text_offset+down_o);
	font->disable_shadow();
}

void Widget_Controls_List::set_items_extra(std::vector<std::string> names, std::vector<std::string> assignments)
{
	Widget_List::set_items(names);
	this->assignments = assignments;
}

//--

Widget_Checkbox::Widget_Checkbox(std::string text, bool checked) :
	Widget(0, 0),
	text(text),
	checked(checked)
{
	w = shim::font->get_text_width(text) + 6 + WIDTH;
	h = MAX(shim::font->get_height(), WIDTH);

	accepts_focus = true;

	mouse_down = false;
	
	interp = new math::I_Sin();
}

Widget_Checkbox::~Widget_Checkbox()
{
	delete interp;
}

void Widget_Checkbox::handle_event(TGUI_Event *event)
{
	bool focussed = is_focussed();

	int xx = calculated_x;
	int yy = calculated_y;

	bool was_checked = checked;

	if ((event->type == TGUI_MOUSE_DOWN || event->type == TGUI_MOUSE_AXIS) && event->mouse.is_repeat == false) {
		if (event->mouse.x >= xx && event->mouse.x < xx+calculated_w && event->mouse.y >= yy && event->mouse.y < yy+calculated_h) {
			mouse_down = true;
		}
		else {
			mouse_down = false;
		}
	}
	if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
		if (event->mouse.x >= xx && event->mouse.x < xx+calculated_w && event->mouse.y >= yy && event->mouse.y < yy+calculated_h) {
			if (mouse_down) {
				mouse_down = false;
				checked = !checked;
			}
		}
	}
	else if (focussed && event->type == TGUI_KEY_UP && event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) {
		checked = !checked;
	}
	else if (focussed && event->type == TGUI_JOY_UP && event->joystick.button == GLOBALS->joy_action) {
		checked = !checked;
	}

	if (was_checked != checked) {
		DOGO_GLOBALS->button->play(false);
	}
}

void Widget_Checkbox::draw()
{
	bool focussed = is_focussed();

	int text_y = calculated_y + h/2 - shim::font->get_height() / 2;

	shim::font->enable_shadow(shim::palette[19], gfx::Font::FULL_SHADOW);
	shim::font->draw(shim::white, text, util::Point<int>(calculated_x+1, text_y), true, false);
	shim::font->disable_shadow();

	float check_x = calculated_x + 5 + shim::font->get_text_width(text);
	float check_y = calculated_y + h/2.0f - WIDTH/2.0f;

	SDL_Colour colour;
	SDL_Colour dark;

	if (focussed) {
		const int STEP = 2000;
		Uint32 t = GET_TICKS() % STEP;
		float p;
		if (t < STEP/2) {
			p = t / (float)(STEP/2);
		}
		else {
			p = 1.0f - ((t - (STEP/2)) / (float)(STEP/2));
		}

		interp->start(0.0f, 0.0f, 0.5f, 0.5f, 1000);
		interp->interpolate(p * 1000);
		p = interp->get_value();

		colour = brighten(shim::palette[28], p);
		dark = shim::palette[38];
	}
	else {
		colour = shim::palette[28];
		dark = shim::black;
	}

	gfx::draw_filled_rectangle(dark, util::Point<float>(check_x, check_y), util::Size<int>(WIDTH, WIDTH));

	if (focussed) {
		gfx::draw_rectangle(colour, util::Point<int>(calculated_x-2, calculated_y-2), util::Size<int>(w+4, h+4));
	}

	if (checked) {
		gfx::draw_filled_rectangle(colour, util::Point<float>(check_x+1, check_y+1), util::Size<float>(WIDTH-2, WIDTH-2));
	}
}

bool Widget_Checkbox::is_checked()
{
	return checked;
}

//--

Widget_Quantity_List::Widget_Quantity_List(int w, int h) :
	Widget_List(w, h)
{
	longpress_started = 0;
	hide_quantities = false;
}

Widget_Quantity_List::Widget_Quantity_List(float w, float h) :
	Widget_List(w, h)
{
	longpress_started = 0;
	hide_quantities = false;
}

Widget_Quantity_List::Widget_Quantity_List(int w, float h) :
	Widget_List(w, h)
{
	longpress_started = 0;
	hide_quantities = false;
}

Widget_Quantity_List::Widget_Quantity_List(float w, int h) :
	Widget_List(w, h)
{
	longpress_started = 0;
	hide_quantities = false;
}

Widget_Quantity_List::~Widget_Quantity_List()
{
}

void Widget_Quantity_List::draw_row(int index, SDL_Colour colour1, SDL_Colour colour2, SDL_Colour shadow_colour, int y, std::string tip)
{
	SDL_Colour col;
	if (index == selected && gui == shim::guis.back()->gui) {
		col = selected_colour(colour1, colour2);
	}
	else {
		col = colour1;
	}
	
	util::Point<int> down_o;
	if ((mouse_down && index == selected && selected == mouse_down_row) || (index == selected && selected == down_selected)) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	int xx = calculated_x+WIN_BORDER;

	int sl = 5; // space size

	if (hide_quantities == false) {
		std::string qs = util::itos(quantities[index]);

		// NOTE: special case of ~0
		if (quantities[index] != (int)0xffffffff) {
			shim::font->draw(col, qs, util::Point<int>(xx+max_q_len-shim::font->get_text_width(qs), y)+text_offset+down_o);
		}

		xx = xx + max_q_len + sl;
	}
	else {
		max_q_len = -sl; // counter - sl below
	}

	int have = calculated_w - (xx-calculated_x) - 5 - (visible_rows() < (int)items.size() ? arrow_size.w : 0) - max_q_len - sl;
	int totlen = shim::font->get_text_width(items[index]);
	int scroll = totlen-have + 2;

	if (totlen > have) {
		const int wait = 1000;
		Uint32 ticks = GET_TICKS() - selected_time;
		int m = scroll * 50;
		Uint32 t = ticks % (wait*2 + m);
		float o;
		if (t < (Uint32)wait || index != selected) {
			o = 0.0f;
		}
		else if ((int)t >= wait+m) {
			o = -scroll;
		}
		else {
			o = -((t - wait) / (float)m * scroll);
		}
		gfx::Font::end_batches();
		// These lists can be transformed, scissor doesn't understand transforms though so we have to calculate
		// However scissor already accounts for black bars, so reverse that
		glm::mat4 mv, _p;
		gfx::get_matrices(mv, _p);
		glm::vec3 zero(0.0f, 0.0f, 0.0f);
		zero = glm::project(zero, mv, _p, glm::vec4(0.0f, 0.0f, (float)shim::real_screen_size.w, (float)shim::real_screen_size.h));
		// don't just use shim::screen_size above as it has to account for black bars
		zero.x -= shim::screen_offset.x;
		zero.y -= shim::screen_offset.y;
		zero.x /= shim::scale;
		zero.y /= shim::scale;
		zero.y = shim::screen_size.h - zero.y;
		gfx::set_scissor(int(zero.x+xx), 0, have, shim::screen_size.h-1);
		shim::font->draw(col, items[index], util::Point<float>(xx+o, y)+text_offset+down_o);
		gfx::Font::end_batches();
		gfx::unset_scissor();
	}
	else {
		shim::font->draw(col, items[index], util::Point<int>(xx, y)+text_offset+down_o);
		if (tip != "") {
			int tw = shim::font->get_text_width(items[index]);
			int t = GET_TICKS() % 1000;
			int tip_xo = t < 500 ? 1 : 0;
			gfx::draw_9patch(DOGO_GLOBALS->tip_window, util::Point<int>(xx+tw+10+tip_xo, y-2), util::Size<int>(shim::font->get_text_width(tip) + 10, shim::font->get_height() + 3));
			DOGO_GLOBALS->tip_tri->draw({float(xx+tw+10-DOGO_GLOBALS->tip_tri->size.w+2+tip_xo), float(y+shim::font->get_height()/2-DOGO_GLOBALS->tip_tri->size.h/2-1)});
			shim::font->disable_shadow();
			shim::font->draw(shim::black, tip, util::Point<int>(xx+tw+10+5+tip_xo, y));
			shim::font->enable_shadow(shadow_colour, gfx::Font::DROP_SHADOW);
		}
	}
}

void Widget_Quantity_List::set_items_extra(std::vector<int> quantities, std::vector<std::string> new_items, std::vector<std::string> descriptions)
{
	Widget_List::set_items(new_items);
	this->quantities = quantities;
	this->descriptions = descriptions;

	max_q_len = 0;

	for (size_t i = 0; i < quantities.size(); i++) {
		int len = shim::font->get_text_width(util::itos(quantities[i]));
		if (len > max_q_len) {
			max_q_len = len;
		}
	}
}

void Widget_Quantity_List::handle_event(TGUI_Event *event)
{
	int scrollbar_x = 0, scrollbar_x2 = 0;

	if (has_scrollbar && visible_rows() < (int)items.size()) {
		scrollbar_x = calculated_x + calculated_w - arrow_size.w - 2;
		scrollbar_x2 = calculated_x + calculated_w;
	}
	else {
		scrollbar_x = scrollbar_x2 = calculated_x + calculated_w;
	}

	/*if (event->type == TGUI_KEY_UP && event->keyboard.code == GLOBALS->key_b3) {
		show_description();
		return;
	}
	else if (event->type == TGUI_JOY_UP && event->joystick.button == GLOBALS->joy_b3) {
		show_description();
		return;
	}
	else*/ if (event->type == TGUI_MOUSE_DOWN && event->mouse.is_repeat == false && gui->get_event_owner(event) == this && event->mouse.x < scrollbar_x) {
		longpress_started = GET_TICKS();
		longpress_pos = util::Point<int>(event->mouse.x, event->mouse.y);
	}
	else if (event->type == TGUI_MOUSE_AXIS && longpress_started != 0) {
		util::Point<int> pos(event->mouse.x, event->mouse.y);
		if ((pos-longpress_pos).length() > 5) {
			longpress_started = 0;
		}
	}
	else if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
		/*
		// Widget_List::handle_event (previous line) sets selection, so all we have to do (for disabled (special) items) is show the description
		if (longpress_started != 0 && gui->get_event_owner(event) == this && event->mouse.x < scrollbar_x && is_disabled(selected) == true && selected >= 0) {
			show_description();
		}
		*/
		longpress_started = 0;
	}
	else if (event->type == TGUI_TICK) {
		bool is_longpress = longpress_started != 0;
		bool go = is_longpress || down_selected >= 0;
		if (go) {
			Uint32 u = is_longpress ? longpress_started : down_time;
			Uint32 now = GET_TICKS();
			Uint32 elapsed = now - u;
			if (descriptions.size() > 0 && elapsed >= (Uint32)LONGPRESS_TIME) {
				longpress_started = 0;
				down_selected = -1;
				show_description();
				// fudge some events so the item isn't used/list isn't scrolled:
				TGUI_Event ev;
				ev.type = TGUI_MOUSE_AXIS;
				ev.mouse.x = 1000000;
				ev.mouse.y = longpress_pos.y;
				ev.mouse.is_touch = false;
				ev.mouse.is_repeat = false;
				ev.mouse.normalised = false;
				Widget_List::handle_event(&ev);
				ev.type = TGUI_MOUSE_UP;
				ev.mouse.button = 1;
				Widget_List::handle_event(&ev);
				return;
			}
		}
	}

	if (event->type == TGUI_KEY_DOWN && event->keyboard.is_repeat == false && event->keyboard.code == GLOBALS->key_action && is_disabled(selected) == true) {
		show_description();
	}
	else if (event->type == TGUI_JOY_DOWN && event->joystick.is_repeat == false && event->joystick.button == GLOBALS->joy_action && is_disabled(selected) == true) {
		show_description();
	}
	else {
		Widget_List::handle_event(event);
	}
}

void Widget_Quantity_List::show_description()
{
	longpress_started = 0;

	if ((int)descriptions.size() <= selected) {
		return;
	}
	
	DOGO_GLOBALS->button->play(false);

	Notification_GUI *notification_gui = new Notification_GUI(descriptions[selected]);
	shim::guis.push_back(notification_gui);
}

void Widget_Quantity_List::set_hide_quantities(bool hide)
{
	hide_quantities = hide;
}

//--

Widget_Combo_List::Widget_Combo_List(int w, int h) :
	Widget_Quantity_List(w, h)
{
}

Widget_Combo_List::Widget_Combo_List(float w, float h) :
	Widget_Quantity_List(w, h)
{
}

Widget_Combo_List::Widget_Combo_List(int w, float h) :
	Widget_Quantity_List(w, h)
{
}

Widget_Combo_List::Widget_Combo_List(float w, int h) :
	Widget_Quantity_List(w, h)
{
}

Widget_Combo_List::~Widget_Combo_List()
{
}

void Widget_Combo_List::draw()
{
	// FIXME: this has nothing to do with drawing, but since Widgets have no update(), this is here

	auto b = static_cast<Battle_Game *>(BATTLE);
	auto players = b->get_players();
	int active_player = b->get_active_player();
	int sp = players[active_player]->get_stats()->mp;

	for (size_t i = 0; i < quantities.size(); i++) {
		if (quantities[i] > sp) {
			set_disabled(int(i), true);
		}
		else {
			set_disabled(int(i), false);
		}
	}
	
	Widget_Quantity_List::draw();
}

//--

Widget_Map::Widget_Map() :
	image(NULL),
	image2(NULL)
{
	w = 1;
	h = 1;
}

Widget_Map::~Widget_Map()
{
}

void Widget_Map::draw()
{
	int dx = calculated_x - image->size.w/2;
	int dy = calculated_y;

	if (image != NULL && image2 != NULL) {
		gfx::draw_filled_rectangle(shim::black, util::Point<int>(dx, dy), util::Size<int>(shim::screen_size.w/2, shim::screen_size.h/2));
		int t = GET_TICKS() % 1000;
		float p = t / 1000.0f;
		if (p < 0.5f) {
			p = p / 0.5f;
		}
		else {
			p = 1.0f - ((p - 0.5f) / 0.5f);
		}
		SDL_Colour tint = shim::white;
		tint.r *= p;
		tint.g *= p;
		tint.b *= p;
		tint.a *= p;
		image2->draw_tinted(tint, util::Point<int>(dx, dy));
		image->draw(util::Point<int>(dx, dy));
	}
}

void Widget_Map::set_images(gfx::Image *image, gfx::Image *image2)
{
	this->image = image;
	this->image2 = image2;
}

//--

SDL_Colour Widget_Image::default_shadow_colour;

void Widget_Image::static_start()
{
	default_shadow_colour.r = 0;
	default_shadow_colour.g = 0;
	default_shadow_colour.b = 0;
	default_shadow_colour.a = 0;
}

void Widget_Image::set_default_shadow_colour(SDL_Colour default_colour)
{
	default_shadow_colour = default_colour;
}

Widget_Image::Widget_Image(gfx::Image *image, bool destroy) :
	Widget(image->size.w, image->size.h),
	image(image),
	destroy(destroy)
{
	shadow_colour = default_shadow_colour;
	shadow_type = gfx::Font::DROP_SHADOW;
}

Widget_Image::~Widget_Image()
{
	if (destroy) {
		delete image;
	}
}

void Widget_Image::draw()
{
	if (shadow_colour.a != 0) {
		draw_tinted_shadowed_image(shim::white, shadow_colour, image, util::Point<int>(calculated_x, calculated_y), shadow_type);
	}
	else {
		image->draw(util::Point<int>(calculated_x, calculated_y));
	}
}

void Widget_Image::set_shadow_colour(SDL_Colour colour)
{
	shadow_colour = colour;
}

void Widget_Image::set_shadow_type(gfx::Font::Shadow_Type type)
{
	shadow_type = type;
}

//--

SDL_Colour Widget_Image_Button::default_shadow_colour;

void Widget_Image_Button::static_start()
{
	default_shadow_colour.r = 0;
	default_shadow_colour.g = 0;
	default_shadow_colour.b = 0;
	default_shadow_colour.a = 0;
}

void Widget_Image_Button::set_default_shadow_colour(SDL_Colour default_colour)
{
	default_shadow_colour = default_colour;
}

Widget_Image_Button::Widget_Image_Button(gfx::Image *image, bool destroy) :
	Widget_Button(image->size.w+4, image->size.h+4),
	image(image),
	destroy(destroy)
{
	shadow_colour = default_shadow_colour;
	shadow_type = gfx::Font::DROP_SHADOW;
	interp = new math::I_Sin();
}

Widget_Image_Button::~Widget_Image_Button()
{
	if (destroy) {
		delete image;
	}
	delete interp;
}

void Widget_Image_Button::draw()
{
	util::Point<int> down_o;
	if (_pressed && _hover) {
		down_o = util::Point<int>(1, 1);
	}
	else {
		down_o = util::Point<int>(0, 0);
	}

	if (shadow_colour.a != 0) {
		draw_tinted_shadowed_image(shim::white, shadow_colour, image, util::Point<int>(calculated_x+1, calculated_y+1)+down_o, shadow_type);
	}
	else {
		image->draw(util::Point<int>(calculated_x+1, calculated_y+1)+down_o);
	}
	
	if (is_focussed()) {
		const int STEP = 2000;
		Uint32 t = GET_TICKS() % STEP;
		float p;
		if (t < STEP/2) {
			p = t / (float)(STEP/2);
		}
		else {
			p = 1.0f - ((t - (STEP/2)) / (float)(STEP/2));
		}

		interp->start(0.0f, 0.0f, 0.5f, 0.5f, 1000);
		interp->interpolate(p * 1000);
		p = interp->get_value();

		SDL_Colour colour = brighten(shim::palette[28], p);

		gfx::draw_rectangle(colour, util::Point<int>(calculated_x, calculated_y), image->size+util::Size<int>(2, 2));
	}
}

void Widget_Image_Button::set_shadow_colour(SDL_Colour colour)
{
	shadow_colour = colour;
}

void Widget_Image_Button::set_shadow_type(gfx::Font::Shadow_Type type)
{
	shadow_type = type;
}

//--

Widget_Slider::Widget_Slider(int width, int stops, int initial_value) :
	Widget(width, 0),
	stops(stops),
	value(initial_value),
	mouse_down(false)
{
	h = WIDTH;

	accepts_focus = true;
	
	interp = new math::I_Sin();

	action_button = nullptr;
}

Widget_Slider::~Widget_Slider()
{
	delete interp;
}

void Widget_Slider::handle_event(TGUI_Event *event)
{
	util::Size<int> tab_size(h, h);

	bool focussed = is_focussed();

	if (focussed && event->type == TGUI_FOCUS) {
		if (event->focus.type == TGUI_FOCUS_LEFT) {
			if (value > 0) {
				if (shim::widget_mml != 0) {
					shim::widget_mml->play(false);
				}
				value--;
			}
		}
		else if (event->focus.type == TGUI_FOCUS_RIGHT) {
			if (value < stops-1) {
				if (shim::widget_mml != 0) {
					shim::widget_mml->play(false);
				}
				value++;
			}
		}
	}
	else if (focussed && ((event->type == TGUI_MOUSE_DOWN && event->mouse.is_repeat == false) || (mouse_down && event->type == TGUI_MOUSE_AXIS))) {
		int old_value = value;
		if (mouse_down) {
			float p = (event->mouse.x - calculated_x) / calculated_w;
			p = MAX(0.0f, MIN(0.999f, p));
			p *= stops;
			value = (int)p;
		}
		else {
			TGUI_Event e = tgui_get_relative_event(this, event);
			if (e.mouse.x >= 0) {
				mouse_down = true;
				float p = (event->mouse.x - calculated_x) / calculated_w;
				p = MAX(0.0f, MIN(0.999f, p));
				p *= stops;
				value = (int)p;
			}
		}
		if (value != old_value) {
			if (shim::widget_mml != 0) {
				shim::widget_mml->play(false);
			}
		}
	}
	else if (event->type == TGUI_MOUSE_UP && event->mouse.is_repeat == false) {
		mouse_down = false;
	}
	else if (((event->type == TGUI_KEY_DOWN && event->keyboard.is_repeat == false && event->keyboard.code == /*GLOBALS->key_b1*/GLOBALS->key_action) || (event->type == TGUI_JOY_DOWN && event->joystick.is_repeat == false && event->joystick.button == GLOBALS->joy_action)) && action_button != nullptr && focussed) {
		DOGO_GLOBALS->button->play(false);
		action_button->set_pressed(true);
	}
}

void Widget_Slider::draw()
{
	bool focussed = is_focussed();

	SDL_Colour colour1;
	SDL_Colour colour2;
	SDL_Colour dark;

	if (focussed) {
		const int STEP = 2000;
		Uint32 t = GET_TICKS() % STEP;
		float p;
		if (t < STEP/2) {
			p = t / (float)(STEP/2);
		}
		else {
			p = 1.0f - ((t - (STEP/2)) / (float)(STEP/2));
		}

		interp->start(0.0f, 0.0f, 0.5f, 0.5f, 1000);
		interp->interpolate(p * 1000);
		p = interp->get_value();

		colour1 = brighten(shim::palette[28], p);
		colour2 = brighten(shim::palette[27], p);
		dark = shim::palette[38];
	}
	else {
		colour1 = shim::palette[28];
		colour2 = shim::palette[27];
		dark = shim::black;
	}


	gfx::draw_line(dark, util::Point<float>(calculated_x-0.5f, calculated_y), util::Point<float>(calculated_x-0.5f, calculated_y+WIDTH));
	gfx::draw_line(dark, util::Point<float>(calculated_x+0.5f+calculated_w, calculated_y), util::Point<float>(calculated_x+0.5f+calculated_w, calculated_y+WIDTH));
	gfx::draw_filled_rectangle(dark, util::Point<int>(calculated_x, calculated_y-1), util::Size<int>(calculated_w, WIDTH+2));

	int x = int((float)value / (stops-1) * calculated_w);

	if (x != 0) {
		gfx::draw_filled_rectangle(colour1, util::Point<float>(calculated_x, calculated_y), util::Size<float>(x, WIDTH/2));
		gfx::draw_filled_rectangle(colour2, util::Point<float>(calculated_x, calculated_y+WIDTH/2), util::Size<float>(x, WIDTH/2));
	}
	
}

int Widget_Slider::get_value()
{
	return value;
}

void Widget_Slider::set_action_button(Widget_Button *button)
{
	action_button = button;
}
