#ifndef MENU_SCREENS_H
#define MENU_SCREENS_H

#include <stack>

class UI_Menu;
class UI_MainMenu;
class UI_OptionsMenu;
class UI_GameplayOptionsMenu;
class UI_TeamOptionsMenu;
class UI_PowerupDropRatesMenu;
class UI_PowerupSettingsMenu;
class UI_ProjectileLimitsMenu;
class UI_ProjectileOptionsMenu;
class UI_GraphicsOptionsMenu;
class UI_EyeCandyOptionsMenu;
class UI_SoundOptionsMenu;
class UI_ScreenSettingsMenu;
class UI_ScreenResizeMenu;
class UI_PlayerControlsSelectMenu;
class UI_PlayerControlsMenu;
class UI_ModeOptionsMenu;
class UI_MatchSelectionMenu;
class UI_GameSettingsMenu;
class UI_MapFilterEditMenu;
class UI_TourStopMenu;
class UI_WorldMenu;
class UI_TeamSelectMenu;
class UI_TournamentScoreboardMenu;
class UI_BonusWheelMenu;
class UI_NetServersMenu;
class UI_NetLobbyMenu;
class UI_NetNewLevelMenu;
class UI_NetNewRoomMenu;
class UI_NetRoomMenu;

struct MenuScreens {
    // Menu state manipulation
    static void PushMenu(UI_Menu*);
    static void PopMenu();
    static UI_Menu* Current();
    static void RewindToMain();

    // The main menu
    static UI_MainMenu* Main;

    // Options menu
    static UI_OptionsMenu* Options;
    static UI_GameplayOptionsMenu* GameplayOptions;
    static UI_TeamOptionsMenu* TeamOptions;
    static UI_PowerupDropRatesMenu* PowerupDropRates;
    static UI_PowerupSettingsMenu* PowerupSettings;
    static UI_ProjectileLimitsMenu* ProjectileLimits;
    static UI_ProjectileOptionsMenu* ProjectileOptions;
    static UI_GraphicsOptionsMenu* GraphicsOptions;
    static UI_EyeCandyOptionsMenu* EyeCandyOptions;
    static UI_SoundOptionsMenu* SoundOptions;
#ifdef _XBOX
    static UI_ScreenSettingsMenu* ScreenSettings;
    static UI_ScreenResizeMenu* ScreenResize;
#endif
    // Controls menu
    static UI_PlayerControlsSelectMenu* PlayerControlsSelect;
    static UI_PlayerControlsMenu* PlayerControls;

    // Gameplay menus
    static UI_ModeOptionsMenu* ModeOptions;
    static UI_MatchSelectionMenu* MatchSelection;
    static UI_GameSettingsMenu* GameSettings;
    static UI_MapFilterEditMenu* MapFilterEdit;
    static UI_TourStopMenu* TourStop;
    static UI_WorldMenu* World;
    static UI_TeamSelectMenu* TeamSelect;
    static UI_TournamentScoreboardMenu* TournamentScoreboard;
    static UI_BonusWheelMenu* BonusWheel;

    // Multiplayer menu
    static UI_NetServersMenu* NetServers;
    static UI_NetLobbyMenu* NetLobby;
    static UI_NetNewLevelMenu* NetNewLevel;
    static UI_NetNewRoomMenu* NetNewRoom;
    static UI_NetRoomMenu* NetRoom;

private:
    static std::stack<UI_Menu*> menu_stack;
};

#endif // MENU_SCREENS_H
