add_executable(smw-worldedit WIN32
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
