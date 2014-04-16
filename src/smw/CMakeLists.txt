
set(SMW_SRCS HashTable.cpp ai.cpp gamemodes.cpp main.cpp
           menu.cpp object.cpp player.cpp
           splash.cpp world.cpp uicustomcontrol.cpp
           objectgame.cpp modeoptionsmenu.cpp objecthazard.cpp Game.cpp net.cpp
           platform/network/sdl/NetworkHandlerSDL.cpp
)

if(DEBUG)
	LIST(APPEND SMW_SRCS HashTable.cpp HashTableTest.cpp)
endif(DEBUG)

add_executable(smw ${COMMON_SRCS} ${SMW_SRCS} ../common/global.h)

# emscripten uses (its own) SDL2
if(NOT USE_EMSCRIPTEN)
	target_link_libraries(smw ${CMAKE_THREAD_LIBS_INIT})

	if(USE_SDL2_LIBS)
		target_link_libraries(smw SDL2 SDL2_mixer SDL2_image SDL2_net SDL2main)
	else()
		target_link_libraries(smw ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLNET_LIBRARY} SDLmain)
	endif()
endif()