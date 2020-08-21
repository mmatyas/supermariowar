set (WORLDEDIT_SRCS worldeditor.cpp)

# Add EXE icon on Windows
if (WIN32)
	LIST(APPEND WORLDEDIT_SRCS "${PROJECT_SOURCE_DIR}/resources/windows/smw-worldedit.rc")
endif()

add_executable (smw-worldedit
	../common/movingplatform.cpp
	../smw/world.cpp
	../smw/objects/moving/MovingObject.cpp
	../smw/objects/overmap/OverMapObject.cpp
	../smw/ObjectContainer.cpp
	../smw/objecthazard.cpp
	../smw/menu/ModeOptionsMenu.cpp
	../common/uicontrol.cpp
	../common/ui/MI_MapPreview.cpp
	../common/ui/MI_MapField.cpp
	../smw/ui/MI_PowerupSlider.cpp
	../smw/ui/MI_FrenzyModeOptions.cpp
	../common/uimenu.cpp
	${WORLDEDIT_SRCS}
)

target_link_libraries (smw-worldedit CommonFiles)

#
# Dependencies
#

if(USE_SDL2_LIBS)
	target_link_libraries(smw-worldedit ${SDL2_LIBRARY} ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARY} ${SDL2MAIN_LIBRARY})
else()
	target_link_libraries(smw-worldedit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMAIN_LIBRARY})
endif()

#
# Installation
#

install(FILES "${PROJECT_BINARY_DIR}/Binaries/Release/smw-worldedit" DESTINATION ${SMW_BINDIR})

# install Linux desktop item and icons
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar-worldeditor.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar-worldeditor.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar-worldeditor.desktop"
		DESTINATION ${XDG_APPS_INSTALL_DIR})

	install(FILES
		"${PROJECT_SOURCE_DIR}/resources/icons/smw-worldedit.png"
		"${PROJECT_SOURCE_DIR}/resources/icons/smw-worldedit.xpm"
		DESTINATION "/usr/share/pixmaps/")
endif()
