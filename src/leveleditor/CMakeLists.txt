set(LEVELEDIT_SRCS leveleditor.cpp)

# Add EXE icon on Windows
if (WIN32)
	LIST(APPEND LEVELEDIT_SRCS "${PROJECT_SOURCE_DIR}/resources/windows/smw-leveledit.rc")
endif()

add_executable(smw-leveledit ../common/movingplatform.cpp ../smw/FPSLimiter.cpp ${LEVELEDIT_SRCS})
target_link_libraries(smw-leveledit CommonFiles)

#
# Dependencies
#

target_link_libraries(smw-leveledit ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMAIN_LIBRARY})

#
# Installation
#

install(FILES "${PROJECT_BINARY_DIR}/Binaries/Release/smw-leveledit" DESTINATION ${SMW_BINDIR})

# install Linux desktop item and icons
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar-leveleditor.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar-leveleditor.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar-leveleditor.desktop"
		DESTINATION ${XDG_APPS_INSTALL_DIR})

	install(FILES
		"${PROJECT_SOURCE_DIR}/resources/icons/smw-leveledit.png"
		"${PROJECT_SOURCE_DIR}/resources/icons/smw-leveledit.xpm"
		DESTINATION "/usr/share/pixmaps/")
endif()
