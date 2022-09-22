#include <wedge2/globals.h>
#include <wedge2/inventory.h>
#include <wedge2/stats.h>

#include "inventory.h"
#include "stats.h"

Object_Interface::~Object_Interface()
{
}

wedge::Object Object_Interface::make_object(wedge::Object_Type type, int id, int quantity)
{
	wedge::Object o;

	o.type = type;
	o.id = id;
	o.quantity = quantity;
	o.name = "";
	o.sell_price = 0;
	o.description = "Unknown...";

	if (type == wedge::OBJECT_ITEM) {
		if (id == ITEM_APPLE) {
			o.name = GLOBALS->game_t->translate(14)/* Originally: Apple */;
			o.sell_price = 5;
			o.description = GLOBALS->game_t->translate(149)/* Originally: Take a bite. It heals 50 HP. */;
		}
		else if (id == ITEM_CARROT) {
			o.name = GLOBALS->game_t->translate(25)/* Originally: Carrot */;
			o.sell_price = 20;
			o.description = GLOBALS->game_t->translate(150)/* Originally: Eat your vegetables! Restores 25 SP. */;
		}
		else if (id == ITEM_CARAMEL) {
			o.name = GLOBALS->game_t->translate(40)/* Originally: Caramel */;
			o.sell_price = 25;
			o.description = GLOBALS->game_t->translate(151)/* Originally: Got a sweet tooth? This will lift your spirits. */;
		}
		else if (id == ITEM_BANANA) {
			o.name = GLOBALS->game_t->translate(232)/* Originally: Banana */;
			o.sell_price = 15;
			o.description = GLOBALS->game_t->translate(233)/* Originally: Perfectly ripe. Heals 100 HP. */;
		}
		else if (id == ITEM_NINJA_STAR) {
			o.name = GLOBALS->game_t->translate(243)/* Originally: Ninja Star */;
			o.sell_price = 100;
			o.description = GLOBALS->game_t->translate(244)/* Originally: A sharp pointed star. Use in battle. */;
		}
		else if (id == ITEM_TURNIP) {
			o.name = GLOBALS->game_t->translate(300)/* Originally: Turnip */;
			o.sell_price = 40;
			o.description = GLOBALS->game_t->translate(301)/* Originally: A big, ripe (?) turnip. Restores 50 SP. */;
		}
		else if (id == ITEM_DAGGER) {
			o.name = GLOBALS->game_t->translate(333)/* Originally: Dagger */;
			o.sell_price = 250;
			o.description = GLOBALS->game_t->translate(334)/* Originally: A razor sharp throwing knife. Use in battle. */;
		}
		else if (id == ITEM_RED_POTION) {
			o.name = GLOBALS->game_t->translate(349)/* Originally: Red Potion */;
			o.sell_price = 250;
			o.description = GLOBALS->game_t->translate(350)/* Originally: Red syrup. Increases Max Simultaneous Turns by 1. */;
		}
		else if (id == ITEM_ORANGE) {
			o.name = GLOBALS->game_t->translate(351)/* Originally: Orange */;
			o.sell_price = 25;
			o.description = GLOBALS->game_t->translate(352)/* Originally: Fresh and juicy. Heals 150 HP. */;
		}
		else if (id == ITEM_BLUE_POTION) {
			o.name = GLOBALS->game_t->translate(363)/* Originally: Blue Potion */;
			o.sell_price = 250;
			o.description = GLOBALS->game_t->translate(364)/* Originally: Funny smelling blue liquid. Increases LUK by 1. */;
		}
		else if (id == ITEM_HOT_PEPPER) {
			o.name = GLOBALS->game_t->translate(444)/* Originally: Hot Pepper */;
			o.sell_price = 75;
			o.description = GLOBALS->game_t->translate(445)/* Originally: Nice and spicy. Restores 100 SP. */;
		}
		else if (id == ITEM_PEACH) {
			o.name = GLOBALS->game_t->translate(446)/* Originally: Peach */;
			o.sell_price = 40;
			o.description = GLOBALS->game_t->translate(447)/* Originally: A fuzzy little peach! Heals 200 HP. */;
		}
		else if (id == ITEM_RANDOMAXE) {
			o.name = GLOBALS->game_t->translate(501)/* Originally: Randomaxe */;
			o.sell_price = 250;
			o.description = GLOBALS->game_t->translate(502)/* Originally: A massive axe capable of huge damage. Use in battle. */;
		}
		else if (id == ITEM_GREEN_POTION) {
			o.name = GLOBALS->game_t->translate(505)/* Originally: Green Potion */;
			o.sell_price = 400;
			o.description = GLOBALS->game_t->translate(506)/* Originally: An intoxicating green fluid. Increases ATK by 1. */;
		}
		else if (id == ITEM_EGGPLANT) {
			o.name = GLOBALS->game_t->translate(554)/* Originally: Eggplant */;
			o.sell_price = 100;
			o.description = GLOBALS->game_t->translate(555)/* Originally: A tasty legume! Restores 200 SP. */;
		}
	}
	else if (type == wedge::OBJECT_ARMOUR) {
		if (id == BELT_WHITE) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("White Belt"));
			o.sell_price = 25;
			o.description = "A white karate belt.";
		}
		else if (id == BELT_YELLOW) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Yellow Belt"));
			o.sell_price = 25;
			o.description = "A yellow karate belt.";
		}
		else if (id == BELT_GOLD) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Gold Belt"));
			o.sell_price = 25;
			o.description = "A gold karate belt.";
		}
		else if (id == BELT_ORANGE) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Orange Belt"));
			o.sell_price = 25;
			o.description = "A orange karate belt.";
		}
		else if (id == BELT_GREEN) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Green Belt"));
			o.sell_price = 25;
			o.description = "A green karate belt.";
		}
		else if (id == BELT_BLUE) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Blue Belt"));
			o.sell_price = 25;
			o.description = "A blue karate belt.";
		}
		else if (id == BELT_PURPLE) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Purple Belt"));
			o.sell_price = 25;
			o.description = "A purple karate belt.";
		}
		else if (id == BELT_BROWN) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Brown Belt"));
			o.sell_price = 25;
			o.description = "A brown karate belt.";
		}
		else if (id == BELT_RED) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Red Belt"));
			o.sell_price = 25;
			o.description = "A red karate belt.";
		}
		else if (id == BELT_BLACK) {
			o.name = GLOBALS->game_t->translate(GLOBALS->english_game_t->get_id("Black Belt"));
			o.sell_price = 25;
			o.description = "A black karate belt.";
		}
	}
	else if (type == wedge::OBJECT_SPECIAL) {
		if (id == SPECIAL_GREEN_GEM) {
			o.name = GLOBALS->game_t->translate(317)/* Originally: Green Gem */;
			o.sell_price = 0;
			o.description = GLOBALS->game_t->translate(318)/* Originally: A dazzling green gem. Boosts Max HP. */;
		}
		else if (id == SPECIAL_PET_TAG) {
			o.name = GLOBALS->game_t->translate(496)/* Originally: Pet Tag */;
			o.sell_price = 0;
			o.description = GLOBALS->game_t->translate(497)/* Originally: A metal tag with Jellybean's name and address on it. */;
		}
	}

	return o;
}

