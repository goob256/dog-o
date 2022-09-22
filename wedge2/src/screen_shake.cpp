#include "wedge2/screen_shake.h"

using namespace wedge;

namespace wedge {

Screen_Shake_Step::Screen_Shake_Step(float amount, Uint32 length, Task *task) :
	Step(task),
	amount(amount),
	length(length),
	cancelled(false)
{
}

Screen_Shake_Step::~Screen_Shake_Step()
{
}

bool Screen_Shake_Step::run()
{
	if (cancelled == false) {
		gfx::screen_shake(amount, length);
	}
	return false;
}

void Screen_Shake_Step::set_cancelled(bool cancelled)
{
	this->cancelled = cancelled;
}

}
