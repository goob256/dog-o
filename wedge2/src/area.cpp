#include "wedge2/area.h"
#include "wedge2/area_game.h"
#include "wedge2/a_star.h"
#include "wedge2/chest.h"
#include "wedge2/general.h"
#include "wedge2/generic_immediate_callback.h"
#include "wedge2/globals.h"
#include "wedge2/input.h"
#include "wedge2/map_entity.h"
#include "wedge2/npc.h"
#include "wedge2/pause_presses.h"
#include "wedge2/player_input.h"
#include "wedge2/systems.h"
#include "wedge2/tile_movement.h"

using namespace wedge;

namespace wedge {

static void end_water_ripple_callback(void *data)
{
	auto sprite = static_cast<gfx::Sprite *>(data);
	AREA->get_current_area()->end_water_ripple(sprite);
}

bool entity_y_compare(Map_Entity *a, Map_Entity *b)
{
	if (a->get_offset() == b->get_offset() && a->get_position() == b->get_position()) {
		int a_index = -1;
		int b_index = -1;
		std::vector<Map_Entity *> players = AREA->get_players();
		for (size_t i = 0; i < players.size(); i++) {
			if (players[i] == a) {
				a_index = (int)i;
			}
			else if (players[i] == b) {
				b_index = (int)i;
			}
		}
		if (a_index >= 0 && b_index >= 0) {
			return a_index > b_index;
		}
	}
	return (a->get_offset().y + a->get_position().y) < (b->get_offset().y + b->get_position().y);
}

Area::Area(std::string name) :
	name(name),
	tilemap(NULL),
	done(false),
	hooks(NULL),
	json(NULL)
{
	overlay_colour.a = 0;
}

Area::Area(util::JSON::Node *json) :
	tilemap(NULL),
	done(false),
	hooks(NULL)
{
	overlay_colour.a = 0;
	name = json->key;
	this->json = json;
}

Area::~Area()
{
	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		delete *it;
	}

	delete tilemap;
	delete hooks;

	AREA->remove_system(entity_movement_system);
	delete entity_movement_system;
}

bool Area::start()
{
	entity_movement_system = new System(AREA);
	AREA->get_systems().push_back(entity_movement_system);

	new_game = (name == "--start--");
	loaded = json != NULL;

	bool is_initial_load = AREA->get_players().size() == 0;

	if (new_game) {
		name = "start";
	}

	tilemap = new gfx::Tilemap(name + ".wm2");

	if (new_game) {
		for (size_t i = 0; i < INSTANCE->stats.size(); i++) {
			Map_Entity *player = AREA->create_player(INSTANCE->stats[i].base.get_name());
			AREA->set_player((int)i, player);
			player->start(this);
			player->set_sprite(new gfx::Sprite(globals->player_sprite_names[i]));
			player->set_position(globals->player_start_positions[i]);
			player->set_direction(globals->player_start_directions[i], true, false);
			player->set_shadow(true, util::Size<int>(12, 6));
			entities.push_back(player);
		}
	}
	else if (loaded) {
		util::JSON::Node *entities = json->find("entities");

		for (size_t i = 0; i < entities->children.size(); i++) {
			util::JSON::Node *node = entities->children[i];
			Map_Entity *entity;
			std::string type;
			util::JSON::Node *type_node = node->find("type");
			if (type_node != NULL) {
				type = type_node->as_string();
			}
			if ((entity = AREA->create_entity(type, node)) == NULL) {
				if (type == "chest") {
					entity = new Chest(node);
				}
				else if (type == "npc") {
					entity = new NPC(node);
				}
				else {
					entity = new Map_Entity(node);
				}
			}
			entity->start(this);
			this->entities.push_back(entity);
			for (size_t i = 0; i < MAX_PARTY; i++) {
				if (entity->get_name() == INSTANCE->stats[i].base.get_name()) {
					AREA->set_player((int)i, entity);
				}
			}
		}

		util::JSON::Node *n = json->find("hook");
		if (n != NULL) {
			hook_save = n->as_string();
		}
	}
	else { // changed areas
	}

	if (is_initial_load) {
		// don't use this generic one for player...
		Map_Entity_Input_Step *meis = AREA->get_player(0)->get_input_step();
		if (meis) {
			meis->die();
		}

		// ... use a Player_Input_System
		Task *new_task = new Task(entity_movement_system);
		Player_Input_Step *step = new Player_Input_Step(AREA->get_player(0), new_task);
		AREA->get_player(0)->set_input_step(step);
		ADD_STEP(step)
		entity_movement_system->get_tasks().push_back(new_task);
	}
	else {
		Map_Entity_Input_Step *meis = AREA->get_player(0)->get_input_step();
		if (meis) {
			meis->reset(entity_movement_system);
		}

		// other players' input step dies in the destructions of steps in each level, so recreate it
		std::vector<Map_Entity *> players = AREA->get_players();
		for (size_t i = 1; i < players.size(); i++) {
			Task *new_task = new Task(entity_movement_system);
			Map_Entity_Input_Step *step = new Map_Entity_Input_Step(players[i], new_task);
			players[i]->set_input_step(step);
			ADD_STEP(step)
			entity_movement_system->get_tasks().push_back(new_task);
		}
	}

	order_player_input_steps();

	return true;
}

void Area::set_hooks(Area_Hooks *hooks)
{
	this->hooks = hooks;

	if (hooks) {
		hooks->start(new_game, loaded, hook_save);
		hooks->set_animated_tiles();
	}
}

Area_Hooks *Area::get_hooks()
{
	return hooks;
}

std::string Area::get_name()
{
	return name;
}

gfx::Tilemap *Area::get_tilemap()
{
	return tilemap;
}

std::list<Map_Entity *> &Area::get_entities()
{
	return entities;
}

