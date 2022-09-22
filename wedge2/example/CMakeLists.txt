project(example)
cmake_minimum_required(VERSION 2.6)

# Options

option(DEBUG "Debug build" on)

set(USER_INCLUDE_PATH "" CACHE FILEPATH "Any extra include directories you want")
set(USER_LIBRARY_PATH "" CACHE FILEPATH "Any extra library directories you want")

include_directories("include")
include_directories(${USER_INCLUDE_PATH})
link_directories(${USER_LIBRARY_PATH})

# Inputs

set(SOURCES
	src/area_game.cpp
	src/battle_game.cpp
	src/battle_player.cpp
	src/battle_transition_in.cpp
	src/battle_transition_in2.cpp
	src/battle_transition_out.cpp
	src/battle_transition_out2.cpp
	src/battles.cpp
	src/dialogue.cpp
	src/enemies.cpp
	src/example.cpp
	src/globals.cpp
	src/hit.cpp
	src/inventory.cpp
	src/transition.cpp
)

# Outputs

add_executable(example ${SOURCES})

# Platform specifics

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	SET(FLAGS "/D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES /EHsc /D_WIN32_WINNT=0x501 /MP /W3 /wd4244 /wd4800 /MT")
	set_target_properties(example PROPERTIES COMPILE_FLAGS "${FLAGS}")
	set_target_properties(example PROPERTIES OUTPUT_NAME example)
	set(LIBS wedge2 shim2 tgui4 SDL2 SDL2main)
elseif(APPLE)
	SET(FLAGS "-Wall")
	if(DEBUG)
		set(FLAGS "${FLAGS} -Wno-extern-c-compat -Wno-c++11-narrowing -Wno-overloaded-virtual -g")
	else()
		set(FLAGS "${FLAGS} -Wno-extern-c-compat -Wno-c++11-narrowing -Wno-overloaded-virtual")
	endif()
	set_target_properties(example PROPERTIES BUILD_WITH_INSTALL_RPATH on)
	set_target_properties(example PROPERTIES INSTALL_RPATH "@executable_path/")
	set_target_properties(example PROPERTIES COMPILE_FLAGS "${FLAGS}")
	set_target_properties(example PROPERTIES LINK_FLAGS "-framework Cocoa -framework IOKit -framework OpenGL -framework CoreAudio -framework AudioUnit -framework ForceFeedback -framework Carbon")
	set_target_properties(example PROPERTIES OUTPUT_NAME "example")
	set(LIBS wedge2 shim2 tgui4 SDL2)
else() # Linux
	SET(FLAGS "-Wall -Wno-comment")
	set(LFLAGS "-Wl,-rpath,\$ORIGIN")
	if(DEBUG)
		set(FLAGS "${FLAGS} -g -fsanitize=address -fsanitize=leak")
		set(LFLAGS "${LFLAGS} -fsanitize=address -fsanitize=leak")
	else()
		set(FLAGS "${FLAGS}")
	endif()
	set_target_properties(example PROPERTIES COMPILE_FLAGS "${FLAGS}")
	set_target_properties(example PROPERTIES LINK_FLAGS "${LFLAGS}")
	set_target_properties(example PROPERTIES OUTPUT_NAME example)
	set(LIBS wedge2_static shim2_static tgui4_static SDL2 GL Xcursor X11 z vorbisfile vorbis ogg)
endif()

target_link_libraries(example ${LIBS})
