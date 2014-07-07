set (WORLDEDIT_SRCS worldeditor.cpp)

add_executable (worldedit
	../common/movingplatform.cpp
	../smw/world.cpp
	../smw/object.cpp
    ../smw/ObjectContainer.cpp
    ../smw/objecthazard.cpp
	../smw/modeoptionsmenu.cpp
	../common/uicontrol.cpp
	../common/uimenu.cpp
	${WORLDEDIT_SRCS}
)
target_link_libraries (worldedit CommonFiles)

# add .bc extension when building with emscripten
if(USE_EMSCRIPTEN)
	set_target_properties(worldedit PROPERTIES SUFFIX ".bc")
endif()

if(NOT USE_EMSCRIPTEN)
	target_link_libraries (worldedit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
endif()
