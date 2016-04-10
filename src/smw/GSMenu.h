#ifndef GAMESTATE_MENU_H
#define GAMESTATE_MENU_H

#include "uicustomcontrol.h"

#include "GameState.h"

class UI_BonusWheelMenu;
class UI_GameSettingsMenu;
class UI_MainMenu;
class UI_MapFilterEditMenu;
class UI_MatchSelectionMenu;
class UI_ModeOptionsMenu;
class UI_OptionsMenu;
class UI_PlayerControlsMenu;
class UI_PlayerControlsSelectMenu;
class UI_TeamSelectMenu;
class UI_TournamentScoreboardMenu;
class UI_TourStopMenu;
class UI_WorldMenu;

class UI_EyeCandyOptionsMenu;
class UI_GameplayOptionsMenu;
class UI_GraphicsOptionsMenu;
class UI_PowerupDropRatesMenu;
class UI_PowerupSettingsMenu;
class UI_ProjectileLimitsMenu;
class UI_ProjectileOptionsMenu;
class UI_SoundOptionsMenu;
class UI_TeamOptionsMenu;

class UI_NetEditServersMenu;
class UI_NetLobbyMenu;
class UI_NetNewRoomMenu;
class UI_NetNewRoomSettingsMenu;
class UI_NetRoomMenu;
class UI_NetServersMenu;

#ifdef _XBOX
class UI_ScreenResizeMenu;
class UI_ScreenSettingsMenu;
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
		UI_NetEditServersMenu* mNetEditServersMenu;
		UI_NetLobbyMenu* mNetLobbyMenu;
		UI_NetNewRoomMenu* mNetNewRoomMenu;
		UI_NetNewRoomSettingsMenu* mNetNewRoomSettingsMenu;
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