void Area::draw(util::Point<float> map_offset, Layer_Spec spec)
{
	int nlayers = tilemap->get_num_layers();
	int middle_layer = get_middle_layer();

	if (spec == BELOW) {
		nlayers = middle_layer + 1;
	}
	
	entities.sort(entity_y_compare);

	std::list<int> entity_layers;
	std::vector<int> pre_draw_layers;
	std::vector<int> post_draw_layers;
	std::list<int> all_used_layers;
	
	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *entity = *it;
		entity_layers.push_back(entity->get_layer());
#ifdef VERBOSE
		util::debugmsg("entity %s on layer %d\n", entity->get_name().c_str(), entity->get_layer());
#endif
	}

	entity_layers.sort();
	entity_layers.unique();

	if (hooks) {
		pre_draw_layers = hooks->get_pre_draw_layers();
		std::sort(pre_draw_layers.begin(), pre_draw_layers.end());
		auto last1 = std::unique(pre_draw_layers.begin(), pre_draw_layers.end());
		pre_draw_layers.erase(last1, pre_draw_layers.end());
		post_draw_layers = hooks->get_post_draw_layers();
		std::sort(post_draw_layers.begin(), post_draw_layers.end());
		auto last2 = std::unique(post_draw_layers.begin(), post_draw_layers.end());
		post_draw_layers.erase(last2, post_draw_layers.end());
	}

	for (std::list<int>::iterator it = entity_layers.begin(); it != entity_layers.end(); it++) {
		all_used_layers.push_back(*it);
	}
	for (size_t i = 0; i < pre_draw_layers.size(); i++) {
		all_used_layers.push_back(pre_draw_layers[i]);
#ifdef VERBOSE
		util::debugmsg("pre draw on %d\n", pre_draw_layers[i]);
#endif
	}
	for (size_t i = 0; i < post_draw_layers.size(); i++) {
		all_used_layers.push_back(post_draw_layers[i]);
#ifdef VERBOSE
		util::debugmsg("post draw on %d\n", post_draw_layers[i]);
#endif
	}
	all_used_layers.sort();
	all_used_layers.unique();
		
	int current_tile_layer = spec == ABOVE ? middle_layer + 1 : 0;
	int start_layer = current_tile_layer-1;
	int next_layer = current_tile_layer-1;

	std::list<int>::iterator it = all_used_layers.begin();
	while (it != all_used_layers.end() && *it < current_tile_layer) {
		it++;
	}

	if (it != all_used_layers.end()) {
		start_layer = *it;
		it++;
		if (it != all_used_layers.end()) {
			next_layer = *it;
			it++;
		}
	}

#ifdef VERBOSE
	util::debugmsg("begin: start_layer=%d\n", start_layer);
#endif

	if (start_layer == -1) {
		tilemap->draw(current_tile_layer, nlayers-1, map_offset, true);
		draw_water_ripples(map_offset);
	}
	else {
		while (true) {
			if (std::find(pre_draw_layers.begin(), pre_draw_layers.end(), start_layer) == pre_draw_layers.end()) {
				tilemap->draw(current_tile_layer, start_layer, map_offset, true);
			}
			else {
				tilemap->draw(current_tile_layer, start_layer-1, map_offset, true);
				hooks->pre_draw(start_layer, map_offset);
				tilemap->draw(start_layer, map_offset, true);
			}
			if (start_layer == middle_layer) {
				draw_water_ripples(map_offset);
			}
			current_tile_layer = start_layer + 1;

			if (std::find(entity_layers.begin(), entity_layers.end(), start_layer) != entity_layers.end()) {
				for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
					Map_Entity *entity = *it;
					int sl = entity->get_shadow_layer();
					if ((sl < 0 && entity->get_layer() == start_layer) || sl == start_layer) {
						entity->draw_shadow(map_offset);
					}
				}
			}
			if (std::find(entity_layers.begin(), entity_layers.end(), start_layer) != entity_layers.end()) {
				for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
					Map_Entity *entity = *it;
					if (entity->get_layer() == start_layer) {
						entity->draw(map_offset);
					}
				}
			}

			if (std::find(post_draw_layers.begin(), post_draw_layers.end(), start_layer) != post_draw_layers.end()) {
				hooks->post_draw(start_layer, map_offset);
			}
			if (next_layer == -1) {
				if (start_layer < nlayers-1) {
					tilemap->draw(start_layer+1, nlayers-1, map_offset, true);
				}
				break;
			}
			start_layer = next_layer;
			if (it != all_used_layers.end()) {
				next_layer = *it;
				it++;
			}
			else {
				next_layer = -1;
			}
		}
	}

	if (overlay_colour.a != 0) {
		gfx::draw_filled_rectangle(overlay_colour, util::Point<int>(0, 0), shim::screen_size);
	}
}

void Area::draw(Layer_Spec spec)
{
	Map_Entity *player = AREA->get_player(0);
	util::Point<int> pos = player->get_position();
	util::Point<float> offset = player->get_offset();
	util::Size<int> player_size = player->get_size();
	util::Point<float> sz(player_size.w / 2.0f, 1.0f - player_size.h / 2.0f);
	add_tiles(pos, offset, sz);
	util::Point<float> map_offset = get_centred_offset(pos, offset, true);
	draw(map_offset, spec);
}

std::string Area::get_next_area_name()
{
	return next_area_name;
}

std::vector< util::Point<int> > Area::get_next_area_positions()
{
	return next_area_positions;
}

std::vector<Direction> Area::get_next_area_directions()
{
	return next_area_directions;
}

bool Area::get_next_area_scrolls_in()
{
	return next_area_scrolls_in;
}

bool Area::exit_gameplay()
{
	return done;
}

bool Area::on_tile(Map_Entity *entity)
{
	entity->set_on_slope(false);
	if (hooks) {
		return hooks->on_tile(entity);
	}
	else {
		return false;
	}
}

bool Area::try_tile(Map_Entity *entity, util::Point<int> tile_pos)
{
	if (hooks) {
		return hooks->try_tile(entity, tile_pos);
	}
	else {
		return false;
	}
}

void Area::set_next_area(std::string name, util::Point<int> position, Direction direction)
{
	next_area_name = name;

	next_area_positions.clear();
	next_area_directions.clear();

	next_area_positions.push_back(position);
	next_area_directions.push_back(direction);

	next_area_scrolls_in = true;

	entity_movement_system->set_paused(AREA->get_pause_entity_movement_on_next_area_change());
}

void Area::set_next_area(std::string name, std::vector< util::Point<int> > player_positions, std::vector<Direction> directions)
{
	next_area_name = name;
	next_area_positions = player_positions;
	next_area_directions = directions;
	next_area_scrolls_in = false;
	entity_movement_system->set_paused(AREA->get_pause_entity_movement_on_next_area_change());
}

util::Point<float> Area::get_centred_offset(util::Point<int> entity_position, util::Point<float> entity_offset, bool clamp_edges)
{
	util::Point<float> entity_pos;
	util::Point<float> centre;
	util::Point<float> map_offset;

	entity_pos = (entity_offset + entity_position) * shim::tile_size;

	util::Size<float> half_screen = shim::screen_size;
	half_screen /= 2.0f;

	centre = -entity_pos + half_screen;

	util::Size<int> tilemap_size = tilemap->get_size() * shim::tile_size;

	if (clamp_edges == false) {
		map_offset.x = centre.x;
	}
	else {
		if (tilemap_size.w < shim::screen_size.w) {
			map_offset.x = (shim::screen_size.w - tilemap_size.w) / 2.0f;
		}
		else {
			if (centre.x > 0.0f) {
				map_offset.x = 0.0f;
			}
			else if (entity_pos.x + half_screen.w > tilemap_size.w) {
				map_offset.x = (float)-(tilemap_size.w - shim::screen_size.w);
			}
			else {
				map_offset.x = centre.x;
			}
		}
	}

	if (clamp_edges == false) {
		map_offset.y = centre.y;
	}
	else {
		if (tilemap_size.h < shim::screen_size.h) {
			map_offset.y = (shim::screen_size.h - tilemap_size.h) / 2.0f;
		}
		else {
			if (centre.y > 0.0f) {
				map_offset.y = 0.0f;
			}
			else if (entity_pos.y + half_screen.h > tilemap_size.h) {
				map_offset.y = (float)-(tilemap_size.h - shim::screen_size.h);
			}
			else {
				map_offset.y = centre.y;
			}
		}
	}

	return map_offset;
}

