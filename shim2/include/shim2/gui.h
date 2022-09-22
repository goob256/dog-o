#ifndef NOO_GUI_H
#define NOO_GUI_H

#include "shim2/main.h"
#include "shim2/shim.h"
#include "shim2/translation.h"

namespace noo {

namespace gui {

EXPORT_CLASS_ALIGN(GUI, 16) {
public:
	TGUI *gui;
	TGUI_Widget *focus; // backup focus

	GUI();
	virtual ~GUI();

	bool is_transitioning_in();
	bool is_transitioning_out();
	bool is_transition_out_finished();

	virtual void handle_event(TGUI_Event *event);

	virtual void update();
	virtual void update_background(); // called when the GUI is not the foremost

	void pre_draw(); // special stuff (starts transition timer)

	virtual void draw_back();
	virtual void draw();
	virtual void draw_fore();

	virtual void resize(util::Size<int> new_size);

	virtual bool is_fullscreen(); // if the top gui returns true, other guis don't get drawn

	bool do_return(bool ret);

	virtual bool transition_done(bool transition_in); // return true to cancel and keep this GUI alive

	virtual void transition_start(float p);
	virtual void transition_end();

	// normally a fade is done if transitions are enabled, but these can be used instead
	void use_enlarge_transition(bool onoff);
	void use_shrink_transition(bool onoff);
	void use_appear_transition(bool onoff);
	void use_slide_transition(bool onoff);
	void use_slide_vertical_transition(bool onoff);

	void exit(); // call this to exit this GUI and remove it from shim::guis after transition and update()

	virtual void lost_device();
	virtual void found_device();

	virtual void transition_in_done(); // called when transition in is done (only if transition is true)

	void set_transition(bool transition);

	// For 16 byte alignment to make glm::mat4 able to use SIMD
#ifdef _WIN32
	void *operator new(size_t i);
	void operator delete(void* p);
#endif

protected:
	static const int MAX_FADE_SCALE = 10;
	
	static bool started_transition_timer;
	static Uint32 transition_start_time;

	void fade_transition(float p);
	void scale_transition(float scale);
	void appear_in_transition(float p);
	void appear_out_transition(float p);
	void slide_transition(float x);
	void slide_vertical_transition(float y);

	bool transition;
	bool transitioning_in;
	bool transitioning_out;
	bool transition_is_enlarge;
	bool transition_is_shrink;
	bool transition_is_appear;
	bool transition_is_slide;
	bool transition_is_slide_vertical;
	glm::mat4 mv_backup;
	glm::mat4 p_backup;
	float last_transition_p;
	float appear_save;
	gfx::Image *plasma;
	int transition_duration;
	float slide_save;
};

enum Popup_Type {
	OK = 0,
	YESNO = 1
};

// Functions
int SHIM2_EXPORT popup(std::string caption, std::string text, Popup_Type type);
int SHIM2_EXPORT fatalerror(std::string caption, std::string text, Popup_Type type, bool do_exit = false);

} // End namespace gui

} // End namespace noo

#endif // NOO_GUI_H
