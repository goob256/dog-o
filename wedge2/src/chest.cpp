#include "wedge2/area_game.h"
#include "wedge2/chest.h"
#include "wedge2/delete_map_entity.h"
#include "wedge2/general.h"
#include "wedge2/globals.h"

using namespace wedge;

namespace wedge {

Chest::Chest(std::string name, std::string sprite_name, Object contents, int milestone, Dialogue_Position dialogue_position) :
	Map_Entity(name),
	open(false),
	contents(contents),
	gold(-1),
	milestone(milestone),
	dialogue_position(dialogue_position),
	remove_when_activated(false),
	achievement(nullptr),
	delete_achievement(false)
{
	if (sprite_name != "") {
		sprite = new gfx::Sprite(sprite_name);
	}
	else {
		sprite = NULL;
	}
}

Chest::Chest(std::string name, std::string sprite_name, int gold, int milestone, Dialogue_Position dialogue_position) :
	Map_Entity(name),
	open(false),
	gold(gold),
	milestone(milestone),
	dialogue_position(dialogue_position),
	remove_when_activated(false),
	achievement(nullptr),
	delete_achievement(false)
{
	if (sprite_name != "") {
		sprite = new gfx::Sprite(sprite_name);
	}
	else {
		sprite = NULL;
	}
}

Chest::Chest(util::JSON::Node *json) :
	Map_Entity(json),
	open(false),
	gold(-1),
	dialogue_position(DIALOGUE_AUTO),
	remove_when_activated(false),
	achievement(nullptr),
	delete_achievement(false)
{
	Object_Type type = OBJECT_NONE;
	int id = 0;
	int quantity = 0;

	milestone = -1; // this didn't exist before, must set a default

	for (size_t i = 0; i < json->children.size(); i++) {
		util::JSON::Node *n = json->children[i];

		if (n->key == "chest_open") {
			open = n->as_bool();
		}
		else if (n->key == "chest_type") {
			type = (Object_Type)n->as_int();
		}
		else if (n->key == "chest_object_id") {
			id = (Item)n->as_int();
		}
		else if (n->key == "chest_quantity") {
			quantity = n->as_int();
		}
		else if (n->key == "chest_gold") {
			gold = n->as_int();
		}
		else if (n->key == "chest_milestone") {
			milestone = n->as_int();
		}
		else if (n->key == "chest_dialogue_position") {
			dialogue_position = (Dialogue_Position)n->as_int();
		}
		else if (n->key == "remove_when_activated") {
			remove_when_activated = n->as_bool();
		}
		else if (n->key == "achievement") {
			delete_achievement = true;
			achievement = (void *)_strdup(n->as_string().c_str());
		}
	}

	if (gold <= 0) {
		contents = OBJECT->make_object(type, id, quantity);
	}

	if (open) {
		if (sprite) {
			sprite->set_animation("open");
		}
	}
}

Chest::~Chest()
{
	// Map_Entity deletes sprite

	if (delete_achievement) {
		free(achievement);
	}
}

bool Chest::activate(Map_Entity *activator)
{
	if (open == false) {
		if (achievement) {
			util::achieve(achievement);
		}
		bool success;
		if (gold > 0) {
			INSTANCE->add_gold(gold);
			success = true;
		}
		else {
			success = INSTANCE->inventory.add(contents) == contents.quantity;
		}
		if (success) {
			if (milestone >= 0) {
				INSTANCE->set_milestone_complete(milestone, true);
			}

			globals->chest->play(false);
			open = true;
			if (sprite) {
				sprite->set_animation("open");
			}
			std::string message;
			if (gold > 0) {
				message = util::string_printf(GLOBALS->game_t->translate(2)/* Originally: Received %d gold! */.c_str(), gold);
			}
			else {
				if (contents.quantity > 1) {
					message = util::string_printf(GLOBALS->game_t->translate(5)/* Originally: Received %d %s! */.c_str(), contents.quantity, contents.name.c_str());
				}
				else {
					message = util::string_printf(GLOBALS->game_t->translate(6)/* Originally: Received %s! */.c_str(), contents.name.c_str());
				}
			}
			globals->do_dialogue("", message, DIALOGUE_MESSAGE, dialogue_position, NULL);
		}
		else {
			globals->do_dialogue("", GLOBALS->game_t->translate(4)/* Originally: Inventory full! */, DIALOGUE_MESSAGE, dialogue_position, NULL);
		}

		INSTANCE->chests_opened++;
	}

	if (remove_when_activated) {
		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new Delete_Map_Entity_Step(this, new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)
	}

	return true;
}

std::string Chest::save()
{
	std::string s;

	s += util::string_printf("\"type\": \"chest\",");
	s += util::string_printf("\"chest_open\": %s,", bool_to_string(open).c_str());
	s += util::string_printf("\"chest_type\": %d,", (int)contents.type);
	s += util::string_printf("\"chest_object_id\": %d,", contents.id);
	s += util::string_printf("\"chest_quantity\": %d,", contents.quantity);
	s += util::string_printf("\"chest_gold\": %d,", gold);
	s += util::string_printf("\"chest_milestone\": %d,", milestone);
	s += util::string_printf("\"chest_dialogue_position\": %d,", (int)dialogue_position);
	s += util::string_printf("\"remove_when_activated\": %s,", bool_to_string(remove_when_activated).c_str());
	if (achievement) {
		s += util::string_printf("\"achievement\": \"%s\",", (char *)achievement);
	}

	s += Map_Entity::save();

	return s;
}

void Chest::set_remove_when_activated(bool remove_when_activated)
{
	this->remove_when_activated = remove_when_activated;
}

void Chest::set_achievement(void *id)
{
	achievement = id;
}

}