System *Area::get_entity_movement_system()
{
	return entity_movement_system;
}

void Area::remove_entity(Map_Entity *entity, bool destroy)
{
	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *e = *it;
		if (e == entity) {
			if (hooks) {
				hooks->remove_entity(entity);
			}
			entities.erase(it);
			if (destroy) {
				delete entity;
			}
			return;
		}
	}
}

void Area::started()
{
	if (hooks) {
		hooks->set_player_start_zones();
		hooks->started();
	}
}

void Area::end()
{
	if (hooks) {
		hooks->end();
	}
}

Map_Entity *Area::entity_on_tile(util::Point<int> pos, std::vector<Map_Entity *> ignore)
{
	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *entity = *it;
		if (std::find(ignore.begin(), ignore.end(), entity) != ignore.end()) {
			continue;
		}
		util::Size<int> size = entity->get_size();
		util::Rectangle<int> rectangle(entity->get_position()-util::Point<int>(0, size.h-1), size);
		if (rectangle.contains(pos)) {
			return entity;
		}
	}

	return NULL;
}

std::vector< util::Rectangle<int> > Area::get_entity_solids(std::vector<Map_Entity *> ignore, bool include_non_solid)
{
	std::vector< util::Rectangle<int> > solids;

	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *entity = *it;
		if (std::find(ignore.begin(), ignore.end(), entity) != ignore.end()) {
			continue;
		}
		if (include_non_solid == false && entity->is_solid() == false) {
			continue;
		}
		util::Size<int> size = entity->get_size();
		solids.push_back(util::Rectangle<int>(entity->get_position()-util::Point<int>(0, size.h-1), size));
	}

	return solids;
}

bool Area::activate_with(Map_Entity *entity)
{
	util::Point<int> pos = add_direction(entity->get_position(), entity->get_direction(), 1);

	Map_Entity *activated = NULL;
	bool is_player = entity == AREA->get_player(0);
	std::vector<Map_Entity *> players = AREA->get_players();

	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *e = *it;
		util::Point<int> epos = e->get_position();
		util::Size<int> esz = e->get_size();
		if (pos.x >= epos.x && pos.y >= epos.y-(esz.h-1) && pos.x < epos.x+esz.w && pos.y <= epos.y) {
			// Don't pick this entity if it's the lead player activating a following player
			if (!(is_player && std::find(players.begin(), players.end(), e) != players.end())) {
				activated = e;
				break;
			}
		}
	}

	bool ret = false;

	if (activated) {
		if (hooks == NULL || hooks->activate(entity, activated) == false) {
			if (activated->activate(entity)) {
				ret = true;
			}
		}
	}

	if (hooks) {
		if (hooks->activate_with(entity)) {
			ret = true;
		}
	}

	return ret;
}

std::string Area::save(bool save_players)
{
	std::string s;
	s += "{";
	s += util::string_printf("\"entities\": {");

	int count = 0;

	std::vector<Map_Entity *> players = AREA->get_players();

	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *entity = *it;
		bool is_player = std::find(players.begin(), players.end(), entity) != players.end();
		if (save_players == false && is_player) {
			continue;
		}
		if (std::find(dont_save_entities.begin(), dont_save_entities.end(), entity->get_name()) != dont_save_entities.end()) {
			continue;
		}
		s += "\"" + entity->get_name() + "\": {" + entity->save() + "}" + (count == (int)entities.size()-1 ? "" : ",");
		count++;
	}

	s += util::string_printf("},");
	s += "\"hook\": \"" + (hooks ? hooks->save() : "") + "\"";
	s += "}";
	return s;
}

Map_Entity *Area::find_entity(std::string name)
{
	for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *entity = *it;
		if (entity->get_name() == name) {
			return entity;
		}
	}
	return NULL;
}

void Area::run()
{
	if (hooks != NULL) {
		hooks->run();
	}
}
	
void Area::lost_device()
{
	if (hooks != NULL) {
		hooks->lost_device();
	}
}

void Area::found_device()
{
	if (hooks != NULL) {
		hooks->found_device();
	}
}
	
void Area::resize(util::Size<int> new_size)
{
	if (hooks != NULL) {
		hooks->resize(new_size);
	}
}

Battle_Game *Area::get_random_battle()
{
	if (hooks) {
		return hooks->get_random_battle();
	}
	else {
		return NULL;
	}
}

bool Area::can_save()
{
	if (hooks) {
		return hooks->can_save();
	}
	else {
		return false;
	}
}

void Area::disconnect_player_input_step()
{
	Map_Entity_Input_Step *input_step = AREA->get_player(0)->get_input_step();
	if (input_step != NULL) {
		input_step->get_task()->remove_step(input_step);
	}
}

void Area::dialogue_done(Map_Entity *entity)
{
	if (hooks) {
		hooks->dialogue_done(entity);
	}
}

bool Area::has_battles()
{
	if (hooks) {
		return hooks->has_battles();
	}
	else {
		return false;
	}
}

void Area::battle_ended(Battle_Game *battle)
{
	if (hooks) {
		hooks->battle_ended(battle);
	}
}

void Area::set_overlay_colour(SDL_Colour colour)
{
	overlay_colour = colour;
}

void Area::set_entities_standing()
{
	// change walking sprites to standing so they're not walking in place on fade and not starting in walking animation on level reload
	Map_Entity_List::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		Map_Entity *entity = *it;
		gfx::Sprite *sprite = entity->get_sprite();
		if (sprite != NULL) {
			if (sprite->get_animation().substr(0, 4) == "walk") {
				entity->set_direction(entity->get_direction(), true, false);
				if (entity->get_do_tilt()) {
					entity->set_angle(0.0f);
				}
			}
		}
	}
}

void Area::handle_event(TGUI_Event *event)
{
	if (hooks) {
		hooks->handle_event(event);
	}
}

