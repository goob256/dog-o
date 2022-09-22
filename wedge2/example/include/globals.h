#ifndef GLOBALS_H
#define GLOBALS_H

#include <wedge2/globals.h>

class Globals : public wedge::Globals
{
public:
	Globals();
	virtual ~Globals();

	bool add_title_gui();
	void do_dialogue(std::string tag, std::string text, wedge::Dialogue_Type type, wedge::Dialogue_Position position, wedge::Step *monitor);
	bool dialogue_active(wedge::Game *game, bool only_if_initialised = false);
	void add_yes_no_gui(std::string text, bool escape_cancels, bool selected, util::Callback callback = 0, void *callback_data = 0);
	bool can_walk();
	bool title_gui_is_top();
	
	class Instance : public wedge::Globals::Instance
	{
	public:
		Instance(util::JSON::Node *root);
		virtual ~Instance();
	};

	audio::Sound *melee;

	bool started;
};

#endif // GLOBALS_H
