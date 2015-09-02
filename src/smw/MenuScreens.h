#ifndef MENU_SCREENS_H
#define MENU_SCREENS_H

class UI_Menu;

struct MenuScreens {
    static UI_Menu* mMainMenu;

    // Options menu
    static UI_Menu* mOptionsMenu;
    static UI_Menu* mGameplayOptionsMenu;
    static UI_Menu* mTeamOptionsMenu;
    static UI_Menu* mPowerupDropRatesMenu;
    static UI_Menu* mPowerupSettingsMenu;
    static UI_Menu* mProjectileLimitsMenu;
    static UI_Menu* mProjectileOptionsMenu;
    static UI_Menu* mGraphicsOptionsMenu;
    static UI_Menu* mEyeCandyOptionsMenu;
    static UI_Menu* mSoundOptionsMenu;
#ifdef _XBOX
    static UI_Menu* mScreenSettingsMenu;
    static UI_Menu* mScreenResizeMenu;
#endif
    // Controls menu
    static UI_Menu* mPlayerControlsSelectMenu;
    static UI_Menu* mPlayerControlsMenu;

    // Gameplay menus
    static UI_Menu* mModeOptionsMenu;
    static UI_Menu* mMatchSelectionMenu;
    static UI_Menu* mGameSettingsMenu;
    static UI_Menu* mMapFilterEditMenu;
    static UI_Menu* mTourStopMenu;
    static UI_Menu* mWorldMenu;
    static UI_Menu* mTeamSelectMenu;
    static UI_Menu* mTournamentScoreboardMenu;
    static UI_Menu* mBonusWheelMenu;

    // Multiplayer menu
    static UI_Menu* mNetServersMenu;
    static UI_Menu* mNetLobbyMenu;
    static UI_Menu* mNetNewLevelMenu;
    static UI_Menu* mNetNewRoomMenu;
    static UI_Menu* mNetRoomMenu;
};

#endif // MENU_SCREENS_H
