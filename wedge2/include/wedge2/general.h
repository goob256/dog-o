#ifndef WEDGE2_GENERAL_H
#define WEDGE2_GENERAL_H

#include "wedge2/main.h"
#include "wedge2/globals.h"
#include "wedge2/stats.h"

namespace wedge {

util::Point<int> WEDGE2_EXPORT add_direction(util::Point<int> pos, Direction direction, int n);
Direction WEDGE2_EXPORT direction_from_offset(util::Point<int> offset);

std::string WEDGE2_EXPORT integer_point_to_string(util::Point<int> p);
std::string WEDGE2_EXPORT float_point_to_string(util::Point<float> p);
std::string WEDGE2_EXPORT integer_size_to_string(util::Size<int> s);
std::string WEDGE2_EXPORT float_size_to_string(util::Size<float> s);
std::string WEDGE2_EXPORT direction_to_string(Direction d);
std::string WEDGE2_EXPORT bool_to_string(bool b);
std::string WEDGE2_EXPORT sprite_to_string(gfx::Sprite *sprite);
std::string WEDGE2_EXPORT image_to_string(gfx::Image *image);

util::Point<int> WEDGE2_EXPORT json_to_integer_point(util::JSON::Node *json);
util::Point<float> WEDGE2_EXPORT json_to_float_point(util::JSON::Node *json);
util::Size<int> WEDGE2_EXPORT json_to_integer_size(util::JSON::Node *json);
util::Size<float> WEDGE2_EXPORT json_to_float_size(util::JSON::Node *json);
gfx::Sprite WEDGE2_EXPORT *json_to_sprite(util::JSON::Node *json);
gfx::Image WEDGE2_EXPORT *json_to_image(util::JSON::Node *json);

std::string WEDGE2_EXPORT save();
bool WEDGE2_EXPORT save(std::string s, std::string filename);
bool WEDGE2_EXPORT save(std::string filename);
bool WEDGE2_EXPORT save_play_time(std::string filename);

util::Point<int> WEDGE2_EXPORT get_mouse_position();

void WEDGE2_EXPORT pause_player_input(bool paused);
void WEDGE2_EXPORT pause_presses(bool paused, bool repeat_pressed = false);
bool WEDGE2_EXPORT are_presses_paused();

// add 'tiles' tiles to pos,offset
void WEDGE2_EXPORT add_tiles(util::Point<int> &pos, util::Point<float> &offset, util::Point<float> tiles);
void WEDGE2_EXPORT abs_to_tile(util::Point<float> pos, util::Point<int> &out_tile, util::Point<float> &out_offset);
void WEDGE2_EXPORT tile_to_abs(util::Point<int> pos, util::Point<float> offset, util::Point<float> &out_pos);

void WEDGE2_EXPORT rumble(float force, int milliseconds);

util::JSON WEDGE2_EXPORT *load_savegame(std::string filename);

void WEDGE2_EXPORT quit_all();

int WEDGE2_EXPORT JOYF_TO_I(float v, float orig);

void WEDGE2_EXPORT clip_rect_to_rect(util::Point<float> clip_pos, util::Size<float> clip_sz, util::Point<float> &pos, util::Size<float> &sz);

}

#endif // WEDGE2_GENERAL_H
