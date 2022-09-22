#include <wedge2/area_game.h>
#include <wedge2/globals.h>
#include <wedge2/special_number.h>
#include <wedge2/systems.h>

#include "shop.h"
					
static void buy_callback(void *data)
{
	wedge::Yes_No_GUI_Callback_Data *d = (wedge::Yes_No_GUI_Callback_Data *)data;
	Shop_GUI::Buy_Callback_Data *user = static_cast<Shop_GUI::Buy_Callback_Data *>(d->userdata);
	if (d->cancelled == false && d->choice) {
		wedge::Object o = user->object;
		o.quantity = user->quantity;
		if (INSTANCE->inventory.add(o) == o.quantity) {
			DOGO_GLOBALS->buysell->play(false);
			INSTANCE->add_gold(-user->quantity * user->object.quantity); // gold is stored in quantity for shopkeeps items
		}
		else {
			Notification_GUI *notification_gui = new Notification_GUI(GLOBALS->game_t->translate(4)/* Originally: Inventory full! */);
			shim::guis.push_back(notification_gui);
		}
		user->gui->set_text();
		user->gui->layout();
	}

	delete user;
}

static void sell_callback(void *data)
{
	wedge::Yes_No_GUI_Callback_Data *d = (wedge::Yes_No_GUI_Callback_Data *)data;
	Shop_GUI::Sell_Callback_Data *user = static_cast<Shop_GUI::Sell_Callback_Data *>(d->userdata);
	if (d->cancelled == false && d->choice) {
		DOGO_GLOBALS->buysell->play(false);
		INSTANCE->add_gold(INSTANCE->inventory.get_all()[user->index].sell_price * user->quantity);
		INSTANCE->inventory.remove(user->index, user->quantity);
		user->gui->set_text();
		user->gui->layout();
	}

	delete user;
}

static void buy_quantity_callback(void *data)
{

	Get_Number_GUI::Callback_Data *d = (Get_Number_GUI::Callback_Data *)data;
	Shop_GUI::Buy_Callback_Data *user = static_cast<Shop_GUI::Buy_Callback_Data *>(d->userdata);

	if (d->number > 0) {
		user->quantity = d->number;
		Yes_No_GUI *yes_no_gui = new Yes_No_GUI(util::string_printf(GLOBALS->game_t->translate(282)/* Originally: Buy %d for %d gold? */.c_str(), user->quantity, user->quantity * user->object.quantity/*price is in quantity*/), true, buy_callback, user);
		shim::guis.push_back(yes_no_gui);
	}
	else {
		delete user;
	}
}

static void sell_quantity_callback(void *data)
{

	Get_Number_GUI::Callback_Data *d = (Get_Number_GUI::Callback_Data *)data;
	Shop_GUI::Sell_Callback_Data *user = static_cast<Shop_GUI::Sell_Callback_Data *>(d->userdata);

	if (d->number > 0) {
		user->quantity = d->number;
		int total = user->quantity * INSTANCE->inventory.get_all()[user->index].sell_price;
		if (INSTANCE->get_gold() + total > GLOBALS->max_gold) {
			delete user;
			std::string s = util::string_printf(GLOBALS->game_t->translate(490)/* Originally: You cannot hold more than %d gold! */.c_str(), GLOBALS->max_gold);
			Notification_GUI *notification_gui = new Notification_GUI(s);
			shim::guis.push_back(notification_gui);
		}
		else {
			Yes_No_GUI *yes_no_gui = new Yes_No_GUI(util::string_printf(GLOBALS->game_t->translate(283)/* Originally: Sell %d for %d gold? */.c_str(), user->quantity, user->quantity * INSTANCE->inventory.get_all()[user->index].sell_price), true, sell_callback, user);
			shim::guis.push_back(yes_no_gui);
		}
	}
	else {
		delete user;
	}
}

Shop_GUI::Shop_GUI(std::vector<wedge::Object> items) :
	items(items)
{
	transition_is_slide = false;
	transition_is_slide_vertical = true;

	buying = items.size() > 0;

	modal_main_widget = new Widget(1.0f, 1.0f);
	modal_main_widget->set_background_colour(shim::palette[22]);

	int window_w = int(shim::screen_size.w * 0.9f);
	int window_h = int(shim::screen_size.h * 0.9f - 30);

	window = new Widget_Window(window_w, window_h);
	window->set_centre_x(true);
	window->set_centre_y(true);
	window->set_image(DOGO_GLOBALS->shop_window);
	window->set_parent(modal_main_widget);

	Widget *header = new Widget(TGUI_Widget::FIT_Y, 1.0f);
	header->set_parent(window);

	list = new Widget_Quantity_List(1.0f, shim::font->get_height() * 6);
	list->set_break_line(true);
	list->set_padding_left(15);
	list->set_padding_right(15);
	list->set_padding_top(9);
	list->set_text_shadow_colour(shim::palette[23]);
	list->set_odd_row_colour(shim::palette[27]);
	list->set_always_show_arrows(true);
	list->set_parent(window);

	set_text();

	gui = new TGUI(modal_main_widget, shim::screen_size.w, shim::screen_size.h);

	tok_img = new gfx::Image("misc/tok.tga");
}