void Area::order_player_input_steps()
{
	// Players have to be in order to avoid jittery walk
	// This function should be called before pretty much everything else
	std::vector<Map_Entity *> players = AREA->get_players();
	// put all the Input_Steps at the back...
	for (size_t i = 0; i < players.size(); i++) {
		std::list<Task *> &tasks = entity_movement_system->get_tasks();
		for (Task_List::iterator it = tasks.begin(); it != tasks.end(); it++) {
			Task *task = *it;
			Step *step = *task->get_steps().begin();
			Map_Entity_Input_Step *meis = players[i]->get_input_step();
			if (step == meis) {
				tasks.erase(it);
				tasks.push_back(task);
				break;
			}
		}
	}
	// Followed by all the Movement_Steps
	for (size_t i = 0; i < players.size(); i++) {
		std::list<Task *> &tasks = entity_movement_system->get_tasks();
		for (Task_List::iterator it = tasks.begin(); it != tasks.end(); it++) {
			Task *task = *it;
			Step *step = *task->get_steps().begin();
			Map_Entity_Input_Step *meis = players[i]->get_input_step();
			if (meis != NULL && step == meis->get_movement_step()) {
				tasks.erase(it);
				tasks.push_back(task);
				break;
			}
		}
	}
}

void Area::add_entity(Map_Entity *entity)
{
	entities.push_back(entity);
	for (size_t i = 0; i < INSTANCE->stats.size(); i++) {
		if (INSTANCE->stats[i].base.get_name() == entity->get_name()) {
			order_player_input_steps();
			break;
		}
	}
}

std::vector<util::A_Star::Way_Point> Area::get_way_points(util::Point<int> from)
{
	if (hooks) {
		return hooks->get_way_points(from);
	}
	std::vector<util::A_Star::Way_Point> v;
	return v; // nothing
}

bool Area::is_corner_portal(util::Point<int> pos)
{
	if (hooks) {
		return hooks->is_corner_portal(pos);
	}
	return false;
}

bool Area::is_new_game()
{
	return new_game;
}

bool Area::was_loaded()
{
	return loaded;
}

int Area::get_middle_layer()
{
	int nlayers = tilemap->get_num_layers();
	return (nlayers-1)/2;
}

bool Area::started_and_moved()
{
	if (hooks) {
		return hooks->started_and_moved();
	}

	return false;
}

std::vector<Area_Hooks::Slope> Area::get_slopes()
{
	if (hooks) {
		return hooks->get_slopes();
	}
	else {
		std::vector<Area_Hooks::Slope> s;
		return s;
	}
}

void Area::dont_save(std::string name)
{
	dont_save_entities.push_back(name);
}

void Area::set_water(std::vector< util::Rectangle<int> > water)
{
	this->water = water;
}

std::vector< util::Rectangle<int> > &Area::get_water()
{
	return water;
}

float Area::get_water_height()
{
	return water_height;
}

void Area::set_water_height(float h)
{
	water_height = h;
}

void Area::start_water_ripple(util::Point<int> pos)
{
	if (std::find(no_ripple_tiles.begin(), no_ripple_tiles.end(), pos) != no_ripple_tiles.end()) {
		return;
	}

	bool is_water = false;
	for (auto &r : water) {
		if (r.contains(pos)) {
			is_water = true;
			break;
		}
	}
	if (is_water == false) {
		return;
	}
	for (auto it = water_ripples.begin(); it != water_ripples.end(); it++) {
		auto p = *it;
		if (p.second == pos) {
			delete p.first;
			water_ripples.erase(it);
			break;
		}
	}
	gfx::Sprite *sprite = new gfx::Sprite("water_ripple");
	sprite->set_animation("only", end_water_ripple_callback, sprite);
	std::pair< gfx::Sprite *, util::Point<int> > p(sprite, pos);
	water_ripples.push_back(p);
}

void Area::end_water_ripple(gfx::Sprite *sprite)
{
	for (auto it = water_ripples.begin(); it != water_ripples.end(); it++) {
		auto p = *it;
		if (p.first == sprite) {
			water_ripples.erase(it);
			delete sprite;
			break;
		}
	}
}

void Area::draw_water_ripples(util::Point<float> map_offset)
{
	for (auto &p : water_ripples) {
		p.first->get_current_image()->draw(map_offset + p.second * shim::tile_size + util::Point<int>(0, shim::tile_size/4));
	}
}

std::vector< std::pair< gfx::Sprite *, util::Point<int> > > &Area::get_water_ripples()
{
	return water_ripples;
}

void Area::set_no_ripple_tiles(std::vector< util::Point<int> > nrt)
{
	no_ripple_tiles = nrt;
}

//--

Area_Hooks::Area_Hooks(Area *area) :
	area(area),
	ignore_next_on_tile(false),
	rand_battle_last(-1),
	done_started_and_moved(false)
{
}

Area_Hooks::~Area_Hooks()
{
}

bool Area_Hooks::start(bool new_game, bool loaded, std::string save)
{
	return true;
}

void Area_Hooks::started()
{
	if (are_presses_paused() == false) {
		Map_Entity *player1 = AREA->get_player(0);
		util::Point<int> pos = player1->get_position();
		util::Size<int> tilemap_size = area->get_tilemap()->get_size();
		auto pix_size = tilemap_size * shim::tile_size;
		bool go = false;
		if (pix_size.w+shim::tile_size*2 < shim::screen_size.w || pix_size.h+shim::tile_size*2 < shim::screen_size.h) {
			go = false;
		}
		else {
			if (pos.x == 0) {
				go = true;
				pos.x++;
			}
			else if (pos.y == 0) {
				go = true;
				pos.y++;
			}
			else if (pos.x == tilemap_size.w-1) {
				go = true;
				pos.x--;
			}
			else if (pos.y == tilemap_size.h-1) {
				go = true;
				pos.y--;
			}
		}
		if (go) {
			wedge::pause_presses(true);

			player1->get_input_step()->set_no_battles_next_step(true);

			NEW_SYSTEM_AND_TASK(AREA)
			ADD_STEP(new A_Star_Step(player1, pos, new_task))
			ADD_TASK(new_task)
			FINISH_SYSTEM(AREA)
		}
		else {
			done_started_and_moved = true;
		}
	}
	else {
		done_started_and_moved = true;
	}
}

void Area_Hooks::end()
{
}

