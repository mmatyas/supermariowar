
set(SMW_SRCS HashTable.cpp ai.cpp gamemodes.cpp main.cpp
           menu.cpp object.cpp player.cpp
           splash.cpp world.cpp uicustomcontrol.cpp
           objectgame.cpp modeoptionsmenu.cpp objecthazard.cpp Game.cpp net.cpp
)

if(DEBUG)
	LIST(APPEND SMW_SRCS HashTable.cpp HashTableTest.cpp)
endif(DEBUG)

add_executable(smw ${COMMON_SRCS} ${SMW_SRCS} ../common/global.h)

# emscripten uses (its own) SDL2
if(NOT USE_EMSCRIPTEN)
	LIST(APPEND SMW_SRCS platform/network/sdl/NetworkHandlerSDL.cpp)
	target_link_libraries(smw ${CMAKE_THREAD_LIBS_INIT})

	if(USE_SDL2_LIBS)
		target_link_libraries(smw SDL2 SDL2_mixer SDL2_image SDL2_net SDL2main)
	else()
		target_link_libraries(smw ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLNET_LIBRARY} SDLmain)
	endif()
else()
	# disable networking for now...
	LIST(APPEND SMW_SRCS platform/network/null/NetworkHandlerNULL.cpp)
endif()
