#ifndef GAMESTATE_MENU_H
#define GAMESTATE_MENU_H

#include "modeoptionsmenu.h"
#include "uicustomcontrol.h"

#include "GameState.h"

#include "menu/BonusWheelMenu.h"
#include "menu/GameSettingsMenu.h"
#include "menu/MainMenu.h"
#include "menu/MapFilterEditMenu.h"
#include "menu/MatchSelectionMenu.h"
#include "menu/OptionsMenu.h"
#include "menu/PlayerControlsMenu.h"
#include "menu/PlayerControlsSelectMenu.h"
#include "menu/TeamSelectMenu.h"
#include "menu/TournamentScoreboardMenu.h"
#include "menu/TourStopMenu.h"
#include "menu/WorldMenu.h"

#include "menu/options/EyeCandyOptionsMenu.h"
#include "menu/options/GameplayOptionsMenu.h"
#include "menu/options/GraphicsOptionsMenu.h"
#include "menu/options/PowerupDropRatesMenu.h"
#include "menu/options/PowerupSettingsMenu.h"
#include "menu/options/ProjectileLimitsMenu.h"
#include "menu/options/ProjectileOptionsMenu.h"
#include "menu/options/SoundOptionsMenu.h"
#include "menu/options/TeamOptionsMenu.h"

#include "menu/network/NetLobbyMenu.h"
#include "menu/network/NetNewLevelMenu.h"
#include "menu/network/NetNewRoomMenu.h"
#include "menu/network/NetRoomMenu.h"
#include "menu/network/NetServersMenu.h"

#ifdef _XBOX
#include "menu/xbox/ScreenResizeMenu.h"
#include "menu/xbox/ScreenSettingsMenu.h"
#endif

#ifdef _DEBUG
class ScriptOperation
{
	public:

    ScriptOperation() {
        iTimesExecuted = 0;
    }
		~ScriptOperation() {}

		short iController; //0 - 3 == specific, 4 == all, 5 == single random
		bool fInput[8];
		short iIterations;  //number of times to execute this operation

		short iTimesExecuted;

	private:

};
#endif

enum DisplayError {
	DISPLAY_ERROR_NONE,
	DISPLAY_ERROR_READ_TOUR_FILE,
	DISPLAY_ERROR_READ_WORLD_FILE,
	DISPLAY_ERROR_MAP_FILTER
};

class MenuState : public GameState
{
	public:
        bool init();
        void update();

        static MenuState& instance();

#ifdef _DEBUG

		void LoadScript(const char * szScriptFile);
		void ResetScript();
		void GetNextScriptOperation();

		std::vector<ScriptOperation*> operations;
		std::vector<ScriptOperation*>::iterator current;
		short iScriptState;
		bool fScriptRunPreGameOptions;

		void StartRecordScript();
		void SaveScript(const char * szScriptFile);
		void SetNextScriptOperation();
		void AddEmtpyLineToScript();
#endif

	private:
		MenuState() {}
		~MenuState() {}
        MenuState(MenuState const&);
        void operator=(MenuState const&);

		void onEnterState();

		void CreateMenu();
		bool ReadTourFile();
		void StartGame();
		void Exit();
		void ResetTournamentBackToMainMenu();

		void SetControllingTeamForSettingsMenu(short iControlTeam, bool fDisplayMessage);
		void DisplayControllingTeamMessage(short iControlTeam);


		UI_Menu * mCurrentMenu;
		UI_MainMenu* mMainMenu;

		// Options menu
		UI_OptionsMenu* mOptionsMenu;
		UI_GameplayOptionsMenu* mGameplayOptionsMenu;
		UI_TeamOptionsMenu* mTeamOptionsMenu;
		UI_PowerupDropRatesMenu* mPowerupDropRatesMenu;
		UI_PowerupSettingsMenu* mPowerupSettingsMenu;
		UI_ProjectileLimitsMenu* mProjectileLimitsMenu;
		UI_ProjectileOptionsMenu* mProjectileOptionsMenu;
		UI_GraphicsOptionsMenu* mGraphicsOptionsMenu;
		UI_EyeCandyOptionsMenu* mEyeCandyOptionsMenu;
		UI_SoundOptionsMenu* mSoundOptionsMenu;
#ifdef _XBOX
		UI_ScreenSettingsMenu* mScreenSettingsMenu;
		UI_ScreenResizeMenu* mScreenResizeMenu;
#endif
		// Controls menu
		UI_PlayerControlsSelectMenu* mPlayerControlsSelectMenu;
		UI_PlayerControlsMenu* mPlayerControlsMenu;

		// Gameplay menus
		UI_ModeOptionsMenu* mModeOptionsMenu;
		UI_MatchSelectionMenu* mMatchSelectionMenu;
		UI_GameSettingsMenu* mGameSettingsMenu;
		UI_MapFilterEditMenu* mMapFilterEditMenu;
		UI_TourStopMenu* mTourStopMenu;
		UI_WorldMenu* mWorldMenu;
		UI_TeamSelectMenu* mTeamSelectMenu;
		UI_TournamentScoreboardMenu* mTournamentScoreboardMenu;
		UI_BonusWheelMenu* mBonusWheelMenu;

		// Multiplayer menu
		UI_NetServersMenu* mNetServersMenu;
		UI_NetLobbyMenu* mNetLobbyMenu;
		UI_NetNewLevelMenu* mNetNewLevelMenu;
		UI_NetNewRoomMenu* mNetNewRoomMenu;
		UI_NetRoomMenu* mNetRoomMenu;

		DisplayError iDisplayError;
		short iDisplayErrorTimer;
		bool fNeedMenuMusicReset;

		const char * szCurrentMapName;

		short iUnlockMinigameOptionIndex;

		short iTournamentAIStep;
		short iTournamentAITimer;

		friend class GameplayState;
};

#endif // GAMESTATE_MENU_H