bool Area_Hooks::on_tile(Map_Entity *entity)
{
	if (entity != AREA->get_player(0)) {
		return false;
	}

	util::Point<int> pos = entity->get_position();

	std::vector<Scroll_Zone *> zones = get_scroll_zones(pos);
	Scroll_Zone *activated = NULL;
	// Remove any player_start_scroll_zones not stepped on anymore (eg if scrolled in on a corner then moved)
	for (std::vector<Scroll_Zone *>::iterator it = player_start_scroll_zones.begin(); it != player_start_scroll_zones.end();) {
		Scroll_Zone *z = *it;
		if (std::find(zones.begin(), zones.end(), z) == zones.end()) {
			it = player_start_scroll_zones.erase(it);
		}
		else {
			it++;
		}
	}
	for (size_t i = 0; i < zones.size(); i++) {
		Scroll_Zone *z = zones[i];
		// if player is on initial scroll zone and not facing scroll zone direction, don't scroll
		if (!(z && std::find(player_start_scroll_zones.begin(), player_start_scroll_zones.end(), z) != player_start_scroll_zones.end() && z->direction != entity->get_direction())) {
			activated = z;
			break;
		}
	}
	
	Fade_Zone *z2 = get_fade_zone(pos);

	util::Point<int> path_goal = entity->get_input_step()->get_path_goal();

	if (path_goal.x >= 0 && path_goal != pos && get_fade_zone(path_goal) != z2 && is_corner_portal(pos)) {
		z2 = NULL; // don't change areas here if it's just a node on a different path
	}
	
	if (z2 == NULL || z2 != player_start_fade_zone) {
		player_start_fade_zone = NULL;
	}

	if (zones.size() > 0 && activated == NULL && z2 == NULL) {
		return false;
	}

	player_start_scroll_zones.clear();
	
	if (z2 && z2 == player_start_fade_zone && activated == NULL) {
		return false;
	}

	if (activated != NULL) {
		area->set_next_area(activated->area_name, util::Point<int>(pos.x-activated->zone.pos.x+activated->topleft_dest.x, pos.y-activated->zone.pos.y+activated->topleft_dest.y), activated->direction);
		return true;
	}

	if (z2) {
		// Redrawing here avoids a little jerk as you step on a tile leading to another area
		if (AREA->get_pause_entity_movement_on_next_area_change()) {
			Map_Entity *player0 = AREA->get_player(0);
			Map_Entity_List &entities = area->get_entities();
			for (Map_Entity_List::iterator it = entities.begin(); it != entities.end(); it++) {
				Map_Entity *e = *it;
				if (e == player0) {
					continue;
				}
				Map_Entity_Input_Step *meis = e->get_input_step();
				if (meis) {
					Tile_Movement_Step *tms = meis->get_movement_step();
					if (tms) {
						tms->die();
					}
					meis->die();
					e->set_input_step(NULL); // So die() isn't called again in destructor
				}
			}
			area->set_entities_standing();
		}
		shim::user_render();
		area->set_next_area(z2->area_name, z2->player_positions, z2->directions);
		return true;
	}

	return false;
}

bool Area_Hooks::try_tile(Map_Entity *entity, util::Point<int> tile_pos)
{
	auto pos = entity->get_position();

	Direction d;

	if (pos.x < tile_pos.x) {
		d = DIR_E;
	}
	else if (pos.x > tile_pos.x) {
		d = DIR_W;
	}
	else {
		d = DIR_NONE;
	}

	if (d != DIR_NONE) {
		for (auto slope : slopes) {
			if (slope.position == tile_pos && slope.direction == d) {
				entity->set_on_slope(true);
				entity->set_direction(d, true, true);
				entity->set_position(tile_pos+(slope.up ? util::Point<float>(0.0f, -1.0f) : util::Point<float>(0.0f, 1.0f)));
				util::Point<float> o = d == DIR_E ? util::Point<float>(-1.0f, 0.0f) : util::Point<float>(1.0f, 0.0f);
				o += slope.up ? util::Point<float>(0.0f, 1.0f) : util::Point<float>(0.0f, -1.0f);
				entity->set_offset(o);
				entity->set_moving(true);
				auto input_step = entity->get_input_step();
				auto movement_step = input_step->get_movement_step();
				if (movement_step->is_waiting_for_next_direction() == false) {
					movement_step->set_moving(true);
				}
				util::Point<float> inc = d == DIR_E ? util::Point<float>(1.0f, 0.0f) : util::Point<float>(-1.0f, 0.0f);
				inc += slope.up ? util::Point<float>(0.0f, -1.0f) : util::Point<float>(0.0f, 1.0f);
				movement_step->set_increment(inc);
				auto players = AREA->get_players();
				for (size_t i = 0; i < players.size(); i++) {
					if (players[i] == entity && i < players.size()-1) {
						auto p = players[i+1];
						auto pos2 = p->get_position();
						Direction d2;
						if (pos2.x < pos.x) {
							d2 = DIR_E;
						}
						else if (pos2.x > pos.x) {
							d2 = DIR_W;
						}
						else if (pos2.y < pos.y) {
							d2 = DIR_S;
						}
						else {
							d2 = DIR_N;
						}
						util::Point<int> pos_x_only = { pos.x, pos2.y };
						bool found = false;
						Slope s;
						for (auto slope2 : slopes) {
							if (slope2.position == pos_x_only && slope2.direction == d2) {
								s = slope2;
								found = true;
								break;
							}
						}
						auto p2_input_step = players[i+1]->get_input_step();
						if (found) {
							if (p2_input_step->get_movement_step()->is_moving()) {
								p2_input_step->get_movement_step()->set_try_tile(pos_x_only);
								//p2_input_step->get_movement_step()->delay_movement(i+1);
							}
							else {
								try_tile(p, pos_x_only);
							}
						}
						else {
							if (p2_input_step->is_following_path()) {
								p2_input_step->add_to_path(pos);
							}
							else {
								p2_input_step->set_path(pos, false, false, true);
								p2_input_step->get_movement_step()->delay_movement(int(i+1));
							}
						}
						break;
					}
				}
				return false;
			}
		}
	}

	return false;
}

bool Area_Hooks::activate(Map_Entity *activator, Map_Entity *activated)
{
	return false;
}

bool Area_Hooks::activate_with(Map_Entity *activator)
{
	return false;
}

void Area_Hooks::pre_draw(int layer, util::Point<float> map_offset)
{
}

void Area_Hooks::post_draw(int layer, util::Point<float> map_offset)
{
}

std::vector<int> Area_Hooks::get_pre_draw_layers()
{
	std::vector<int> v;
	return v;
}

std::vector<int> Area_Hooks::get_post_draw_layers()
{
	std::vector<int> v;
	return v;
}

void Area_Hooks::remove_entity(Map_Entity *entity)
{
}

void Area_Hooks::lost_device()
{
}

void Area_Hooks::found_device()
{
}

std::string Area_Hooks::save()
{
	return "";
}

bool Area_Hooks::has_battles()
{
	return false;
}

Battle_Game *Area_Hooks::get_random_battle()
{
	return NULL;
}

bool Area_Hooks::can_save()
{
	return false;
}

