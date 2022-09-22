#include <wedge2/area_game.h>
#include <wedge2/globals.h>

#include "shop.h"
#include "shop_game.h"
					
Shop_Game::Shop_Game(std::vector<wedge::Object> items) :
	items(items)
{
}

Shop_Game::~Shop_Game()
{
	for (std::vector<gui::GUI *>::iterator it = shim::guis.begin(); it != shim::guis.end(); it++) {
		gui::GUI *gui = *it;
		if (dynamic_cast<Shop_GUI *>(gui)) {
			gui->exit();
		}
	}
}

bool Shop_Game::start()
{
	if (Game::start() == false) {
		return false;
	}

	Shop_GUI *gui = new Shop_GUI(items);
	shim::guis.push_back(gui);

	return true;
}

bool Shop_Game::run()
{
	Game::run(); // don't return if false, there are no systems usually

	if (shim::guis.size() == 0) {
		return false;
	}

	return true;
}

void Shop_Game::draw()
{
	Game::draw();
}

void Shop_Game::draw_fore()
{
	Game::draw_fore();
}
