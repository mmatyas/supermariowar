add_executable(smw WIN32
	main.cpp

	ai.cpp
	ai.h
	FPSLimiter.cpp
	FPSLimiter.h
	GSGameplay.cpp
	GSGameplay.h
	GSMenu.cpp
	GSMenu.h
	GSSplashScreen.cpp
	GSSplashScreen.h
	net.cpp
	net.h
	network/FileCompressor.cpp
	network/FileCompressor.h
	network/NetConfigManager.cpp
	network/NetConfigManager.h
	ObjectContainer.cpp
	ObjectContainer.h
	objectgame.cpp
	objectgame.h
	objecthazard.cpp
	objecthazard.h
	player.cpp
	player.h
	world.cpp
	world.h

	../common/movingplatform.cpp
	../common/movingplatform.h
)

add_subdirectory(player_components)
add_subdirectory(menu)
add_subdirectory(gamemodes)
add_subdirectory(objects)
add_subdirectory(ui)

target_compile_features(smw PUBLIC cxx_std_14)
target_compile_definitions(smw PRIVATE
	GIT_REVISION="${SMW_GIT_REVISION}"
	GIT_DATE="${SMW_GIT_DATE}"
)

if(NO_NETWORK)
	target_compile_definitions(smw PRIVATE NETWORK_DISABLED)
else()
	target_sources(smw PRIVATE
		platform/network/enet/NetworkLayerENet.cpp
		platform/network/enet/NetworkLayerENet.h
	)
endif()

if (WIN32)
	# Add EXE icon on Windows
	target_sources(smw PRIVATE "${PROJECT_SOURCE_DIR}/resources/windows/smw.rc")
endif()

#
# Dependencies
#

target_link_libraries(smw CommonFiles)
if(NOT NO_NETWORK)
	target_link_libraries(smw NetplayFiles)
endif()

if(NOT EMSCRIPTEN)
	target_link_libraries(smw ZLIB::ZLIB)
endif()

target_link_libraries(smw yaml-cpp)

#
# Installation
#

install(TARGETS smw DESTINATION "${SMW_BINDIR}")

# install Linux desktop item and icons
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT SMW_INSTALL_PORTABLE)
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar.desktop" DESTINATION "${SMW_DESKTOPDIR}")
	install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw.xpm" DESTINATION "${SMW_PIXMAPDIR}")
	install(FILES "${PROJECT_SOURCE_DIR}/resources/icons/smw.png" DESTINATION "${SMW_ICONDIR}/32x32/apps/")
endif()