void Area_Hooks::dialogue_done(Map_Entity *entity)
{
}

std::vector<Area_Hooks::Scroll_Zone *> Area_Hooks::get_scroll_zones(util::Point<int> pos)
{
	std::vector<Scroll_Zone *> zones;

	for (size_t i = 0; i < scroll_zones.size(); i++) {
		Scroll_Zone &z = scroll_zones[i];

		if (pos.x >= z.zone.pos.x && pos.x < z.zone.pos.x+z.zone.size.w && pos.y >= z.zone.pos.y && pos.y < z.zone.pos.y+z.zone.size.h) {
			zones.push_back(&z);
		}
	}

	return zones;
}

Area_Hooks::Fade_Zone *Area_Hooks::get_fade_zone(util::Point<int> pos)
{
	for (size_t i = 0; i < fade_zones.size(); i++) {
		Fade_Zone &z = fade_zones[i];

		if (pos.x >= z.zone.pos.x && pos.x < z.zone.pos.x+z.zone.size.w && pos.y >= z.zone.pos.y && pos.y < z.zone.pos.y+z.zone.size.h) {
			return &z;
		}
	}

	return NULL;
}

void Area_Hooks::insert_rand_battle_type(int type)
{
	int start = (type == rand_battle_last ? (rand_battle_table[0] == type ? 2 : 0) : 0);
	int pos = util::rand(start, (int)rand_battle_table.size());
	rand_battle_table.insert(rand_battle_table.begin() + pos, type);
}

void Area_Hooks::gen_rand_battle_table(int num_types)
{
	rand_battle_table.clear();

	for (int i = 0; i < num_types; i++) {
		if (i != rand_battle_last) {
			gen_rand_battle_type(i);
		}
	}

	if (rand_battle_last >= 0) {
		gen_rand_battle_type(rand_battle_last);
	}

	rand_battle_last = rand_battle_table.back();
}

void Area_Hooks::gen_rand_battle_type(int type)
{
	int n = util::rand(1, 2);
	for (int j = 0; j < n; j++) {
		insert_rand_battle_type(type);
	}
}

void Area_Hooks::set_animated_tiles()
{
}

void Area_Hooks::run()
{
}

void Area_Hooks::resize(util::Size<int> new_size)
{
}

void Area_Hooks::handle_event(TGUI_Event *event)
{
}

void Area_Hooks::set_player_start_zones()
{
	Map_Entity *player = AREA->get_player(0);
	util::Point<int> pos = player->get_position();
	player_start_scroll_zones = get_scroll_zones(pos);
	player_start_fade_zone = get_fade_zone(pos);
}

void Area_Hooks::battle_ended(Battle_Game *battle)
{
}

