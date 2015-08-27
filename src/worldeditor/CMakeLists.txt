set (WORLDEDIT_SRCS worldeditor.cpp)

add_executable (smw-worldedit
	../common/movingplatform.cpp
	../smw/world.cpp
	../smw/objects/moving/MovingObject.cpp
	../smw/objects/overmap/OverMapObject.cpp
    ../smw/ObjectContainer.cpp
    ../smw/objecthazard.cpp
	../smw/menu/ModeOptionsMenu.cpp
	../common/uicontrol.cpp
	../common/uimenu.cpp
	${WORLDEDIT_SRCS}
)
target_link_libraries (smw-worldedit CommonFiles)

# add .bc extension when building with emscripten
if(TARGET_EMSCRIPTEN)
	set_target_properties(smw-worldedit PROPERTIES SUFFIX ".bc")
endif()

if(NOT TARGET_EMSCRIPTEN)
	target_link_libraries (smw-worldedit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} SDLmain)
endif()

install(FILES "${PROJECT_BINARY_DIR}/Binaries/Release/smw-worldedit" DESTINATION ${SMW_BINDIR})
