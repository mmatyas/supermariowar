#ifndef __MENU_H_
#define __MENU_H_

#include "modeoptionsmenu.h"
#include "uicustomcontrol.h"

#ifdef _DEBUG
class ScriptOperation
{
	public:

		ScriptOperation() {iTimesExecuted = 0;}
		~ScriptOperation() {}

		short iController; //0 - 3 == specific, 4 == all, 5 == single random
		bool fInput[8];
		short iIterations;  //number of times to execute this operation

		short iTimesExecuted;

	private:

};
#endif

enum MatchType {MATCH_TYPE_SINGLE_GAME = 0, MATCH_TYPE_TOURNAMENT = 1, MATCH_TYPE_TOUR = 2, MATCH_TYPE_WORLD = 4, MATCH_TYPE_MINIGAME = 3, MATCH_TYPE_QUICK_GAME = 5};
enum DisplayError {DISPLAY_ERROR_NONE, DISPLAY_ERROR_READ_TOUR_FILE, DISPLAY_ERROR_READ_WORLD_FILE, DISPLAY_ERROR_MAP_FILTER};

class Menu
{
	public:
		Menu() {}
		~Menu() {}

		void CreateMenu();
		void RunMenu();

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

		bool ReadTourFile();
		void StartGame();
		void Exit();
		void WriteGameOptions();
		void ResetTournamentBackToMainMenu();
	
		void SetControllingTeamForSettingsMenu(short iControlTeam, bool fDisplayMessage);
		void DisplayControllingTeamMessage(short iControlTeam);
		
		ModeOptionsMenu modeOptionsMenu;

		UI_Menu * mCurrentMenu;
		
		//Main Menu
		UI_Menu mMainMenu;
		
		MI_Image * miSMWTitle;
		MI_Image * miSMWVersion;
		MI_Text * miSMWVersionText;
	
		MI_Button * miMainStartButton;
		MI_Button * miQuickGameButton;

		MI_PlayerSelect * miPlayerSelect;
	
		MI_Button * miOptionsButton;
		MI_Button * miControlsButton;

		MI_Button * miExitButton;


		//Player Controls Select Menu
		UI_Menu mPlayerControlsSelectMenu;

		MI_Button * miPlayer1ControlsButton;
		MI_Button * miPlayer2ControlsButton;
		MI_Button * miPlayer3ControlsButton;
		MI_Button * miPlayer4ControlsButton;
		
		MI_Button * miPlayerControlsBackButton;

		MI_Image * miPlayerControlsLeftHeaderBar;
		MI_Image * miPlayerControlsMenuRightHeaderBar;
		MI_Text * miPlayerControlsMenuHeaderText;


		//Player Controls Menu
		UI_Menu mPlayerControlsMenu;
		MI_InputControlContainer * miInputContainer;

		//Options Menu
		UI_Menu mOptionsMenu;
		MI_Button * miGameplayOptionsMenuButton;
		MI_Button * miTeamOptionsMenuButton;
		MI_Button * miPowerupOptionsMenuButton;
		MI_Button * miProjectilesOptionsMenuButton;
		MI_Button * miProjectilesLimitsMenuButton;
		MI_Button * miPowerupSettingsMenuButton;
		MI_Button * miGraphicsOptionsMenuButton;
		MI_Button * miEyeCandyOptionsMenuButton;
		MI_Button * miSoundOptionsMenuButton;
		MI_Button * miGenerateMapThumbsButton;
		
		MI_Button * miOptionsMenuBackButton;

		MI_Image * miOptionsMenuLeftHeaderBar;
		MI_Image * miOptionsMenuRightHeaderBar;
		MI_Text * miOptionsMenuHeaderText;
		
		MI_Image * miGenerateThumbsDialogImage;
		MI_Text * miGenerateThumbsDialogAreYouText;
		MI_Text * miGenerateThumbsDialogSureText;
		MI_Button * miGenerateThumbsDialogYesButton;
		MI_Button * miGenerateThumbsDialogNoButton;

