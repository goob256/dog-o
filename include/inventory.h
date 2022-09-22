#ifndef INVENTORY_H
#define INVENTORY_H

#include <wedge2/inventory.h>

// 0 is none, defined in Wedge

enum Item_Types
{
	ITEM_APPLE = 1,
	ITEM_CARROT = 2,
	ITEM_CARAMEL = 3,
	ITEM_BANANA = 4,
	ITEM_NINJA_STAR = 5,
	ITEM_TURNIP = 6,
	ITEM_DAGGER = 7,
	ITEM_RED_POTION = 8,
	ITEM_ORANGE = 9,
	ITEM_BLUE_POTION = 10,
	ITEM_HOT_PEPPER = 11,
	ITEM_PEACH = 12,
	ITEM_RANDOMAXE = 13,
	ITEM_GREEN_POTION = 14,
	ITEM_EGGPLANT = 15,
	ITEM_SIZE
};

enum Special_Types
{
	SPECIAL_GREEN_GEM = 1,
	SPECIAL_PET_TAG = 2,
	SPECIAL_SIZE
};

enum Armour_Types
{
	BELT_WHITE = 1,
	BELT_YELLOW,
	BELT_GOLD,
	BELT_ORANGE,
	BELT_GREEN,
	BELT_BLUE,
	BELT_PURPLE,
	BELT_BROWN,
	BELT_RED,
	BELT_BLACK
};

class Object_Interface : public wedge::Object_Interface
{
public:
	virtual ~Object_Interface();

	wedge::Object make_object(wedge::Object_Type type, int id, int quantity);
	int use(wedge::Object object, wedge::Base_Stats *target);
	wedge::Fixed_Stats get_armour_stats(int id);
};

#endif // INVENTORY_H
