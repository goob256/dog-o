#ifndef NOO_AUDIO_H
#define NOO_AUDIO_H

#include "shim2/main.h"

namespace noo {

namespace audio {

bool SHIM2_EXPORT static_start();
bool SHIM2_EXPORT start();
void SHIM2_EXPORT end();
int SHIM2_EXPORT millis_to_samples(int millis);
int SHIM2_EXPORT samples_to_millis(int samples, int freq = -1);
void SHIM2_EXPORT pause_sfx(bool paused);

} // End namespace audio

} // End namespace noo

#endif // NOO_AUDIO_H