		//Gameplay Options Menu
		UI_Menu mGameplayOptionsMenu;
		MI_SelectField * miRespawnField;
		MI_SelectField * miShieldStyleField;
		MI_SelectField * miShieldTimeField;
		MI_SelectField * miBoundsTimeField;
		MI_SelectField * miSuicideTimeField;
		MI_SelectField * miWarpLockStyleField;
		MI_SelectField * miWarpLockTimeField;
		MI_SelectField * miBotsField;
		MI_SelectField * miPointSpeedField;
		MI_Button * miGameplayOptionsMenuBackButton;

		MI_Image * miGameplayOptionsMenuLeftHeaderBar;
		MI_Image * miGameplayOptionsMenuRightHeaderBar;
		MI_Text * miGameplayOptionsMenuHeaderText;

		//Team Options Menu
		UI_Menu mTeamOptionsMenu;
		MI_SelectField * miTeamKillsField;
		MI_SelectField * miTeamColorsField;
		MI_SelectField * miTournamentControlField;
		MI_Button * miTeamOptionsMenuBackButton;

		MI_Image * miTeamOptionsMenuLeftHeaderBar;
		MI_Image * miTeamOptionsMenuRightHeaderBar;
		MI_Text * miTeamOptionsMenuHeaderText;

		//Powerup Weight Selection
		UI_Menu mPowerupSelectionMenu;
		MI_PowerupSelection * miPowerupSelection;

		//MI_PowerupSlider * miPowerupSlider[NUM_POWERUPS];
		//MI_Button * miPowerupSelectionBackButton;
		//MI_Button * miPowerupSelectionRestoreDefaultsButton;

		MI_Image * miPowerupSelectionLeftHeaderBar;
		MI_Image * miPowerupSelectionMenuRightHeaderBar;
		MI_Text * miPowerupSelectionMenuHeaderText;

		//Powerup Settings Menu
		UI_Menu mPowerupSettingsMenu;
		MI_SelectField * miStoredPowerupDelayField;
		MI_SelectField * miItemRespawnField;
		MI_SelectField * miSwapStyleField;
		MI_SelectField * miBonusWheelField;
		MI_SelectField * miKeepPowerupField;
		MI_SelectField * miHiddenBlockRespawnField;
		MI_StoredPowerupResetButton * miStoredPowerupResetButton;
		MI_Button * miPowerupSettingsMenuBackButton;

		MI_Image * miPowerupSettingsMenuLeftHeaderBar;
		MI_Image * miPowerupSettingsMenuRightHeaderBar;
		MI_Text * miPowerupSettingsMenuHeaderText;

		MI_Image * miPowerupSelectionDialogImage;
		MI_Text * miPowerupSelectionDialogExitText;
		MI_Text * miPowerupSelectionDialogTournamentText;
		MI_Button * miPowerupSelectionDialogYesButton;
		MI_Button * miPowerupSelectionDialogNoButton;

		//Weapons Use Limits Menu
		UI_Menu mProjectilesLimitsMenu;
		MI_SelectField * miFireballLimitField;
		MI_SelectField * miHammerLimitField;
		MI_SelectField * miBoomerangLimitField;
		MI_SelectField * miFeatherLimitField;
		MI_SelectField * miLeafLimitField;
		MI_SelectField * miPwingsLimitField;
		MI_SelectField * miTanookiLimitField;
		MI_SelectField * miBombLimitField;
		MI_SelectField * miWandLimitField;

		MI_Button * miProjectilesLimitsMenuBackButton;

		MI_Image * miProjectilesLimitsMenuLeftHeaderBar;
		MI_Image * miProjectilesLimitsMenuRightHeaderBar;
		MI_Text * miProjectilesLimitsMenuHeaderText;

		//Weapons & Projectiles Options Menu
		UI_Menu mProjectilesOptionsMenu;

