#ifndef GAMESTATE_MENU_H
#define GAMESTATE_MENU_H

#include "GameState.h"

#include <memory>
#include <vector>

class UI_Menu;

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
		MenuState();
		~MenuState();
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


		UI_Menu* mCurrentMenu = nullptr;
		std::unique_ptr<UI_MainMenu> mMainMenu;

		// Options menu
		std::unique_ptr<UI_OptionsMenu> mOptionsMenu;
		std::unique_ptr<UI_GameplayOptionsMenu> mGameplayOptionsMenu;
		std::unique_ptr<UI_TeamOptionsMenu> mTeamOptionsMenu;
		std::unique_ptr<UI_PowerupDropRatesMenu> mPowerupDropRatesMenu;
		std::unique_ptr<UI_PowerupSettingsMenu> mPowerupSettingsMenu;
		std::unique_ptr<UI_ProjectileLimitsMenu> mProjectileLimitsMenu;
		std::unique_ptr<UI_ProjectileOptionsMenu> mProjectileOptionsMenu;
		std::unique_ptr<UI_GraphicsOptionsMenu> mGraphicsOptionsMenu;
		std::unique_ptr<UI_EyeCandyOptionsMenu> mEyeCandyOptionsMenu;
		std::unique_ptr<UI_SoundOptionsMenu> mSoundOptionsMenu;

		// Controls menu
		std::unique_ptr<UI_PlayerControlsSelectMenu> mPlayerControlsSelectMenu;
		std::unique_ptr<UI_PlayerControlsMenu> mPlayerControlsMenu;

		// Gameplay menus
		std::unique_ptr<UI_ModeOptionsMenu> mModeOptionsMenu;
		std::unique_ptr<UI_MatchSelectionMenu> mMatchSelectionMenu;
		std::unique_ptr<UI_GameSettingsMenu> mGameSettingsMenu;
		std::unique_ptr<UI_MapFilterEditMenu> mMapFilterEditMenu;
		std::unique_ptr<UI_TourStopMenu> mTourStopMenu;
		std::unique_ptr<UI_WorldMenu> mWorldMenu;
		std::unique_ptr<UI_TeamSelectMenu> mTeamSelectMenu;
		std::unique_ptr<UI_TournamentScoreboardMenu> mTournamentScoreboardMenu;
		std::unique_ptr<UI_BonusWheelMenu> mBonusWheelMenu;

		// Multiplayer menu
		std::unique_ptr<UI_NetServersMenu> mNetServersMenu;
		std::unique_ptr<UI_NetEditServersMenu> mNetEditServersMenu;
		std::unique_ptr<UI_NetLobbyMenu> mNetLobbyMenu;
		std::unique_ptr<UI_NetNewRoomMenu> mNetNewRoomMenu;
		std::unique_ptr<UI_NetNewRoomSettingsMenu> mNetNewRoomSettingsMenu;
		std::unique_ptr<UI_NetRoomMenu> mNetRoomMenu;

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
