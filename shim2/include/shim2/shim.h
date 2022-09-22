#ifndef NOO_SHIM_H
#define NOO_SHIM_H

#include "shim2/main.h"
#include "shim2/a_star.h"

namespace noo {

namespace audio {
	class MML;
}
namespace gfx {
	class CPA;
	class Font;
	class Image;
	class Shader;
}
namespace gui {
	class GUI;
}
namespace util {
	class CPA;
	class Translation;
	class JSON;
}

namespace shim {

// Must be called first thing
bool SHIM2_EXPORT static_start(int sdl_init_flags = 0);
bool SHIM2_EXPORT static_start_all(int sdl_init_flags = 0);

void SHIM2_EXPORT static_end();
void SHIM2_EXPORT static_end_all();

// Call either start and end (if you init subsystems individually) or start_all and end_all

bool SHIM2_EXPORT start();
void SHIM2_EXPORT end();

/* scaled_gfx_* is the size you want to work with. For example you may want
 * a pixelated game of 240x160 pixels, but a bigger window where the pixels
 * are scaled up. gfx_window_* provides the window size (or fullscreen
 * resolution if going fullscreen.) force_integer_scaling allows you to get a size as
 * close to scaled_gfx_* as possible while keeping an integer scaling i.e.,
 * perfectly square pixels at each location.  If only gfx_window_* are -1,
 * we try to get a window size in a multiple of scaled_gfx_* at a big size
 * that fits the screen.  If you pass -1 for scaled_gfx_*, no scaling will
 * be done i.e., you work with gfx_window_* resolution. If all parameters
 * are -1/false, a fullscreen mode at the current desktop resolution is
 * used.
 */
bool SHIM2_EXPORT start_all(int scaled_gfx_w = -1, int scaled_gfx_h = -1, bool force_integer_scaling = false, int gfx_window_w = -1, int gfx_window_h = -1);
void SHIM2_EXPORT end_all();

TGUI_Event SHIM2_EXPORT *handle_event(SDL_Event *sdl_event);
// if this returns false, don't draw until it returns true again
bool SHIM2_EXPORT update();
void SHIM2_EXPORT push_event(TGUI_Event event);
TGUI_Event SHIM2_EXPORT *pop_pushed_event();
//bool SHIM2_EXPORT event_in_queue(TGUI_Event e);

// These are all global to the shim namespace
// graphics
extern SHIM2_EXPORT bool opengl;
extern SHIM2_EXPORT SDL_Colour palette[256];
extern SHIM2_EXPORT int palette_size;
extern SHIM2_EXPORT SDL_Colour black;
extern SHIM2_EXPORT SDL_Colour white;
extern SHIM2_EXPORT SDL_Colour magenta;
extern SHIM2_EXPORT SDL_Colour transparent;
extern SHIM2_EXPORT SDL_Colour interface_bg;
extern SHIM2_EXPORT SDL_Colour interface_highlight;
extern SHIM2_EXPORT SDL_Colour interface_text;
extern SHIM2_EXPORT SDL_Colour interface_text_shadow;
extern SHIM2_EXPORT SDL_Colour interface_edit_fg;
extern SHIM2_EXPORT SDL_Colour interface_edit_bg;
extern SHIM2_EXPORT std::vector<gui::GUI *> guis;
extern SHIM2_EXPORT float scale;
extern SHIM2_EXPORT std::string window_title; // set this first thing to change it (before call to shim::start)
extern SHIM2_EXPORT std::string organisation_name; // set this first thing too
extern SHIM2_EXPORT std::string game_name; // set this first thing too
extern SHIM2_EXPORT gfx::Shader *current_shader;
extern SHIM2_EXPORT gfx::Shader *default_shader;
extern SHIM2_EXPORT gfx::Shader *model_shader;
extern SHIM2_EXPORT gfx::Shader *appear_shader;
extern SHIM2_EXPORT int tile_size;
extern SHIM2_EXPORT util::Size<int> screen_size; // before scaling
extern SHIM2_EXPORT util::Size<int> real_screen_size; // actual window size
extern SHIM2_EXPORT gfx::Font *font;
extern SHIM2_EXPORT bool create_depth_buffer;
extern SHIM2_EXPORT bool create_stencil_buffer;
extern SHIM2_EXPORT util::Size<int> depth_buffer_size;
extern SHIM2_EXPORT util::Point<int> screen_offset; // begin of where game is drawn after black bars
extern SHIM2_EXPORT float black_bar_percent;
extern SHIM2_EXPORT float z_add; // added to all z values for drawing images/primitives
extern void SHIM2_EXPORT (*user_render)();
extern SHIM2_EXPORT int refresh_rate;
extern SHIM2_EXPORT bool hide_window;
extern SHIM2_EXPORT int adapter;
extern SHIM2_EXPORT util::Point<int> cursor_hotspot;
extern SHIM2_EXPORT util::Point<float> screen_shake_save;
extern SHIM2_EXPORT bool using_screen_shake;
#ifdef _WIN32
extern SHIM2_EXPORT IDirect3DDevice9 *d3d_device;
#endif
// audio
extern SHIM2_EXPORT audio::MML *music;
extern SHIM2_EXPORT audio::MML *widget_mml;
extern SHIM2_EXPORT float music_volume;
extern SHIM2_EXPORT float sfx_volume;
extern SHIM2_EXPORT int samplerate;
// input
extern SHIM2_EXPORT bool convert_xbox_dpad_to_arrows;
extern SHIM2_EXPORT int xbox_l;
extern SHIM2_EXPORT int xbox_r;
extern SHIM2_EXPORT int xbox_u;
extern SHIM2_EXPORT int xbox_d;
extern SHIM2_EXPORT int key_l;
extern SHIM2_EXPORT int key_r;
extern SHIM2_EXPORT int key_u;
extern SHIM2_EXPORT int key_d;
extern SHIM2_EXPORT int fullscreen_key; // key toggles fullscreen window if set
extern SHIM2_EXPORT int devsettings_key; // key toggles developer settings
extern SHIM2_EXPORT bool convert_directions_to_focus_events;
extern SHIM2_EXPORT float joystick_activate_threshold;
extern SHIM2_EXPORT float joystick_deactivate_threshold;
extern SHIM2_EXPORT bool mouse_button_repeats;
extern SHIM2_EXPORT int mouse_button_repeat_max_movement;
extern SHIM2_EXPORT bool dpad_below;
extern SHIM2_EXPORT bool allow_dpad_below;
extern SHIM2_EXPORT bool dpad_enabled;
extern SHIM2_EXPORT void (*joystick_disconnect_callback)();
extern SHIM2_EXPORT bool force_tablet;
// other
extern SHIM2_EXPORT util::CPA *cpa;
extern SHIM2_EXPORT int notification_duration; // in millis
extern SHIM2_EXPORT int notification_fade_duration; // in millis
extern SHIM2_EXPORT Uint32 timer_event_id;
extern SHIM2_EXPORT int argc;
extern SHIM2_EXPORT char **argv;
// this is for loading data from the EXE
extern SHIM2_EXPORT int cpa_extra_bytes_after_exe_data;
// these two are for loading data from a memory buffer
extern SHIM2_EXPORT Uint8 *cpa_pointer_to_data;
extern SHIM2_EXPORT int cpa_data_size;
extern SHIM2_EXPORT bool logging;
// game should always set this to logic ticks per second (default is 60)
// Note that this doesn't control execution speed (that is up to your main loop) but it tells some things how fast your main loop is running
extern SHIM2_EXPORT int logic_rate;
extern SHIM2_EXPORT bool use_cwd;
extern SHIM2_EXPORT bool log_tags;
extern SHIM2_EXPORT int error_level; // 0=none, 1=errors, 2=info, 3=debug/opengl
#ifdef TVOS
extern SHIM2_EXPORT bool pass_menu_to_os;
#endif
extern SHIM2_EXPORT int devsettings_num_rows; // in rows
extern SHIM2_EXPORT int devsettings_max_width; // in pixels
extern SHIM2_EXPORT bool debug;
#ifdef STEAMWORKS
extern SHIM2_EXPORT bool steam_init_failed;
extern SHIM2_EXPORT void (*steam_overlay_activated_callback)();
#endif

extern SHIM2_EXPORT util::JSON *shim_json;

extern SHIM2_EXPORT std::vector<util::A_Star::Way_Point> (*get_way_points)(util::Point<int> start);

} // End namespace shim

} // End namespace noo

#endif // NOO_SHIM_H
