#ifndef __MENU_H_
#define __MENU_H_


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

enum DisplayError {DISPLAY_ERROR_NONE, DISPLAY_ERROR_READ_TOUR_FILE, DISPLAY_ERROR_MAP_FILTER};

class Menu
{
	public:
		Menu() {}
		~Menu() {}

		void CreateMenu();
		void RunMenu();

#ifdef _DEBUG

		void LoadScript(char * szScriptFile);
		void ResetScript();
		void GetNextScriptOperation();

		std::vector<ScriptOperation*> operations;
		std::vector<ScriptOperation*>::iterator current;
		short iScriptState;
		bool fScriptRunPreGameOptions;

		void StartRecordScript();
		void SaveScript(char * szScriptFile);
		void SetNextScriptOperation();
		void AddEmtpyLineToScript();
#endif

	private:

		bool ReadTourFile();
		void StartGame();
		void Exit();
		void WriteGameOptions();
		void ResetTournamentBackToMainMenu();
	
		UI_Menu * mCurrentMenu;
		
		//Main Menu
		UI_Menu mMainMenu;
		
		MI_Image * miSMWTitle;
		MI_Image * miSMWVersion;
		//MI_Text * miSMWVersionText;
	
		MI_Button * miMainStartButton;
		MI_PlayerSelect * miPlayerSelect;
		MI_SelectField * miTournamentField;
	
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
		MI_Button * miPowerupSettingsMenuButton;
		MI_Button * miGraphicsOptionsMenuButton;
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
		MI_SelectField * miShieldField;
		MI_SelectField * miBoundsTimeField;
		MI_SelectField * miWarpLocksField;
		MI_SelectField * miBotsField;
		MI_SelectField * miFrameLimiterField;
		MI_SelectField * miPointSpeedField;
		MI_SelectField * miSecretsField;
		MI_Button * miGameplayOptionsMenuBackButton;

		MI_Image * miGameplayOptionsMenuLeftHeaderBar;
		MI_Image * miGameplayOptionsMenuRightHeaderBar;
		MI_Text * miGameplayOptionsMenuHeaderText;

		//Team Options Menu
		UI_Menu mTeamOptionsMenu;
		MI_SelectField * miTeamKillsField;
		MI_SelectField * miTeamColorsField;
		MI_Button * miTeamOptionsMenuBackButton;

		MI_Image * miTeamOptionsMenuLeftHeaderBar;
		MI_Image * miTeamOptionsMenuRightHeaderBar;
		MI_Text * miTeamOptionsMenuHeaderText;

		//Powerup Weight Selection
		UI_Menu mPowerupSelectionMenu;
		MI_PowerupSlider * miPowerupSlider[NUM_POWERUPS];
		MI_Button * miPowerupSelectionBackButton;
		MI_Button * miPowerupSelectionRestoreDefaultsButton;

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


		//Weapons & Projectiles Options Menu
		UI_Menu mProjectilesOptionsMenu;
		MI_SelectField * miFireballLifeField;
		MI_SelectField * miFireballLimitField;
		MI_SelectField * miHammerLifeField;
		MI_SelectField * miHammerDelayField;
		MI_SelectField * miHammerOneKillField;
		MI_SelectField * miHammerLimitField;
		MI_SelectField * miShellLifeField;
		MI_SelectField * miBlueBlockLifeField;
		MI_SelectField * miBoomerangStyleField;
		MI_SelectField * miBoomerangLifeField;
		MI_SelectField * miBoomerangLimitField;
		MI_SelectField * miFeatherJumpsField;
		MI_SelectField * miFeatherLimitField;
		MI_Button * miProjectilesOptionsMenuBackButton;

		MI_Text * miFireballText;
		MI_Text * miHammerText;
		MI_Text * miBoomerangText;
		MI_Text * miFeatherText;
		MI_Text * miShellText;
		MI_Text * miBlueBlockText;

		MI_Image * miProjectilesOptionsMenuLeftHeaderBar;
		MI_Image * miProjectilesOptionsMenuRightHeaderBar;
		MI_Text * miProjectilesOptionsMenuHeaderText;

		//Graphics Options Menu
		UI_Menu mGraphicsOptionsMenu;
		MI_SelectField * miSpawnStyleField;
		MI_SelectField * miAwardStyleField;
		MI_SelectField * miScoreStyleField;
		MI_SelectField * miCrunchField;
		MI_SelectField * miTopLayerField;
		MI_SelectField * miWinningCrownField;
		
#ifdef _XBOX
		MI_Button * miScreenSettingsButton;
#else
		MI_SelectField * miFullscreenField;
#endif //_XBOX

		MI_PacksField * miMenuGraphicsPackField;
		MI_PacksField * miGameGraphicsPackField;
		MI_Button * miGraphicsOptionsMenuBackButton;

		MI_Image * miGraphicsOptionsMenuLeftHeaderBar;
		MI_Image * miGraphicsOptionsMenuRightHeaderBar;
		MI_Text * miGraphicsOptionsMenuHeaderText;

		//Sound Options Menu
		UI_Menu mSoundOptionsMenu;
		MI_SliderField * miSoundVolumeField;
		MI_SliderField * miMusicVolumeField;
		MI_SelectField * miPlayNextMusicField;
		MI_AnnouncerField * miAnnouncerField;
		MI_PlaylistField * miPlaylistField;
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
	
