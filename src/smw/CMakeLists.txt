
message( "COMMONS for SMW = ${COMMON_SRCS}" )

set(SMW_SRCS HashTable.cpp ai.cpp gamemodes.cpp main.cpp 
           menu.cpp object.cpp player.cpp 
           splash.cpp world.cpp uicustomcontrol.cpp 
        objectgame.cpp modeoptionsmenu.cpp objecthazard.cpp Game.cpp
)
add_executable(smw ${COMMON_SRCS} ${SMW_SRCS} ../common/global.h)

target_link_libraries(smw ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(smw ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