std::vector<util::A_Star::Way_Point> Area_Hooks::get_way_points(util::Point<int> from)
{
	// Automatically handle slopes

	std::vector<std::vector<util::Point<int>>> slope_groups;

	std::vector<util::Point<int>> slope_pts;

	for (auto s : slopes) {
		bool found = false;
		for (auto pt : slope_pts) {
			if (pt == s.position) {
				found = true;
				break;
			}
		}
		if (found == false && s.up) {
			slope_pts.push_back(s.position);
		}
	}

	auto slope_pts_bak = slope_pts;

	while (slope_pts.size() > 0) {
		auto pt = slope_pts.back();
		slope_pts.pop_back();
		std::vector<util::Point<int>> v;
		v.push_back(pt);
		for (auto it = slope_pts.begin(); it != slope_pts.end();) {
			auto pt2 = *it;
			float dx = std::abs(pt.x - pt2.x);
			float dy = std::abs(pt.y - pt2.y);
			if (dx == dy) {
				it = slope_pts.erase(it);
				v.push_back(pt2);
			}
			else {
				it++;
			}
		}
		slope_groups.push_back(v);
	}

	bool all_1 = true;
	for (auto &v : slope_groups) {
		if (v.size() != 1) {
			all_1 = false;
			break;
		}
	}
	
	std::vector<util::A_Star::Way_Point> v;
	auto tilemap = area->get_tilemap();
	auto tm_sz = tilemap->get_size();

	// Special case if all slopes are 1 tile (e.g., in/out of water slopes)
	if (all_1) {
		auto w = area->get_water();
		bool in_water = false;
		for (auto &r : w) {
			if (r.contains(from)) {
				in_water = true;
				break;
			}
		}
		for (int y = 0; y < tm_sz.h; y++) {
			for (int x = 0; x < tm_sz.w; x++) {
				if (x == from.x) {
					continue;
				}

				bool in_water2 = false;
				for (auto &r : w) {
					if (r.contains({x, y})) {
						in_water2 = true;
						break;
					}
				}

				if (in_water == in_water2) {
					continue;
				}

				for (int xx = from.x; xx >= x; xx--) {
					util::A_Star::Way_Point wp;
					for (auto &s : slopes) {
						if (in_water) {
							if (s.position.x == xx && s.up == true) {
								wp.to.push_back({x, y});
								if (s.position.x+1 != from.x) {
									wp.by.push_back(s.position+util::Point<int>(1, 0));
									wp.by_as_is.push_back(false);
								}
								wp.by.push_back(s.position);
								wp.by_as_is.push_back(true);
								//wp.by.push_back(s.position+util::Point<int>(0, -1));
								//wp.by_as_is.push_back(true);
								if (s.position.x == x && s.position.y-1 == y) {
									// nuthin
								}
								else if (x < s.position.x) {
									wp.by.push_back(s.position+util::Point<int>(-1, -1));
									wp.by_as_is.push_back(true);
								}
								wp.by.push_back({x, y});
								wp.by_as_is.push_back(false);
								wp.only_by = true;
								v.push_back(wp);
							}
						}
						else {
							if (s.position.x == xx && s.up == false) {
								wp.to.push_back({x, y});
								if (s.position.x+1 != from.x) {
									wp.by.push_back(s.position+util::Point<int>(1, 0));
									wp.by_as_is.push_back(false);
								}
								wp.by.push_back(s.position);
								wp.by_as_is.push_back(true);
								//wp.by.push_back(s.position+util::Point<int>(0, 1));
								//wp.by_as_is.push_back(true);
								if (s.position.x == x && s.position.y+1 == y) {
									// nuthin
								}
								else if (x < s.position.x) {
									wp.by.push_back(s.position+util::Point<int>(-1, 1));
									wp.by_as_is.push_back(true);
								}
								else if (y > s.position.y+1) {
									wp.by.push_back(s.position+util::Point<int>(0, 2));
									wp.by_as_is.push_back(true);
								}
								wp.by.push_back({x, y});
								wp.by_as_is.push_back(false);
								wp.only_by = true;
								v.push_back(wp);
							}
						}
					}
					if (wp.by.size() > 0) {
						util::Point<int> tmp = wp.by.back();
						if (tmp.x != x || tmp.y != y) {
							wp.by.push_back({x, y});
							wp.by_as_is.push_back(false);
						}
					}
				}

				for (int xx = from.x; xx <= x; xx++) {
					util::A_Star::Way_Point wp;
					for (auto &s : slopes) {
						if (in_water) {
							if (s.position.x == xx && s.up == true) {
								wp.to.push_back({x, y});
								if (s.position.x-1 != from.x) {
									wp.by.push_back(s.position+util::Point<int>(-1, 0));
									wp.by_as_is.push_back(false);
								}
								wp.by.push_back(s.position);
								wp.by_as_is.push_back(true);
								//wp.by.push_back(s.position+util::Point<int>(0, -1));
								//wp.by_as_is.push_back(true);
								if (s.position.x == x && s.position.y-1 == y) {
									// nuthin
								}
								else if (x > s.position.x) {
									wp.by.push_back(s.position+util::Point<int>(1, -1));
									wp.by_as_is.push_back(true);
								}
								wp.by.push_back({x, y});
								wp.by_as_is.push_back(false);
								wp.only_by = true;
								v.push_back(wp);
							}
						}
						else {
							if (s.position.x == xx && s.up == false) {
								wp.to.push_back({x, y});
								if (s.position.x-1 != from.x) {
									wp.by.push_back(s.position+util::Point<int>(-1, 0));
									wp.by_as_is.push_back(false);
								}
								wp.by.push_back(s.position);
								wp.by_as_is.push_back(true);
								//wp.by.push_back(s.position+util::Point<int>(0, 1));
								//wp.by_as_is.push_back(true);
								if (s.position.x == x && s.position.y+1 == y) {
									// nuthin
								}
								else if (x > s.position.x) {
									wp.by.push_back(s.position+util::Point<int>(1, 1));
									wp.by_as_is.push_back(true);
								}
								else if (y > s.position.y+1) {
									wp.by.push_back(s.position+util::Point<int>(0, 2));
									wp.by_as_is.push_back(true);
								}
								wp.by.push_back({x, y});
								wp.by_as_is.push_back(false);
								wp.only_by = true;
								v.push_back(wp);
							}
						}
					}
					if (wp.by.size() > 0) {
						util::Point<int> tmp = wp.by.back();
						if (tmp.x != x || tmp.y != y) {
							wp.by.push_back({x, y});
							wp.by_as_is.push_back(false);
						}
					}
				}
			}
		}

		return v;
	}

	// sort from bottom to top
	for (auto &group : slope_groups) {
		for (size_t i = 0; i < group.size(); i++) {
			int miny = INT_MAX;
			int index = 0;
			for (size_t j = i; j < group.size(); j++) {
				if (group[j].y < miny) {
					miny = group[j].y;
					index = (int)j;
				}
			}
			auto pt = group[index];
			group.erase(group.begin() + index);
			group.insert(group.begin(), pt);
		}
	}
	
	for (int y = 0; y < tm_sz.h; y++) {
		for (int x = 0; x < tm_sz.w; x++) {
			if (x == from.x) {
				continue;
			}

			// Need to pick the points we're going through
			std::vector<int> active_groups;
			for (int xx = from.x; xx >= x; xx--) {
				for (size_t i = 0; i < slope_groups.size(); i++) {
					for (auto pt : slope_groups[i]) {
						if (pt.x == xx) {
							active_groups.push_back((int)i);
							break;
						}
					}
				}
			}
			for (int xx = from.x; xx <= x; xx++) {
				for (size_t i = 0; i < slope_groups.size(); i++) {
					for (auto pt : slope_groups[i]) {
						if (pt.x == xx) {
							active_groups.push_back((int)i);
							break;
						}
					}
				}
			}

			std::sort(active_groups.begin(), active_groups.end());
			auto it = std::unique(active_groups.begin(), active_groups.end());
			active_groups.resize(std::distance(active_groups.begin(), it));

			util::Point<int> p = { x, y };
			bool found = false;
			for (auto i : active_groups) {
				for (auto pt : slope_groups[i]) {
					if (pt == p) {
						found = true;
						break;
					}
				}
				if (found) {
					break;
				}
			}
			if (found == false) {
				if (tilemap->is_solid(-1, {x, y})) {
					continue;
				}
			}

			std::vector<int> slanting_right;
			std::vector<int> slanting_left;
			for (auto i : active_groups) {
				if (slope_groups[i].size() > 1) {
					if (slope_groups[i][0].x < slope_groups[i][1].x) {
						slanting_right.push_back(i);
					}
					else {
						slanting_left.push_back(i);
					}
				}
				else {
					auto pt = slope_groups[i][0];
					for (auto s : slopes) {
						if (s.position == pt) {
							if (s.right) {
								slanting_right.push_back(i);
							}
							else {
								slanting_left.push_back(i);
							}
						}
					}
				}
			}

			util::A_Star::Way_Point wp;
			wp.to.push_back({x, y});
			for (int xx = from.x-1; xx >= x; xx--) {
				std::vector<int> picked_right;
				std::vector<int> picked_left;
				for (auto i : slanting_right) {
					bool found = false;
					for (auto pt : slope_groups[i]) {
						util::Point<int> above = { pt.x, pt.y-1 };
						if (from == above) {
							found = true;
							picked_right.push_back(i);
							break;
						}
					}
					if (found) {
						continue;
					}
					if (slope_groups[i].back().y == from.y+1) {
						picked_right.push_back(i);
					}
				}
				for (auto i : slanting_left) {
					bool found = false;
					for (auto pt : slope_groups[i]) {
						util::Point<int> above = { pt.x, pt.y-1 };
						if (from == above) {
							found = true;
							picked_left.push_back(i);
							break;
						}
					}
					if (found) {
						continue;
					}
					if (slope_groups[i][0].y == from.y) {
						picked_left.push_back(i);
					}
				}

				for (auto i : picked_right) {
					int bottom_x = slope_groups[i][0].x;
					int bottom_y = slope_groups[i][0].y;
					if (xx == bottom_x-1) {
						wp.by.push_back({bottom_x-1, bottom_y-1});
						wp.by_as_is.push_back(true);
						if (util::Point<int>(x, y) != util::Point<int>(bottom_x-1, bottom_y)) {
							wp.by.push_back({bottom_x-2, bottom_y});
							wp.by_as_is.push_back(true);
						}
					}
					auto back = slope_groups[i].back();
					if (xx == back.x) {
						wp.by.push_back({back.x, back.y-1});
						wp.by_as_is.push_back(false);
					}
					for (size_t j = 0; j < slope_groups[i].size()-1; j++) {
						int gx = slope_groups[i][j].x;
						int gy = slope_groups[i][j].y;
						if (xx == gx) {
							if (from.x != gx+1 && std::abs(from.x-gx) != std::abs(from.y-gy)-1) {
								if (wp.by.size() > 0) {
									util::Point<int> tmp = wp.by.back();
									if (tmp.x != gx+1 || tmp.y != gy-2) {
										wp.by.push_back({gx+1, gy-2});
										wp.by_as_is.push_back(false);
									}
								}
								else {
									wp.by.push_back({gx+1, gy-2});
									wp.by_as_is.push_back(false);
								}
							}
							wp.by.push_back({gx, gy-2});
							wp.by_as_is.push_back(true);
						}
					}
				}
				for (auto i : picked_left) {
					int bottom_x = slope_groups[i][0].x;
					int bottom_y = slope_groups[i][0].y;
					if (xx == bottom_x) {
						wp.by.push_back({bottom_x, bottom_y});
						wp.by_as_is.push_back(false);
					}
					for (size_t j = 1; j < slope_groups[i].size()-1; j++) {
						auto pt = slope_groups[i][j];
						if (pt.x == xx) {
							wp.by.push_back({pt.x, pt.y});
							wp.by_as_is.push_back(true);
						}
					}
					auto back = slope_groups[i].back();
					if (xx == back.x) {
						if (x != back.x) {
							util::Point<int> add = { back.x, back.y-1 };
							if (wp.by.size() == 0) {
								wp.by.push_back(add);
								wp.by_as_is.push_back(true);
							}
							else {
								auto pt2 = wp.by.back();
								if (pt2 != add) {
									wp.by.push_back(add);
									wp.by_as_is.push_back(true);
								}
							}
						}
						else {
							wp.by.push_back({back.x, back.y});
							wp.by_as_is.push_back(true);
						}
					}
				}
			}
			for (int xx = from.x+1; xx <= x; xx++) {
				std::vector<int> picked_right;
				std::vector<int> picked_left;
				for (auto i : slanting_right) {
					bool found = false;
					for (auto pt : slope_groups[i]) {
						util::Point<int> above = { pt.x, pt.y-1 };
						if (from == above) {
							found = true;
							picked_right.push_back(i);
							break;
						}
					}
					if (found) {
						continue;
					}
					if (slope_groups[i][0].y == from.y) {
						picked_right.push_back(i);
					}
				}
				for (auto i : slanting_left) {
					bool found = false;
					for (auto pt : slope_groups[i]) {
						util::Point<int> above = { pt.x, pt.y-1 };
						if (from == above) {
							found = true;
							picked_left.push_back(i);
							break;
						}
					}
					if (found) {
						continue;
					}
					if (slope_groups[i].back().y == from.y+1) {
						picked_left.push_back(i);
					}
				}

				for (auto i : picked_right) {
					int bottom_x = slope_groups[i][0].x;
					int bottom_y = slope_groups[i][0].y;
					if (xx == bottom_x) {
						wp.by.push_back({bottom_x, bottom_y});
						wp.by_as_is.push_back(false);
					}
					for (size_t j = 1; j < slope_groups[i].size()-1; j++) {
						auto pt = slope_groups[i][j];
						if (pt.x == xx) {
							wp.by.push_back({pt.x, pt.y});
							wp.by_as_is.push_back(true);
						}
					}
					auto back = slope_groups[i].back();
					if (xx == back.x) {
						if (x != back.x) {
							util::Point<int> add = { back.x, back.y-1 };
							if (wp.by.size() == 0) {
								wp.by.push_back(add);
								wp.by_as_is.push_back(true);
							}
							else {
								auto pt2 = wp.by.back();
								if (pt2 != add) {
									wp.by.push_back(add);
									wp.by_as_is.push_back(true);
								}
							}
						}
						else {
							wp.by.push_back({back.x, back.y});
							wp.by_as_is.push_back(true);
						}
					}
				}
				for (auto i : picked_left) {
					int bottom_x = slope_groups[i][0].x;
					int bottom_y = slope_groups[i][0].y;
					if (xx == bottom_x+1) {
						wp.by.push_back({bottom_x+1, bottom_y-1});
						wp.by_as_is.push_back(true);
						if (util::Point<int>(x, y) != util::Point<int>(bottom_x+1, bottom_y)) {
							wp.by.push_back({bottom_x+2, bottom_y});
							wp.by_as_is.push_back(true);
						}
					}
					auto back = slope_groups[i].back();
					if (xx == back.x) {
						wp.by.push_back({back.x, back.y-1});
						wp.by_as_is.push_back(false);
					}
					for (size_t j = 0; j < slope_groups[i].size()-1; j++) {
						int gx = slope_groups[i][j].x;
						int gy = slope_groups[i][j].y;
						if (xx == gx) {
							if (from.x != gx-1 && std::abs(from.x-gx) != std::abs(from.y-gy)-1) {
								if (wp.by.size() > 0) {
									util::Point<int> tmp = wp.by.back();
									if (tmp.x != gx-1 || tmp.y != gy-2) {
										wp.by.push_back({gx-1, gy-2});
										wp.by_as_is.push_back(false);
									}
								}
								else {
									wp.by.push_back({gx-1, gy-2});
									wp.by_as_is.push_back(false);
								}
							}
							wp.by.push_back({gx, gy-2});
							wp.by_as_is.push_back(true);
						}
					}
				}
			}
			found = false;
			for (auto pt : slope_pts_bak) {
				if (util::Point<int>(x, y) == pt) {
					found = true;
					break;
				}
			}
			if (found == false) {
				if (wp.by.size() == 0) {
					wp.by.push_back({x, y});
					wp.by_as_is.push_back(false);
				}
				else {
					util::Point<int> tmp = wp.by.back();
					if (tmp.x != x || tmp.y != y) {
						wp.by.push_back({x, y});
						wp.by_as_is.push_back(false);
					}
				}
			}
			wp.only_by = true;
			v.push_back(wp);
		}
	}

	return v;
}

bool Area_Hooks::is_corner_portal(util::Point<int> pos)
{
	return false;
}

bool Area_Hooks::started_and_moved()
{
	return false;
}

void Area_Hooks::set_done_started_and_moved(bool done_started_and_moved)
{
	this->done_started_and_moved = done_started_and_moved;
}

bool Area_Hooks::get_done_started_and_moved()
{
	return done_started_and_moved;
}

std::vector<Area_Hooks::Slope> Area_Hooks::get_slopes()
{
	return slopes;
}

void Area_Hooks::maybe_autosave()
{
}

void Area_Hooks::pause(bool onoff)
{
}

}
