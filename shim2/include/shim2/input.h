#ifndef NOO_INPUT_H
#define NOO_INPUT_H

#include "shim2/main.h"
#include "shim2/steamworks.h"

namespace noo {

namespace input {

struct SHIM2_EXPORT Focus_Event : public TGUI_Event {
	TGUI_Event_Type orig_type;
	union {
		TGUI_Event::TGUI_Event_Keyboard orig_keyboard;
		TGUI_Event::TGUI_Event_Joystick orig_joystick;
	} u;

	virtual ~Focus_Event();
};

bool start();
void reset();
void end();
void update();
void handle_event(TGUI_Event *event);

bool SHIM2_EXPORT convert_to_focus_event(TGUI_Event *event, Focus_Event *focus);
void SHIM2_EXPORT convert_focus_to_original(TGUI_Event *event);
void SHIM2_EXPORT rumble(float strength, Uint32 length);
bool SHIM2_EXPORT is_joystick_connected();
std::string SHIM2_EXPORT get_joystick_button_name(int button);
std::string SHIM2_EXPORT get_joystick_button_colour_code(int button);
void SHIM2_EXPORT drop_repeats(bool joystick = true, bool mouse = true);
SDL_JoystickID SHIM2_EXPORT get_controller_id();

#ifdef STEAMWORKS
ControllerHandle_t SHIM2_EXPORT get_controller_handle();
#endif

bool SHIM2_EXPORT system_has_touchscreen();
bool SHIM2_EXPORT system_has_keyboard();

} // End namespace input

} // End namespace noo

#endif // NOO_INPUT_H