int Object_Interface::use(wedge::Object object, wedge::Base_Stats *target)
{
	int amount = 0;

	if (object.type == wedge::OBJECT_ITEM) {
		if (object.id == ITEM_APPLE) {
			if (target->hp > 0) {
				target->hp = MIN(target->fixed.max_hp, target->hp + 50);
				amount = 50;
			}
		}
		else if (object.id == ITEM_CARROT) {
			if (target->hp > 0) {
				target->mp = MIN(target->fixed.max_mp, target->mp + 25);
				amount = 25;
			}
		}
		else if (object.id == ITEM_TURNIP) {
			if (target->hp > 0) {
				target->mp = MIN(target->fixed.max_mp, target->mp + 50);
				amount = 50;
			}
		}
		else if (object.id == ITEM_CARAMEL) {
			target->hp = target->fixed.max_hp;
		}
		else if (object.id == ITEM_BANANA) {
			if (target->hp > 0) {
				target->hp = MIN(target->fixed.max_hp, target->hp + 100);
				amount = 100;
			}
		}
		else if (object.id == ITEM_RED_POTION) {
			int extra_turns = target->fixed.get_extra(EXTRA_TURNS);
			if (extra_turns < 0) {
				extra_turns = 0;
			}
			extra_turns++;
			target->fixed.set_extra(EXTRA_TURNS, extra_turns);
		}
		else if (object.id == ITEM_ORANGE) {
			if (target->hp > 0) {
				target->hp = MIN(target->fixed.max_hp, target->hp + 150);
				amount = 150;
			}
		}
		else if (object.id == ITEM_BLUE_POTION) {
			int luck = target->fixed.get_extra(LUCK);
			if (luck < 0) {
				luck = 0;
			}
			luck++;
			target->fixed.set_extra(LUCK, luck);
		}
		else if (object.id == ITEM_HOT_PEPPER) {
			if (target->hp > 0) {
				target->mp = MIN(target->fixed.max_mp, target->mp + 100);
				amount = 100;
			}
		}
		else if (object.id == ITEM_PEACH) {
			if (target->hp > 0) {
				target->hp = MIN(target->fixed.max_hp, target->hp + 200);
				amount = 200;
			}
		}
		else if (object.id == ITEM_GREEN_POTION) {
			target->fixed.attack++;
		}
		else if (object.id == ITEM_EGGPLANT) {
			if (target->hp > 0) {
				target->mp = MIN(target->fixed.max_mp, target->mp + 200);
				amount = 200;
			}
		}
		// This is already in wedge::Inventory::use
		/*
		if (wedge::globals->item_sfx[object.id] != NULL) {
			wedge::globals->item_sfx[object.id]->play(false);
		}
		*/
	}

	return amount;
}

wedge::Fixed_Stats Object_Interface::get_armour_stats(int id)
{
	wedge::Fixed_Stats stats;
		
	if (id == BELT_WHITE) {
		stats.defence = 5;
	}
	else if (id == BELT_YELLOW) {
		stats.defence = 10;
	}
	else if (id == BELT_GOLD) {
		stats.defence = 15;
	}
	else if (id == BELT_ORANGE) {
		stats.defence = 20;
	}
	else if (id == BELT_GREEN) {
		stats.defence = 25;
	}
	else if (id == BELT_BLUE) {
		stats.defence = 30;
	}
	else if (id == BELT_PURPLE) {
		stats.defence = 35;
	}
	else if (id == BELT_BROWN) {
		stats.defence = 40;
	}
	else if (id == BELT_RED) {
		stats.defence = 45;
	}
	else if (id == BELT_BLACK) {
		stats.defence = 50;
	}

	return stats;
}
