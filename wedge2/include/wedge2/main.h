#ifndef WEDGE2_MAIN_H
#define WEDGE2_MAIN_H

#include <shim2/shim2.h>

#ifdef _WIN32
#ifdef WEDGE2_STATIC
#define WEDGE2_EXPORT
#else
#ifdef WEDGE2_BUILD
#define WEDGE2_EXPORT __declspec(dllexport)
#else
#define WEDGE2_EXPORT __declspec(dllimport)
#endif
#endif
#else
#define WEDGE2_EXPORT
#endif

namespace wedge {

bool WEDGE2_EXPORT start(util::Size<int> base_screen_size, util::Size<int> window_size);
bool WEDGE2_EXPORT go();
void WEDGE2_EXPORT end();
void WEDGE2_EXPORT handle_event(TGUI_Event *event);

}

#endif // NOOSKEWL_EDGE_MAIN_H
