#include <wedge2/area_game.h>
#include <wedge2/globals.h>
#include <wedge2/shop_step.h>

#include "barter.h"
#include "globals.h"
#include "question.h"

Barter_Step::Barter_Step(std::vector<wedge::Object> items, wedge::Task *task) :
	wedge::Step(task),
	items(items),
	done(false)
{
}

Barter_Step::~Barter_Step()
{
}

bool Barter_Step::run()
{
	if (done) {
		send_done_signal();
	}
	return !done;
}

void Barter_Step::done_signal(wedge::Step *step)
{
	done = true;

	Question_Step *q = static_cast<Question_Step *>(step);

	int choice = q->get_choice();

	if (choice == 0) {
		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(297)/* Originally: I've got just the thing for you! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
		ADD_STEP(new wedge::Shop_Step(items, new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)
	}
	else if (choice == 1) {
		std::vector<wedge::Object> tmp;

		NEW_SYSTEM_AND_TASK(AREA)
		ADD_STEP(new Dialogue_Step(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(298)/* Originally: OK, show me what you've got! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, new_task))
		ADD_STEP(new wedge::Shop_Step(tmp, new_task))
		ADD_TASK(new_task)
		FINISH_SYSTEM(AREA)
	}
	else {
		GLOBALS->do_dialogue(GLOBALS->game_t->translate(290)/* Originally: Tok */ + TAG_END, GLOBALS->game_t->translate(299)/* Originally: Suit yourself! */, wedge::DIALOGUE_SPEECH, wedge::DIALOGUE_AUTO, NULL);
	}
}
