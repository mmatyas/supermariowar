add_executable(smw-leveledit WIN32
	../common/movingplatform.cpp
	../smw/FPSLimiter.cpp
	leveleditor.cpp
)
if (WIN32)
	# Add EXE icon on Windows
	target_sources(smw-leveledit PRIVATE "${PROJECT_SOURCE_DIR}/resources/windows/smw-leveledit.rc")
endif()

target_link_libraries(smw-leveledit CommonFiles)

#
# Installation
#

install(TARGETS smw-leveledit DESTINATION "${SMW_BINDIR}")

# install Linux desktop item and icons
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT SMW_INSTALL_PORTABLE)
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar-leveleditor.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar-leveleditor.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar-leveleditor.desktop"
		DESTINATION ${XDG_APPS_INSTALL_DIR})

	install(FILES
		"${PROJECT_SOURCE_DIR}/resources/icons/smw-leveledit.png"
		"${PROJECT_SOURCE_DIR}/resources/icons/smw-leveledit.xpm"
		DESTINATION "/usr/share/pixmaps/")
endif()