Shop_GUI::~Shop_GUI()
{
	delete tok_img;
}

void Shop_GUI::draw()
{
	GUI::draw();

	int win_y = window->get_y();
	int pic_y = win_y - 15;
	
	int text_y = pic_y+10-DOGO_GLOBALS->bold_font->get_height()/2;

	std::string text;
	if (buying) {
		text = GLOBALS->game_t->translate(285)/* Originally: BUY */;
	}
	else {
		text = GLOBALS->game_t->translate(286)/* Originally: SELL */;
	}
	int w = DOGO_GLOBALS->bold_font->get_text_width(text);
	int x = shim::screen_size.w / 2 - w / 2;

	gfx::draw_primitives_start();
	gfx::draw_filled_rectangle(shim::palette[24], util::Point<int>(x - 6, text_y), util::Size<int>(w + 12, DOGO_GLOBALS->bold_font->get_height()));
	gfx::draw_filled_rectangle(shim::palette[26], util::Point<int>(x - 5, text_y), util::Size<int>(w + 10, DOGO_GLOBALS->bold_font->get_height()));
	gfx::draw_filled_rectangle(shim::palette[28], util::Point<int>(x - 4, text_y), util::Size<int>(w + 8, DOGO_GLOBALS->bold_font->get_height()));
	gfx::draw_filled_rectangle(shim::palette[30], util::Point<int>(x - 3, text_y), util::Size<int>(w + 6, DOGO_GLOBALS->bold_font->get_height()));
	gfx::draw_filled_rectangle(shim::palette[33], util::Point<int>(x - 2, text_y), util::Size<int>(w + 4, DOGO_GLOBALS->bold_font->get_height()));
	gfx::draw_primitives_end();

	DOGO_GLOBALS->bold_font->draw(shim::black, text, util::Point<int>(shim::screen_size.w / 2 - w/2, text_y));

	tok_img->draw({window->get_x()+9.0f, (float)pic_y});

	std::string gold = util::itos(INSTANCE->get_gold()) + " " + util::uppercase(GLOBALS->game_t->translate(284)/* Originally: Gold */);

	text_y = pic_y+10-shim::font->get_height()/2 + 5;

	//shim::font->enable_shadow(shim::palette[24], gfx::Font::FULL_SHADOW);
	int gold_x = shim::screen_size.w*0.95f-9-shim::font->get_text_width(gold);
	//gfx::draw_filled_rectangle(shim::palette[24], {gold_x-1.0f, (float)text_y+2.0f}, {(float)shim::font->get_text_width(gold)+2.0f, (float)shim::font->get_height()-4.0f});
	shim::font->draw(shim::palette[8], gold, {(float)gold_x, (float)text_y});
	//shim::font->disable_shadow();

	text_y = win_y + window->get_height() - 2;

	if (list != nullptr) {
		std::string text;
		int selected = list->get_selected();
		if (buying) {
			int q = 0;
			wedge::Object o = items[selected];
			wedge::Object *inv = INSTANCE->inventory.get_all();
			for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
				if (inv[i].type == o.type && inv[i].id == o.id) {
					q = inv[i].quantity;
					break;
				}
			}
			text = GLOBALS->game_t->translate(292)/* Originally: In inventory */ + ": " + util::itos(q);
		}
		else {
			int price = INSTANCE->inventory.get_all()[inventory_indices[selected]].sell_price;
			text = GLOBALS->game_t->translate(302)/* Originally: Selling price */ + ": " + util::itos(price);
		}
		shim::font->draw(shim::palette[35], text, {shim::screen_size.w/2.0f-shim::font->get_text_width(text)/2, (float)text_y});
	}
}

void Shop_GUI::handle_event(TGUI_Event *event)
{
	if (transitioning_in || transitioning_out) {
		return;
	}

	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == GLOBALS->key_back) || (event->type == TGUI_JOY_DOWN && event->joystick.button == GLOBALS->joy_back)) {
		DOGO_GLOBALS->button->play(false);
		AREA->end_shop();
		exit();
	}
	else {
		Dogo_GUI::handle_event(event);
	}
}

