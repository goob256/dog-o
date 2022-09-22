#include "wedge2/general.h"
#include "wedge2/globals.h"
#include "wedge2/inventory.h"
#include "wedge2/spells.h"
#include "wedge2/stats.h"

using namespace wedge;

namespace wedge {

Fixed_Stats::Fixed_Stats() :
	max_hp(0),
	max_mp(0),
	attack(0),
	defence(0),
	weakness(WEAK_STRONG_NONE),
	strength(WEAK_STRONG_NONE)
{
}

Fixed_Stats::Fixed_Stats(util::JSON::Node *json)
{
	util::JSON::Node *n;

	n = json->find("max_hp");
	if (n != NULL) {
		max_hp = n->as_int();
	}
	n = json->find("max_mp");
	if (n != NULL) {
		max_mp = n->as_int();
	}
	n = json->find("attack");
	if (n != NULL) {
		attack = n->as_int();
	}
	n = json->find("defence");
	if (n != NULL) {
		defence = n->as_int();
	}
	n = json->find("weakness");
	if (n != NULL) {
		weakness = (Weak_Strong)n->as_int();
	}
	n = json->find("strength");
	if (n != NULL) {
		strength = (Weak_Strong)n->as_int();
	}
	n = json->find("extra");
	if (n != NULL) {
		for (size_t i = 0; i < n->children.size(); i++) {
			extra.push_back(n->children[i]->as_int());
		}
	}
}

std::string Fixed_Stats::save()
{
	std::string s;
	s = "{";
	s += util::string_printf("\"max_hp\": %d, \"max_mp\": %d, \"attack\": %d, \"defence\": %d, \"weakness\": %d, \"strength\": %d,", max_hp, max_mp, attack, defence, (int)weakness, (int)strength);
	s += "\"extra\": [";
	for (size_t i = 0; i < extra.size(); i++) {
		s += util::itos(extra[i]);
		if (i < extra.size()-1) {
			s += ",";
		}
	}
	s += "]";
	s += "}";
	return s;
}

void Fixed_Stats::set_extra(int index, int value)
{
	int pad = index - (int)extra.size() + 1;
	for (int i = 0; i < pad; i++) {
		extra.push_back(0);
	}
	extra[index] = value;
}

int Fixed_Stats::get_extra(int index) const
{
	if ((int)extra.size() <= index) {
		return -1;
	}
	return extra[index];
}

Fixed_Stats &Fixed_Stats::operator+=(const Fixed_Stats &rhs)
{
	max_hp += rhs.max_hp;
	max_mp += rhs.max_mp;
	attack += rhs.attack;
	defence += rhs.defence;
	int min = int(MIN(extra.size(), rhs.extra.size()));
	for (int i = 0; i < min; i++) {
		set_extra(i, get_extra(i) + rhs.get_extra(i));
	}
	return *this;
}

//

Base_Stats::Base_Stats() :
	status(STATUS_OK)
{
	hp = fixed.max_hp;
	mp = fixed.max_mp;
}

Base_Stats::Base_Stats(util::JSON::Node *json)
{
	util::JSON::Node *n;

	n = json->find("name");
	if (n != NULL) {
		name = n->as_string();
	}
	n = json->find("status");
	if (n != NULL) {
		status = n->as_int();
	}
	n = json->find("hp");
	if (n != NULL) {
		hp = n->as_int();
	}
	n = json->find("mp");
	if (n != NULL) {
		mp = n->as_int();
	}
	n = json->find("fixed");
	if (n != NULL) {
		fixed = Fixed_Stats(n);
	}
	n = json->find("spells");
	if (n) {
		for (size_t i = 0; i < n->children.size(); i++) {
			std::string name = n->children[i]->as_string();
			add_spell(name);
		}
	}
	n = json->find("extra");
	if (n != NULL) {
		for (size_t i = 0; i < n->children.size(); i++) {
			extra.push_back(n->children[i]->as_int());
		}
	}
}

std::string Base_Stats::save()
{
	std::string s;
	s += "{";
	s += util::string_printf("\"name\": \"%s\", \"status\": %d, \"hp\": %d, \"mp\": %d, \"fixed\": %s,", name.c_str(), (int)status, hp, mp, fixed.save().c_str());
	s += "\"spells\": [";
	for (size_t i = 0; i < spells.size(); i++) {
		s += "\"" + spells[i] + "\"";
		if (i < spells.size()-1) {
			s += ",";
		}
	}
	s += "],";
	s += "\"extra\": [";
	for (size_t i = 0; i < extra.size(); i++) {
		s += util::itos(extra[i]);
		if (i < extra.size()-1) {
			s += ",";
		}
	}
	s += "]";
	s += "}";
	return s;
}

void Base_Stats::add_spell(std::string name)
{
	std::vector<std::string>::iterator it;

	int adding_cost = SPELLS->get_cost(name);

	for (it = spells.begin(); it != spells.end(); it++) {
		std::string s = *it;
		int cost = SPELLS->get_cost(s);
		if (cost < adding_cost) {
			break;
		}
		else if (cost == adding_cost && s > name) {
			break;
		}
	}

	spells.insert(it, name);
}

std::string Base_Stats::spell(int index)
{
	return spells[index];
}

int Base_Stats::num_spells()
{
	return (int)spells.size();
}

std::vector<std::string> Base_Stats::get_spells()
{
	return spells;
}

void Base_Stats::set_extra(int index, int value)
{
	int pad = (int)extra.size() - index + 1;
	for (int i = 0; i < pad; i++) {
		extra.push_back(0);
	}
	extra[index] = value;
}

int Base_Stats::get_extra(int index) const
{
	return extra[index];
}

void Base_Stats::set_name(std::string name)
{
	this->name = name;
}

std::string Base_Stats::get_name()
{
	return name;
}

}
//--

