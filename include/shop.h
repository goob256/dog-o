#ifndef SHOP_H
#define SHOP_H

#include <wedge2/main.h>
#include <wedge2/inventory.h>

#include "gui.h"
#include "widgets.h"

class Shop_GUI : public Dogo_GUI {
public:
	struct Buy_Callback_Data {
		wedge::Object object; // price is in quantity
		int quantity;
		Shop_GUI *gui;
	};

	struct Sell_Callback_Data {
		int index;
		int quantity;
		Shop_GUI *gui;
	};

	Shop_GUI(std::vector<wedge::Object> items/*price goes in quantity*/); /* if items.size() == 0, selling, else buying */
	virtual ~Shop_GUI();

	void draw();
	void handle_event(TGUI_Event *event);
	void update();

	void set_text();
	void layout();

private:
	void confirm();

	std::vector<int> inventory_indices;

	bool buying;
	std::vector<wedge::Object> items;

	Widget_Window *window;
	Widget_Quantity_List *list;

	gfx::Image *tok_img;

	Widget *modal_main_widget;
};

#endif // SHOP_H