		MI_SelectField * miFireballLifeField;
		MI_SelectField * miHammerLifeField;
		MI_SelectField * miHammerDelayField;
		MI_SelectField * miHammerOneKillField;
		MI_SelectField * miShellLifeField;
		MI_SelectField * miWandFreezeTimeField;

		MI_SelectField * miBlueBlockLifeField;
		MI_SelectField * miGrayBlockLifeField;
		MI_SelectField * miRedBlockLifeField;
		MI_SelectField * miBoomerangStyleField;
		MI_SelectField * miBoomerangLifeField;
		MI_SelectField * miFeatherJumpsField;
		
		MI_Button * miProjectilesOptionsMenuBackButton;

		MI_Text * miFireballText;
		MI_Text * miHammerText;
		MI_Text * miBoomerangText;
		MI_Text * miFeatherText;
		MI_Text * miShellText;
		MI_Text * miWandText;
		MI_Text * miBlueBlockText;

		MI_Image * miProjectilesOptionsMenuLeftHeaderBar;
		MI_Image * miProjectilesOptionsMenuRightHeaderBar;
		MI_Text * miProjectilesOptionsMenuHeaderText;

		//Graphics Options Menu
		UI_Menu mGraphicsOptionsMenu;
		MI_SelectField * miTopLayerField;
		MI_SelectField * miFrameLimiterField;

#ifdef _XBOX
		MI_Button * miScreenSettingsButton;
#else
		MI_SelectField * miFullscreenField;
#endif //_XBOX

		MI_PacksField * miMenuGraphicsPackField;
		MI_PacksField * miWorldGraphicsPackField;
		MI_PacksField * miGameGraphicsPackField;
		MI_Button * miGraphicsOptionsMenuBackButton;

		MI_Image * miGraphicsOptionsMenuLeftHeaderBar;
		MI_Image * miGraphicsOptionsMenuRightHeaderBar;
		MI_Text * miGraphicsOptionsMenuHeaderText;

		//Eye Candy Options Menu
		UI_Menu mEyeCandyOptionsMenu;
		MI_SelectField * miSpawnStyleField;
		MI_SelectField * miAwardStyleField;
		MI_SelectField * miScoreStyleField;
		MI_SelectField * miCrunchField;
		MI_SelectField * miWinningCrownField;
		MI_SelectField * miStartCountDownField;
		MI_SelectField * miStartModeDisplayField;
		MI_SelectField * miDeadTeamNoticeField;

		MI_Button * miEyeCandyOptionsMenuBackButton;
		
		MI_Image * miEyeCandyOptionsMenuLeftHeaderBar;
		MI_Image * miEyeCandyOptionsMenuRightHeaderBar;
		MI_Text * miEyeCandyOptionsMenuHeaderText;

		//Sound Options Menu
		UI_Menu mSoundOptionsMenu;
		MI_SliderField * miSoundVolumeField;
		MI_SliderField * miMusicVolumeField;
		MI_SelectField * miPlayNextMusicField;
		MI_AnnouncerField * miAnnouncerField;
		MI_PlaylistField * miPlaylistField;
		MI_SelectField * miWorldMusicField;
		MI_PacksField * miSoundPackField;
		MI_Button * miSoundOptionsMenuBackButton;

		MI_Image * miSoundOptionsMenuLeftHeaderBar;
		MI_Image * miSoundOptionsMenuRightHeaderBar;
		MI_Text * miSoundOptionsMenuHeaderText;

#ifdef _XBOX
		UI_Menu mScreenSettingsMenu;
		
		MI_Button * miScreenResizeButton;
		MI_SelectField * miScreenHardwareFilterField;
		MI_SelectField * miScreenFlickerFilterField;
		MI_SelectField * miScreenSoftFilterField;
		//MI_SelectField * miScreenAspectRatioField;
		
		MI_Button * miScreenSettingsMenuBackButton;

