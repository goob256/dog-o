#ifndef SHOP_GAME_H
#define SHOP_GAME_H

#include <wedge2/main.h>
#include <wedge2/systems.h>

class Shop_Game : public wedge::Game
{
public:
	Shop_Game(std::vector<wedge::Object> items/*price goes in quantity*/);
	virtual ~Shop_Game();

	bool start();
	bool run();
	void draw();
	void draw_fore();

private:
	std::vector<wedge::Object> items;
};

#endif // SHOP_GAME_H