		//Game Settings Menu
		UI_Menu mGameSettingsMenu;

		MI_ImageSelectField * miModeField;
		MI_SelectField * miGoalField[17];
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

		
		//Game Mode Settings Menu
		UI_Menu mModeSettingsMenu[17];

		//Jail
		MI_SelectField * miJailModeTagFreeField;
		MI_SelectField * miJailModeTimeFreeField;
		MI_Button * miJailModeBackButton;

		MI_Image * miJailModeLeftHeaderBar;
		MI_Image * miJailModeRightHeaderBar;
		MI_Text * miJailModeHeaderText;

		//Coins
		MI_SelectField * miCoinModePenaltyField;
		MI_SelectField * miCoinModeQuantityField;
		MI_Button * miCoinModeBackButton;

		MI_Image * miCoinModeLeftHeaderBar;
		MI_Image * miCoinModeRightHeaderBar;
		MI_Text * miCoinModeHeaderText;

		//Capture The Flag
		MI_SliderField * miFlagModeSpeedField;
		MI_SelectField * miFlagModeTouchReturnField;
		MI_SelectField * miFlagModePointMoveField;
		MI_SelectField * miFlagModeAutoReturnField;
		MI_SelectField * miFlagModeHomeScoreField;
		MI_Button * miFlagModeBackButton;
		
		MI_Image * miFlagModeLeftHeaderBar;
		MI_Image * miFlagModeRightHeaderBar;
		MI_Text * miFlagModeHeaderText;

		//Chicken
		MI_SelectField * miChickenModeTagOnTouchField;
		MI_Button * miChickenModeBackButton;
		
		MI_Image * miChickenModeLeftHeaderBar;
		MI_Image * miChickenModeRightHeaderBar;
		MI_Text * miChickenModeHeaderText;

		//Tag
		MI_SelectField * miTagModeTagOnTouchField;
		MI_Button * miTagModeBackButton;
		
		MI_Image * miTagModeLeftHeaderBar;
		MI_Image * miTagModeRightHeaderBar;
		MI_Text * miTagModeHeaderText;

		//Star
		MI_SelectField * miStarModeTimeField;
		MI_SelectField * miStarModeShineField;
		MI_Button * miStarModeBackButton;
		
		MI_Image * miStarModeLeftHeaderBar;
		MI_Image * miStarModeRightHeaderBar;
		MI_Text * miStarModeHeaderText;

		//Domination
		MI_SelectField * miDominationModeQuantityField;
		MI_SelectField * miDominationModeLoseOnDeathField;
		MI_SelectField * miDominationModeRelocateOnDeathField;
		MI_SelectField * miDominationModeStealOnDeathField;
		MI_SelectField * miDominationModeRelocateFrequencyField;
		
		MI_Text * miDominationModeDeathText;

		MI_Button * miDominationModeBackButton;
		
		MI_Image * miDominationModeLeftHeaderBar;
		MI_Image * miDominationModeRightHeaderBar;
		MI_Text * miDominationModeHeaderText;

		//King of the Hill
		MI_SelectField * miKingOfTheHillModeSizeField;
		MI_SelectField * miKingOfTheHillModeRelocateFrequencyField;
		MI_Button * miKingOfTheHillModeBackButton;
		
		MI_Image * miKingOfTheHillModeLeftHeaderBar;
		MI_Image * miKingOfTheHillModeRightHeaderBar;
		MI_Text * miKingOfTheHillModeHeaderText;

		//Race
		MI_SelectField * miRaceModeQuantityField;
		MI_SelectField * miRaceModeSpeedField;
		MI_SelectField * miRaceModePenaltyField;
		MI_Button * miRaceModeBackButton;
		
		MI_Image * miRaceModeLeftHeaderBar;
		MI_Image * miRaceModeRightHeaderBar;
		MI_Text * miRaceModeHeaderText;

		//Stomp
		MI_SelectField * miStompModeRateField;
		MI_PowerupSlider * miStompModeEnemySlider[3];
		MI_Button * miStompModeBackButton;
		
		MI_Image * miStompModeLeftHeaderBar;
		MI_Image * miStompModeRightHeaderBar;
		MI_Text * miStompModeHeaderText;

		//Frenzy
		MI_SelectField * miFrenzyModeQuantityField;
		MI_SelectField * miFrenzyModeRateField;
		MI_SelectField * miFrenzyModeStoredShellsField;
		MI_PowerupSlider * miFrenzyModePowerupSlider[12];
		MI_Button * miFrenzyModeBackButton;
		
		MI_Image * miFrenzyModeLeftHeaderBar;
		MI_Image * miFrenzyModeRightHeaderBar;
		MI_Text * miFrenzyModeHeaderText;

		//Survival
		MI_PowerupSlider * miSurvivalModeEnemySlider[3];
		MI_SelectField * miSurvivalModeDensityField;
		MI_SelectField * miSurvivalModeSpeedField;
		MI_SelectField * miSurvivalModeShieldField;
		MI_Button * miSurvivalModeBackButton;
		
		MI_Image * miSurvivalModeLeftHeaderBar;
		MI_Image * miSurvivalModeRightHeaderBar;
		MI_Text * miSurvivalModeHeaderText;


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
};

#endif //__MENU_H_