		MI_Image * miScreenSettingsMenuLeftHeaderBar;
		MI_Image * miScreenSettingsMenuRightHeaderBar;
		MI_Text * miScreenSettingsMenuHeaderText;

		UI_Menu mScreenResizeMenu;
		MI_ScreenResize * miScreenResize;
#endif
	
		//Match Selection Menu
		UI_Menu mMatchSelectionMenu;

		MI_WorldPreviewDisplay * miWorldPreviewDisplay;
		MI_Image * miMatchSelectionDisplayImage;
		MI_SelectField * miMatchSelectionField;

		MI_SelectField * miTournamentField;
		MI_SelectField * miTourField;
		MI_SelectField * miWorldField;
		MI_SelectField * miMinigameField;

		MI_Button * miMatchSelectionStartButton;

		MI_Image * miMatchSelectionMenuLeftHeaderBar;
		MI_Image * miMatchSelectionMenuRightHeaderBar;
		MI_Text * miMatchSelectionMenuHeaderText;


		//Game Settings Menu
		UI_Menu mGameSettingsMenu;

		MI_ImageSelectField * miModeField;
		MI_SelectField * miGoalField[22];
		MI_Button * miModeSettingsButton;
		MI_MapField * miMapField;
		MI_Button * miSettingsStartButton;
		MI_Button * miMapFiltersButton;
		MI_Button * miMapThumbnailsButton;
		
		MI_Image * miMapFiltersOnImage;
		
		MI_Image * miGameSettingsLeftHeaderBar;
		MI_Image * miGameSettingsMenuRightHeaderBar;
		MI_Text * miGameSettingsMenuHeaderText;

		MI_Image * miGameSettingsExitDialogImage;
		MI_Text * miGameSettingsExitDialogExitText;
		MI_Text * miGameSettingsExitDialogTournamentText;
		MI_Button * miGameSettingsExitDialogYesButton;
		MI_Button * miGameSettingsExitDialogNoButton;

		MI_MapFilterScroll * miMapFilterScroll;
		
		//Map Filter Edit Menu
		UI_Menu mMapFilterEditMenu;
		MI_MapBrowser * miMapBrowser;

		//Tour Stop Menu
		UI_Menu mTourStopMenu;

		MI_TourStop * miTourStop;
		
		MI_Image * miTourStopExitDialogImage;
		MI_Text * miTourStopExitDialogExitTourText;
		MI_Button * miTourStopExitDialogYesButton;
		MI_Button * miTourStopExitDialogNoButton;

		//World "Menu"
		UI_Menu mWorldMenu;
		MI_World * miWorld;
		MI_TourStop * miWorldStop;

		MI_Image * miWorldExitDialogImage;
		MI_Text * miWorldExitDialogExitTourText;
		MI_Button * miWorldExitDialogYesButton;
		MI_Button * miWorldExitDialogNoButton;

		
		//Team Select
		UI_Menu mTeamSelectMenu;
		MI_TeamSelect * miTeamSelect;
		
		MI_Image * miTeamSelectLeftHeaderBar;
		MI_Image * miTeamSelectRightHeaderBar;
		MI_Text * miTeamSelectHeaderText;


		//Tournament Scoreboard
		UI_Menu mTournamentScoreboardMenu;
		MI_TournamentScoreboard * miTournamentScoreboard;
		MI_Button * miTournamentScoreboardNextButton;
		MI_Image * miTournamentScoreboardImage;


		//Bonus Wheel
		UI_Menu mBonusWheelMenu;
		MI_BonusWheel * miBonusWheel;

		DisplayError iDisplayError;
		short iDisplayErrorTimer;
		bool fNeedMenuMusicReset;

		const char * szCurrentMapName;

		short iUnlockMinigameOptionIndex;

		short iTournamentAIStep;
		short iTournamentAITimer;

		friend void RunGame();
};

#endif //__MENU_H_

