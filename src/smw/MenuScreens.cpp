#include "MenuScreens.h"

#include "uimenu.h"

#include <cassert>

// The main menu
UI_MainMenu* MenuScreens::Main = nullptr;

// Options menu
UI_OptionsMenu* MenuScreens::Options = nullptr;
UI_GameplayOptionsMenu* MenuScreens::GameplayOptions = nullptr;
UI_TeamOptionsMenu* MenuScreens::TeamOptions = nullptr;
UI_PowerupDropRatesMenu* MenuScreens::PowerupDropRates = nullptr;
UI_PowerupSettingsMenu* MenuScreens::PowerupSettings = nullptr;
UI_ProjectileLimitsMenu* MenuScreens::ProjectileLimits = nullptr;
UI_ProjectileOptionsMenu* MenuScreens::ProjectileOptions = nullptr;
UI_GraphicsOptionsMenu* MenuScreens::GraphicsOptions = nullptr;
UI_EyeCandyOptionsMenu* MenuScreens::EyeCandyOptions = nullptr;
UI_SoundOptionsMenu* MenuScreens::SoundOptions = nullptr;
#ifdef _XBOX
    UI_ScreenSettingsMenu* MenuScreens::ScreenSettings = nullptr;
    UI_ScreenResizeMenu* MenuScreens::ScreenResize = nullptr;
#endif
// Controls menu
UI_PlayerControlsSelectMenu* MenuScreens::PlayerControlsSelect = nullptr;
UI_PlayerControlsMenu* MenuScreens::PlayerControls = nullptr;

// Gameplay menus
UI_ModeOptionsMenu* MenuScreens::ModeOptions = nullptr;
UI_MatchSelectionMenu* MenuScreens::MatchSelection = nullptr;
UI_GameSettingsMenu* MenuScreens::GameSettings = nullptr;
UI_MapFilterEditMenu* MenuScreens::MapFilterEdit = nullptr;
UI_TourStopMenu* MenuScreens::TourStop = nullptr;
UI_WorldMenu* MenuScreens::World = nullptr;
UI_TeamSelectMenu* MenuScreens::TeamSelect = nullptr;
UI_TournamentScoreboardMenu* MenuScreens::TournamentScoreboard = nullptr;
UI_BonusWheelMenu* MenuScreens::BonusWheel = nullptr;

// Multiplayer menu
UI_NetServersMenu* MenuScreens::NetServers = nullptr;
UI_NetLobbyMenu* MenuScreens::NetLobby = nullptr;
UI_NetNewLevelMenu* MenuScreens::NetNewLevel = nullptr;
UI_NetNewRoomMenu* MenuScreens::NetNewRoom = nullptr;
UI_NetRoomMenu* MenuScreens::NetRoom = nullptr;

// The menu stack
std::stack<UI_Menu*> MenuScreens::menu_stack;

void MenuScreens::PushMenu(UI_Menu* state)
{
	assert(state != nullptr);

    if (MenuScreens::menu_stack.size() > 0)
		MenuScreens::menu_stack.top()->OnPush();

	MenuScreens::menu_stack.push(state);
	MenuScreens::menu_stack.top()->OnEnter();
}

void MenuScreens::PopMenu()
{
	assert(MenuScreens::menu_stack.size() > 0);

	MenuScreens::menu_stack.top()->OnLeave();
	MenuScreens::menu_stack.pop();

    if (MenuScreens::menu_stack.size() > 0)
		MenuScreens::menu_stack.top()->OnPop();
}

UI_Menu* MenuScreens::Current()
{
	assert(MenuScreens::menu_stack.size() > 0);
	return MenuScreens::menu_stack.top();
}

void MenuScreens::RewindToMain()
{
    while (MenuScreens::menu_stack.size() > 1)
        PopMenu();

    assert(MenuScreens::Current() == (UI_Menu*)MenuScreens::Main);
}
