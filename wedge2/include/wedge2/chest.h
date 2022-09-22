#ifndef WEDGE2_CHEST_H
#define WEDGE2_CHEST_H

#include "wedge2/main.h"
#include "wedge2/globals.h"
#include "wedge2/inventory.h"
#include "wedge2/map_entity.h"

namespace wedge {

class WEDGE2_EXPORT Chest : public Map_Entity
{
public:
	Chest(std::string entity_name, std::string sprite_name, Object contents, int milestone = -1, Dialogue_Position dialogue_position = DIALOGUE_AUTO);
	Chest(std::string entity_name, std::string sprite_name, int gold, int milestone = -1, Dialogue_Position dialogue_position = DIALOGUE_AUTO);
	Chest(util::JSON::Node *json);
	virtual ~Chest();

	bool activate(Map_Entity *activator);

	std::string save();

	void set_remove_when_activated(bool remove_when_activated);

	void set_achievement(void *id);

protected:
	bool open;
	Object contents;
	int gold;
	int milestone;
	Dialogue_Position dialogue_position;
	bool remove_when_activated;
	void *achievement;
	bool delete_achievement;
};

}

#endif // WEDGE2_CHEST_H
