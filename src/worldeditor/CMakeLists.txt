add_executable(smw-worldedit WIN32
	../common/movingplatform.cpp
	../common/movingplatform.h
	../smw/world.cpp
	../smw/world.h
	../smw/objects/moving/MovingObject.cpp
	../smw/objects/moving/MovingObject.h
	../smw/objects/overmap/OverMapObject.cpp
	../smw/objects/overmap/OverMapObject.h
	../smw/ObjectContainer.cpp
	../smw/ObjectContainer.h
	../smw/objecthazard.cpp
	../smw/objecthazard.h
	../smw/menu/ModeOptionsMenu.cpp
	../smw/menu/ModeOptionsMenu.h
	../common/uicontrol.cpp
	../common/uicontrol.h
	../common/ui/MI_MapPreview.cpp
	../common/ui/MI_MapPreview.h
	../common/ui/MI_MapField.cpp
	../common/ui/MI_MapField.h
	../smw/ui/MI_PowerupSlider.cpp
	../smw/ui/MI_PowerupSlider.h
	../smw/ui/MI_FrenzyModeOptions.cpp
	../smw/ui/MI_FrenzyModeOptions.h
	../common/uimenu.cpp
	../common/uimenu.h
	worldeditor.cpp
)
if (WIN32)
	# Add EXE icon on Windows
	target_sources(smw-worldedit PRIVATE "${PROJECT_SOURCE_DIR}/resources/windows/smw-worldedit.rc")
endif()

target_link_libraries(smw-worldedit CommonFiles)

#
# Installation
#

install(TARGETS smw-worldedit DESTINATION "${SMW_BINDIR}")

# install Linux desktop item and icons
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT SMW_INSTALL_PORTABLE)
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar-worldeditor.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar-worldeditor.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar-worldeditor.desktop" DESTINATION "${SMW_DESKTOPDIR}")
	install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw-worldedit.xpm" DESTINATION "${SMW_PIXMAPDIR}")
	install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw-worldedit.png" DESTINATION "${SMW_ICONDIR}/32x32/apps/")
endif()
