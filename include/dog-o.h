#ifndef DOGO_H
#define DOGO_H

#define SCR_W 240
#define SCR_H 135

// we mix sfx a little higher than music
#define sfx_amp 1.0f
#define music_amp 0.5f

extern float sfx_volume;
extern float music_volume;
extern bool tv_safe_mode;
extern int windowed;
extern int fullscreen;
extern int screen_w;
extern int screen_h;
extern bool autosave_help_shown;
extern bool easy_combos;
extern bool simple_turn_display;

extern int orig_argc;
extern char **orig_argv;

extern std::string extra_args;
extern std::string extra_args_orig;

#endif // DOGO_H
