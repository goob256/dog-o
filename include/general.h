#ifndef GENERAL_H
#define GENERAL_H

#include <wedge2/globals.h>
#include <wedge2/battle_entity.h>

#include "combo.h"
#include "dog-o.h"

class Dialogue_Step;

std::vector<Dialogue_Step *> active_dialogues(wedge::Game *game);

SDL_Colour make_translucent(SDL_Colour colour, float alpha);

bool save_settings();
bool load_settings();

std::string save_dir();

util::JSON *load_savegame(int slot, std::string prefix = "save");
std::string save_filename(int slot, std::string prefix = "save");

std::string play_time_to_string(int time);

#ifdef TVOS
void save_autosave_index();
#endif
void autosave(bool wait);

void start_autosave_thread();
void end_autosave_thread();

bool can_autosave();

bool can_show_settings(bool check_events = false, bool can_be_moving = false, bool only_initialised_dialogues_block = false, bool allow_paused_presses_if_changing_areas = false);

void get_save_info(int number, bool &__exists, bool &_corrupt, std::string &_time_text, std::string &_place_text, int &difficulty, std::string prefix = "save");

bool save();

SDL_Colour selected_colour(SDL_Colour colour1, SDL_Colour colour2);

void delete_shim_args();
void set_shim_args(bool initial, bool force_windowed, bool force_fullscreen);

SDL_Colour brighten(SDL_Colour colour, float amount);

std::string get_key_name(int code);

void show_notice(std::string text, bool flip = false);

void apply_tv_safe_mode(bool onoff);

Combo get_combo(std::string name);
audio::Sound *get_combo_sfx(std::string name);
bool get_combo_multi(std::string name);
int get_combo_cost(std::string name);
float get_combo_multiplier(std::string name);
bool get_combo_friendly(std::string name);

void get_use_item_info(int amount, int id, SDL_Colour &colour, SDL_Colour &shadow_colour, std::string &text);

bool settings_active();

void draw_tinted_shadowed_image(SDL_Colour tint, SDL_Colour shadow_colour, gfx::Image *image, util::Point<float> dest_pos, gfx::Font::Shadow_Type shadow_type);

void get_gauge_colour(float p, SDL_Colour& colour1, SDL_Colour& colour2);

SDL_Colour blend_colours(SDL_Colour a, SDL_Colour b, float p);

std::vector<std::string> get_belt_moves(std::string player_name, int belt);

bool for_use_in_battle(int id);

// returns true if dodged
bool add_special_number(wedge::Battle_Entity *actor, wedge::Battle_Entity *target, int damage, bool lucky_misses, bool play_sounds);

void real_draw_darkness(gfx::Image *darkness_image1, gfx::Image *darkness_image2, util::Point<float> darkness_offset1, util::Point<float> darkness_offset2, util::Point<float> map_offset, float alpha);

void do_question(std::string tag, std::string text, wedge::Dialogue_Type type, std::vector<std::string> choices, wedge::Step *monitor, int escape_choice = -1);

bool save_play_time();

void do_combo_screen_shake(std::string name);

// Difficulty level stuff
float enemy_speed_mul();
float enemy_attack_mul();
float enemy_defence_mul();
float combo_timing_mul();

void pop_player();

int get_profile_pic_index(int player_index);

void remove_belt(int player_index, std::vector<std::string> moves);

void cancel_all_screen_shake(wedge::Game *game);
void cancel_all_special_numbers(wedge::Game *game);

#ifdef ANDROID
void exit_android();
#endif

#endif // GENERAL_H
