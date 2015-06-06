set(SMW_SRCS ai.cpp gamemodes.cpp GSMenu.cpp
           object.cpp player.cpp world.cpp uicustomcontrol.cpp
           objectgame.cpp objecthazard.cpp
           net.cpp FPSLimiter.cpp GSSplashScreen.cpp
           GSGameplay.cpp ObjectContainer.cpp main.cpp
)

#
# Game objects
#
LIST(APPEND SMW_SRCS
	objects/blocks/PowerupBlock.cpp
	objects/blocks/BreakableBlock.cpp
)

#
# Player components
#
LIST(APPEND SMW_SRCS
	player_components/PlayerAwardEffects.cpp
	player_components/PlayerCollisions.cpp
	player_components/PlayerCape.cpp
	player_components/PlayerCardCollection.cpp
	player_components/PlayerKuriboShoe.cpp
	player_components/PlayerSpinStatus.cpp
	player_components/PlayerTail.cpp
	player_components/PlayerTanookiSuit.cpp
	player_components/PlayerSecretCode.cpp
	player_components/PlayerSuperStomp.cpp
	player_components/PlayerWarpStatus.cpp
	player_components/PlayerWings.cpp
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

	menu/network/NetLobbyMenu.cpp
	menu/network/NetNewLevelMenu.cpp
	menu/network/NetNewRoomMenu.cpp
	menu/network/NetRoomMenu.cpp
	menu/network/NetServersMenu.cpp
)

if(NOT NO_NETWORK)
	# disable networking in emscripten build for now...
	if(NOT TARGET_EMSCRIPTEN)
		LIST(APPEND SMW_SRCS platform/network/enet/NetworkLayerENet.cpp)
	endif()
endif()

#add_executable(smw ${COMMON_SRCS} ${SMW_SRCS} ../common/global.h)
add_executable(smw ../common/movingplatform.cpp ../common/uicontrol.cpp ../common/uimenu.cpp ${SMW_SRCS})
target_link_libraries(smw CommonFiles)

# add .bc extension when building with emscripten
if(TARGET_EMSCRIPTEN)
	set_target_properties(smw PROPERTIES SUFFIX ".bc")
endif()

# emscripten uses its own SDL
if(NOT TARGET_EMSCRIPTEN)
	target_link_libraries(smw ${CMAKE_THREAD_LIBS_INIT})

	if(USE_SDL2_LIBS)
		target_link_libraries(smw SDL2 SDL2_mixer SDL2_image SDL2_net SDL2main)
	else()
		target_link_libraries(smw ${SDL_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLNET_LIBRARY} SDLmain)
	endif()

	if(NOT NO_NETWORK)
		target_link_libraries(smw enet)
	endif()
endif()

install(FILES "${PROJECT_BINARY_DIR}/Binaries/Release/smw" DESTINATION ${bindir})
