#include "wedge2/map_entity.h"
#include "wedge2/stop_sound.h"

using namespace wedge;

namespace wedge {

Stop_Sound_Step::Stop_Sound_Step(audio::Sound *sound, Task *task) :
	Step(task),
	sound(sound)
{
}

Stop_Sound_Step::~Stop_Sound_Step()
{
}

bool Stop_Sound_Step::run()
{
	sound->stop();
	send_done_signal();
	return false;
}

}