void Shop_GUI::update()
{
	float p;
	Uint32 now = GET_TICKS();

	if (transitioning_out) {
		Uint32 elapsed = now - transition_start_time;
		p = 1.0f - (MIN(transition_duration, int(elapsed)) / (float)transition_duration);
	}
	else if (transitioning_in) {
		Uint32 elapsed = now - transition_start_time;
		p = MIN(transition_duration, int(elapsed)) / (float)transition_duration;
	}
	else {
		p = 1.0f;
	}

	SDL_Colour bg_col = shim::palette[22];
	bg_col.r *= p;
	bg_col.g *= p;
	bg_col.b *= p;
	bg_col.a *= p;
	modal_main_widget->set_background_colour(bg_col);

	if (list != NULL) {
		int pressed;
		if ((pressed = list->pressed()) >= 0) {
			confirm();
		}
	}

	Dogo_GUI::update();
}

void Shop_GUI::set_text()
{
	inventory_indices.clear();

	std::vector<int> quantities;
	std::vector<std::string> names;
	std::vector<std::string> descriptions;

	std::vector<wedge::Object> objects;

	if (buying) {
		objects = items;
	}
	else {
		wedge::Object *inv = INSTANCE->inventory.get_all();
		for (int i = 0; i < wedge::Inventory::MAX_OBJECTS; i++) {
			if (inv[i].type == wedge::OBJECT_ITEM) {
				objects.push_back(inv[i]);
				inventory_indices.push_back(i);
			}
		}
	}

	std::vector<int> highlight;

	for (size_t i = 0; i < objects.size(); i++) {
		quantities.push_back(objects[i].quantity);
		names.push_back(GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id(objects[i].name)));
		descriptions.push_back(objects[i].description);
	}

	if (names.size() == 0) {
		list->set_parent(NULL);
		delete list;
		list = NULL;
		if (gui != NULL) {
			gui->focus_something();
		}
		Widget_Label *empty_label;
		empty_label = new Widget_Label(GLOBALS->game_t->translate(153)/* Originally: No items */, -1);
		empty_label->set_padding_top(10);
		empty_label->set_centre_x(true);
		empty_label->set_break_line(true);
		empty_label->set_shadow_colour(shim::palette[23]);
		empty_label->set_parent(window);
		if (gui) {
			gui->layout();
		}
	}
	else {
		list->set_items_extra(quantities, names, descriptions);
		int selected = list->get_selected();
		if (selected >= (int)names.size()) {
			list->set_selected(selected-1);
		}
		for (size_t i = 0; i < names.size(); i++) {
			list->set_highlight((int)i, false);
			list->set_disabled((int)i, false);
		}
		if (buying) {
			for (size_t i = 0; i < items.size(); i++) {
				if (items[i].quantity > INSTANCE->get_gold()) { // NOTE: quantity = cost here
					list->set_disabled((int)i, true);
				}
				wedge::Object o = items[i];
				o.quantity = wedge::Inventory::MAX_STACK+1;
				int max = INSTANCE->inventory.add(o);
				if (max <= 0) {
					list->set_disabled((int)i, true); // already have 99
				}
			}
		}
	}
}

void Shop_GUI::confirm()
{
	int selected = list->get_selected();
	if (buying) {
		Buy_Callback_Data *dat = new Buy_Callback_Data;
		dat->object = items[selected];
		dat->gui = this;

		int stops = INSTANCE->get_gold() / dat->object.quantity + 1;

		wedge::Object o = dat->object;
		o.quantity = wedge::Inventory::MAX_STACK+1;
		int max = INSTANCE->inventory.add(o);

		if (max <= 0) {
			Notification_GUI *notification_gui = new Notification_GUI(GLOBALS->game_t->translate(4)/* Originally: Inventory full! */);
			shim::guis.push_back(notification_gui);
		}
		else {
			max = MIN(stops, max+1);
			Get_Number_GUI *quantity_gui = new Get_Number_GUI(GLOBALS->game_t->translate(287)/* Originally: Buy how many? */, max, 0, buy_quantity_callback, dat);
			shim::guis.push_back(quantity_gui);
		}
	}
	else {
		Sell_Callback_Data *dat = new Sell_Callback_Data;
		dat->index = inventory_indices[selected];
		dat->gui = this;

		Get_Number_GUI *quantity_gui = new Get_Number_GUI(GLOBALS->game_t->translate(288)/* Originally: Sell how many? */, INSTANCE->inventory.get_all()[dat->index].quantity+1, 0, sell_quantity_callback, dat);
		shim::guis.push_back(quantity_gui);
	}
}

void Shop_GUI::layout()
{
	gui->layout();
}
