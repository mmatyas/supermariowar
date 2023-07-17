add_executable(smw WIN32
	ai.cpp
	FPSLimiter.cpp
	gamemodes.cpp
	GSGameplay.cpp
	GSMenu.cpp
	GSSplashScreen.cpp
	main.cpp
	net.cpp
	network/FileCompressor.cpp
	network/NetConfigManager.cpp
	ObjectContainer.cpp
	objectgame.cpp
	objecthazard.cpp
	player.cpp
	uicustomcontrol.cpp
	world.cpp
	../common/movingplatform.cpp
	../common/uicontrol.cpp
	../common/uimenu.cpp
)

#
# Game objects
#
target_sources(smw PRIVATE
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
target_sources(smw PRIVATE
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

#
# Menu screens
#
target_sources(smw PRIVATE
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

	ui/NetworkListScroll.cpp
	ui/MI_StringScroll.cpp
	ui/MI_PowerupSlider.cpp
	ui/MI_FrenzyModeOptions.cpp
	ui/MI_InputControlContainer.cpp
	ui/MI_TeamSelect.cpp
	ui/MI_NetRoomTeamSelect.cpp
)

if(NOT NO_NETWORK)
	target_sources(smw PRIVATE platform/network/enet/NetworkLayerENet.cpp)
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

	install(FILES "${PROJECT_BINARY_DIR}/supermariowar.desktop"
		DESTINATION ${XDG_APPS_INSTALL_DIR})

	install(FILES
		"${PROJECT_SOURCE_DIR}/resources/icons/smw.png"
		"${PROJECT_SOURCE_DIR}/resources/icons/smw.xpm"
		DESTINATION "/usr/share/pixmaps/")
endif()
