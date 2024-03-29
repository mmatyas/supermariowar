add_executable(smw WIN32
	main.cpp

	ai.cpp
	ai.h
	FPSLimiter.cpp
	FPSLimiter.h
	gamemodes.cpp
	gamemodes.h
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
	../common/uicontrol.cpp
	../common/uicontrol.h
	../common/uimenu.cpp
	../common/uimenu.h
)

#
# Game objects
#
target_sources(smw PRIVATE
	objects/blocks/IO_Block.cpp

	objects/blocks/BounceBlock.cpp
	objects/blocks/BounceBlock.h
	objects/blocks/BreakableBlock.cpp
	objects/blocks/BreakableBlock.h
	objects/blocks/DonutBlock.cpp
	objects/blocks/DonutBlock.h
	objects/blocks/FlipBlock.cpp
	objects/blocks/FlipBlock.h
	objects/blocks/NoteBlock.cpp
	objects/blocks/NoteBlock.h
	objects/blocks/OnOffSwitchBlock.cpp
	objects/blocks/OnOffSwitchBlock.h
	objects/blocks/PowerupBlock.cpp
	objects/blocks/PowerupBlock.h
	objects/blocks/ViewBlock.cpp
	objects/blocks/ViewBlock.h
	objects/blocks/SwitchBlock.cpp
	objects/blocks/SwitchBlock.h
	objects/blocks/ThrowBlock.cpp
	objects/blocks/ThrowBlock.h
	objects/blocks/WeaponBreakableBlock.cpp
	objects/blocks/WeaponBreakableBlock.h

	objects/moving/MovingObject.cpp
	objects/moving/MovingObject.h

	objects/overmap/OverMapObject.cpp
	objects/overmap/OverMapObject.h
)

#
# Player components
#
target_sources(smw PRIVATE
	player_components/PlayerAwardEffects.cpp
	player_components/PlayerAwardEffects.h
	player_components/PlayerBurnupTimer.cpp
	player_components/PlayerBurnupTimer.h
	player_components/PlayerCollisions.cpp
	player_components/PlayerCollisions.h
	player_components/PlayerCape.cpp
	player_components/PlayerCape.h
	player_components/PlayerCardCollection.cpp
	player_components/PlayerCardCollection.h
	player_components/PlayerInvincibility.cpp
	player_components/PlayerInvincibility.h
	player_components/PlayerJail.cpp
	player_components/PlayerJail.h
	player_components/PlayerKuriboShoe.cpp
	player_components/PlayerKuriboShoe.h
	player_components/PlayerOutOfArenaTimer.cpp
	player_components/PlayerOutOfArenaTimer.h
	player_components/PlayerSecretCode.cpp
	player_components/PlayerSecretCode.h
	player_components/PlayerShield.cpp
	player_components/PlayerShield.h
	player_components/PlayerSpinStatus.cpp
	player_components/PlayerSpinStatus.h
	player_components/PlayerSuicideTimer.cpp
	player_components/PlayerSuicideTimer.h
	player_components/PlayerSuperStomp.cpp
	player_components/PlayerSuperStomp.h
	player_components/PlayerTail.cpp
	player_components/PlayerTail.h
	player_components/PlayerTanookiSuit.cpp
	player_components/PlayerTanookiSuit.h
	player_components/PlayerWarpStatus.cpp
	player_components/PlayerWarpStatus.h
	player_components/PlayerWings.cpp
	player_components/PlayerWings.h
)

#
# Menu screens
#
target_sources(smw PRIVATE
	menu/MainMenu.cpp
	menu/MainMenu.h
	menu/BonusWheelMenu.cpp
	menu/BonusWheelMenu.h
	menu/GameSettingsMenu.cpp
	menu/GameSettingsMenu.h
	menu/ModeOptionsMenu.cpp
	menu/ModeOptionsMenu.h
	menu/MapFilterEditMenu.cpp
	menu/MapFilterEditMenu.h
	menu/MatchSelectionMenu.cpp
	menu/MatchSelectionMenu.h
	menu/OptionsMenu.cpp
	menu/OptionsMenu.h
	menu/PlayerControlsMenu.cpp
	menu/PlayerControlsMenu.h
	menu/PlayerControlsSelectMenu.cpp
	menu/PlayerControlsSelectMenu.h
	menu/TeamSelectMenu.cpp
	menu/TeamSelectMenu.h
	menu/TournamentScoreboardMenu.cpp
	menu/TournamentScoreboardMenu.h
	menu/TourStopMenu.cpp
	menu/TourStopMenu.h
	menu/WorldMenu.cpp
	menu/WorldMenu.h

	menu/options/EyeCandyOptionsMenu.cpp
	menu/options/EyeCandyOptionsMenu.h
	menu/options/GameplayOptionsMenu.cpp
	menu/options/GameplayOptionsMenu.h
	menu/options/GraphicsOptionsMenu.cpp
	menu/options/GraphicsOptionsMenu.h
	menu/options/PowerupDropRatesMenu.cpp
	menu/options/PowerupDropRatesMenu.h
	menu/options/PowerupSettingsMenu.cpp
	menu/options/PowerupSettingsMenu.h
	menu/options/ProjectileLimitsMenu.cpp
	menu/options/ProjectileLimitsMenu.h
	menu/options/ProjectileOptionsMenu.cpp
	menu/options/ProjectileOptionsMenu.h
	menu/options/SoundOptionsMenu.cpp
	menu/options/SoundOptionsMenu.h
	menu/options/TeamOptionsMenu.cpp
	menu/options/TeamOptionsMenu.h

	menu/network/NetEditServersMenu.cpp
	menu/network/NetEditServersMenu.h
	menu/network/NetLobbyMenu.cpp
	menu/network/NetLobbyMenu.h
	menu/network/NetNewRoomMenu.cpp
	menu/network/NetNewRoomMenu.h
	menu/network/NetNewRoomSettingsMenu.cpp
	menu/network/NetNewRoomSettingsMenu.h
	menu/network/NetRoomMenu.cpp
	menu/network/NetRoomMenu.h
	menu/network/NetServersMenu.cpp
	menu/network/NetServersMenu.h

	gamemodes/BonusHouse.cpp
	gamemodes/BonusHouse.h
	gamemodes/CaptureTheFlag.cpp
	gamemodes/CaptureTheFlag.h
	gamemodes/CardCollection.cpp
	gamemodes/CardCollection.h
	gamemodes/Chase.cpp
	gamemodes/Chase.h
	gamemodes/Classic.cpp
	gamemodes/Classic.h
	gamemodes/Domination.cpp
	gamemodes/Domination.h
	gamemodes/GameModeTimer.cpp
	gamemodes/GameModeTimer.h
	gamemodes/Greed.cpp
	gamemodes/Greed.h
	gamemodes/Health.cpp
	gamemodes/Health.h
	gamemodes/KingOfTheHill.cpp
	gamemodes/KingOfTheHill.h
	gamemodes/MiniBoss.cpp
	gamemodes/MiniBoss.h
	gamemodes/MiniBoxes.cpp
	gamemodes/MiniBoxes.h
	gamemodes/MiniPipe.cpp
	gamemodes/MiniPipe.h
	gamemodes/Star.cpp
	gamemodes/Star.h
	gamemodes/TimeLimit.cpp
	gamemodes/TimeLimit.h
)

add_subdirectory(ui)

target_compile_features(smw PUBLIC cxx_std_14)

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