Weapon_Stats::Weapon_Stats() :
	id(WEAPON_NONE)
{
	stats = OBJECT->get_weapon_stats(id);
}

Weapon_Stats::Weapon_Stats(Weapon id) :
	id(id)
{
	stats = OBJECT->get_weapon_stats(id);
}

Weapon_Stats::Weapon_Stats(util::JSON::Node *json)
{
	util::JSON::Node *n;

	n = json->find("id");
	if (n != NULL) {
		id = (Weapon)n->as_int();
	}
	stats = OBJECT->get_weapon_stats(id);
}

std::string Weapon_Stats::save()
{
	std::string s;
	s += "{ ";
	s += util::string_printf("\"id\": %d", (int)id);
	s += " }";
	return s;
}

Armour_Stats::Armour_Stats() :
	id(ARMOUR_NONE)
{
	stats = OBJECT->get_armour_stats(id);
}

Armour_Stats::Armour_Stats(Armour id) :
	id(id)
{
	stats = OBJECT->get_armour_stats(id);
}

Armour_Stats::Armour_Stats(util::JSON::Node *json)
{
	util::JSON::Node *n;

	n = json->find("id");
	if (n != NULL) {
		id = (Armour)n->as_int();
	}
	stats = OBJECT->get_armour_stats(id);
}

std::string Armour_Stats::save()
{
	std::string s;
	s += "{ ";
	s += util::string_printf("\"id\": %d", (int)id);
	s += " }";
	return s;
}

Player_Stats::Player_Stats()
{
}

Player_Stats::Player_Stats(util::JSON::Node *json)
{
	util::JSON::Node *n;

	/*
	n = json->find("name");
	if (n != NULL) {
		name = n->as_string();
	}
	*/
	n = json->find("level");
	if (n != NULL) {
		level = n->as_int();
	}
	n = json->find("experience");
	if (n != NULL) {
		experience = n->as_int();
	}
	n = json->find("stats");
	if (n != NULL) {
		base = Base_Stats(n);
	}
	n = json->find("weapon");
	if (n != NULL) {
		weapon = Weapon_Stats(n);
	}
	n = json->find("weapon_index");
	if (n != NULL) {
		weapon_index = n->as_int();
	}
	n = json->find("armour");
	if (n != NULL) {
		armour = Armour_Stats(n);
	}
#if 0
	n = json->find("armour_index");
	if (n != NULL) {
		armour_index = n->as_int();
	}
#endif
	n = json->find("sprite");
	if (n != NULL) {
		sprite = json_to_sprite(n);
	}
}

std::string Player_Stats::save()
{
	std::string s;
	s += "{";
	//s += util::string_printf("\"name\": \"%s\",", name.c_str());
	s += util::string_printf("\"level\": %d,", level);
	s += util::string_printf("\"experience\": %d,", experience);
	s += "\"stats\": " + base.save() + ",";
	s += "\"weapon\": " + weapon.save() + ",";
	s += "\"weapon_index\": " + util::itos(weapon_index) + ",";
	s += "\"armour\": " + armour.save() + ",";
#if 0
	s += "\"armour_index\": " + util::itos(armour_index) + ",";
#endif
	s += "\"sprite\": " + sprite_to_string(sprite);
	s += "}";
	return s;
}
