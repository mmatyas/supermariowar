SET(WORLDEDIT_SRCS ../smw/world.cpp worldeditor.cpp ../smw/uimenu.cpp ../smw/object.cpp 
        ../smw/uicontrol.cpp ../smw/modeoptionsmenu.cpp ../smw/objecthazard.cpp
)

add_executable(worldedit ${COMMON_SRCS} ${WORLDEDIT_SRCS} ../common/global.h)

target_link_libraries(worldedit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
