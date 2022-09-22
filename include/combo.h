#ifndef COMBO_H
#define COMBO_H

#include <wedge2/main.h>

enum Combo_Button {
	B_NONE = 0,
	B_U,
	B_R,
	B_D,
	B_L,
	B_Y,
	B_B,
	B_A,
	B_X
};

struct Combo_Event {
	Uint32 min_hold;
	Uint32 max_hold;
	Combo_Button button;
};

typedef std::vector<Combo_Event> Combo;

class Combo_Detector
{
public:
	struct Detected {
		Combo_Button button;
		Uint32 down_time;
		Uint32 release_time;
	};

	static const int MAX_DELAY_BETWEEN = 250;

	Combo_Detector(std::vector<Combo> combos);
	~Combo_Detector();

	void reset();

	void handle_event(TGUI_Event *event);

	void check(); // must be run before calling error(), good() or num_ok()
	bool error();
	int good(); // -1 if none, else index into combos
	int num_ok(); // number of buttons that match

	std::vector<Detected> get_detected();

private:
	//static bool ret_early;

	Combo_Button get_button(TGUI_Event *event);
	bool ignore_event(TGUI_Event *event);
	void process_event(TGUI_Event *event);
	void handle_joy(TGUI_Event *event, int x, int y, int old_x, int old_y);

	std::vector<Combo> combos;
	std::vector<Detected> detected;

	bool down;
	Combo_Button button;
	Uint32 down_time;
	int combo_good;
	bool combo_error;
	float joy_axis0;
	float joy_axis1;
	int n_ok;
	int max_delay_between;
};

#endif // COMBO_H
