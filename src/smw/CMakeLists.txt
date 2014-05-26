
set(SMW_SRCS ai.cpp gamemodes.cpp main.cpp GSMenu.cpp
           object.cpp player.cpp world.cpp uicustomcontrol.cpp
           objectgame.cpp modeoptionsmenu.cpp objecthazard.cpp
           net.cpp FPSLimiter.cpp GSSplashScreen.cpp
           GSGameplay.cpp
)

# disable networking in emscripten build for now...
if(NOT USE_EMSCRIPTEN)
	LIST(APPEND SMW_SRCS platform/network/sdl/NetworkHandlerSDL.cpp)
endif()

add_executable(smw ${COMMON_SRCS} ${SMW_SRCS} ../common/global.h)
# add .bc extension when building with emscripten
if(USE_EMSCRIPTEN)
	set_target_properties(smw PROPERTIES SUFFIX ".bc")
endif()

# emscripten uses its own SDL
if(NOT USE_EMSCRIPTEN)
	target_link_libraries(smw ${CMAKE_THREAD_LIBS_INIT})

	if(USE_SDL2_LIBS)
		target_link_libraries(smw SDL2 SDL2_mixer SDL2_image SDL2_net SDL2main)
	else()
		target_link_libraries(smw ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLNET_LIBRARY} SDLmain)
	endif()
endif()
