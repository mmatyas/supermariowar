set(SMW_SRCS ai.cpp gamemodes.cpp GSMenu.cpp
           player.cpp world.cpp uicustomcontrol.cpp
           objectgame.cpp objecthazard.cpp
           net.cpp FPSLimiter.cpp GSSplashScreen.cpp
           GSGameplay.cpp ObjectContainer.cpp main.cpp
           network/NetConfigManager.cpp
           network/FileCompressor.cpp
)

#
# Game objects
#
LIST(APPEND SMW_SRCS
	objects/blocks/IO_Block.cpp
	objects/blocks/BounceBlock.cpp
	objects/blocks/BreakableBlock.cpp
	objects/blocks/DonutBlock.cpp
	objects/blocks/FlipBlock.cpp
	objects/blocks/NoteBlock.cpp
	objects/blocks/OnOffSwitchBlock.cpp
	objects/blocks/PowerupBlock.cpp
	objects/blocks/ViewBlock.cpp
	objects/blocks/SwitchBlock.cpp
	objects/blocks/ThrowBlock.cpp
	objects/blocks/WeaponBreakableBlock.cpp

	objects/moving/MovingObject.cpp

	objects/overmap/OverMapObject.cpp
)

#
# Player components
#
LIST(APPEND SMW_SRCS
	player_components/PlayerAwardEffects.cpp
	player_components/PlayerBurnupTimer.cpp
	player_components/PlayerCollisions.cpp
	player_components/PlayerCape.cpp
	player_components/PlayerCardCollection.cpp
	player_components/PlayerInvincibility.cpp
	player_components/PlayerJail.cpp
	player_components/PlayerKuriboShoe.cpp
	player_components/PlayerOutOfArenaTimer.cpp
	player_components/PlayerSecretCode.cpp
	player_components/PlayerShield.cpp
	player_components/PlayerSpinStatus.cpp
	player_components/PlayerSuicideTimer.cpp
	player_components/PlayerSuperStomp.cpp
	player_components/PlayerTail.cpp
	player_components/PlayerTanookiSuit.cpp
	player_components/PlayerWarpStatus.cpp
	player_components/PlayerWings.cpp
)

LIST(APPEND SMW_SRCS
	ui/NetworkListScroll.cpp
	ui/MI_StringScroll.cpp
	ui/MI_PowerupSlider.cpp
	ui/MI_FrenzyModeOptions.cpp
	ui/MI_InputControlContainer.cpp
	ui/MI_TeamSelect.cpp
	ui/MI_NetRoomTeamSelect.cpp
)

#
# Menu screens
#
LIST(APPEND SMW_SRCS
	menu/MainMenu.cpp
	menu/BonusWheelMenu.cpp
	menu/GameSettingsMenu.cpp
	menu/ModeOptionsMenu.cpp
	menu/MapFilterEditMenu.cpp
	menu/MatchSelectionMenu.cpp
	menu/OptionsMenu.cpp
	menu/PlayerControlsMenu.cpp
	menu/PlayerControlsSelectMenu.cpp
	menu/TeamSelectMenu.cpp
	menu/TournamentScoreboardMenu.cpp
	menu/TourStopMenu.cpp
	menu/WorldMenu.cpp

	menu/options/EyeCandyOptionsMenu.cpp
	menu/options/GameplayOptionsMenu.cpp
	menu/options/GraphicsOptionsMenu.cpp
	menu/options/PowerupDropRatesMenu.cpp
	menu/options/PowerupSettingsMenu.cpp
	menu/options/ProjectileLimitsMenu.cpp
	menu/options/ProjectileOptionsMenu.cpp
	menu/options/SoundOptionsMenu.cpp
	menu/options/TeamOptionsMenu.cpp

	menu/network/NetEditServersMenu.cpp
	menu/network/NetLobbyMenu.cpp
	menu/network/NetNewRoomMenu.cpp
	menu/network/NetNewRoomSettingsMenu.cpp
	menu/network/NetRoomMenu.cpp
	menu/network/NetServersMenu.cpp
)

if(NOT NO_NETWORK)
	LIST(APPEND SMW_SRCS platform/network/enet/NetworkLayerENet.cpp)
endif()

# Add EXE icon on Windows
if (WIN32)
	LIST(APPEND SMW_SRCS "${PROJECT_SOURCE_DIR}/resources/windows/smw.rc")
endif()

#add_executable(smw ${COMMON_SRCS} ${SMW_SRCS} ../common/global.h)
add_executable(smw ../common/movingplatform.cpp ../common/uicontrol.cpp ../common/uimenu.cpp ${SMW_SRCS})
target_link_libraries(smw CommonFiles)
if(NOT NO_NETWORK)
	target_link_libraries(smw NetplayFiles)
endif()

#
# Dependencies
#

# TODO: fix emscripten build
# emscripten uses its own SDL
# if(NOT TARGET_EMSCRIPTEN)

target_link_libraries(smw ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(smw ${ZLIB_LIBRARY})
target_link_libraries(smw yaml-cpp)

if(USE_SDL2_LIBS)
	target_link_libraries(smw ${SDL2_LIBRARY} ${SDL2MIXER_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2MAIN_LIBRARY})
else()
	target_link_libraries(smw ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMAIN_LIBRARY})
endif()

if(NOT NO_NETWORK)
	target_link_libraries(smw enet)
	if (WIN32)
		# WinSock2 libs, required by ENet on Windows
		target_link_libraries(smw ws2_32 winmm)
	endif()
endif()

#
# Installation
#

install(FILES "${PROJECT_BINARY_DIR}/Binaries/Release/smw" DESTINATION ${SMW_BINDIR})

# install Linux desktop item and icons
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	configure_file("${PROJECT_SOURCE_DIR}/resources/linux/supermariowar.desktop.in"
		"${PROJECT_BINARY_DIR}/supermariowar.desktop" @ONLY)

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar.desktop"
		DESTINATION ${XDG_APPS_INSTALL_DIR})

	install(FILES
		"${PROJECT_SOURCE_DIR}/resources/icons/smw.png"
		"${PROJECT_SOURCE_DIR}/resources/icons/smw.xpm"
		DESTINATION "/usr/share/pixmaps/")
endif()
