project("dog-o")
cmake_minimum_required(VERSION 2.6)

# Options

option(DEBUG "Debug build" OFF)
option(USE_STATIC_LIBS "Link with static libraries" ON)
option(STEAMWORKS "Build for Steam" OFF)
option(DEMO "Build the demo version" OFF)
option(WANT_TTF "Build with TTF support" OFF)
option(ASHEDIT_TRAILER "Build AshEdit trailer" OFF)

set(USER_INCLUDE_PATH "" CACHE FILEPATH "Any extra include directories you want")
set(USER_LIBRARY_PATH "" CACHE FILEPATH "Any extra library directories you want")

include_directories("include")
include_directories(${USER_INCLUDE_PATH})
link_directories(${USER_LIBRARY_PATH})

if(USE_STATIC_LIBS)
	set(WEDGE2_LIB wedge2_static)
	set(SHIM2_LIB shim2_static)
	set(TGUI4_LIB tgui4_static)
else()
	set(WEDGE2_LIB wedge2)
	set(SHIM2_LIB shim2)
	set(TGUI4_LIB tgui4)
endif()

# Inputs

set(SOURCES
	src/achievements.cpp
	src/area_game.cpp
	src/autosave.cpp
	src/barter.cpp
	src/battle_combo_drawer.cpp
	src/battle_game.cpp
	src/battle_player.cpp
	src/battle_transition_in.cpp
	src/battle_transition_in2.cpp
	src/battle_transition_out.cpp
	src/battle_transition_out2.cpp
	src/battles.cpp
	src/coin.cpp
	src/combo.cpp
	src/dialogue.cpp
	src/enemies.cpp
	src/fire.cpp
	src/dog-o.cpp
	src/general.cpp
	src/globals.cpp
	src/gui.cpp
	src/gui_drawing_hook.cpp
	src/hit.cpp
	src/inventory.cpp
	src/menu.cpp
	src/multi.cpp
	src/pan_camera.cpp
	src/question.cpp
	src/shop.cpp
	src/shop_game.cpp
	src/start_battle.cpp
	src/transition.cpp
	src/widgets.cpp
)

if(WIN32)
	list(APPEND SOURCES misc/dog-o.manifest misc/versioninfo.rc misc/origdata/gfx/icons/windows_icons.rc)
endif()

if(APPLE)
	set(SOURCES ${SOURCES} src/apple.M)
endif()

# Outputs

add_executable("dog-o" ${SOURCES})

# Platform specifics

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /IGNORE:4075 /IGNORE:4099")
	SET(FLAGS "${FLAGS} /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /MP /W3 /wd4244 /wd4800 /wd4101 /MT")
	SET(FLAGS "${FLAGS} /DNTDDI_VERSION=0x06010000 /D_WIN32_WINNT=0x601")
	if(DEBUG)
		set(FLAGS "${FLAGS} /DDEBUG=1")
	endif()
	if(USE_STATIC_LIBS)
		set(FLAGS "${FLAGS} /DTGUI4_STATIC /DSHIM2_STATIC /DWEDGE2_STATIC")
	endif()
	if(STEAMWORKS)
		set(FLAGS "${FLAGS} /DSTEAMWORKS")
		set(LIBS ${LIBS} steam_api)
	endif()
	if(DEMO)
		set(FLAGS "${FLAGS} /DDEMO")
	endif()
	set_target_properties("dog-o" PROPERTIES COMPILE_FLAGS "${FLAGS}")
	set_target_properties("dog-o" PROPERTIES OUTPUT_NAME "dog-o")
	#set(LIBS ${LIBS} ${WEDGE2_LIB} ${SHIM2_LIB} ${TGUI4_LIB} SDL2 SDL2main libvorbisfile_static libvorbis_static ogg zlibstatic d3d9 winmm dbghelp shcore)
	if(ASHEDIT_TRAILER)
		set(LIBS ${LIBS} ${WEDGE2_LIB} ${SHIM2_LIB} ${TGUI4_LIB} SDL2 SDL2main ogg FLAC freetype zlibstatic d3d9 winmm dbghelp shcore)
	else()
		set(LIBS ${LIBS} ${WEDGE2_LIB} ${SHIM2_LIB} ${TGUI4_LIB} SDL2 SDL2main ogg FLAC zlibstatic d3d9 winmm dbghelp shcore)
	endif()
elseif(APPLE)
	SET(FLAGS "-Wall -Wno-extern-c-compat -Wno-c++11-narrowing -Wno-overloaded-virtual -DTGUI4_STATIC -DSHIM2_STATIC -DWEDGE2_STATIC")
	set(FLAGS "${FLAGS} -stdlib=libc++ -std=c++11")
	if(DEBUG)
		set(FLAGS "${FLAGS} -DDEBUG=1 -g")
	endif()
	if(STEAMWORKS)
		set(FLAGS "${FLAGS} -DSTEAMWORKS")
		set(LIBS ${LIBS} steam_api)
	endif()
	if(DEMO)
		set(FLAGS "${FLAGS} -DDEMO")
	endif()
	set_target_properties("dog-o" PROPERTIES BUILD_WITH_INSTALL_RPATH on)
	set_target_properties("dog-o" PROPERTIES INSTALL_RPATH "@executable_path/")
	set_target_properties("dog-o" PROPERTIES COMPILE_FLAGS "${FLAGS}")
	set_target_properties("dog-o" PROPERTIES LINK_FLAGS "-framework Cocoa -framework IOKit -framework OpenGL -framework CoreAudio -framework AudioUnit -framework ForceFeedback -framework Carbon")
	set_target_properties("dog-o" PROPERTIES OUTPUT_NAME "dog-o")
	#set(LIBS ${LIBS} ${WEDGE2_LIB} ${SHIM2_LIB} ${TGUI4_LIB} SDL2 vorbisfile vorbis ogg z)
	set(LIBS ${LIBS} ${WEDGE2_LIB} ${SHIM2_LIB} ${TGUI4_LIB} SDL2 z)
else() # Linux
	SET(FLAGS "-Wall -Wno-comment -Wno-strict-aliasing -std=c++0x")
	set(LFLAGS "-Wl,-rpath,\$ORIGIN")
	if(DEBUG)
		set(FLAGS "${FLAGS} -DDEBUG=1 -g -fsanitize=address -fsanitize=leak")
		set(LFLAGS "${LFLAGS} -fsanitize=address -fsanitize=leak")
	else()
		set(FLAGS "${FLAGS}")
	endif()
	if(STEAMWORKS)
		set(FLAGS "${FLAGS} -DSTEAMWORKS")
		set(LIBS ${LIBS} steam_api)
	endif()
	if(DEMO)
		set(FLAGS "${FLAGS} -DDEMO")
	endif()
	set_target_properties("dog-o" PROPERTIES COMPILE_FLAGS "${FLAGS}")
	set_target_properties("dog-o" PROPERTIES LINK_FLAGS "${LFLAGS}")
	set_target_properties("dog-o" PROPERTIES OUTPUT_NAME "dog-o")
	#set(LIBS ${LIBS} wedge2_static shim2_static tgui4_static SDL2 GL Xcursor X11 z vorbisfile vorbis ogg)
	set(LIBS ${LIBS} wedge2_static shim2_static tgui4_static SDL2 GL Xcursor X11 FLAC ogg z)
endif()

target_link_libraries("dog-o" ${LIBS})
