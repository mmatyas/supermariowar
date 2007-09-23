
#include "global.h"
#include <math.h>

#ifdef _XBOX
	#include <xtl.h>
#endif

#ifdef _WIN32
	#ifndef _XBOX
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
#endif

/*
#ifdef _XBOX
extern void reconnectjoysticks();
extern int joystickcount;
#endif
*/

#ifdef _DEBUG
extern bool	g_fAutoTest;
extern bool g_fRecordTest;
#endif

extern short g_iVersion[];

extern void SetGameModeSettingsFromMenu();
extern void LoadMapObjects();
extern bool LoadStartGraphics();
extern bool LoadMenuGraphics();
extern bool LoadGameGraphics();
extern bool LoadGameSounds();
extern bool LoadFullSkin(gfxSprite ** sprites, short skinID, short colorID);

extern void UpdateScoreBoard();

//Rearrange display of powerups
short iPowerupDisplayMap[NUM_POWERUPS] = { 4, 0, 1, 2, 3, 6, 10, 12, 11, 14, 13, 7, 16, 17, 18, 19, 15, 9, 5, 8};
short iPowerupPositionMap[NUM_POWERUPS] = { 1, 2, 3, 4, 0, 18, 5, 11, 19, 17, 6, 8, 7, 10, 9, 16, 12, 13, 14, 15};

extern Uint8 GetScreenBackgroundFade();
extern short LookupTeamID(short id);

extern char * g_szAutoFilterNames[NUM_AUTO_FILTERS];
extern short g_iAutoFilterIcons[NUM_AUTO_FILTERS];

extern void ResetTourStops();
extern WorldMap g_worldmap;

extern void LoadCurrentMapBackground();

extern TourStop * ParseTourStopLine(char * buffer, short iVersion[4], bool fIsWorld);

void Menu::WriteGameOptions()
{
	FILE *fp;
#ifdef _XBOX
	fp = fopen("D:\\options.bin", "wb");
#else
#ifdef PREFIXPATH
    char * folder=getenv("HOME");
#ifdef __MACOSX__
	std::string optionsbin=std::string(folder)+
		std::string("/Library/Preferences/smw.options.bin");
#else
    std::string optionsbin=std::string(folder)+std::string("/.smw.options.bin");
#endif
    fp= fopen(optionsbin.c_str(), "wb");
#else
	fp = fopen("options.bin", "wb");
#endif
#endif

	if(fp != NULL)
	{
		fwrite(g_iVersion, sizeof(short), 4, fp);

#ifdef _XBOX

		fwrite(&game_values.flickerfilter, sizeof(short), 1, fp);
		fwrite(&game_values.hardwarefilter, sizeof(short), 1, fp);
		fwrite(&game_values.softfilter, sizeof(short), 1, fp);
		fwrite(&game_values.aspectratio10x11, sizeof(bool), 1, fp);
		
		fwrite(&game_values.screenResizeX, sizeof(float), 1, fp);
		fwrite(&game_values.screenResizeY, sizeof(float), 1, fp);
		fwrite(&game_values.screenResizeW, sizeof(float), 1, fp);
		fwrite(&game_values.screenResizeH, sizeof(float), 1, fp);
#endif

		unsigned char abyte[27];
		abyte[0] = (unsigned char) game_values.spawnstyle;
		abyte[1] = (unsigned char) game_values.awardstyle;
		abyte[2] = (unsigned char) announcerlist.GetCurrentIndex();
		abyte[3] = (unsigned char) game_values.friendlyfire;
		abyte[4] = (unsigned char) game_values.screencrunch;
		abyte[5] = (unsigned char) game_values.toplayer;
		abyte[6] = (unsigned char) game_values.scoreboardstyle;
		abyte[7] = (unsigned char) game_values.teamcolors;
		abyte[8] = (unsigned char) game_values.sound;
		abyte[9] = (unsigned char) game_values.music;
		abyte[10] = (unsigned char) game_values.musicvolume;
		abyte[11] = (unsigned char) game_values.soundvolume;
		abyte[12] = (unsigned char) game_values.respawn;
		abyte[13] = (unsigned char) musiclist.GetCurrentIndex();
		abyte[14] = (unsigned char) game_values.outofboundstime;
		abyte[15] = (unsigned char) game_values.cpudifficulty;
		abyte[16] = (unsigned char) menugraphicspacklist.GetCurrentIndex();
		abyte[17] = (unsigned char) soundpacklist.GetCurrentIndex();
		abyte[18] = (unsigned char) game_values.framelimiter;
		abyte[19] = (unsigned char) game_values.bonuswheel;
		abyte[20] = (unsigned char) game_values.keeppowerup;
		abyte[21] = (unsigned char) game_values.showwinningcrown;
		abyte[22] = (unsigned char) game_values.playnextmusic;
		abyte[23] = (unsigned char) game_values.pointspeed;
		abyte[24] = (unsigned char) game_values.swapstyle;
		abyte[25] = (unsigned char) gamegraphicspacklist.GetCurrentIndex();
		abyte[26] = (unsigned char) game_values.secrets;
		fwrite(abyte, sizeof(unsigned char), 27, fp); 

		fwrite(&game_values.spawninvincibility, sizeof(short), 1, fp);
		fwrite(&game_values.itemrespawntime, sizeof(short), 1, fp);
		fwrite(&game_values.fireballttl, sizeof(short), 1, fp);
		fwrite(&game_values.fireballlimit, sizeof(short), 1, fp);
		fwrite(&game_values.hammerdelay, sizeof(short), 1, fp);
		fwrite(&game_values.hammerttl, sizeof(short), 1, fp);
		fwrite(&game_values.hammerpower, sizeof(bool), 1, fp);
		fwrite(&game_values.hammerlimit, sizeof(short), 1, fp);
		fwrite(&game_values.boomerangstyle, sizeof(short), 1, fp);
		fwrite(&game_values.boomeranglife, sizeof(short), 1, fp);
		fwrite(&game_values.boomeranglimit, sizeof(short), 1, fp);
		fwrite(&game_values.featherjumps, sizeof(short), 1, fp);
		fwrite(&game_values.featherlimit, sizeof(short), 1, fp);
		fwrite(&game_values.shellttl, sizeof(short), 1, fp);
		fwrite(&game_values.blueblockttl, sizeof(short), 1, fp);
		fwrite(&game_values.storedpowerupdelay, sizeof(short), 1, fp);
		fwrite(&game_values.warplocks, sizeof(short), 1, fp);

		fwrite(game_values.inputConfiguration, sizeof(CInputPlayerControl), 8, fp);

		for(int iPlayer = 0; iPlayer < 4; iPlayer++)
		{
			fwrite(&game_values.playerInput.inputControls[iPlayer]->iDevice, sizeof(short), 1, fp);
		}

#ifndef _XBOX
		fwrite(&game_values.fullscreen, sizeof(bool), 1, fp);
#endif
		//Write out game mode goals
		for(short k = 0; k < GAMEMODE_LAST; k++)
		{
			short iGoal = miGoalField[k]->GetShortValue();
			fwrite(&iGoal, sizeof(short), 1, fp);
		}

		//Write out powerup weights
		for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
			fwrite(&game_values.powerupweights[iPowerup], sizeof(short), 1, fp);

		fwrite(&game_values.gamemodemenusettings, sizeof(GameModeSettings), 1, fp);

		fwrite(&miTeamSelect->iTeamCounts, sizeof(short), 4, fp);
		fwrite(&miTeamSelect->iTeamIDs, sizeof(short), 12, fp);
		fwrite(&game_values.skinids, sizeof(short), 4, fp);
		fwrite(&game_values.randomskin, sizeof(bool), 4, fp);
		fwrite(&game_values.playercontrol, sizeof(short), 4, fp);

		fclose(fp);
	}

	maplist.WriteFilters();
	maplist.WriteMapSummaryCache();
}

void Menu::CreateMenu()
{
	char szTemp[256];

	//***********************
	// Main Menu
	//***********************

	mCurrentMenu = &mMainMenu;

	miSMWTitle = new MI_Image(&menu_smw, 320 - ((short)menu_smw.getWidth() >> 1), 30, 0, 0, 372, 140, 1, 1, 0);
	miSMWVersion = new MI_Image(&menu_version, 570, 10, 0, 0, 58, 32, 1, 1, 0);
	miSMWVersionText = new MI_Text("Alpha", 630, 45, 0, 2, 2);
	
	miMainStartButton = new MI_Button(&spr_selectfield, 120, 210, "Start", 400, 0);
	miMainStartButton->SetCode(MENU_CODE_TO_MATCH_SELECTION_MENU);

	miPlayerSelect = new MI_PlayerSelect(&menu_player_select, 120, 250, "Players", 400, 140);
	
	miOptionsButton = new MI_Button(&spr_selectfield, 120, 322, "Options", 400, 0);
	miOptionsButton->SetCode(MENU_CODE_TO_OPTIONS_MENU);

	miControlsButton = new MI_Button(&spr_selectfield, 120, 362, "Controls", 400, 0);
	miControlsButton->SetCode(MENU_CODE_TO_CONTROLS_MENU);

	miExitButton = new MI_Button(&spr_selectfield, 120, 402, "Exit", 400, 0);
	miExitButton->SetCode(MENU_CODE_EXIT_APPLICATION);

	mMainMenu.AddControl(miMainStartButton, miExitButton, miPlayerSelect, NULL, NULL);
	mMainMenu.AddControl(miPlayerSelect, miMainStartButton, miOptionsButton, NULL, NULL);
	mMainMenu.AddControl(miOptionsButton, miPlayerSelect, miControlsButton, NULL, NULL);
	mMainMenu.AddControl(miControlsButton, miOptionsButton, miExitButton, NULL, NULL);
	mMainMenu.AddControl(miExitButton, miControlsButton, miMainStartButton, NULL, NULL);
	
	mMainMenu.SetHeadControl(miMainStartButton);
#ifndef _XBOX
	mMainMenu.SetCancelCode(MENU_CODE_EXIT_APPLICATION);
#endif

	mMainMenu.AddNonControl(miSMWTitle);
	mMainMenu.AddNonControl(miSMWVersion);
	mMainMenu.AddNonControl(miSMWVersionText);

	//***********************
	// Player Control Select Menu
	//***********************
	
	miPlayer1ControlsButton = new MI_Button(&spr_selectfield, 120, 140, "Player 1", 400, 1);
	miPlayer1ControlsButton->SetCode(MENU_CODE_TO_PLAYER_1_CONTROLS);

	miPlayer2ControlsButton = new MI_Button(&spr_selectfield, 120, 180, "Player 2", 400, 1);
	miPlayer2ControlsButton->SetCode(MENU_CODE_TO_PLAYER_2_CONTROLS);

	miPlayer3ControlsButton = new MI_Button(&spr_selectfield, 120, 220, "Player 3", 400, 1);
	miPlayer3ControlsButton->SetCode(MENU_CODE_TO_PLAYER_3_CONTROLS);

	miPlayer4ControlsButton = new MI_Button(&spr_selectfield, 120, 260, "Player 4", 400, 1);
	miPlayer4ControlsButton->SetCode(MENU_CODE_TO_PLAYER_4_CONTROLS);

	miPlayerControlsBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miPlayerControlsBackButton->SetCode(MENU_CODE_TO_MAIN_MENU);
	
	miPlayerControlsLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miPlayerControlsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miPlayerControlsMenuHeaderText = new MI_Text("Player Controls Menu", 320, 5, 0, 2, 1);

	mPlayerControlsSelectMenu.AddControl(miPlayer1ControlsButton, miPlayerControlsBackButton, miPlayer2ControlsButton, NULL, miPlayerControlsBackButton);
	mPlayerControlsSelectMenu.AddControl(miPlayer2ControlsButton, miPlayer1ControlsButton, miPlayer3ControlsButton, NULL, miPlayerControlsBackButton);
	mPlayerControlsSelectMenu.AddControl(miPlayer3ControlsButton, miPlayer2ControlsButton, miPlayer4ControlsButton, NULL, miPlayerControlsBackButton);
	mPlayerControlsSelectMenu.AddControl(miPlayer4ControlsButton, miPlayer3ControlsButton, miPlayerControlsBackButton, NULL, miPlayerControlsBackButton);
	mPlayerControlsSelectMenu.AddControl(miPlayerControlsBackButton, miPlayer4ControlsButton, miPlayer1ControlsButton, miPlayer1ControlsButton, NULL);

	mPlayerControlsSelectMenu.AddNonControl(miPlayerControlsLeftHeaderBar);
	mPlayerControlsSelectMenu.AddNonControl(miPlayerControlsMenuRightHeaderBar);
	mPlayerControlsSelectMenu.AddNonControl(miPlayerControlsMenuHeaderText);
	
	mPlayerControlsSelectMenu.SetHeadControl(miPlayer1ControlsButton);
	mPlayerControlsSelectMenu.SetCancelCode(MENU_CODE_TO_MAIN_MENU);

	//***********************
	// Player Controls Menu
	//***********************

	miInputContainer = new MI_InputControlContainer(&menu_plain_field, 94, 10, 0);
	miInputContainer->SetAutoModify(true);

	mPlayerControlsMenu.AddControl(miInputContainer, NULL, NULL, NULL, NULL);
	mPlayerControlsMenu.SetHeadControl(miInputContainer);
	mPlayerControlsMenu.SetCancelCode(MENU_CODE_BACK_TO_CONTROLS_MENU);


	//***********************
	// Options Menu
	//***********************

	miGameplayOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 80, "Gameplay", 400, 1);
	miGameplayOptionsMenuButton->SetCode(MENU_CODE_TO_GAMEPLAY_OPTIONS_MENU);
	
	miTeamOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 120, "Team", 400, 1);
	miTeamOptionsMenuButton->SetCode(MENU_CODE_TO_TEAM_OPTIONS_MENU);

	miPowerupOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 160, "Item Selection", 400, 1);
	miPowerupOptionsMenuButton->SetCode(MENU_CODE_TO_POWERUP_SELECTION_MENU);

	miPowerupSettingsMenuButton = new MI_Button(&spr_selectfield, 120, 200, "Item Settings", 400, 1);
	miPowerupSettingsMenuButton->SetCode(MENU_CODE_TO_POWERUP_SETTINGS_MENU);

	miProjectilesOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 240, "Weapons & Projectiles", 400, 1);
	miProjectilesOptionsMenuButton->SetCode(MENU_CODE_TO_PROJECTILES_OPTIONS_MENU);

	miGraphicsOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 280, "Graphics", 400, 1);
	miGraphicsOptionsMenuButton->SetCode(MENU_CODE_TO_GRAPHICS_OPTIONS_MENU);
	
	miSoundOptionsMenuButton = new MI_Button((game_values.soundcapable ? &spr_selectfield : &spr_selectfielddisabled), 120, 320, "Sound", 400, 1);
	
	if(game_values.soundcapable)
		miSoundOptionsMenuButton->SetCode(MENU_CODE_TO_SOUND_OPTIONS_MENU);

	miGenerateMapThumbsButton = new MI_Button(&spr_selectfield, 120, 360, "Refresh Maps", 400, 1);
	miGenerateMapThumbsButton->SetCode(MENU_CODE_SAVE_ALL_MAP_THUMBNAILS);

	miOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miOptionsMenuBackButton->SetCode(MENU_CODE_TO_MAIN_MENU);

	miOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miOptionsMenuHeaderText = new MI_Text("Options Menu", 320, 5, 0, 2, 1);

	miGenerateThumbsDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miGenerateThumbsDialogAreYouText = new MI_Text("Are You", 320, 195, 0, 2, 1);
	miGenerateThumbsDialogSureText = new MI_Text("Sure?", 320, 220, 0, 2, 1);
	miGenerateThumbsDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miGenerateThumbsDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);
	
	miGenerateThumbsDialogYesButton->SetCode(MENU_CODE_GENERATE_THUMBS_RESET_YES);
	miGenerateThumbsDialogNoButton->SetCode(MENU_CODE_GENERATE_THUMBS_RESET_NO);

	miGenerateThumbsDialogImage->Show(false);
	miGenerateThumbsDialogAreYouText->Show(false);
	miGenerateThumbsDialogSureText->Show(false);
	miGenerateThumbsDialogYesButton->Show(false);
	miGenerateThumbsDialogNoButton->Show(false);

	mOptionsMenu.AddControl(miGameplayOptionsMenuButton, miOptionsMenuBackButton, miTeamOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miTeamOptionsMenuButton, miGameplayOptionsMenuButton, miPowerupOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miPowerupOptionsMenuButton, miTeamOptionsMenuButton, miPowerupSettingsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miPowerupSettingsMenuButton, miPowerupOptionsMenuButton, miProjectilesOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miProjectilesOptionsMenuButton, miPowerupSettingsMenuButton, miGraphicsOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miGraphicsOptionsMenuButton, miProjectilesOptionsMenuButton, miSoundOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miSoundOptionsMenuButton, miGraphicsOptionsMenuButton, miGenerateMapThumbsButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miGenerateMapThumbsButton, miSoundOptionsMenuButton, miOptionsMenuBackButton, NULL, miOptionsMenuBackButton);

	mOptionsMenu.AddControl(miOptionsMenuBackButton, miGenerateMapThumbsButton, miGameplayOptionsMenuButton, miGenerateMapThumbsButton, NULL);
	
	mOptionsMenu.AddNonControl(miOptionsMenuLeftHeaderBar);
	mOptionsMenu.AddNonControl(miOptionsMenuRightHeaderBar);
	mOptionsMenu.AddNonControl(miOptionsMenuHeaderText);

	mOptionsMenu.AddNonControl(miGenerateThumbsDialogImage);
	mOptionsMenu.AddNonControl(miGenerateThumbsDialogAreYouText);
	mOptionsMenu.AddNonControl(miGenerateThumbsDialogSureText);

	mOptionsMenu.AddControl(miGenerateThumbsDialogYesButton, NULL, NULL, NULL, miGenerateThumbsDialogNoButton);
	mOptionsMenu.AddControl(miGenerateThumbsDialogNoButton, NULL, NULL, miGenerateThumbsDialogYesButton, NULL);

	mOptionsMenu.SetHeadControl(miGameplayOptionsMenuButton);
	mOptionsMenu.SetCancelCode(MENU_CODE_TO_MAIN_MENU);


	//***********************
	// Graphics Options
	//***********************

	miSpawnStyleField = new MI_SelectField(&spr_selectfield, 120, 60, "Spawn", 400, 150);
	miSpawnStyleField->Add("Instant", 0, "", false, false);
	miSpawnStyleField->Add("Door", 1, "", false, false);
	miSpawnStyleField->Add("Swirl", 2, "", false, false);
	miSpawnStyleField->SetData(&game_values.spawnstyle, NULL, NULL);
	miSpawnStyleField->SetKey(game_values.spawnstyle);

	miAwardStyleField = new MI_SelectField(&spr_selectfield, 120, 100, "Awards", 400, 150);
	miAwardStyleField->Add("None", 0, "", false, false);
	miAwardStyleField->Add("Fireworks", 1, "", false, false);
	miAwardStyleField->Add("Spiral", 2, "", false, false);
	miAwardStyleField->Add("Ring", 3, "", false, false);
	miAwardStyleField->Add("Souls", 4, "", false, false);
	miAwardStyleField->Add("Text", 5, "", false, false);
	miAwardStyleField->SetData(&game_values.awardstyle, NULL, NULL);
	miAwardStyleField->SetKey(game_values.awardstyle);

	miScoreStyleField = new MI_SelectField(&spr_selectfield, 120, 140, "Scores", 400, 150);
	miScoreStyleField->Add("Top", 0, "", false, false);
	miScoreStyleField->Add("Bottom", 1, "", false, false);
	miScoreStyleField->Add("Corners", 2, "", false, false);
	miScoreStyleField->SetData(&game_values.scoreboardstyle, NULL, NULL);
	miScoreStyleField->SetKey(game_values.scoreboardstyle);

	miCrunchField = new MI_SelectField(&spr_selectfield, 120, 180, "Crunch", 400, 150);
	miCrunchField->Add("Off", 0, "", false, false);
	miCrunchField->Add("On", 1, "", true, false);
	miCrunchField->SetData(NULL, NULL, &game_values.screencrunch);
	miCrunchField->SetKey(game_values.screencrunch ? 1 : 0);
	miCrunchField->SetAutoAdvance(true);

	miTopLayerField = new MI_SelectField(&spr_selectfield, 120, 220, "Top Layer", 400, 150);
	miTopLayerField->Add("Background", 0, "", false, false);
	miTopLayerField->Add("Foreground", 1, "", true, false);
	miTopLayerField->SetData(NULL, NULL, &game_values.toplayer);
	miTopLayerField->SetKey(game_values.toplayer ? 1 : 0);
	miTopLayerField->SetAutoAdvance(true);

	miWinningCrownField = new MI_SelectField(&spr_selectfield, 120, 260, "Crown", 400, 150);
	miWinningCrownField->Add("Off", 0, "", false, false);
	miWinningCrownField->Add("On", 1, "", true, false);
	miWinningCrownField->SetData(NULL, NULL, &game_values.showwinningcrown);
	miWinningCrownField->SetKey(game_values.showwinningcrown ? 1 : 0);
	miWinningCrownField->SetAutoAdvance(true);

#ifdef _XBOX
	miScreenSettingsButton = new MI_Button(&spr_selectfield, 120, 300, "Screen Settings", 400, 0);
	miScreenSettingsButton->SetCode(MENU_CODE_TO_SCREEN_SETTINGS);
#else
	miFullscreenField = new MI_SelectField(&spr_selectfield, 120, 300, "Screen", 400, 150);
	miFullscreenField->Add("Windowed", 0, "", false, false);
	miFullscreenField->Add("Fullscreen", 1, "", true, false);
	miFullscreenField->SetData(NULL, NULL, &game_values.fullscreen);
	miFullscreenField->SetKey(game_values.fullscreen ? 1 : 0);
	miFullscreenField->SetAutoAdvance(true);
	miFullscreenField->SetItemChangedCode(MENU_CODE_TOGGLE_FULLSCREEN);
#endif //_XBOX

	miMenuGraphicsPackField = new MI_PacksField(&spr_selectfield, 120, 340, "Menu Gfx", 400, 150, &menugraphicspacklist, MENU_CODE_MENU_GRAPHICS_PACK_CHANGED);
	miGameGraphicsPackField = new MI_PacksField(&spr_selectfield, 120, 380, "Game Gfx", 400, 150, &gamegraphicspacklist, MENU_CODE_GAME_GRAPHICS_PACK_CHANGED);

	miGraphicsOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miGraphicsOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miGraphicsOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miGraphicsOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miGraphicsOptionsMenuHeaderText = new MI_Text("Graphics Options Menu", 320, 5, 0, 2, 1);

	mGraphicsOptionsMenu.AddControl(miSpawnStyleField, miGraphicsOptionsMenuBackButton, miAwardStyleField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miAwardStyleField, miSpawnStyleField, miScoreStyleField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miScoreStyleField, miAwardStyleField, miCrunchField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miCrunchField, miScoreStyleField, miTopLayerField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miTopLayerField, miCrunchField, miWinningCrownField, NULL, miGraphicsOptionsMenuBackButton);
#ifdef _XBOX
	mGraphicsOptionsMenu.AddControl(miWinningCrownField, miTopLayerField, miScreenSettingsButton, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miScreenSettingsButton, miWinningCrownField, miMenuGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miMenuGraphicsPackField, miScreenSettingsButton, miGameGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
#else
	mGraphicsOptionsMenu.AddControl(miWinningCrownField, miTopLayerField, miFullscreenField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miFullscreenField, miWinningCrownField, miMenuGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miMenuGraphicsPackField, miFullscreenField, miGameGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
#endif

	mGraphicsOptionsMenu.AddControl(miGameGraphicsPackField, miMenuGraphicsPackField, miGraphicsOptionsMenuBackButton, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miGraphicsOptionsMenuBackButton, miGameGraphicsPackField, miSpawnStyleField, miGameGraphicsPackField, NULL);

	mGraphicsOptionsMenu.AddNonControl(miGraphicsOptionsMenuLeftHeaderBar);
	mGraphicsOptionsMenu.AddNonControl(miGraphicsOptionsMenuRightHeaderBar);
	mGraphicsOptionsMenu.AddNonControl(miGraphicsOptionsMenuHeaderText);

	mGraphicsOptionsMenu.SetHeadControl(miSpawnStyleField);
	mGraphicsOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Team Options
	//***********************


	miTeamKillsField = new MI_SelectField(&spr_selectfield, 120, 200, "Kills", 400, 150);
	miTeamKillsField->Add("Off", 0, "", false, false);
	miTeamKillsField->Add("On", 1, "", true, false);
	miTeamKillsField->SetData(NULL, NULL, &game_values.friendlyfire);
	miTeamKillsField->SetKey(game_values.friendlyfire ? 1 : 0);
	miTeamKillsField->SetAutoAdvance(true);

	miTeamColorsField = new MI_SelectField(&spr_selectfield, 120, 240, "Colors", 400, 150);
	miTeamColorsField->Add("Off", 0, "", false, false);
	miTeamColorsField->Add("On", 1, "", true, false);
	miTeamColorsField->SetData(NULL, NULL, &game_values.teamcolors);
	miTeamColorsField->SetKey(game_values.teamcolors ? 1 : 0);
	miTeamColorsField->SetAutoAdvance(true);

	miTeamOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miTeamOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miTeamOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miTeamOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miTeamOptionsMenuHeaderText = new MI_Text("Team Options Menu", 320, 5, 0, 2, 1);

	mTeamOptionsMenu.AddControl(miTeamKillsField, miTeamOptionsMenuBackButton, miTeamColorsField, NULL, miTeamOptionsMenuBackButton);
	mTeamOptionsMenu.AddControl(miTeamColorsField, miTeamKillsField, miTeamOptionsMenuBackButton, NULL, miTeamOptionsMenuBackButton);
	mTeamOptionsMenu.AddControl(miTeamOptionsMenuBackButton, miTeamColorsField, miTeamKillsField, miTeamColorsField, NULL);

	mTeamOptionsMenu.AddNonControl(miTeamOptionsMenuLeftHeaderBar);
	mTeamOptionsMenu.AddNonControl(miTeamOptionsMenuRightHeaderBar);
	mTeamOptionsMenu.AddNonControl(miTeamOptionsMenuHeaderText);

	mTeamOptionsMenu.SetHeadControl(miTeamKillsField);
	mTeamOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Gameplay Options
	//***********************

	miRespawnField = new MI_SelectField(&spr_selectfield, 120, 80, "Respawn", 400, 180);
	miRespawnField->Add("Instant", 0, "", false, false);
	miRespawnField->Add("0.5 Seconds", 1, "", false, false);
	miRespawnField->Add("1.0 Seconds", 2, "", false, false);
	miRespawnField->Add("1.5 Seconds", 3, "", false, false);
	miRespawnField->Add("2.0 Seconds", 4, "", false, false);
	miRespawnField->Add("2.5 Seconds", 5, "", false, false);
	miRespawnField->Add("3.0 Seconds", 6, "", false, false);
	miRespawnField->Add("3.5 Seconds", 7, "", false, false);
	miRespawnField->Add("4.0 Seconds", 8, "", false, false);
	miRespawnField->Add("4.5 Seconds", 9, "", false, false);
	miRespawnField->Add("5.0 Seconds", 10, "", false, false);
	miRespawnField->Add("5.5 Seconds", 11, "", false, false);
	miRespawnField->Add("6.0 Seconds", 12, "", false, false);
	miRespawnField->Add("6.5 Seconds", 13, "", false, false);
	miRespawnField->Add("7.0 Seconds", 14, "", false, false);
	miRespawnField->Add("7.5 Seconds", 15, "", false, false);
	miRespawnField->Add("8.0 Seconds", 16, "", false, false);
	miRespawnField->Add("8.5 Seconds", 17, "", false, false);
	miRespawnField->Add("9.0 Seconds", 18, "", false, false);
	miRespawnField->Add("9.5 Seconds", 19, "", false, false);
	miRespawnField->Add("10.0 Seconds", 20, "", false, false);
	miRespawnField->SetData(&game_values.respawn, NULL, NULL);
	miRespawnField->SetKey(game_values.respawn);

	miShieldField = new MI_SelectField(&spr_selectfield, 120, 120, "Shield", 400, 180);
	miShieldField->Add("None", 0, "", false, false);
	miShieldField->Add("0.5 Seconds", 31, "", false, false);
	miShieldField->Add("1.0 Seconds", 62, "", false, false);
	miShieldField->Add("1.5 Seconds", 93, "", false, false);
	miShieldField->Add("2.0 Seconds", 124, "", false, false);
	miShieldField->Add("2.5 Seconds", 155, "", false, false);
	miShieldField->Add("3.0 Seconds", 186, "", false, false);
	miShieldField->Add("3.5 Seconds", 217, "", false, false);
	miShieldField->Add("4.0 Seconds", 248, "", false, false);
	miShieldField->Add("4.5 Seconds", 279, "", false, false);
	miShieldField->Add("5.0 Seconds", 310, "", false, false);
	miShieldField->SetData(&game_values.spawninvincibility, NULL, NULL);
	miShieldField->SetKey(game_values.spawninvincibility);

	miBoundsTimeField = new MI_SelectField(&spr_selectfield, 120, 160, "Bounds Time", 400, 180);
	miBoundsTimeField->Add("Infinite", 0, "", false, false);
	miBoundsTimeField->Add("1 Second", 1, "", false, false);
	miBoundsTimeField->Add("2 Seconds", 2, "", false, false);
	miBoundsTimeField->Add("3 Seconds", 3, "", false, false);
	miBoundsTimeField->Add("4 Seconds", 4, "", false, false);
	miBoundsTimeField->Add("5 Seconds", 5, "", false, false);
	miBoundsTimeField->Add("6 Seconds", 6, "", false, false);
	miBoundsTimeField->Add("7 Seconds", 7, "", false, false);
	miBoundsTimeField->Add("8 Seconds", 8, "", false, false);
	miBoundsTimeField->Add("9 Seconds", 9, "", false, false);
	miBoundsTimeField->Add("10 Seconds", 10, "", false, false);
	miBoundsTimeField->SetData(&game_values.outofboundstime, NULL, NULL);
	miBoundsTimeField->SetKey(game_values.outofboundstime);

	miWarpLocksField = new MI_SelectField(&spr_selectfield, 120, 200, "Warp Locks", 400, 180);
	miWarpLocksField->Add("Off", 0, "", false, false);
	miWarpLocksField->Add("1 Second", 62, "", false, false);
	miWarpLocksField->Add("2 Seconds", 124, "", false, false);
	miWarpLocksField->Add("3 Seconds", 186, "", false, false);
	miWarpLocksField->Add("4 Seconds", 248, "", false, false);
	miWarpLocksField->Add("5 Seconds", 310, "", false, false);
	miWarpLocksField->Add("6 Seconds", 372, "", false, false);
	miWarpLocksField->Add("7 Seconds", 434, "", false, false);
	miWarpLocksField->Add("8 Seconds", 496, "", false, false);
	miWarpLocksField->Add("9 Seconds", 558, "", false, false);
	miWarpLocksField->Add("10 Seconds", 620, "", false, false);
	miWarpLocksField->SetData(&game_values.warplocks, NULL, NULL);
	miWarpLocksField->SetKey(game_values.warplocks);

	miBotsField = new MI_SelectField(&spr_selectfield, 120, 240, "Bots", 400, 180);
	miBotsField->Add("Very Easy", 0, "", false, false);
	miBotsField->Add("Easy", 1, "", false, false);
	miBotsField->Add("Moderate", 2, "", false, false);
	miBotsField->Add("Hard", 3, "", false, false);
	miBotsField->Add("Very Hard", 4, "", false, false);
	miBotsField->SetData(&game_values.cpudifficulty, NULL, NULL);
	miBotsField->SetKey(game_values.cpudifficulty);

	miFrameLimiterField = new MI_SelectField(&spr_selectfield, 120, 280, "Frame Limit", 400, 180);
	miFrameLimiterField->Add("10 FPS", 100, "", false, false);
	miFrameLimiterField->Add("15 FPS", 67, "", false, false);
	miFrameLimiterField->Add("20 FPS", 50, "", false, false);
	miFrameLimiterField->Add("25 FPS", 40, "", false, false);
	miFrameLimiterField->Add("30 FPS", 33, "", false, false);
	miFrameLimiterField->Add("35 FPS", 28, "", false, false);
	miFrameLimiterField->Add("40 FPS", 25, "", false, false);
	miFrameLimiterField->Add("45 FPS", 22, "", false, false);
	miFrameLimiterField->Add("50 FPS", 20, "", false, false);
	miFrameLimiterField->Add("55 FPS", 18, "", false, false);
	miFrameLimiterField->Add("62 FPS (Normal)", 16, "", false, false);
	miFrameLimiterField->Add("66 FPS", 15, "", false, false);
	miFrameLimiterField->Add("71 FPS", 14, "", false, false);
	miFrameLimiterField->Add("77 FPS", 13, "", false, false);
	miFrameLimiterField->Add("83 FPS", 12, "", false, false);
	miFrameLimiterField->Add("90 FPS", 11, "", false, false);
	miFrameLimiterField->Add("100 FPS", 10, "", false, false);
	miFrameLimiterField->Add("111 FPS", 9, "", false, false);
	miFrameLimiterField->Add("125 FPS", 8, "", false, false);
	miFrameLimiterField->Add("142 FPS", 7, "", false, false);
	miFrameLimiterField->Add("166 FPS", 6, "", false, false);
	miFrameLimiterField->Add("200 FPS", 5, "", false, false);
	miFrameLimiterField->Add("250 FPS", 4, "", false, false);
	miFrameLimiterField->Add("333 FPS", 3, "", false, false);
	miFrameLimiterField->Add("500 FPS", 2, "", false, false);
	miFrameLimiterField->Add("No Limit", 0, "", false, false);
	miFrameLimiterField->SetData(&game_values.framelimiter, NULL, NULL);
	miFrameLimiterField->SetKey(game_values.framelimiter);

	miPointSpeedField = new MI_SelectField(&spr_selectfield, 120, 320, "Point Speed", 400, 180);
	miPointSpeedField->Add("Very Slow", 60, "", false, false);
	miPointSpeedField->Add("Slow", 40, "", false, false);
	miPointSpeedField->Add("Moderate", 20, "", false, false);
	miPointSpeedField->Add("Fast", 10, "", false, false);
	miPointSpeedField->Add("Very Fast", 5, "", false, false);
	miPointSpeedField->SetData(&game_values.pointspeed, NULL, NULL);
	miPointSpeedField->SetKey(game_values.pointspeed);
	
	miSecretsField = new MI_SelectField(&spr_selectfield, 120, 360, "Secrets", 400, 180);
	miSecretsField->Add("Off", 0, "", false, false);
	miSecretsField->Add("On", 1, "", true, false);
	miSecretsField->SetData(NULL, NULL, &game_values.secrets);
	miSecretsField->SetKey(game_values.secrets ? 1 : 0);
	miSecretsField->SetAutoAdvance(true);

	miGameplayOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miGameplayOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miGameplayOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miGameplayOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miGameplayOptionsMenuHeaderText = new MI_Text("Gameplay Options Menu", 320, 5, 0, 2, 1);

	mGameplayOptionsMenu.AddControl(miRespawnField, miGameplayOptionsMenuBackButton, miShieldField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miShieldField, miRespawnField, miBoundsTimeField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miBoundsTimeField, miShieldField, miWarpLocksField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miWarpLocksField, miBoundsTimeField, miBotsField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miBotsField, miWarpLocksField, miFrameLimiterField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miFrameLimiterField, miBotsField, miPointSpeedField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miPointSpeedField, miFrameLimiterField, miSecretsField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miSecretsField, miPointSpeedField, miGameplayOptionsMenuBackButton, NULL, miGameplayOptionsMenuBackButton);
	
	mGameplayOptionsMenu.AddControl(miGameplayOptionsMenuBackButton, miSecretsField, miRespawnField, miSecretsField, NULL);

	mGameplayOptionsMenu.AddNonControl(miGameplayOptionsMenuLeftHeaderBar);
	mGameplayOptionsMenu.AddNonControl(miGameplayOptionsMenuRightHeaderBar);
	mGameplayOptionsMenu.AddNonControl(miGameplayOptionsMenuHeaderText);

	mGameplayOptionsMenu.SetHeadControl(miRespawnField);
	mGameplayOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Sound Options
	//***********************

	miSoundVolumeField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 120, 120, "Sound", 400, 150, 384);
	miSoundVolumeField->Add("Off", 0, "", false, false);
	miSoundVolumeField->Add("1", 16, "", false, false);
	miSoundVolumeField->Add("2", 32, "", false, false);
	miSoundVolumeField->Add("3", 48, "", false, false);
	miSoundVolumeField->Add("4", 64, "", false, false);
	miSoundVolumeField->Add("5", 80, "", false, false);
	miSoundVolumeField->Add("6", 96, "", false, false);
	miSoundVolumeField->Add("7", 112, "", false, false);
	miSoundVolumeField->Add("Max", 128, "", false, false);
	miSoundVolumeField->SetData(&game_values.soundvolume, NULL, NULL);
	miSoundVolumeField->SetKey(game_values.soundvolume);
	miSoundVolumeField->SetNoWrap(true);
	miSoundVolumeField->SetItemChangedCode(MENU_CODE_SOUND_VOLUME_CHANGED);
	
	miMusicVolumeField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 120, 160, "Music", 400, 150, 384);
	miMusicVolumeField->Add("Off", 0, "", false, false);
	miMusicVolumeField->Add("1", 16, "", false, false);
	miMusicVolumeField->Add("2", 32, "", false, false);
	miMusicVolumeField->Add("3", 48, "", false, false);
	miMusicVolumeField->Add("4", 64, "", false, false);
	miMusicVolumeField->Add("5", 80, "", false, false);
	miMusicVolumeField->Add("6", 96, "", false, false);
	miMusicVolumeField->Add("7", 112, "", false, false);
	miMusicVolumeField->Add("Max", 128, "", false, false);
	miMusicVolumeField->SetData(&game_values.musicvolume, NULL, NULL);
	miMusicVolumeField->SetKey(game_values.musicvolume);
	miMusicVolumeField->SetNoWrap(true);
	miMusicVolumeField->SetItemChangedCode(MENU_CODE_MUSIC_VOLUME_CHANGED);

	miPlayNextMusicField = new MI_SelectField(&spr_selectfield, 120, 200, "Next Music", 400, 150);
	miPlayNextMusicField->Add("Off", 0, "", false, false);
	miPlayNextMusicField->Add("On", 1, "", true, false);
	miPlayNextMusicField->SetData(NULL, NULL, &game_values.playnextmusic);
	miPlayNextMusicField->SetKey(game_values.playnextmusic ? 1 : 0);
	miPlayNextMusicField->SetAutoAdvance(true);

	miAnnouncerField = new MI_AnnouncerField(&spr_selectfield, 120, 240, "Announcer", 400, 150, &announcerlist);
	miPlaylistField = new MI_PlaylistField(&spr_selectfield, 120, 280, "Playlist", 400, 150);
	miSoundPackField = new MI_PacksField(&spr_selectfield, 120, 320, "Sfx Pack", 400, 150, &soundpacklist, MENU_CODE_SOUND_PACK_CHANGED);

	miSoundOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miSoundOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miSoundOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miSoundOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miSoundOptionsMenuHeaderText = new MI_Text("Sound Options Menu", 320, 5, 0, 2, 1);

	mSoundOptionsMenu.AddControl(miSoundVolumeField, miSoundOptionsMenuBackButton, miMusicVolumeField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miMusicVolumeField, miSoundVolumeField, miPlayNextMusicField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miPlayNextMusicField, miMusicVolumeField, miAnnouncerField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miAnnouncerField, miPlayNextMusicField, miPlaylistField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miPlaylistField, miAnnouncerField, miSoundPackField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miSoundPackField, miPlaylistField, miSoundOptionsMenuBackButton, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miSoundOptionsMenuBackButton, miSoundPackField, miSoundVolumeField, miSoundPackField, NULL);
	
	mSoundOptionsMenu.AddNonControl(miSoundOptionsMenuLeftHeaderBar);
	mSoundOptionsMenu.AddNonControl(miSoundOptionsMenuRightHeaderBar);
	mSoundOptionsMenu.AddNonControl(miSoundOptionsMenuHeaderText);

	mSoundOptionsMenu.SetHeadControl(miSoundVolumeField);
	mSoundOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);


	//***********************
	// Projectile Options
	//***********************

	miFireballLifeField = new MI_SelectField(&spr_selectfield, 10, 80, "Life", 305, 105);
	miFireballLifeField->Add("1 Second", 62, "", false, false);
	miFireballLifeField->Add("2 Seconds", 124, "", false, false);
	miFireballLifeField->Add("3 Seconds", 186, "", false, false);
	miFireballLifeField->Add("4 Seconds", 248, "", false, false);
	miFireballLifeField->Add("5 Seconds", 310, "", false, false);
	miFireballLifeField->Add("6 Seconds", 372, "", false, false);
	miFireballLifeField->Add("7 Seconds", 434, "", false, false);
	miFireballLifeField->Add("8 Seconds", 496, "", false, false);
	miFireballLifeField->Add("9 Seconds", 558, "", false, false);
	miFireballLifeField->Add("10 Seconds", 620, "", false, false);
	miFireballLifeField->SetData(&game_values.fireballttl, NULL, NULL);
	miFireballLifeField->SetKey(game_values.fireballttl);

	miFireballLimitField = new MI_SelectField(&spr_selectfield, 10, 120, "Limit", 305, 105);
	miFireballLimitField->Add("Unlimited", 0, "", false, false);
	miFireballLimitField->Add("2", 2, "", false, false);
	miFireballLimitField->Add("5", 5, "", false, false);
	miFireballLimitField->Add("8", 8, "", false, false);
	miFireballLimitField->Add("10", 10, "", false, false);
	miFireballLimitField->Add("12", 12, "", false, false);
	miFireballLimitField->Add("15", 15, "", false, false);
	miFireballLimitField->Add("20", 20, "", false, false);
	miFireballLimitField->Add("25", 25, "", false, false);
	miFireballLimitField->Add("30", 30, "", false, false);
	miFireballLimitField->Add("40", 40, "", false, false);
	miFireballLimitField->Add("50", 50, "", false, false);
	miFireballLimitField->SetData(&game_values.fireballlimit, NULL, NULL);
	miFireballLimitField->SetKey(game_values.fireballlimit);

	miFeatherJumpsField = new MI_SelectField(&spr_selectfield, 10, 200, "Jumps", 305, 105);
	miFeatherJumpsField->Add("1", 1, "", false, false);
	miFeatherJumpsField->Add("2", 2, "", false, false);
	miFeatherJumpsField->Add("3", 3, "", false, false);
	miFeatherJumpsField->Add("4", 4, "", false, false);
	miFeatherJumpsField->Add("5", 5, "", false, false);
	miFeatherJumpsField->SetData(&game_values.featherjumps, NULL, NULL);
	miFeatherJumpsField->SetKey(game_values.featherjumps);

	miFeatherLimitField = new MI_SelectField(&spr_selectfield, 10, 240, "Limit", 305, 105);
	miFeatherLimitField->Add("Unlimited", 0, "", false, false);
	miFeatherLimitField->Add("2", 2, "", false, false);
	miFeatherLimitField->Add("5", 5, "", false, false);
	miFeatherLimitField->Add("8", 8, "", false, false);
	miFeatherLimitField->Add("10", 10, "", false, false);
	miFeatherLimitField->Add("12", 12, "", false, false);
	miFeatherLimitField->Add("15", 15, "", false, false);
	miFeatherLimitField->Add("20", 20, "", false, false);
	miFeatherLimitField->Add("25", 25, "", false, false);
	miFeatherLimitField->Add("30", 30, "", false, false);
	miFeatherLimitField->Add("40", 40, "", false, false);
	miFeatherLimitField->Add("50", 50, "", false, false);
	miFeatherLimitField->SetData(&game_values.featherlimit, NULL, NULL);
	miFeatherLimitField->SetKey(game_values.featherlimit);

	miBoomerangStyleField = new MI_SelectField(&spr_selectfield, 10, 320, "Style", 305, 105);
	miBoomerangStyleField->Add("Flat", 0, "", false, false);
	miBoomerangStyleField->Add("SMB3", 1, "", false, false);
	miBoomerangStyleField->Add("Zelda", 2, "", false, false);
	miBoomerangStyleField->Add("Random", 3, "", false, false);
	miBoomerangStyleField->SetData(&game_values.boomerangstyle, NULL, NULL);
	miBoomerangStyleField->SetKey(game_values.boomerangstyle);

	miBoomerangLifeField = new MI_SelectField(&spr_selectfield, 10, 360, "Life", 305, 105);
	miBoomerangLifeField->Add("1 Second", 62, "", false, false);
	miBoomerangLifeField->Add("2 Seconds", 124, "", false, false);
	miBoomerangLifeField->Add("3 Seconds", 186, "", false, false);
	miBoomerangLifeField->Add("4 Seconds", 248, "", false, false);
	miBoomerangLifeField->Add("5 Seconds", 310, "", false, false);
	miBoomerangLifeField->Add("6 Seconds", 372, "", false, false);
	miBoomerangLifeField->Add("7 Seconds", 434, "", false, false);
	miBoomerangLifeField->Add("8 Seconds", 496, "", false, false);
	miBoomerangLifeField->Add("9 Seconds", 558, "", false, false);
	miBoomerangLifeField->Add("10 Seconds", 620, "", false, false);
	miBoomerangLifeField->SetData(&game_values.boomeranglife, NULL, NULL);
	miBoomerangLifeField->SetKey(game_values.boomeranglife);

	miBoomerangLimitField = new MI_SelectField(&spr_selectfield, 10, 400, "Limit", 305, 105);
	miBoomerangLimitField->Add("Unlimited", 0, "", false, false);
	miBoomerangLimitField->Add("2", 2, "", false, false);
	miBoomerangLimitField->Add("5", 5, "", false, false);
	miBoomerangLimitField->Add("8", 8, "", false, false);
	miBoomerangLimitField->Add("10", 10, "", false, false);
	miBoomerangLimitField->Add("12", 12, "", false, false);
	miBoomerangLimitField->Add("15", 15, "", false, false);
	miBoomerangLimitField->Add("20", 20, "", false, false);
	miBoomerangLimitField->Add("25", 25, "", false, false);
	miBoomerangLimitField->Add("30", 30, "", false, false);
	miBoomerangLimitField->Add("40", 40, "", false, false);
	miBoomerangLimitField->Add("50", 50, "", false, false);
	miBoomerangLimitField->SetData(&game_values.boomeranglimit, NULL, NULL);
	miBoomerangLimitField->SetKey(game_values.boomeranglimit);

	miHammerLifeField = new MI_SelectField(&spr_selectfield, 325, 80, "Life", 305, 105);
	miHammerLifeField->Add("No Limit", 310, "", false, false);
	miHammerLifeField->Add("0.5 Seconds", 31, "", false, false);
	miHammerLifeField->Add("0.6 Seconds", 37, "", false, false);
	miHammerLifeField->Add("0.7 Seconds", 43, "", false, false);
	miHammerLifeField->Add("0.8 Seconds", 49, "", false, false);
	miHammerLifeField->Add("0.9 Seconds", 55, "", false, false);
	miHammerLifeField->Add("1.0 Seconds", 62, "", false, false);
	miHammerLifeField->Add("1.1 Seconds", 68, "", false, false);
	miHammerLifeField->Add("1.2 Seconds", 74, "", false, false);
	miHammerLifeField->SetData(&game_values.hammerttl, NULL, NULL);
	miHammerLifeField->SetKey(game_values.hammerttl);

	miHammerDelayField = new MI_SelectField(&spr_selectfield, 325, 120, "Delay", 305, 105);
	miHammerDelayField->Add("None", 0, "", false, false);
	miHammerDelayField->Add("0.1 Seconds", 6, "", false, false);
	miHammerDelayField->Add("0.2 Seconds", 12, "", false, false);
	miHammerDelayField->Add("0.3 Seconds", 19, "", false, false);
	miHammerDelayField->Add("0.4 Seconds", 25, "", false, false);
	miHammerDelayField->Add("0.5 Seconds", 31, "", false, false);
	miHammerDelayField->Add("0.6 Seconds", 37, "", false, false);
	miHammerDelayField->Add("0.7 Seconds", 43, "", false, false);
	miHammerDelayField->Add("0.8 Seconds", 49, "", false, false);
	miHammerDelayField->Add("0.9 Seconds", 55, "", false, false);
	miHammerDelayField->Add("1.0 Seconds", 62, "", false, false);
	miHammerDelayField->SetData(&game_values.hammerdelay, NULL, NULL);
	miHammerDelayField->SetKey(game_values.hammerdelay);

	miHammerOneKillField = new MI_SelectField(&spr_selectfield, 325, 160, "Power", 305, 105);
	miHammerOneKillField->Add("One Kill", 0, "", true, false);
	miHammerOneKillField->Add("Multiple Kills", 1, "", false, false);
	miHammerOneKillField->SetData(NULL, NULL, &game_values.hammerpower);
	miHammerOneKillField->SetKey(game_values.hammerpower ? 0 : 1);
	miHammerOneKillField->SetAutoAdvance(true);

	miHammerLimitField = new MI_SelectField(&spr_selectfield, 325, 200, "Limit", 305, 105);
	miHammerLimitField->Add("Unlimited", 0, "", false, false);
	miHammerLimitField->Add("2", 2, "", false, false);
	miHammerLimitField->Add("5", 5, "", false, false);
	miHammerLimitField->Add("8", 8, "", false, false);
	miHammerLimitField->Add("10", 10, "", false, false);
	miHammerLimitField->Add("12", 12, "", false, false);
	miHammerLimitField->Add("15", 15, "", false, false);
	miHammerLimitField->Add("20", 20, "", false, false);
	miHammerLimitField->Add("25", 25, "", false, false);
	miHammerLimitField->Add("30", 30, "", false, false);
	miHammerLimitField->Add("40", 40, "", false, false);
	miHammerLimitField->Add("50", 50, "", false, false);
	miHammerLimitField->SetData(&game_values.hammerlimit, NULL, NULL);
	miHammerLimitField->SetKey(game_values.hammerlimit);

	miShellLifeField = new MI_SelectField(&spr_selectfield, 325, 280, "Life", 305, 105);
	miShellLifeField->Add("Unlimited", 0, "", false, false);
	miShellLifeField->Add("1 Second", 62, "", false, false);
	miShellLifeField->Add("2 Seconds", 124, "", false, false);
	miShellLifeField->Add("3 Seconds", 186, "", false, false);
	miShellLifeField->Add("4 Seconds", 248, "", false, false);
	miShellLifeField->Add("5 Seconds", 310, "", false, false);
	miShellLifeField->Add("6 Seconds", 372, "", false, false);
	miShellLifeField->Add("7 Seconds", 434, "", false, false);
	miShellLifeField->Add("8 Seconds", 496, "", false, false);
	miShellLifeField->Add("9 Seconds", 558, "", false, false);
	miShellLifeField->Add("10 Seconds", 620, "", false, false);
	miShellLifeField->Add("15 Seconds", 930, "", false, false);
	miShellLifeField->Add("20 Seconds", 1240, "", false, false);
	miShellLifeField->Add("25 Seconds", 1550, "", false, false);
	miShellLifeField->Add("30 Seconds", 1860, "", false, false);
	miShellLifeField->SetData(&game_values.shellttl, NULL, NULL);
	miShellLifeField->SetKey(game_values.shellttl);

	miBlueBlockLifeField = new MI_SelectField(&spr_selectfield, 325, 360, "Life", 305, 105);
	miBlueBlockLifeField->Add("Unlimited", 0, "", false, false);
	miBlueBlockLifeField->Add("1 Second", 62, "", false, false);
	miBlueBlockLifeField->Add("2 Seconds", 124, "", false, false);
	miBlueBlockLifeField->Add("3 Seconds", 186, "", false, false);
	miBlueBlockLifeField->Add("4 Seconds", 248, "", false, false);
	miBlueBlockLifeField->Add("5 Seconds", 310, "", false, false);
	miBlueBlockLifeField->Add("6 Seconds", 372, "", false, false);
	miBlueBlockLifeField->Add("7 Seconds", 434, "", false, false);
	miBlueBlockLifeField->Add("8 Seconds", 496, "", false, false);
	miBlueBlockLifeField->Add("9 Seconds", 558, "", false, false);
	miBlueBlockLifeField->Add("10 Seconds", 620, "", false, false);
	miBlueBlockLifeField->Add("15 Seconds", 930, "", false, false);
	miBlueBlockLifeField->Add("20 Seconds", 1240, "", false, false);
	miBlueBlockLifeField->Add("25 Seconds", 1550, "", false, false);
	miBlueBlockLifeField->Add("30 Seconds", 1860, "", false, false);
	miBlueBlockLifeField->SetData(&game_values.blueblockttl, NULL, NULL);
	miBlueBlockLifeField->SetKey(game_values.blueblockttl);

	miProjectilesOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miProjectilesOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miProjectilesOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miProjectilesOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miProjectilesOptionsMenuHeaderText = new MI_Text("Projectile & Weapon Options Menu", 320, 5, 0, 2, 1);

	miFireballText = new MI_Text("Fireball", 10, 50, 0, 2, 0);
	miFeatherText = new MI_Text("Feather", 10, 170, 0, 2, 0);
	miBoomerangText = new MI_Text("Boomerang", 10, 290, 0, 2, 0);
	miHammerText = new MI_Text("Hammer", 325, 50, 0, 2, 0);
	miShellText = new MI_Text("Shell", 325, 250, 0, 2, 0);
	miBlueBlockText = new MI_Text("Blue Block", 325, 330, 0, 2, 0);

	mProjectilesOptionsMenu.AddControl(miFireballLifeField, miProjectilesOptionsMenuBackButton, miFireballLimitField, NULL, miHammerLifeField);
	mProjectilesOptionsMenu.AddControl(miFireballLimitField, miFireballLifeField, miFeatherJumpsField, NULL, miHammerDelayField);
	
	mProjectilesOptionsMenu.AddControl(miFeatherJumpsField, miFireballLimitField, miFeatherLimitField, NULL, miHammerLimitField);
	mProjectilesOptionsMenu.AddControl(miFeatherLimitField, miFeatherJumpsField, miBoomerangStyleField, NULL, miShellLifeField);
	
	mProjectilesOptionsMenu.AddControl(miBoomerangStyleField, miFeatherLimitField, miBoomerangLifeField, NULL, miBlueBlockLifeField);
	mProjectilesOptionsMenu.AddControl(miBoomerangLifeField, miBoomerangStyleField, miBoomerangLimitField, NULL, miBlueBlockLifeField);
	mProjectilesOptionsMenu.AddControl(miBoomerangLimitField, miBoomerangLifeField, miHammerLifeField, NULL, miBlueBlockLifeField);

	mProjectilesOptionsMenu.AddControl(miHammerLifeField, miBoomerangLimitField, miHammerDelayField, miFireballLifeField, NULL);
	mProjectilesOptionsMenu.AddControl(miHammerDelayField, miHammerLifeField, miHammerOneKillField, miFireballLimitField, NULL);
	mProjectilesOptionsMenu.AddControl(miHammerOneKillField, miHammerDelayField, miHammerLimitField, miFireballLimitField, NULL);
	mProjectilesOptionsMenu.AddControl(miHammerLimitField, miHammerOneKillField, miShellLifeField, miFeatherJumpsField, NULL);
	
	mProjectilesOptionsMenu.AddControl(miShellLifeField, miHammerLimitField, miBlueBlockLifeField, miFeatherLimitField, NULL);
	mProjectilesOptionsMenu.AddControl(miBlueBlockLifeField, miShellLifeField, miProjectilesOptionsMenuBackButton, miBoomerangLifeField, NULL);

	mProjectilesOptionsMenu.AddControl(miProjectilesOptionsMenuBackButton, miBlueBlockLifeField, miFireballLifeField, miBlueBlockLifeField, NULL);

	mProjectilesOptionsMenu.AddNonControl(miFireballText);
	mProjectilesOptionsMenu.AddNonControl(miFeatherText);
	mProjectilesOptionsMenu.AddNonControl(miBoomerangText);
	mProjectilesOptionsMenu.AddNonControl(miHammerText);
	mProjectilesOptionsMenu.AddNonControl(miShellText);
	mProjectilesOptionsMenu.AddNonControl(miBlueBlockText);

	mProjectilesOptionsMenu.AddNonControl(miProjectilesOptionsMenuLeftHeaderBar);
	mProjectilesOptionsMenu.AddNonControl(miProjectilesOptionsMenuRightHeaderBar);
	mProjectilesOptionsMenu.AddNonControl(miProjectilesOptionsMenuHeaderText);

	mProjectilesOptionsMenu.SetHeadControl(miFireballLifeField);
	mProjectilesOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);


#ifdef _XBOX
	//***********************
	// Screen Settings
	//***********************

	MI_Button * miScreenResizeButton;
	miScreenResizeButton = new MI_Button(&spr_selectfield, 120, 160, "Resize Screen", 400, 0);
	miScreenResizeButton->SetCode(MENU_CODE_TO_SCREEN_RESIZE);

	miScreenHardwareFilterField = new MI_SelectField(&spr_selectfield, 120, 200, "Screen Filter", 400, 180);
	//miScreenHardwareFilterField->Add("None", 0, "", false, false);
	miScreenHardwareFilterField->Add("Point", 1, "", false, false);
	miScreenHardwareFilterField->Add("Bilinear", 2, "", false, false);
	miScreenHardwareFilterField->Add("Trilinear", 3, "", false, false);
	miScreenHardwareFilterField->Add("Anisotrpoic", 4, "", false, false);
	miScreenHardwareFilterField->Add("Quincunx", 5, "", false, false);
	miScreenHardwareFilterField->Add("Gaussian Cubic", 6, "", false, false);
	miScreenHardwareFilterField->SetData(&game_values.hardwarefilter, NULL, NULL);
	miScreenHardwareFilterField->SetKey(game_values.hardwarefilter);
	miScreenHardwareFilterField->SetItemChangedCode(MENU_CODE_SCREEN_FILTER_CHANGED);

	miScreenFlickerFilterField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 120, 240, "Flicker Filter", 400, 180, 380);
	miScreenFlickerFilterField->Add("0", 0, "", false, false);
	miScreenFlickerFilterField->Add("1", 1, "", false, false);
	miScreenFlickerFilterField->Add("2", 2, "", false, false);
	miScreenFlickerFilterField->Add("3", 3, "", false, false);
	miScreenFlickerFilterField->Add("4", 4, "", false, false);
	miScreenFlickerFilterField->Add("5", 5, "", false, false);
	miScreenFlickerFilterField->SetData(&game_values.flickerfilter, NULL, NULL);
	miScreenFlickerFilterField->SetKey(game_values.flickerfilter);
	miScreenFlickerFilterField->SetNoWrap(true);
	miScreenFlickerFilterField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

	miScreenSoftFilterField = new MI_SelectField(&spr_selectfield, 120, 280, "Soften Filter", 400, 180);
	miScreenSoftFilterField->Add("Off", 0, "", false, false);
	miScreenSoftFilterField->Add("On", 1, "", true, false);
	miScreenSoftFilterField->SetData(&game_values.softfilter, NULL, NULL);
	miScreenSoftFilterField->SetKey(game_values.softfilter);
	miScreenSoftFilterField->SetAutoAdvance(true);
	miScreenSoftFilterField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

	/*
	miScreenAspectRatioField = new MI_SelectField(&spr_selectfield, 120, 300, "10x11 Aspect", 400, 180);
	miScreenAspectRatioField->Add("Off", 0, "", false, false);
	miScreenAspectRatioField->Add("On", 1, "", true, false);
	miScreenAspectRatioField->SetData(NULL, NULL, &game_values.aspectratio10x11);
	miScreenAspectRatioField->SetKey(game_values.aspectratio10x11 ? 1 : 0);
	miScreenAspectRatioField->SetAutoAdvance(true);
	miScreenAspectRatioField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);
	*/

	miScreenSettingsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miScreenSettingsMenuBackButton->SetCode(MENU_CODE_BACK_TO_GRAPHIC_OPTIONS_MENU);

	miScreenSettingsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miScreenSettingsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miScreenSettingsMenuHeaderText = new MI_Text("Screen Settings Menu", 320, 5, 0, 2, 1);

	mScreenSettingsMenu.AddNonControl(miScreenSettingsMenuLeftHeaderBar);
	mScreenSettingsMenu.AddNonControl(miScreenSettingsMenuRightHeaderBar);
	mScreenSettingsMenu.AddNonControl(miScreenSettingsMenuHeaderText);

	mScreenSettingsMenu.AddControl(miScreenResizeButton, miScreenSettingsMenuBackButton, miScreenHardwareFilterField, NULL, miScreenSettingsMenuBackButton);
	mScreenSettingsMenu.AddControl(miScreenHardwareFilterField, miScreenResizeButton, miScreenFlickerFilterField, NULL, miScreenSettingsMenuBackButton);
	mScreenSettingsMenu.AddControl(miScreenFlickerFilterField, miScreenHardwareFilterField, miScreenSoftFilterField, NULL, miScreenSettingsMenuBackButton);
	mScreenSettingsMenu.AddControl(miScreenSoftFilterField, miScreenFlickerFilterField, miScreenSettingsMenuBackButton, NULL, miScreenSettingsMenuBackButton);
	//mScreenSettingsMenu.AddControl(miScreenAspectRatioField, miScreenSoftFilterField, miScreenSettingsMenuBackButton, NULL, miScreenSettingsMenuBackButton);
	mScreenSettingsMenu.AddControl(miScreenSettingsMenuBackButton, miScreenSoftFilterField, miScreenResizeButton, miScreenSoftFilterField, NULL);

	mScreenSettingsMenu.SetHeadControl(miScreenResizeButton);
	mScreenSettingsMenu.SetCancelCode(MENU_CODE_BACK_TO_GRAPHIC_OPTIONS_MENU);

	//***********************
	// Screen Resize
	//***********************

	miScreenResize = new MI_ScreenResize();

	mScreenResizeMenu.AddControl(miScreenResize, NULL, NULL, NULL, NULL);
	mScreenResizeMenu.SetHeadControl(miScreenResize);
	mScreenResizeMenu.SetCancelCode(MENU_CODE_BACK_TO_SCREEN_SETTINGS_MENU);
#endif

	//***********************
	// Match Selection Menu
	//***********************

	miMatchSelectionStartButton = new MI_Button(&spr_selectfield, 270, 420, "Start", 100, 0);
	miMatchSelectionStartButton->SetCode(MENU_CODE_MATCH_SELECTION_START);

	miMatchSelectionField = new MI_SelectField(&spr_selectfield, 130, 340, "Match", 380, 100);
	miMatchSelectionField->Add("Single Game", 0, "", false, false);
	miMatchSelectionField->Add("Tournament", 1, "", false, false);
	miMatchSelectionField->Add("Tour", 2, "", false, false);
	miMatchSelectionField->Add("World", 3, "", false, false);
	miMatchSelectionField->SetData(&game_values.matchtype, NULL, NULL);
	miMatchSelectionField->SetKey(game_values.matchtype);
	miMatchSelectionField->SetItemChangedCode(MENU_CODE_MATCH_SELECTION_MATCH_CHANGED);

	miTournamentField = new MI_SelectField(&spr_selectfield, 130, 380, "Wins", 380, 100);
	miTournamentField->Add("2", 2, "", false, false);
	miTournamentField->Add("3", 3, "", false, false);
	miTournamentField->Add("4", 4, "", false, false);
	miTournamentField->Add("5", 5, "", false, false);
	miTournamentField->Add("6", 6, "", false, false);
	miTournamentField->Add("7", 7, "", false, false);
	miTournamentField->Add("8", 8, "", false, false);
	miTournamentField->Add("9", 9, "", false, false);
	miTournamentField->Add("10", 10, "", false, false);
	miTournamentField->SetData(&game_values.tournamentgames, NULL, NULL);
	miTournamentField->SetKey(game_values.tournamentgames);
	miTournamentField->Show(false);
	
	miTourField = new MI_SelectField(&spr_selectfield, 130, 380, "Tour", 380, 100);
	for(short iTour = 0; iTour < tourlist.GetCount(); iTour++)
	{
		GetNameFromFileName(szTemp, tourlist.GetIndex(iTour));
		//strcat(szTemp, " Tour");
		miTourField->Add(szTemp, iTour, "", true, false);
	}
	miTourField->SetData(&game_values.tourindex, NULL, NULL);
	miTourField->SetKey(game_values.tourindex);
	miTourField->Show(false);

	miWorldField = new MI_SelectField(&spr_selectfield, 130, 380, "World", 380, 100);
	for(short iWorld = 0; iWorld < worldlist.GetCount(); iWorld++)
	{
		GetNameFromFileName(szTemp, worldlist.GetIndex(iWorld));
		miWorldField->Add(szTemp, iWorld, "", true, false);
	}
	miWorldField->SetData(&game_values.worldindex, NULL, NULL);
	miWorldField->SetKey(game_values.worldindex);
	miWorldField->Show(false);

	miMatchSelectionMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miMatchSelectionMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miMatchSelectionMenuHeaderText = new MI_Text("Match Type Menu", 320, 5, 0, 2, 1);

	miMatchSelectionDisplayImage = new MI_Image(&menu_match_select, 160, 80, 0, 0, 320, 240, 1, 1, 0);

	mMatchSelectionMenu.AddNonControl(miMatchSelectionMenuLeftHeaderBar);
	mMatchSelectionMenu.AddNonControl(miMatchSelectionMenuRightHeaderBar);
	mMatchSelectionMenu.AddNonControl(miMatchSelectionMenuHeaderText);

	mMatchSelectionMenu.AddNonControl(miMatchSelectionDisplayImage);

	mMatchSelectionMenu.AddControl(miMatchSelectionField, miMatchSelectionStartButton, miTournamentField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miTournamentField, miMatchSelectionField, miTourField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miTourField, miTournamentField, miWorldField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miWorldField, miTourField, miMatchSelectionStartButton, NULL, NULL);
	mMatchSelectionMenu.AddControl(miMatchSelectionStartButton, miWorldField, miMatchSelectionField, NULL, NULL);

	mMatchSelectionMenu.SetHeadControl(miMatchSelectionStartButton);
	mMatchSelectionMenu.SetCancelCode(MENU_CODE_TO_MAIN_MENU);

	//***********************
	// Game Settings
	//***********************
	
	miSettingsStartButton = new MI_Button(&spr_selectfield, 70, 45, "Start", 500, 0);
	miSettingsStartButton->SetCode(MENU_CODE_START_GAME);

	miModeField = new MI_ImageSelectField(&spr_selectfield, &menu_mode_small, 70, 85, "Mode", 500, 120, 16, 16);
	
	for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
	{
		miModeField->Add(gamemodes[iGameMode]->GetModeName(), iGameMode, "", false, false);
	}
	miModeField->SetData(&currentgamemode, NULL, NULL);
	miModeField->SetKey(0);
	miModeField->SetItemChangedCode(MENU_CODE_MODE_CHANGED);

	for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
	{
		miGoalField[iGameMode] = new MI_SelectField(&spr_selectfield, 70, 125, gamemodes[iGameMode]->GetGoalName(), 352, 120);
		//miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
		miGoalField[iGameMode]->Show(iGameMode == 0);

		for(short iGameModeOption = 0; iGameModeOption < GAMEMODE_NUM_OPTIONS; iGameModeOption++)
		{
			SModeOption * option = &gamemodes[iGameMode]->GetOptions()[iGameModeOption];
			miGoalField[iGameMode]->Add(option->szName, option->iValue, "", false, false);
		}

		miGoalField[iGameMode]->SetData(&gamemodes[iGameMode]->goal, NULL, NULL);
		miGoalField[iGameMode]->SetKey(gamemodes[iGameMode]->goal);
	}

	miModeSettingsButton = new MI_Button(&spr_selectfield, 430, 125, "Settings", 140, 0);
	miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);

	miMapField = new MI_MapField(&spr_selectfield, 70, 165, "Map", 500, 120, true);
	szCurrentMapName = miMapField->GetMapName();

	miMapFiltersButton = new MI_Button(&spr_selectfield, 430, 205, "Filters", 140, 0);
	miMapFiltersButton->SetCode(MENU_CODE_TO_MAP_FILTERS);

	miMapFiltersOnImage = new MI_Image(&menu_map_filter, 530, 213, 0, 48, 16, 16, 1, 1, 0);
	miMapFiltersOnImage->Show(false);

	miMapThumbnailsButton = new MI_Button(&spr_selectfield, 430, 245, "Thumbs", 140, 0);
	miMapThumbnailsButton->SetCode(MENU_CODE_TO_MAP_BROWSER_THUMBNAILS);

	miMapFilterScroll = new MI_MapFilterScroll(&menu_plain_field, 120, 72, 400, 9);
	miMapFilterScroll->SetAutoModify(true);
	miMapFilterScroll->Show(false);
	
	//Add auto map filters
	for(short iFilter = 0; iFilter < NUM_AUTO_FILTERS; iFilter++)
	{
		miMapFilterScroll->Add(g_szAutoFilterNames[iFilter], g_iAutoFilterIcons[iFilter]);
	}

	//Add user defined filters
	for(short iFilter = 0; iFilter < filterslist.GetCount(); iFilter++)
	{
		GetNameFromFileName(szTemp, filterslist.GetIndex(iFilter));
		miMapFilterScroll->Add(szTemp, game_values.piFilterIcons[NUM_AUTO_FILTERS + iFilter]);
	}
	

	miGameSettingsLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miGameSettingsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miGameSettingsMenuHeaderText = new MI_Text("Single Game Menu", 320, 5, 0, 2, 1);


	//Exit tournament dialog box
	miGameSettingsExitDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miGameSettingsExitDialogExitText = new MI_Text("Exit", 320, 195, 0, 2, 1);
	miGameSettingsExitDialogTournamentText = new MI_Text("Tournament", 320, 220, 0, 2, 1);
	miGameSettingsExitDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miGameSettingsExitDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);
	
	miGameSettingsExitDialogYesButton->SetCode(MENU_CODE_EXIT_TOURNAMENT_YES);
	miGameSettingsExitDialogNoButton->SetCode(MENU_CODE_EXIT_TOURNAMENT_NO);

	miGameSettingsExitDialogImage->Show(false);
	miGameSettingsExitDialogTournamentText->Show(false);
	miGameSettingsExitDialogExitText->Show(false);
	miGameSettingsExitDialogYesButton->Show(false);
	miGameSettingsExitDialogNoButton->Show(false);
	
	mGameSettingsMenu.AddControl(miSettingsStartButton, miMapThumbnailsButton, miModeField, NULL, NULL);
	mGameSettingsMenu.AddControl(miModeField, miSettingsStartButton, miGoalField[0], NULL, NULL);
	mGameSettingsMenu.AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[1], miGoalField[0], miGoalField[2], miGoalField[0], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[2], miGoalField[1], miGoalField[3], miGoalField[1], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[3], miGoalField[2], miGoalField[4], miGoalField[2], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[4], miGoalField[3], miGoalField[5], miGoalField[3], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[5], miGoalField[4], miGoalField[6], miGoalField[4], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[6], miGoalField[5], miGoalField[7], miGoalField[5], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[7], miGoalField[6], miGoalField[8], miGoalField[6], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[8], miGoalField[7], miGoalField[9], miGoalField[7], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[9], miGoalField[8], miGoalField[10], miGoalField[8], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[10], miGoalField[9], miGoalField[11], miGoalField[9], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[11], miGoalField[10], miGoalField[12], miGoalField[10], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[12], miGoalField[11], miGoalField[13], miGoalField[11], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[13], miGoalField[12], miGoalField[14], miGoalField[12], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[14], miGoalField[13], miGoalField[15], miGoalField[13], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[15], miGoalField[14], miGoalField[16], miGoalField[14], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miGoalField[16], miGoalField[15], miMapField, miGoalField[15], miModeSettingsButton);
	mGameSettingsMenu.AddControl(miModeSettingsButton, miModeField, miMapField, miGoalField[16], NULL);
	mGameSettingsMenu.AddControl(miMapField, miGoalField[16], miMapFiltersButton, NULL, NULL);
	mGameSettingsMenu.AddControl(miMapFiltersButton, miMapField, miMapThumbnailsButton, NULL, NULL);
	mGameSettingsMenu.AddControl(miMapThumbnailsButton, miMapFiltersButton, miSettingsStartButton, NULL, NULL);

	mGameSettingsMenu.AddControl(miMapFilterScroll, NULL, NULL, NULL, NULL);
	
	mGameSettingsMenu.AddNonControl(miGameSettingsLeftHeaderBar);
	mGameSettingsMenu.AddNonControl(miGameSettingsMenuRightHeaderBar);
	mGameSettingsMenu.AddNonControl(miGameSettingsMenuHeaderText);
	
	mGameSettingsMenu.AddNonControl(miGameSettingsExitDialogImage);
	mGameSettingsMenu.AddNonControl(miGameSettingsExitDialogExitText);
	mGameSettingsMenu.AddNonControl(miGameSettingsExitDialogTournamentText);

	mGameSettingsMenu.AddNonControl(miMapFiltersOnImage);

	mGameSettingsMenu.AddControl(miGameSettingsExitDialogYesButton, NULL, NULL, NULL, miGameSettingsExitDialogNoButton);
	mGameSettingsMenu.AddControl(miGameSettingsExitDialogNoButton, NULL, NULL, miGameSettingsExitDialogYesButton, NULL);

	mGameSettingsMenu.SetHeadControl(miSettingsStartButton);
	
	mGameSettingsMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

	//***********************
	// Map Filter Edit
	//***********************

	miMapBrowser = new MI_MapBrowser();
	miMapBrowser->SetAutoModify(true);

	mMapFilterEditMenu.AddControl(miMapBrowser, NULL, NULL, NULL, NULL);
	mMapFilterEditMenu.SetHeadControl(miMapBrowser);
	mMapFilterEditMenu.SetCancelCode(MENU_CODE_MAP_BROWSER_EXIT);

	//***********************
	// Tour Stop
	//***********************
	
	miTourStop = new MI_TourStop(70, 45, false);
	
	//Exit tour dialog box
	miTourStopExitDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miTourStopExitDialogExitTourText = new MI_Text("Exit Tour", 320, 205, 0, 2, 1);

	miTourStopExitDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miTourStopExitDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);
	
	miTourStopExitDialogYesButton->SetCode(MENU_CODE_EXIT_TOUR_YES);
	miTourStopExitDialogNoButton->SetCode(MENU_CODE_EXIT_TOUR_NO);

	miTourStopExitDialogImage->Show(false);
	miTourStopExitDialogExitTourText->Show(false);
	miTourStopExitDialogYesButton->Show(false);
	miTourStopExitDialogNoButton->Show(false);
	
	mTourStopMenu.AddControl(miTourStop, NULL, NULL, NULL, NULL);

	mTourStopMenu.AddNonControl(miTourStopExitDialogImage);
	mTourStopMenu.AddNonControl(miTourStopExitDialogExitTourText);

	mTourStopMenu.AddControl(miTourStopExitDialogYesButton, NULL, NULL, NULL, miTourStopExitDialogNoButton);
	mTourStopMenu.AddControl(miTourStopExitDialogNoButton, NULL, NULL, miTourStopExitDialogYesButton, NULL);

	mTourStopMenu.SetHeadControl(miTourStop);
	mTourStopMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);


	//***********************
	// World Menu
	//***********************
	
	miWorld = new MI_World();
	miWorld->SetAutoModify(true);
	
	miWorldStop = new MI_TourStop(70, 45, true);
	miWorldStop->Show(false);

	//Exit tour dialog box
	miWorldExitDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miWorldExitDialogExitTourText = new MI_Text("Exit World", 320, 205, 0, 2, 1);

	miWorldExitDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miWorldExitDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);
	
	miWorldExitDialogYesButton->SetCode(MENU_CODE_EXIT_WORLD_YES);
	miWorldExitDialogNoButton->SetCode(MENU_CODE_EXIT_WORLD_NO);

	miWorldExitDialogImage->Show(false);
	miWorldExitDialogExitTourText->Show(false);
	miWorldExitDialogYesButton->Show(false);
	miWorldExitDialogNoButton->Show(false);
	
	mWorldMenu.AddControl(miWorld, NULL, NULL, NULL, NULL);
	
	mWorldMenu.AddControl(miWorldStop, NULL, NULL, NULL, NULL);

	mWorldMenu.AddNonControl(miWorldExitDialogImage);
	mWorldMenu.AddNonControl(miWorldExitDialogExitTourText);

	mWorldMenu.AddControl(miWorldExitDialogYesButton, NULL, NULL, NULL, miWorldExitDialogNoButton);
	mWorldMenu.AddControl(miWorldExitDialogNoButton, NULL, NULL, miWorldExitDialogYesButton, NULL);

	mWorldMenu.SetHeadControl(miWorld);
	mWorldMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);


	//***********************
	// Jail Mode Settings
	//***********************

	miJailModeTimeFreeField = new MI_SelectField(&spr_selectfield, 120, 200, "Free Timer", 400, 180);
	miJailModeTimeFreeField->Add("None", 1, "", false, false);
	miJailModeTimeFreeField->Add("5 Seconds", 310, "", false, false);
	miJailModeTimeFreeField->Add("10 Seconds", 620, "", false, false);
	miJailModeTimeFreeField->Add("15 Seconds", 930, "", false, false);
	miJailModeTimeFreeField->Add("20 Seconds", 1240, "", false, false);
	miJailModeTimeFreeField->Add("25 Seconds", 1550, "", false, false);
	miJailModeTimeFreeField->Add("30 Seconds", 1860, "", false, false);
	miJailModeTimeFreeField->Add("35 Seconds", 2170, "", false, false);
	miJailModeTimeFreeField->Add("40 Seconds", 2480, "", false, false);
	miJailModeTimeFreeField->Add("45 Seconds", 2790, "", false, false);
	miJailModeTimeFreeField->Add("50 Seconds", 3100, "", false, false);
	miJailModeTimeFreeField->Add("55 Seconds", 3410, "", false, false);
	miJailModeTimeFreeField->Add("60 Seconds", 3720, "", false, false);
	miJailModeTimeFreeField->SetData(&game_values.gamemodemenusettings.jail.timetofree, NULL, NULL);
	miJailModeTimeFreeField->SetKey(game_values.gamemodemenusettings.jail.timetofree);

	miJailModeTagFreeField = new MI_SelectField(&spr_selectfield, 120, 240, "Tag Free", 400, 180);
	miJailModeTagFreeField->Add("Off", 0, "", false, false);
	miJailModeTagFreeField->Add("On", 1, "", true, false);
	miJailModeTagFreeField->SetData(NULL, NULL, &game_values.gamemodemenusettings.jail.tagfree);
	miJailModeTagFreeField->SetKey(game_values.gamemodemenusettings.jail.tagfree ? 1 : 0);
	miJailModeTagFreeField->SetAutoAdvance(true);

	miJailModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miJailModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miJailModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miJailModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miJailModeHeaderText = new MI_Text("Jail Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[3].AddControl(miJailModeTimeFreeField, miJailModeBackButton, miJailModeTagFreeField, NULL, miJailModeBackButton);
	mModeSettingsMenu[3].AddControl(miJailModeTagFreeField, miJailModeTimeFreeField, miJailModeBackButton, NULL, miJailModeBackButton);
	mModeSettingsMenu[3].AddControl(miJailModeBackButton, miJailModeTagFreeField, miJailModeTimeFreeField, miJailModeTagFreeField, NULL);
	
	mModeSettingsMenu[3].AddNonControl(miJailModeLeftHeaderBar);
	mModeSettingsMenu[3].AddNonControl(miJailModeRightHeaderBar);
	mModeSettingsMenu[3].AddNonControl(miJailModeHeaderText);
	
	mModeSettingsMenu[3].SetHeadControl(miJailModeTimeFreeField);
	mModeSettingsMenu[3].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	
	//***********************
	// Coins Mode Settings
	//***********************

	miCoinModePenaltyField = new MI_SelectField(&spr_selectfield, 120, 200, "Penalty", 400, 180);
	miCoinModePenaltyField->Add("Off", 0, "", false, false);
	miCoinModePenaltyField->Add("On", 1, "", true, false);
	miCoinModePenaltyField->SetData(NULL, NULL, &game_values.gamemodemenusettings.coins.penalty);
	miCoinModePenaltyField->SetKey(game_values.gamemodemenusettings.coins.penalty ? 1 : 0);
	miCoinModePenaltyField->SetAutoAdvance(true);

	miCoinModeQuantityField = new MI_SelectField(&spr_selectfield, 120, 240, "Quantity", 400, 180);
	miCoinModeQuantityField->Add("1", 1, "", false, false);
	miCoinModeQuantityField->Add("2", 2, "", false, false);
	miCoinModeQuantityField->Add("3", 3, "", false, false);
	miCoinModeQuantityField->Add("4", 4, "", false, false);
	miCoinModeQuantityField->Add("5", 5, "", false, false);
	miCoinModeQuantityField->SetData(&game_values.gamemodemenusettings.coins.quantity, NULL, NULL);
	miCoinModeQuantityField->SetKey(game_values.gamemodemenusettings.coins.quantity);

	miCoinModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miCoinModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miCoinModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miCoinModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miCoinModeHeaderText = new MI_Text("Coin Collection Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[4].AddControl(miCoinModePenaltyField, miCoinModeBackButton, miCoinModeQuantityField, NULL, miCoinModeBackButton);
	mModeSettingsMenu[4].AddControl(miCoinModeQuantityField, miCoinModePenaltyField, miCoinModeBackButton, NULL, miCoinModeBackButton);
	mModeSettingsMenu[4].AddControl(miCoinModeBackButton, miCoinModeQuantityField, miCoinModePenaltyField, miCoinModeQuantityField, NULL);
	
	mModeSettingsMenu[4].AddNonControl(miCoinModeLeftHeaderBar);
	mModeSettingsMenu[4].AddNonControl(miCoinModeRightHeaderBar);
	mModeSettingsMenu[4].AddNonControl(miCoinModeHeaderText);
	
	mModeSettingsMenu[4].SetHeadControl(miCoinModePenaltyField);
	mModeSettingsMenu[4].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);
	
	//***********************
	// Flag Mode Settings
	//***********************

	miFlagModeSpeedField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 120, 140, "Speed", 400, 180, 380);
	miFlagModeSpeedField->Add("0", 0, "", false, false);
	miFlagModeSpeedField->Add("1", 1, "", false, false);
	miFlagModeSpeedField->Add("2", 2, "", false, false);
	miFlagModeSpeedField->Add("3", 3, "", false, false);
	miFlagModeSpeedField->Add("4", 4, "", false, false);
	miFlagModeSpeedField->Add("5", 5, "", false, false);
	miFlagModeSpeedField->Add("6", 6, "", false, false);
	miFlagModeSpeedField->Add("7", 7, "", false, false);
	miFlagModeSpeedField->Add("8", 8, "", false, false);
	miFlagModeSpeedField->SetData(&game_values.gamemodemenusettings.flag.speed, NULL, NULL);
	miFlagModeSpeedField->SetKey(game_values.gamemodemenusettings.flag.speed);
	miFlagModeSpeedField->SetNoWrap(true);

	miFlagModeTouchReturnField = new MI_SelectField(&spr_selectfield, 120, 180, "Touch Return", 400, 180);
	miFlagModeTouchReturnField->Add("Off", 0, "", false, false);
	miFlagModeTouchReturnField->Add("On", 1, "", true, false);
	miFlagModeTouchReturnField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.touchreturn);
	miFlagModeTouchReturnField->SetKey(game_values.gamemodemenusettings.flag.touchreturn ? 1 : 0);
	miFlagModeTouchReturnField->SetAutoAdvance(true);

	miFlagModePointMoveField = new MI_SelectField(&spr_selectfield, 120, 220, "Point Move", 400, 180);
	miFlagModePointMoveField->Add("Off", 0, "", false, false);
	miFlagModePointMoveField->Add("On", 1, "", true, false);
	miFlagModePointMoveField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.pointmove);
	miFlagModePointMoveField->SetKey(game_values.gamemodemenusettings.flag.pointmove ? 1 : 0);
	miFlagModePointMoveField->SetAutoAdvance(true);

	miFlagModeAutoReturnField = new MI_SelectField(&spr_selectfield, 120, 260, "Auto Return", 400, 180);
	miFlagModeAutoReturnField->Add("None", 0, "", false, false);
	miFlagModeAutoReturnField->Add("5 Seconds", 310, "", false, false);
	miFlagModeAutoReturnField->Add("10 Seconds", 620, "", false, false);
	miFlagModeAutoReturnField->Add("15 Seconds", 930, "", false, false);
	miFlagModeAutoReturnField->Add("20 Seconds", 1240, "", false, false);
	miFlagModeAutoReturnField->Add("25 Seconds", 1550, "", false, false);
	miFlagModeAutoReturnField->Add("30 Seconds", 1860, "", false, false);
	miFlagModeAutoReturnField->Add("35 Seconds", 2170, "", false, false);
	miFlagModeAutoReturnField->Add("40 Seconds", 2480, "", false, false);
	miFlagModeAutoReturnField->Add("45 Seconds", 2790, "", false, false);
	miFlagModeAutoReturnField->Add("50 Seconds", 3100, "", false, false);
	miFlagModeAutoReturnField->Add("55 Seconds", 3410, "", false, false);
	miFlagModeAutoReturnField->Add("60 Seconds", 3720, "", false, false);
	miFlagModeAutoReturnField->SetData(&game_values.gamemodemenusettings.flag.autoreturn, NULL, NULL);
	miFlagModeAutoReturnField->SetKey(game_values.gamemodemenusettings.flag.autoreturn);

	miFlagModeHomeScoreField = new MI_SelectField(&spr_selectfield, 120, 300, "Need Home", 400, 180);
	miFlagModeHomeScoreField->Add("Off", 0, "", false, false);
	miFlagModeHomeScoreField->Add("On", 1, "", true, false);
	miFlagModeHomeScoreField->SetData(NULL, NULL, &game_values.gamemodemenusettings.flag.homescore);
	miFlagModeHomeScoreField->SetKey(game_values.gamemodemenusettings.flag.homescore ? 1 : 0);
	miFlagModeHomeScoreField->SetAutoAdvance(true);
	
	miFlagModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miFlagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miFlagModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miFlagModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miFlagModeHeaderText = new MI_Text("Capture The Flag Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[7].AddControl(miFlagModeSpeedField, miFlagModeBackButton, miFlagModeTouchReturnField, NULL, miFlagModeBackButton);
	mModeSettingsMenu[7].AddControl(miFlagModeTouchReturnField, miFlagModeSpeedField, miFlagModePointMoveField, NULL, miFlagModeBackButton);
	mModeSettingsMenu[7].AddControl(miFlagModePointMoveField, miFlagModeTouchReturnField, miFlagModeAutoReturnField, NULL, miFlagModeBackButton);
	mModeSettingsMenu[7].AddControl(miFlagModeAutoReturnField, miFlagModePointMoveField, miFlagModeHomeScoreField, NULL, miFlagModeBackButton);
	mModeSettingsMenu[7].AddControl(miFlagModeHomeScoreField, miFlagModeAutoReturnField, miFlagModeBackButton, NULL, miFlagModeBackButton);
	
	mModeSettingsMenu[7].AddControl(miFlagModeBackButton, miFlagModeHomeScoreField, miFlagModeSpeedField, miFlagModeHomeScoreField, NULL);
	
	mModeSettingsMenu[7].AddNonControl(miFlagModeLeftHeaderBar);
	mModeSettingsMenu[7].AddNonControl(miFlagModeRightHeaderBar);
	mModeSettingsMenu[7].AddNonControl(miFlagModeHeaderText);
	
	mModeSettingsMenu[7].SetHeadControl(miFlagModeSpeedField);
	mModeSettingsMenu[7].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);



	//***********************
	// Chicken Mode Settings
	//***********************

	miChickenModeShowTargetField = new MI_SelectField(&spr_selectfield, 120, 220, "Show Target", 400, 180);
	miChickenModeShowTargetField->Add("Off", 0, "", false, false);
	miChickenModeShowTargetField->Add("On", 1, "", true, false);
	miChickenModeShowTargetField->SetData(NULL, NULL, &game_values.gamemodemenusettings.chicken.usetarget);
	miChickenModeShowTargetField->SetKey(game_values.gamemodemenusettings.chicken.usetarget ? 1 : 0);
	miChickenModeShowTargetField->SetAutoAdvance(true);

	miChickenModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miChickenModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miChickenModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miChickenModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miChickenModeHeaderText = new MI_Text("Chicken Mode Menu", 320, 5, 0, 2, 1);


	mModeSettingsMenu[8].AddControl(miChickenModeShowTargetField, miChickenModeBackButton, miChickenModeBackButton, NULL, miChickenModeBackButton);
	mModeSettingsMenu[8].AddControl(miChickenModeBackButton, miChickenModeShowTargetField, miChickenModeShowTargetField, miChickenModeShowTargetField, NULL);
	
	mModeSettingsMenu[8].AddNonControl(miChickenModeLeftHeaderBar);
	mModeSettingsMenu[8].AddNonControl(miChickenModeRightHeaderBar);
	mModeSettingsMenu[8].AddNonControl(miChickenModeHeaderText);
	
	mModeSettingsMenu[8].SetHeadControl(miChickenModeShowTargetField);
	mModeSettingsMenu[8].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


	//***********************
	// Tag Mode Settings
	//***********************

	miTagModeTagOnTouchField = new MI_SelectField(&spr_selectfield, 120, 220, "Touch Tag", 400, 180);
	miTagModeTagOnTouchField->Add("Off", 0, "", false, false);
	miTagModeTagOnTouchField->Add("On", 1, "", true, false);
	miTagModeTagOnTouchField->SetData(NULL, NULL, &game_values.gamemodemenusettings.tag.tagontouch);
	miTagModeTagOnTouchField->SetKey(game_values.gamemodemenusettings.tag.tagontouch ? 1 : 0);
	miTagModeTagOnTouchField->SetAutoAdvance(true);

	miTagModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miTagModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miTagModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miTagModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miTagModeHeaderText = new MI_Text("Tag Mode Menu", 320, 5, 0, 2, 1);


	mModeSettingsMenu[9].AddControl(miTagModeTagOnTouchField, miTagModeBackButton, miTagModeBackButton, NULL, miTagModeBackButton);
	mModeSettingsMenu[9].AddControl(miTagModeBackButton, miTagModeTagOnTouchField, miTagModeTagOnTouchField, miTagModeTagOnTouchField, NULL);
	
	mModeSettingsMenu[9].AddNonControl(miTagModeLeftHeaderBar);
	mModeSettingsMenu[9].AddNonControl(miTagModeRightHeaderBar);
	mModeSettingsMenu[9].AddNonControl(miTagModeHeaderText);
	
	mModeSettingsMenu[9].SetHeadControl(miTagModeTagOnTouchField);
	mModeSettingsMenu[9].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);



	//***********************
	// Star Mode Settings
	//***********************

	miStarModeTimeField = new MI_SelectField(&spr_selectfield, 120, 200, "Time", 400, 180);
	miStarModeTimeField->Add("5 Seconds", 5, "", false, false);
	miStarModeTimeField->Add("10 Seconds", 10, "", false, false);
	miStarModeTimeField->Add("15 Seconds", 15, "", false, false);
	miStarModeTimeField->Add("20 Seconds", 20, "", false, false);
	miStarModeTimeField->Add("25 Seconds", 25, "", false, false);
	miStarModeTimeField->Add("30 Seconds", 30, "", false, false);
	miStarModeTimeField->Add("35 Seconds", 35, "", false, false);
	miStarModeTimeField->Add("40 Seconds", 40, "", false, false);
	miStarModeTimeField->Add("45 Seconds", 45, "", false, false);
	miStarModeTimeField->Add("50 Seconds", 50, "", false, false);
	miStarModeTimeField->Add("55 Seconds", 55, "", false, false);
	miStarModeTimeField->Add("60 Seconds", 60, "", false, false);
	miStarModeTimeField->SetData(&game_values.gamemodemenusettings.star.time, NULL, NULL);
	miStarModeTimeField->SetKey(game_values.gamemodemenusettings.star.time);

	miStarModeShineField = new MI_SelectField(&spr_selectfield, 120, 240, "Star Type", 400, 180);
	miStarModeShineField->Add("Ztar", 0, "", false, false);
	miStarModeShineField->Add("Shine", 1, "", false, false);
	miStarModeShineField->SetData(&game_values.gamemodemenusettings.star.shine, NULL, NULL);
	miStarModeShineField->SetKey(game_values.gamemodemenusettings.star.shine);
	miStarModeShineField->SetAutoAdvance(true);

	miStarModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miStarModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miStarModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miStarModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miStarModeHeaderText = new MI_Text("Star Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[10].AddControl(miStarModeTimeField, miStarModeBackButton, miStarModeShineField, NULL, miStarModeBackButton);
	mModeSettingsMenu[10].AddControl(miStarModeShineField, miStarModeTimeField, miStarModeBackButton, NULL, miStarModeBackButton);
	mModeSettingsMenu[10].AddControl(miStarModeBackButton, miStarModeShineField, miStarModeTimeField, miStarModeShineField, NULL);
	
	mModeSettingsMenu[10].AddNonControl(miStarModeLeftHeaderBar);
	mModeSettingsMenu[10].AddNonControl(miStarModeRightHeaderBar);
	mModeSettingsMenu[10].AddNonControl(miStarModeHeaderText);
	
	mModeSettingsMenu[10].SetHeadControl(miStarModeTimeField);
	mModeSettingsMenu[10].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);
	

	//***********************
	// Domination Mode Settings
	//***********************

	miDominationModeQuantityField = new MI_SelectField(&spr_selectfield, 120, 120, "Quantity", 400, 180);
	miDominationModeQuantityField->Add("1 Base", 1, "", false, false);
	miDominationModeQuantityField->Add("2 Bases", 2, "", false, false);
	miDominationModeQuantityField->Add("3 Bases", 3, "", false, false);
	miDominationModeQuantityField->Add("4 Bases", 4, "", false, false);
	miDominationModeQuantityField->Add("5 Bases", 5, "", false, false);
	miDominationModeQuantityField->Add("6 Bases", 6, "", false, false);
	miDominationModeQuantityField->Add("7 Bases", 7, "", false, false);
	miDominationModeQuantityField->Add("8 Bases", 8, "", false, false);
	miDominationModeQuantityField->Add("9 Bases", 9, "", false, false);
	miDominationModeQuantityField->Add("10 Bases", 10, "", false, false);
	miDominationModeQuantityField->Add("# Players - 1", 11, "", false, false);
	miDominationModeQuantityField->Add("# Players", 12, "", false, false);
	miDominationModeQuantityField->Add("# Players + 1", 13, "", false, false);
	miDominationModeQuantityField->Add("# Players + 2", 14, "", false, false);
	miDominationModeQuantityField->Add("# Players + 3", 15, "", false, false);
	miDominationModeQuantityField->Add("# Players + 4", 16, "", false, false);
	miDominationModeQuantityField->Add("# Players + 5", 17, "", false, false);
	miDominationModeQuantityField->Add("# Players + 6", 18, "", false, false);
	miDominationModeQuantityField->Add("2x Players - 3", 19, "", false, false);
	miDominationModeQuantityField->Add("2x Players - 2", 20, "", false, false);
	miDominationModeQuantityField->Add("2x Players - 1", 21, "", false, false);
	miDominationModeQuantityField->Add("2x Players", 22, "", false, false);
	miDominationModeQuantityField->Add("2x Players + 1", 23, "", false, false);
	miDominationModeQuantityField->Add("2x Players + 2", 24, "", false, false);
	miDominationModeQuantityField->SetData(&game_values.gamemodemenusettings.domination.quantity, NULL, NULL);
	miDominationModeQuantityField->SetKey(game_values.gamemodemenusettings.domination.quantity);

	miDominationModeRelocateFrequencyField = new MI_SelectField(&spr_selectfield, 120, 160, "Relocate", 400, 180);
	miDominationModeRelocateFrequencyField->Add("Never", 0, "", false, false);
	miDominationModeRelocateFrequencyField->Add("5 Seconds", 310, "", false, false);
	miDominationModeRelocateFrequencyField->Add("10 Seconds", 620, "", false, false);
	miDominationModeRelocateFrequencyField->Add("15 Seconds", 930, "", false, false);
	miDominationModeRelocateFrequencyField->Add("20 Seconds", 1240, "", false, false);
	miDominationModeRelocateFrequencyField->Add("30 Seconds", 1860, "", false, false);
	miDominationModeRelocateFrequencyField->Add("45 Seconds", 2790, "", false, false);
	miDominationModeRelocateFrequencyField->Add("1 Minute", 3720, "", false, false);
	miDominationModeRelocateFrequencyField->Add("1.5 Minutes", 5580, "", false, false);
	miDominationModeRelocateFrequencyField->Add("2 Minutes", 7440, "", false, false);
	miDominationModeRelocateFrequencyField->Add("2.5 Minutes", 9300, "", false, false);
	miDominationModeRelocateFrequencyField->Add("3 Minutes", 11160, "", false, false);
	miDominationModeRelocateFrequencyField->SetData(&game_values.gamemodemenusettings.domination.relocationfrequency, NULL, NULL);
	miDominationModeRelocateFrequencyField->SetKey(game_values.gamemodemenusettings.domination.relocationfrequency);

	miDominationModeDeathText = new MI_Text("On Death", 120, 210, 0, 2, 0);

	miDominationModeLoseOnDeathField = new MI_SelectField(&spr_selectfield, 120, 240, "Lose Bases", 400, 180);
	miDominationModeLoseOnDeathField->Add("Off", 0, "", false, false);
	miDominationModeLoseOnDeathField->Add("On", 1, "", true, false);
	miDominationModeLoseOnDeathField->SetData(NULL, NULL, &game_values.gamemodemenusettings.domination.loseondeath);
	miDominationModeLoseOnDeathField->SetKey(game_values.gamemodemenusettings.domination.loseondeath ? 1 : 0);
	miDominationModeLoseOnDeathField->SetAutoAdvance(true);

	miDominationModeRelocateOnDeathField = new MI_SelectField(&spr_selectfield, 120, 280, "Move Bases", 400, 180);
	miDominationModeRelocateOnDeathField->Add("Off", 0, "", false, false);
	miDominationModeRelocateOnDeathField->Add("On", 1, "", true, false);
	miDominationModeRelocateOnDeathField->SetData(NULL, NULL, &game_values.gamemodemenusettings.domination.relocateondeath);
	miDominationModeRelocateOnDeathField->SetKey(game_values.gamemodemenusettings.domination.relocateondeath ? 1 : 0);
	miDominationModeRelocateOnDeathField->SetAutoAdvance(true);

	miDominationModeStealOnDeathField = new MI_SelectField(&spr_selectfield, 120, 320, "Steal Bases", 400, 180);
	miDominationModeStealOnDeathField->Add("Off", 0, "", false, false);
	miDominationModeStealOnDeathField->Add("On", 1, "", true, false);
	miDominationModeStealOnDeathField->SetData(NULL, NULL, &game_values.gamemodemenusettings.domination.stealondeath);
	miDominationModeStealOnDeathField->SetKey(game_values.gamemodemenusettings.domination.stealondeath ? 1 : 0);
	miDominationModeStealOnDeathField->SetAutoAdvance(true);


	miDominationModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miDominationModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miDominationModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miDominationModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miDominationModeHeaderText = new MI_Text("Domination Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[11].AddControl(miDominationModeQuantityField, miDominationModeBackButton, miDominationModeRelocateFrequencyField, NULL, miDominationModeBackButton);
	mModeSettingsMenu[11].AddControl(miDominationModeRelocateFrequencyField, miDominationModeQuantityField, miDominationModeLoseOnDeathField, NULL, miDominationModeBackButton);
	mModeSettingsMenu[11].AddControl(miDominationModeLoseOnDeathField, miDominationModeRelocateFrequencyField, miDominationModeRelocateOnDeathField, NULL, miDominationModeBackButton);
	mModeSettingsMenu[11].AddControl(miDominationModeRelocateOnDeathField, miDominationModeLoseOnDeathField, miDominationModeStealOnDeathField, NULL, miDominationModeBackButton);
	mModeSettingsMenu[11].AddControl(miDominationModeStealOnDeathField, miDominationModeRelocateOnDeathField, miDominationModeBackButton, NULL, miDominationModeBackButton);
	
	mModeSettingsMenu[11].AddControl(miDominationModeBackButton, miDominationModeStealOnDeathField, miDominationModeQuantityField, miDominationModeStealOnDeathField, NULL);
	
	mModeSettingsMenu[11].AddNonControl(miDominationModeLeftHeaderBar);
	mModeSettingsMenu[11].AddNonControl(miDominationModeRightHeaderBar);
	mModeSettingsMenu[11].AddNonControl(miDominationModeHeaderText);
	mModeSettingsMenu[11].AddNonControl(miDominationModeDeathText);
	
	mModeSettingsMenu[11].SetHeadControl(miDominationModeQuantityField);
	mModeSettingsMenu[11].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);



	//***********************
	// King of the Hill Mode Settings
	//***********************

	miKingOfTheHillModeSizeField = new MI_SelectField(&spr_selectfield, 120, 200, "Size", 400, 180);
	miKingOfTheHillModeSizeField->Add("2 x 2", 2, "", false, false);
	miKingOfTheHillModeSizeField->Add("3 x 3", 3, "", false, false);
	miKingOfTheHillModeSizeField->Add("4 x 4", 4, "", false, false);
	miKingOfTheHillModeSizeField->Add("5 x 5", 5, "", false, false);
	miKingOfTheHillModeSizeField->SetData(&game_values.gamemodemenusettings.kingofthehill.areasize, NULL, NULL);
	miKingOfTheHillModeSizeField->SetKey(game_values.gamemodemenusettings.kingofthehill.areasize);

	miKingOfTheHillModeRelocateFrequencyField = new MI_SelectField(&spr_selectfield, 120, 240, "Relocate", 400, 180);
	miKingOfTheHillModeRelocateFrequencyField->Add("Never", 0, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("5 Seconds", 310, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("10 Seconds", 620, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("15 Seconds", 930, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("20 Seconds", 1240, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("30 Seconds", 1860, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("45 Seconds", 2790, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("1 Minute", 3720, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("1.5 Minutes", 5580, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("2 Minutes", 7440, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("2.5 Minutes", 9300, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->Add("3 Minutes", 11160, "", false, false);
	miKingOfTheHillModeRelocateFrequencyField->SetData(&game_values.gamemodemenusettings.kingofthehill.relocationfrequency, NULL, NULL);
	miKingOfTheHillModeRelocateFrequencyField->SetKey(game_values.gamemodemenusettings.kingofthehill.relocationfrequency);

	miKingOfTheHillModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miKingOfTheHillModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miKingOfTheHillModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miKingOfTheHillModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miKingOfTheHillModeHeaderText = new MI_Text("King of the Hill Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[12].AddControl(miKingOfTheHillModeSizeField, miKingOfTheHillModeBackButton, miKingOfTheHillModeRelocateFrequencyField, NULL, miKingOfTheHillModeBackButton);
	mModeSettingsMenu[12].AddControl(miKingOfTheHillModeRelocateFrequencyField, miKingOfTheHillModeSizeField, miKingOfTheHillModeBackButton, NULL, miKingOfTheHillModeBackButton);
	
	mModeSettingsMenu[12].AddControl(miKingOfTheHillModeBackButton, miKingOfTheHillModeRelocateFrequencyField, miKingOfTheHillModeSizeField, miKingOfTheHillModeRelocateFrequencyField, NULL);
	
	mModeSettingsMenu[12].AddNonControl(miKingOfTheHillModeLeftHeaderBar);
	mModeSettingsMenu[12].AddNonControl(miKingOfTheHillModeRightHeaderBar);
	mModeSettingsMenu[12].AddNonControl(miKingOfTheHillModeHeaderText);
	
	mModeSettingsMenu[12].SetHeadControl(miKingOfTheHillModeSizeField);
	mModeSettingsMenu[12].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

			
	//***********************
	// Race Mode Settings
	//***********************

	miRaceModeQuantityField = new MI_SelectField(&spr_selectfield, 120, 180, "Quantity", 400, 180);
	miRaceModeQuantityField->Add("2", 2, "", false, false);
	miRaceModeQuantityField->Add("3", 3, "", false, false);
	miRaceModeQuantityField->Add("4", 4, "", false, false);
	miRaceModeQuantityField->Add("5", 5, "", false, false);
	miRaceModeQuantityField->Add("6", 6, "", false, false);
	miRaceModeQuantityField->Add("7", 7, "", false, false);
	miRaceModeQuantityField->Add("8", 8, "", false, false);
	miRaceModeQuantityField->SetData(&game_values.gamemodemenusettings.race.quantity, NULL, NULL);
	miRaceModeQuantityField->SetKey(game_values.gamemodemenusettings.race.quantity);

	miRaceModeSpeedField = new MI_SelectField(&spr_selectfield, 120, 220, "Speed", 400, 180);
	miRaceModeSpeedField->Add("Very Slow", 2, "", false, false);
	miRaceModeSpeedField->Add("Slow", 3, "", false, false);
	miRaceModeSpeedField->Add("Moderate", 4, "", false, false);
	miRaceModeSpeedField->Add("Fast", 6, "", false, false);
	miRaceModeSpeedField->Add("Very Fast", 8, "", false, false);
	miRaceModeSpeedField->SetData(&game_values.gamemodemenusettings.race.speed, NULL, NULL);
	miRaceModeSpeedField->SetKey(game_values.gamemodemenusettings.race.speed);

	miRaceModePenaltyField = new MI_SelectField(&spr_selectfield, 120, 260, "Penalty", 400, 180);
	miRaceModePenaltyField->Add("None", 0, "", false, false);
	miRaceModePenaltyField->Add("One Goal", 1, "", false, false);
	miRaceModePenaltyField->Add("All Goals", 2, "", false, false);
	miRaceModePenaltyField->SetData(&game_values.gamemodemenusettings.race.penalty, NULL, NULL);
	miRaceModePenaltyField->SetKey(game_values.gamemodemenusettings.race.penalty);
	
	miRaceModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miRaceModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miRaceModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miRaceModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miRaceModeHeaderText = new MI_Text("Race Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[13].AddControl(miRaceModeQuantityField, miRaceModeBackButton, miRaceModeSpeedField, NULL, miRaceModeBackButton);
	mModeSettingsMenu[13].AddControl(miRaceModeSpeedField, miRaceModeQuantityField, miRaceModePenaltyField, NULL, miRaceModeBackButton);
	mModeSettingsMenu[13].AddControl(miRaceModePenaltyField, miRaceModeSpeedField, miRaceModeBackButton, NULL, miRaceModeBackButton);

	mModeSettingsMenu[13].AddControl(miRaceModeBackButton, miRaceModePenaltyField, miRaceModeQuantityField, miRaceModePenaltyField, NULL);
	
	mModeSettingsMenu[13].AddNonControl(miRaceModeLeftHeaderBar);
	mModeSettingsMenu[13].AddNonControl(miRaceModeRightHeaderBar);
	mModeSettingsMenu[13].AddNonControl(miRaceModeHeaderText);
	
	mModeSettingsMenu[13].SetHeadControl(miRaceModeQuantityField);
	mModeSettingsMenu[13].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);



	//***********************
	// Stomp Mode Settings
	//***********************

	miStompModeRateField = new MI_SelectField(&spr_selectfield, 120, 140, "Rate", 400, 180);
	miStompModeRateField->Add("Very Slow", 150, "", false, false);
	miStompModeRateField->Add("Slow", 120, "", false, false);
	miStompModeRateField->Add("Moderate", 90, "", false, false);
	miStompModeRateField->Add("Fast", 60, "", false, false);
	miStompModeRateField->Add("Very Fast", 30, "", false, false);
	miStompModeRateField->SetData(&game_values.gamemodemenusettings.stomp.rate, NULL, NULL);
	miStompModeRateField->SetKey(game_values.gamemodemenusettings.stomp.rate);

	for(short iEnemy = 0; iEnemy < 4; iEnemy++)
	{
		miStompModeEnemySlider[iEnemy] = new MI_PowerupSlider(&spr_selectfield, &menu_slider_bar, &menu_stomp, 120, 180 + 40 * iEnemy, 400, iEnemy);
		miStompModeEnemySlider[iEnemy]->Add("", 0, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 1, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 2, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 3, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 4, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 5, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 6, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 7, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 8, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 9, "", false, false);
		miStompModeEnemySlider[iEnemy]->Add("", 10, "", false, false);
		miStompModeEnemySlider[iEnemy]->SetNoWrap(true);
		miStompModeEnemySlider[iEnemy]->SetData(&game_values.gamemodemenusettings.stomp.enemyweight[iEnemy], NULL, NULL);
		miStompModeEnemySlider[iEnemy]->SetKey(game_values.gamemodemenusettings.stomp.enemyweight[iEnemy]);
	}

	miStompModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miStompModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miStompModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miStompModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miStompModeHeaderText = new MI_Text("Stomp Mode Menu", 320, 5, 0, 2, 1);


	mModeSettingsMenu[5].AddControl(miStompModeRateField, miStompModeBackButton, miStompModeEnemySlider[0], NULL, miStompModeBackButton);
	
	mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[0], miStompModeRateField,      miStompModeEnemySlider[1], NULL, miStompModeBackButton);
	mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[1], miStompModeEnemySlider[0], miStompModeEnemySlider[2], NULL, miStompModeBackButton);
	mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[2], miStompModeEnemySlider[1], miStompModeEnemySlider[3], NULL, miStompModeBackButton);
	mModeSettingsMenu[5].AddControl(miStompModeEnemySlider[3], miStompModeEnemySlider[2], miStompModeBackButton,     NULL, miStompModeBackButton);

	mModeSettingsMenu[5].AddControl(miStompModeBackButton, miStompModeEnemySlider[3], miStompModeRateField, miStompModeEnemySlider[3], NULL);
	
	mModeSettingsMenu[5].AddNonControl(miStompModeLeftHeaderBar);
	mModeSettingsMenu[5].AddNonControl(miStompModeRightHeaderBar);
	mModeSettingsMenu[5].AddNonControl(miStompModeHeaderText);
	
	mModeSettingsMenu[5].SetHeadControl(miStompModeRateField);
	mModeSettingsMenu[5].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


	//***********************
	// Frenzy Mode Settings
	//***********************

	miFrenzyModeQuantityField = new MI_SelectField(&spr_selectfield, 120, 60, "Limit", 400, 180);
	miFrenzyModeQuantityField->Add("Single Powerup", 0, "", false, false);
	miFrenzyModeQuantityField->Add("1 Powerup", 1, "", false, false);
	miFrenzyModeQuantityField->Add("2 Powerups", 2, "", false, false);
	miFrenzyModeQuantityField->Add("3 Powerups", 3, "", false, false);
	miFrenzyModeQuantityField->Add("4 Powerups", 4, "", false, false);
	miFrenzyModeQuantityField->Add("5 Powerups", 5, "", false, false);
	miFrenzyModeQuantityField->Add("# Players - 1", 6, "", false, false);
	miFrenzyModeQuantityField->Add("# Players", 7, "", false, false);
	miFrenzyModeQuantityField->Add("# Players + 1", 8, "", false, false);
	miFrenzyModeQuantityField->Add("# Players + 2", 9, "", false, false);
	miFrenzyModeQuantityField->Add("# Players + 3", 10, "", false, false);
	miFrenzyModeQuantityField->SetData(&game_values.gamemodemenusettings.frenzy.quantity, NULL, NULL);
	miFrenzyModeQuantityField->SetKey(game_values.gamemodemenusettings.frenzy.quantity);

	miFrenzyModeRateField = new MI_SelectField(&spr_selectfield, 120, 100, "Rate", 400, 180);
	miFrenzyModeRateField->Add("Instant", 0, "", false, false);
	miFrenzyModeRateField->Add("1 Second", 62, "", false, false);
	miFrenzyModeRateField->Add("2 Seconds", 124, "", false, false);
	miFrenzyModeRateField->Add("3 Seconds", 186, "", false, false);
	miFrenzyModeRateField->Add("5 Seconds", 310, "", false, false);
	miFrenzyModeRateField->Add("10 Seconds", 620, "", false, false);
	miFrenzyModeRateField->Add("15 Seconds", 930, "", false, false);
	miFrenzyModeRateField->Add("20 Seconds", 1240, "", false, false);
	miFrenzyModeRateField->Add("25 Seconds", 1550, "", false, false);
	miFrenzyModeRateField->Add("30 Seconds", 1860, "", false, false);
	miFrenzyModeRateField->SetData(&game_values.gamemodemenusettings.frenzy.rate, NULL, NULL);
	miFrenzyModeRateField->SetKey(game_values.gamemodemenusettings.frenzy.rate);

	miFrenzyModeStoredShellsField = new MI_SelectField(&spr_selectfield, 120, 140, "Store Shells", 400, 180);
	miFrenzyModeStoredShellsField->Add("Off", 0, "", false, false);
	miFrenzyModeStoredShellsField->Add("On", 1, "", true, false);
	miFrenzyModeStoredShellsField->SetData(NULL, NULL, &game_values.gamemodemenusettings.frenzy.storedshells);
	miFrenzyModeStoredShellsField->SetKey(game_values.gamemodemenusettings.frenzy.storedshells ? 1 : 0);
	miFrenzyModeStoredShellsField->SetAutoAdvance(true);

	short iPowerupMap[] = {8, 5, 11, 17, 19, 9, 16, 10, 12, 13, 14, 15};
	for(short iPowerup = 0; iPowerup < 12; iPowerup++)
	{
		miFrenzyModePowerupSlider[iPowerup] = new MI_PowerupSlider(&spr_selectfield, &menu_slider_bar, &spr_storedpoweruplarge, iPowerup < 6 ? 65 : 330, 180 + 40 * iPowerup - (iPowerup < 6 ? 0 : 240), 245, iPowerupMap[iPowerup]);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 0, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 1, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 2, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 3, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 4, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 5, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 6, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 7, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 8, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 9, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->Add("", 10, "", false, false);
		miFrenzyModePowerupSlider[iPowerup]->SetNoWrap(true);
		miFrenzyModePowerupSlider[iPowerup]->SetData(&game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup], NULL, NULL);
		miFrenzyModePowerupSlider[iPowerup]->SetKey(game_values.gamemodemenusettings.frenzy.powerupweight[iPowerup]);
	}

	miFrenzyModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miFrenzyModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miFrenzyModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miFrenzyModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miFrenzyModeHeaderText = new MI_Text("Frenzy Mode Menu", 320, 5, 0, 2, 1);


	mModeSettingsMenu[15].AddControl(miFrenzyModeQuantityField, miFrenzyModeBackButton, miFrenzyModeRateField, NULL, miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModeRateField, miFrenzyModeQuantityField, miFrenzyModeStoredShellsField, NULL, miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModeStoredShellsField, miFrenzyModeRateField, miFrenzyModePowerupSlider[0], NULL, miFrenzyModeBackButton);

	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[0], miFrenzyModeStoredShellsField, miFrenzyModePowerupSlider[1], NULL, miFrenzyModePowerupSlider[6]);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[1], miFrenzyModePowerupSlider[0], miFrenzyModePowerupSlider[2], NULL, miFrenzyModePowerupSlider[7]);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[2], miFrenzyModePowerupSlider[1], miFrenzyModePowerupSlider[3], NULL, miFrenzyModePowerupSlider[8]);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[3], miFrenzyModePowerupSlider[2], miFrenzyModePowerupSlider[4], NULL, miFrenzyModePowerupSlider[9]);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[4], miFrenzyModePowerupSlider[3], miFrenzyModePowerupSlider[5], NULL, miFrenzyModePowerupSlider[10]);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[5], miFrenzyModePowerupSlider[4], miFrenzyModePowerupSlider[6], NULL, miFrenzyModePowerupSlider[11]);
	
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[6], miFrenzyModePowerupSlider[5], miFrenzyModePowerupSlider[7], miFrenzyModePowerupSlider[0], miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[7], miFrenzyModePowerupSlider[6], miFrenzyModePowerupSlider[8], miFrenzyModePowerupSlider[1], miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[8], miFrenzyModePowerupSlider[7], miFrenzyModePowerupSlider[9], miFrenzyModePowerupSlider[2], miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[9], miFrenzyModePowerupSlider[8], miFrenzyModePowerupSlider[10], miFrenzyModePowerupSlider[3], miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[10], miFrenzyModePowerupSlider[9], miFrenzyModePowerupSlider[11], miFrenzyModePowerupSlider[4], miFrenzyModeBackButton);
	mModeSettingsMenu[15].AddControl(miFrenzyModePowerupSlider[11], miFrenzyModePowerupSlider[10], miFrenzyModeBackButton,       miFrenzyModePowerupSlider[5], miFrenzyModeBackButton);

	mModeSettingsMenu[15].AddControl(miFrenzyModeBackButton, miFrenzyModePowerupSlider[11], miFrenzyModeQuantityField, miFrenzyModePowerupSlider[11], NULL);
	
	mModeSettingsMenu[15].AddNonControl(miFrenzyModeLeftHeaderBar);
	mModeSettingsMenu[15].AddNonControl(miFrenzyModeRightHeaderBar);
	mModeSettingsMenu[15].AddNonControl(miFrenzyModeHeaderText);
	
	mModeSettingsMenu[15].SetHeadControl(miFrenzyModeQuantityField);
	mModeSettingsMenu[15].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


	//***********************
	// Survival Mode Settings
	//***********************

	for(short iEnemy = 0; iEnemy < 3; iEnemy++)
	{
		miSurvivalModeEnemySlider[iEnemy] = new MI_PowerupSlider(&spr_selectfield, &menu_slider_bar, &menu_survival, 120, 120 + 40 * iEnemy, 400, iEnemy);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 0, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 1, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 2, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 3, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 4, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 5, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 6, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 7, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 8, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 9, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->Add("", 10, "", false, false);
		miSurvivalModeEnemySlider[iEnemy]->SetNoWrap(true);
		miSurvivalModeEnemySlider[iEnemy]->SetData(&game_values.gamemodemenusettings.survival.enemyweight[iEnemy], NULL, NULL);
		miSurvivalModeEnemySlider[iEnemy]->SetKey(game_values.gamemodemenusettings.survival.enemyweight[iEnemy]);
	}

	miSurvivalModeDensityField = new MI_SelectField(&spr_selectfield, 120, 240, "Density", 400, 180);
	miSurvivalModeDensityField->Add("Very Low", 40, "", false, false);
	miSurvivalModeDensityField->Add("Low", 30, "", false, false);
	miSurvivalModeDensityField->Add("Medium", 20, "", false, false);
	miSurvivalModeDensityField->Add("High", 15, "", false, false);
	miSurvivalModeDensityField->Add("Very High", 10, "", false, false);
	miSurvivalModeDensityField->SetData(&game_values.gamemodemenusettings.survival.density, NULL, NULL);
	miSurvivalModeDensityField->SetKey(game_values.gamemodemenusettings.survival.density);

	miSurvivalModeSpeedField = new MI_SelectField(&spr_selectfield, 120, 280, "Speed", 400, 180);
	miSurvivalModeSpeedField->Add("Very Slow", 2, "", false, false);
	miSurvivalModeSpeedField->Add("Slow", 3, "", false, false);
	miSurvivalModeSpeedField->Add("Moderate", 4, "", false, false);
	miSurvivalModeSpeedField->Add("Fast", 6, "", false, false);
	miSurvivalModeSpeedField->Add("Very Fast", 8, "", false, false);
	miSurvivalModeSpeedField->SetData(&game_values.gamemodemenusettings.survival.speed, NULL, NULL);
	miSurvivalModeSpeedField->SetKey(game_values.gamemodemenusettings.survival.speed);
	
	miSurvivalModeShieldField = new MI_SelectField(&spr_selectfield, 120, 320, "Shield", 400, 180);
	miSurvivalModeShieldField->Add("Off", 0, "", false, false);
	miSurvivalModeShieldField->Add("On", 1, "", true, false);
	miSurvivalModeShieldField->SetData(NULL, NULL, &game_values.gamemodemenusettings.survival.shield);
	miSurvivalModeShieldField->SetKey(game_values.gamemodemenusettings.survival.shield ? 1 : 0);
	miSurvivalModeShieldField->SetAutoAdvance(true);

	miSurvivalModeBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miSurvivalModeBackButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);

	miSurvivalModeLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miSurvivalModeRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miSurvivalModeHeaderText = new MI_Text("Survival Mode Menu", 320, 5, 0, 2, 1);

	mModeSettingsMenu[16].AddControl(miSurvivalModeEnemySlider[0], miSurvivalModeBackButton, miSurvivalModeEnemySlider[1], NULL, miSurvivalModeBackButton);
	mModeSettingsMenu[16].AddControl(miSurvivalModeEnemySlider[1], miSurvivalModeEnemySlider[0], miSurvivalModeEnemySlider[2], NULL, miSurvivalModeBackButton);
	mModeSettingsMenu[16].AddControl(miSurvivalModeEnemySlider[2], miSurvivalModeEnemySlider[1], miSurvivalModeDensityField, NULL, miSurvivalModeBackButton);
	mModeSettingsMenu[16].AddControl(miSurvivalModeDensityField, miSurvivalModeEnemySlider[2], miSurvivalModeSpeedField, NULL, miSurvivalModeBackButton);
	mModeSettingsMenu[16].AddControl(miSurvivalModeSpeedField, miSurvivalModeDensityField, miSurvivalModeShieldField, NULL, miSurvivalModeBackButton);
	mModeSettingsMenu[16].AddControl(miSurvivalModeShieldField, miSurvivalModeSpeedField, miSurvivalModeBackButton, NULL, miSurvivalModeBackButton);
	mModeSettingsMenu[16].AddControl(miSurvivalModeBackButton, miSurvivalModeShieldField, miSurvivalModeEnemySlider[0], miSurvivalModeShieldField, NULL);
	
	mModeSettingsMenu[16].AddNonControl(miSurvivalModeLeftHeaderBar);
	mModeSettingsMenu[16].AddNonControl(miSurvivalModeRightHeaderBar);
	mModeSettingsMenu[16].AddNonControl(miSurvivalModeHeaderText);
	
	mModeSettingsMenu[16].SetHeadControl(miSurvivalModeEnemySlider[0]);
	mModeSettingsMenu[16].SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS);


	//***********************
	// Team Select Settings
	//***********************
	
	miTeamSelect = new MI_TeamSelect(&spr_player_select_background, 112, 96);
	miTeamSelect->SetAutoModify(true);
		
	miTeamSelectLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miTeamSelectRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miTeamSelectHeaderText = new MI_Text("Team and Character Selection", 320, 5, 0, 2, 1);

	mTeamSelectMenu.AddControl(miTeamSelect, NULL, NULL, NULL, NULL);
	
	mTeamSelectMenu.AddNonControl(miTeamSelectLeftHeaderBar);
	mTeamSelectMenu.AddNonControl(miTeamSelectRightHeaderBar);
	mTeamSelectMenu.AddNonControl(miTeamSelectHeaderText);
	
	mTeamSelectMenu.SetHeadControl(miTeamSelect);
	mTeamSelectMenu.SetCancelCode(MENU_CODE_BACK_TO_MATCH_SELECTION_MENU);

	//***********************
	// Tournament Scoreboard
	//***********************

	miTournamentScoreboard = new MI_TournamentScoreboard(&spr_tournament_background, 70, 98);
	
	miTournamentScoreboardNextButton = new MI_Button(&spr_selectfield, 220, 416, "Next", 200, 1);
	miTournamentScoreboardNextButton->SetCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU);

	miTournamentScoreboardImage = new MI_Image(&spr_scoreboard, 124, 12, 0, 0, 386, 59, 1, 1, 0);

	mTournamentScoreboardMenu.AddNonControl(miTournamentScoreboard);
	mTournamentScoreboardMenu.AddNonControl(miTournamentScoreboardImage);
	mTournamentScoreboardMenu.AddControl(miTournamentScoreboardNextButton, NULL, NULL, NULL, NULL);
	mTournamentScoreboardMenu.SetHeadControl(miTournamentScoreboardNextButton);
	mTournamentScoreboardMenu.SetCancelCode(MENU_CODE_BACK_TO_GAME_SETUP_MENU);

	//***********************
	// Bonus Wheel
	//***********************

	miBonusWheel = new MI_BonusWheel(&spr_tournament_powerup_splash, &spr_storedpoweruplarge, 144, 38);

	mBonusWheelMenu.AddControl(miBonusWheel, NULL, NULL, NULL, NULL);
	mBonusWheelMenu.SetHeadControl(miBonusWheel);
	mBonusWheelMenu.SetCancelCode(MENU_CODE_BONUS_DONE);

	//***********************
	// Powerup Selection Menu
	//***********************
	
	for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
	{
		miPowerupSlider[iPowerup] = new MI_PowerupSlider(&spr_selectfield, &menu_slider_bar, &spr_storedpoweruplarge, 50 + (iPowerupDisplayMap[iPowerup] < 10 ? 0 : 295), 44 + 38 * (iPowerupDisplayMap[iPowerup] < 10 ? iPowerupDisplayMap[iPowerup] : iPowerupDisplayMap[iPowerup] - 10), 245, iPowerup);
		miPowerupSlider[iPowerup]->Add("", 0, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 1, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 2, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 3, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 4, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 5, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 6, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 7, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 8, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 9, "", false, false);
		miPowerupSlider[iPowerup]->Add("", 10, "", false, false);
		miPowerupSlider[iPowerup]->SetNoWrap(true);
		miPowerupSlider[iPowerup]->SetData(&game_values.powerupweights[iPowerup], NULL, NULL);
		miPowerupSlider[iPowerup]->SetKey(game_values.powerupweights[iPowerup]);
	}

	miPowerupSelectionBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miPowerupSelectionBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
	
	miPowerupSelectionRestoreDefaultsButton = new MI_Button(&spr_selectfield, 220, 432, "Restore Defaults", 245, 1);
	miPowerupSelectionRestoreDefaultsButton->SetCode(MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS);

	miPowerupSelectionLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miPowerupSelectionMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miPowerupSelectionMenuHeaderText = new MI_Text("Item Selection Menu", 320, 5, 0, 2, 1);

	//Are You Sure dialog box
	miPowerupSelectionDialogImage = new MI_Image(&spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
	miPowerupSelectionDialogExitText = new MI_Text("Are You", 320, 195, 0, 2, 1);
	miPowerupSelectionDialogTournamentText = new MI_Text("Sure?", 320, 220, 0, 2, 1);
	miPowerupSelectionDialogYesButton = new MI_Button(&spr_selectfield, 235, 250, "Yes", 80, 1);
	miPowerupSelectionDialogNoButton = new MI_Button(&spr_selectfield, 325, 250, "No", 80, 1);
	
	miPowerupSelectionDialogYesButton->SetCode(MENU_CODE_POWERUP_RESET_YES);
	miPowerupSelectionDialogNoButton->SetCode(MENU_CODE_POWERUP_RESET_NO);

	miPowerupSelectionDialogImage->Show(false);
	miPowerupSelectionDialogTournamentText->Show(false);
	miPowerupSelectionDialogExitText->Show(false);
	miPowerupSelectionDialogYesButton->Show(false);
	miPowerupSelectionDialogNoButton->Show(false);

	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[0]], miPowerupSelectionBackButton, miPowerupSlider[iPowerupPositionMap[1]], NULL, miPowerupSlider[iPowerupPositionMap[10]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[1]], miPowerupSlider[iPowerupPositionMap[0]], miPowerupSlider[iPowerupPositionMap[2]], NULL, miPowerupSlider[iPowerupPositionMap[11]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[2]], miPowerupSlider[iPowerupPositionMap[1]], miPowerupSlider[iPowerupPositionMap[3]], NULL, miPowerupSlider[iPowerupPositionMap[12]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[3]], miPowerupSlider[iPowerupPositionMap[2]], miPowerupSlider[iPowerupPositionMap[4]], NULL, miPowerupSlider[iPowerupPositionMap[13]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[4]], miPowerupSlider[iPowerupPositionMap[3]], miPowerupSlider[iPowerupPositionMap[5]], NULL, miPowerupSlider[iPowerupPositionMap[14]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[5]], miPowerupSlider[iPowerupPositionMap[4]], miPowerupSlider[iPowerupPositionMap[6]], NULL, miPowerupSlider[iPowerupPositionMap[15]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[6]], miPowerupSlider[iPowerupPositionMap[5]], miPowerupSlider[iPowerupPositionMap[7]], NULL, miPowerupSlider[iPowerupPositionMap[16]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[7]], miPowerupSlider[iPowerupPositionMap[6]], miPowerupSlider[iPowerupPositionMap[8]], NULL, miPowerupSlider[iPowerupPositionMap[17]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[8]], miPowerupSlider[iPowerupPositionMap[7]], miPowerupSlider[iPowerupPositionMap[9]], NULL, miPowerupSlider[iPowerupPositionMap[18]]);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[9]], miPowerupSlider[iPowerupPositionMap[8]], miPowerupSelectionRestoreDefaultsButton, NULL, miPowerupSlider[iPowerupPositionMap[19]]);
	
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[10]], miPowerupSelectionRestoreDefaultsButton, miPowerupSlider[iPowerupPositionMap[11]], miPowerupSlider[iPowerupPositionMap[0]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[11]], miPowerupSlider[iPowerupPositionMap[10]], miPowerupSlider[iPowerupPositionMap[12]], miPowerupSlider[iPowerupPositionMap[1]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[12]], miPowerupSlider[iPowerupPositionMap[11]], miPowerupSlider[iPowerupPositionMap[13]], miPowerupSlider[iPowerupPositionMap[2]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[13]], miPowerupSlider[iPowerupPositionMap[12]], miPowerupSlider[iPowerupPositionMap[14]], miPowerupSlider[iPowerupPositionMap[3]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[14]], miPowerupSlider[iPowerupPositionMap[13]], miPowerupSlider[iPowerupPositionMap[15]], miPowerupSlider[iPowerupPositionMap[4]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[15]], miPowerupSlider[iPowerupPositionMap[14]], miPowerupSlider[iPowerupPositionMap[16]], miPowerupSlider[iPowerupPositionMap[5]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[16]], miPowerupSlider[iPowerupPositionMap[15]], miPowerupSlider[iPowerupPositionMap[17]], miPowerupSlider[iPowerupPositionMap[6]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[17]], miPowerupSlider[iPowerupPositionMap[16]], miPowerupSlider[iPowerupPositionMap[18]], miPowerupSlider[iPowerupPositionMap[7]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[18]], miPowerupSlider[iPowerupPositionMap[17]], miPowerupSlider[iPowerupPositionMap[19]], miPowerupSlider[iPowerupPositionMap[8]], NULL);
	mPowerupSelectionMenu.AddControl(miPowerupSlider[iPowerupPositionMap[19]], miPowerupSlider[iPowerupPositionMap[18]], miPowerupSelectionBackButton, miPowerupSlider[iPowerupPositionMap[9]], NULL);
	
	mPowerupSelectionMenu.AddControl(miPowerupSelectionRestoreDefaultsButton, miPowerupSlider[iPowerupPositionMap[9]], miPowerupSlider[iPowerupPositionMap[10]], NULL, miPowerupSelectionBackButton);

	mPowerupSelectionMenu.AddControl(miPowerupSelectionBackButton, miPowerupSlider[iPowerupPositionMap[19]], miPowerupSlider[iPowerupPositionMap[0]], miPowerupSelectionRestoreDefaultsButton, NULL);

	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionLeftHeaderBar);
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionMenuRightHeaderBar);
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionMenuHeaderText);
	
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionDialogImage);
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionDialogTournamentText);
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionDialogExitText);

	mPowerupSelectionMenu.AddControl(miPowerupSelectionDialogYesButton, NULL, NULL, NULL, miPowerupSelectionDialogNoButton);
	mPowerupSelectionMenu.AddControl(miPowerupSelectionDialogNoButton, NULL, NULL, miPowerupSelectionDialogYesButton, NULL);

	mPowerupSelectionMenu.SetHeadControl(miPowerupSlider[iPowerupPositionMap[0]]);
	mPowerupSelectionMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);


	//***********************
	// Powerup Settings Menu
	//***********************
	
	miStoredPowerupDelayField = new MI_SelectField(&spr_selectfield, 120, 120, "Stored Use", 400, 180);
	miStoredPowerupDelayField->Add("Very Slow", 2, "", false, false);
	miStoredPowerupDelayField->Add("Slow", 3, "", false, false);
	miStoredPowerupDelayField->Add("Moderate", 4, "", false, false);
	miStoredPowerupDelayField->Add("Fast", 5, "", false, false);
	miStoredPowerupDelayField->Add("Very Fast", 6, "", false, false);
	miStoredPowerupDelayField->SetData(&game_values.storedpowerupdelay, NULL, NULL);
	miStoredPowerupDelayField->SetKey(game_values.storedpowerupdelay);

	miItemRespawnField = new MI_SelectField(&spr_selectfield, 120, 160, "Item Spawn", 400, 180);
	miItemRespawnField->Add("5 Seconds", 310, "", false, false);
	miItemRespawnField->Add("10 Seconds", 620, "", false, false);
	miItemRespawnField->Add("15 Seconds", 930, "", false, false);
	miItemRespawnField->Add("20 Seconds", 1240, "", false, false);
	miItemRespawnField->Add("25 Seconds", 1550, "", false, false);
	miItemRespawnField->Add("30 Seconds", 1860, "", false, false);
	miItemRespawnField->Add("35 Seconds", 2170, "", false, false);
	miItemRespawnField->Add("40 Seconds", 2480, "", false, false);
	miItemRespawnField->Add("45 Seconds", 2790, "", false, false);
	miItemRespawnField->Add("50 Seconds", 3100, "", false, false);
	miItemRespawnField->Add("55 Seconds", 3410, "", false, false);
	miItemRespawnField->Add("60 Seconds", 3720, "", false, false);
	miItemRespawnField->SetData(&game_values.itemrespawntime, NULL, NULL);
	miItemRespawnField->SetKey(game_values.itemrespawntime);

	miSwapStyleField = new MI_SelectField(&spr_selectfield, 120, 200, "Swap Style", 400, 180);
	miSwapStyleField->Add("Walk", 0, "", false, false);
	miSwapStyleField->Add("Blink", 1, "", false, false);
	miSwapStyleField->Add("Instant", 2, "", false, false);
	miSwapStyleField->SetData(&game_values.swapstyle, NULL, NULL);
	miSwapStyleField->SetKey(game_values.swapstyle);

	miBonusWheelField = new MI_SelectField(&spr_selectfield, 120, 240, "Bonus Wheel", 400, 180);
	miBonusWheelField->Add("Off", 0, "", false, false);
	miBonusWheelField->Add("Tournament Win", 1, "", false, false);
	miBonusWheelField->Add("Every Game", 2, "", false, false);
	miBonusWheelField->SetData(&game_values.bonuswheel, NULL, NULL);
	miBonusWheelField->SetKey(game_values.bonuswheel);

	miKeepPowerupField = new MI_SelectField(&spr_selectfield, 120, 280, "Bonus Item", 400, 180);
	miKeepPowerupField->Add("Until Next Spin", 0, "", false, false);
	miKeepPowerupField->Add("Keep Always", 1, "", true, false);
	miKeepPowerupField->SetData(NULL, NULL, &game_values.keeppowerup);
	miKeepPowerupField->SetKey(game_values.keeppowerup ? 1 : 0);
	miKeepPowerupField->SetAutoAdvance(true);

	miStoredPowerupResetButton = new MI_StoredPowerupResetButton(&spr_selectfield, 120, 320, "Reset Stored Items", 400, 0);
	miStoredPowerupResetButton->SetCode(MENU_CODE_RESET_STORED_POWERUPS);

	miPowerupSettingsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miPowerupSettingsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
	
	miPowerupSettingsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miPowerupSettingsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miPowerupSettingsMenuHeaderText = new MI_Text("Item Settings Menu", 320, 5, 0, 2, 1);

	mPowerupSettingsMenu.AddControl(miStoredPowerupDelayField, miPowerupSettingsMenuBackButton, miItemRespawnField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miItemRespawnField, miStoredPowerupDelayField, miSwapStyleField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miSwapStyleField, miItemRespawnField, miBonusWheelField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miBonusWheelField, miSwapStyleField, miKeepPowerupField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miKeepPowerupField, miBonusWheelField, miStoredPowerupResetButton, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miStoredPowerupResetButton, miKeepPowerupField, miPowerupSettingsMenuBackButton, NULL, miPowerupSettingsMenuBackButton);
	
	mPowerupSettingsMenu.AddControl(miPowerupSettingsMenuBackButton, miStoredPowerupResetButton, miStoredPowerupDelayField, miStoredPowerupResetButton, NULL);

	mPowerupSettingsMenu.AddNonControl(miPowerupSettingsMenuLeftHeaderBar);
	mPowerupSettingsMenu.AddNonControl(miPowerupSettingsMenuRightHeaderBar);
	mPowerupSettingsMenu.AddNonControl(miPowerupSettingsMenuHeaderText);
	
	mPowerupSettingsMenu.SetHeadControl(miStoredPowerupDelayField);
	mPowerupSettingsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
}

//---------------------------------------------------------------
// RUN THE MENU
//---------------------------------------------------------------

void Menu::RunMenu()
{
	//Reset the keys each time we switch from menu to game and back
	game_values.playerInput.ResetKeys();

	fNeedMenuMusicReset = false;

	if(game_values.gamemode->winningteam > -1 && game_values.tournamentwinner == -1 && 
		((game_values.matchtype != MATCH_TYPE_TOUR && game_values.bonuswheel == 2) || (game_values.matchtype == MATCH_TYPE_TOUR && game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType)))
	{
		miBonusWheel->Reset(false);
		mCurrentMenu = &mBonusWheelMenu;
	}
	else if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME)
	{
		mCurrentMenu = &mTournamentScoreboardMenu;
	}

	if(game_values.matchtype == MATCH_TYPE_WORLD)
	{
		if(game_values.gamemode->winningteam > -1 && game_values.tournamentwinner == -1)  //Stage is completed
		{
			miWorld->SetControllingTeam(game_values.gamemode->winningteam);
			miWorld->SetCurrentStageToCompleted(game_values.gamemode->winningteam);

			miWorldStop->Show(false);

			mWorldMenu.SetHeadControl(miWorld);
			mWorldMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

			mWorldMenu.RestoreCurrent();
		}
		else if(game_values.tournamentwinner > -1) //World is completed
		{
			miBonusWheel->Reset(false);
			mCurrentMenu = &mBonusWheelMenu;
		}

		UpdateScoreBoard();
		miTournamentScoreboard->RefreshWorldScores(game_values.gamemode->winningteam);

		//If we're suposed to skip the scoreboard, then reset back to the world map
		if(game_values.worldskipscoreboard)
		{
			mCurrentMenu = &mWorldMenu;
			mCurrentMenu->ResetMenu();

			game_values.worldskipscoreboard = false;
		}
	}
	else if(game_values.matchtype == MATCH_TYPE_TOUR)
	{
		miTournamentScoreboard->RefreshTourScores();

		if(game_values.tourstopcurrent < game_values.tourstoptotal)
			miTourStop->Refresh(game_values.tourstopcurrent);
	}
	else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT)
	{
		miTournamentScoreboard->StopSwirl();
		if(game_values.gamemode->winningteam > -1)
			miTournamentScoreboard->RefreshTournamentScores(game_values.gamemode->winningteam);
	}

	//Reset game mode back to the current game mode in case we came from boss mode
	game_values.gamemode = gamemodes[currentgamemode];

	//Keep track if we entered the menu loop as part of a tournament, if we exit the tournament
	//we need to reset the menu music back to normal
	if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME)
		fNeedMenuMusicReset = true;

	if(game_values.music)
	{
		if(game_values.tournamentwinner < 0)
		{
			if(game_values.matchtype == MATCH_TYPE_SINGLE_GAME)
				backgroundmusic[2].play(false, false);
			else if(game_values.matchtype == MATCH_TYPE_WORLD)
				backgroundmusic[6].play(false, false);
			else
				backgroundmusic[3].play(false, false);
		}
	}

	if(game_values.matchtype != MATCH_TYPE_WORLD)
	{
		if(mCurrentMenu == &mGameSettingsMenu || mCurrentMenu == &mTournamentScoreboardMenu)
			miMapField->LoadCurrentMap();
	}

	float realfps = 0, flipfps = 0;
	unsigned int framestart, ticks;
	int delay;

	while (true)
	{
		framestart = SDL_GetTicks();

/*
#ifdef _XBOX
		if(joystickcount != SDL_NumJoysticks())
			reconnectjoysticks();
#endif
*/

		/*
		//Sample network code for TCP communication
		if(game_values.networktype == 1)
		{
			if(game_values.networkhost)
			{
				netServer.update();

				static int messagecounter = 0;

				if(++messagecounter > 120)
				{
					messagecounter = 0;
					char * szMessage = "Hello";
					netServer.broadcastmessage(szMessage);
				}
			}
			else
			{
				netClient.update();
			}
		}
		*/

		//Reset the keys that were down the last frame
		game_values.playerInput.ClearPressedKeys(1);

		//handle messages
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
#ifndef _XBOX
				case SDL_QUIT:
					Exit();
					return;
				break;
#endif

				case SDL_KEYDOWN:
				{
					if(event.key.keysym.sym == SDLK_F1)
					{
						game_values.showfps = !game_values.showfps;
					}

#ifndef _XBOX
					if(event.key.keysym.mod & (KMOD_LALT | KMOD_RALT))
					{
						//ALT + F4 = close window
						if(event.key.keysym.sym == SDLK_F4)
						{
							Exit();
							return;
						} //ALT + Enter = fullscreen/windowed toggle
						else if(event.key.keysym.sym == SDLK_RETURN)
						{
							game_values.fullscreen = !game_values.fullscreen;
							gfx_setresolution(640, 480, game_values.fullscreen);
							blitdest = screen;
							continue;
						}
					}
#endif
#ifdef _DEBUG
					//Pressing insert in debug mode turns on automated testing
					if(event.key.keysym.sym == SDLK_INSERT)
					{
						g_fAutoTest = !g_fAutoTest;

						if(g_fAutoTest)
						{
							mCurrentMenu = &mMainMenu;
							mCurrentMenu->ResetMenu();

							LoadScript("Scripts/StartMenuAutomation.txt");
						}
					}
					
					if(event.key.keysym.sym == SDLK_HOME)
					{
						g_fRecordTest = !g_fRecordTest;

						if(g_fRecordTest)
						{
							StartRecordScript();
						}
						else
						{
							SaveScript("Scripts/SavedScript.txt");
						}
					}

					if(event.key.keysym.sym == SDLK_END)
					{
						AddEmtpyLineToScript();
					}
					
#endif
					break;
					
				}

			default:
				break;
			}

			game_values.playerInput.Update(event, 1);
		}

#ifdef _DEBUG
		if(g_fAutoTest)
			GetNextScriptOperation();

		if(g_fRecordTest)
			SetNextScriptOperation();
#endif

		bool fGenerateMapThumbs = false;
		if(GS_MENU == game_values.gamestate)
		{
			MenuCodeEnum code = mCurrentMenu->SendInput(&game_values.playerInput);

			bool fShowSettingsButton[] = {false, false, false, true, true, true, false, true, true, true, true, true, true, true, false, true, true};

			if(MENU_CODE_EXIT_APPLICATION == code)
			{
				Exit();
				return;
			}
			else if(MENU_CODE_TO_MAIN_MENU == code)
			{
				iDisplayError = DISPLAY_ERROR_NONE;
				iDisplayErrorTimer = 0;

				mCurrentMenu = &mMainMenu;
			}
			else if(MENU_CODE_BACK_TO_MATCH_SELECTION_MENU == code)
			{
				mCurrentMenu = &mMatchSelectionMenu;
			}
			else if(MENU_CODE_TO_MATCH_SELECTION_MENU == code)
			{
				mCurrentMenu = &mMatchSelectionMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_MATCH_SELECTION_START == code)
			{
				miTeamSelect->Reset();
				mCurrentMenu = &mTeamSelectMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_MATCH_SELECTION_MATCH_CHANGED == code)
			{
				miTournamentField->Show(game_values.matchtype == MATCH_TYPE_TOURNAMENT);
				miTourField->Show(game_values.matchtype == MATCH_TYPE_TOUR);
				miWorldField->Show(game_values.matchtype == MATCH_TYPE_WORLD);

				miMatchSelectionDisplayImage->Show(game_values.matchtype != MATCH_TYPE_WORLD);
				miMatchSelectionDisplayImage->SetImage(0, 240 * game_values.matchtype, 320, 240);

			}
			else if(MENU_CODE_TO_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_BACK_TO_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mOptionsMenu;
			}
			else if(MENU_CODE_BACK_TO_GRAPHIC_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mGraphicsOptionsMenu;
			}
			else if(MENU_CODE_TO_CONTROLS_MENU == code)
			{
				mCurrentMenu = &mPlayerControlsSelectMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_BACK_TO_CONTROLS_MENU == code)
			{
				mCurrentMenu = &mPlayerControlsSelectMenu;
			}
			else if(MENU_CODE_TO_PLAYER_1_CONTROLS == code)
			{
				miInputContainer->SetPlayer(0);
				mCurrentMenu = &mPlayerControlsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_TO_PLAYER_2_CONTROLS == code)
			{
				miInputContainer->SetPlayer(1);
				mCurrentMenu = &mPlayerControlsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_TO_PLAYER_3_CONTROLS == code)
			{
				miInputContainer->SetPlayer(2);
				mCurrentMenu = &mPlayerControlsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_TO_PLAYER_4_CONTROLS == code)
			{
				miInputContainer->SetPlayer(3);
				mCurrentMenu = &mPlayerControlsMenu;
				mCurrentMenu->ResetMenu();
			}
	#ifdef _XBOX
			else if(MENU_CODE_TO_SCREEN_SETTINGS == code)
			{
				mCurrentMenu = &mScreenSettingsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_TO_SCREEN_RESIZE == code)
			{
				mCurrentMenu = &mScreenResizeMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_SCREEN_FILTER_CHANGED == code)
			{
				SDL_SetHardwareFilter(game_values.hardwarefilter);
				//Forces HW render so filter is applied
				SDL_XBOX_SetScreenPosition(game_values.screenResizeX, game_values.screenResizeY);
			}
			else if(MENU_CODE_SCREEN_SETTINGS_CHANGED == code)
			{
				gfx_setresolution(640, 480, false);
				blitdest = screen;
			}
			else if(MENU_CODE_BACK_TO_SCREEN_SETTINGS_MENU == code)
			{
				mCurrentMenu = &mScreenSettingsMenu;
			}
	#else
			else if(MENU_CODE_TOGGLE_FULLSCREEN == code)
			{
				gfx_setresolution(640, 480, game_values.fullscreen);
				blitdest = screen;
			}
	#endif
			else if(MENU_CODE_TO_GAME_SETUP_MENU == code)
			{
				//Moves teams to the first arrays in the list and counts the number of teams
				score_cnt = miTeamSelect->OrganizeTeams();
				iDisplayError = DISPLAY_ERROR_NONE;
				iDisplayErrorTimer = 0;
				bool fErrorReadingTourFile = false;

				//Load the tour here if one was selected
				if(game_values.matchtype == MATCH_TYPE_TOUR)
				{
					if(!ReadTourFile())
					{
						iDisplayError = DISPLAY_ERROR_READ_TOUR_FILE;
						iDisplayErrorTimer = 120;
						fErrorReadingTourFile = true;
					}
					else
					{
						miTournamentScoreboard->CreateScoreboard(score_cnt, game_values.tourstoptotal, &spr_tour_markers);
					}
				}
				else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT)
				{
					miTournamentScoreboard->CreateScoreboard(score_cnt, game_values.tournamentgames, &menu_mode_large);
				}
				else if(game_values.matchtype == MATCH_TYPE_WORLD)
				{
					if(!g_worldmap.Load())
					{
						iDisplayError = DISPLAY_ERROR_READ_WORLD_FILE;
						iDisplayErrorTimer = 120;
						fErrorReadingTourFile = true;
					}
					else
					{
						miTournamentScoreboard->CreateScoreboard(score_cnt, 0, &spr_tour_markers);

						for(short iPlayer = 0; iPlayer < 4; iPlayer++)
						{
							game_values.storedpowerups[iPlayer] = -1;
							g_worldmap.SetInitialPowerups();
						}

						miWorld->Init();
						miWorld->SetControllingTeam(rand() % score_cnt);
					}
				}

				if(!fErrorReadingTourFile)
				{
					mTournamentScoreboardMenu.ClearEyeCandy();

					//Initialize tournament values
					game_values.tournamentwinner = -1;
					
					//Setup wins counters for tournament/tour
					for(int k = 0; k < 4; k++)
					{
						game_values.tournament_scores[k].wins = 0;
						game_values.tournament_scores[k].total = 0;
					}

					if(MATCH_TYPE_SINGLE_GAME == game_values.matchtype || MATCH_TYPE_TOURNAMENT == game_values.matchtype)
					{
						maplist.findexact(szCurrentMapName);
						miMapField->LoadCurrentMap();

						game_values.gamemode = gamemodes[miModeField->GetShortValue()];
						
						for(short iMode = 0; iMode < GAMEMODE_LAST; iMode++)
						{
							gamemodes[iMode]->goal = miGoalField[iMode]->GetShortValue();	
						}

						miModeSettingsButton->Show(fShowSettingsButton[miModeField->GetShortValue()]);
						
						mCurrentMenu = &mGameSettingsMenu;
						mCurrentMenu->ResetMenu();
					}
					else if(MATCH_TYPE_TOUR == game_values.matchtype)
					{
						mCurrentMenu = &mTourStopMenu;
						mCurrentMenu->ResetMenu();
					}
					else if(MATCH_TYPE_WORLD == game_values.matchtype)
					{
						game_values.screenfadespeed = 8;
						game_values.screenfade = 8;
					}

					//Setup items on next menu
					for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
					{
						miGoalField[iGameMode]->HideItem(-1, game_values.matchtype == MATCH_TYPE_TOURNAMENT);
					}

					if(game_values.matchtype == MATCH_TYPE_WORLD)
						miGameSettingsMenuHeaderText->SetText("World Game Menu");
					else if(game_values.matchtype == MATCH_TYPE_TOUR)
						miGameSettingsMenuHeaderText->SetText("Tour Game Menu");
					else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT)
						miGameSettingsMenuHeaderText->SetText("Tournament Game Menu");
					else
						miGameSettingsMenuHeaderText->SetText("Single Game Menu");

				}
			}
			else if(MENU_CODE_BACK_TO_GAME_SETUP_MENU == code)
			{
				if(game_values.matchtype == MATCH_TYPE_WORLD)
				{
					if(game_values.tournamentwinner == -2 || game_values.tournamentwinner > -1) //If world is over then return back to main menu
						ResetTournamentBackToMainMenu();
					else
						mCurrentMenu = &mWorldMenu;
				}
				else
				{
					if(game_values.tournamentwinner == -2) //Tied Tour Result
					{
						ResetTournamentBackToMainMenu();
					}
					else if(game_values.tournamentwinner > -1) //Tournament/Tour Won and Bonus Wheel will be spun
					{
						if(((game_values.matchtype != MATCH_TYPE_TOUR && game_values.bonuswheel == 0) || (game_values.matchtype == MATCH_TYPE_TOUR && !game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType)))
						{
							ResetTournamentBackToMainMenu();
						}
						else
						{
							miBonusWheel->Reset(true);
							mCurrentMenu = &mBonusWheelMenu;
						}
					}
					else  //Next Tour/Tourament Game
					{
						if(game_values.matchtype == MATCH_TYPE_TOUR)
							mCurrentMenu = &mTourStopMenu;
						else
							mCurrentMenu = &mGameSettingsMenu;
					}
				}

				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS == code)
			{
				mCurrentMenu = &mGameSettingsMenu;
			}
			else if(MENU_CODE_MODE_CHANGED == code)
			{
				game_values.gamemode = gamemodes[miModeField->GetShortValue()];
				miModeSettingsButton->Show(fShowSettingsButton[miModeField->GetShortValue()]);

				for(short iMode = 0; iMode < GAMEMODE_LAST; iMode++)
				{
					miGoalField[iMode]->Show(miModeField->GetShortValue() == iMode);
				}
			}
			else if(MENU_CODE_BACK_TEAM_SELECT_MENU == code)
			{
				if(game_values.matchtype == MATCH_TYPE_WORLD)
				{
					miWorldExitDialogImage->Show(true);
					miWorldExitDialogExitTourText->Show(true);
					miWorldExitDialogYesButton->Show(true);
					miWorldExitDialogNoButton->Show(true);

					mWorldMenu.RememberCurrent();

					mWorldMenu.SetHeadControl(miWorldExitDialogNoButton);
					mWorldMenu.SetCancelCode(MENU_CODE_NONE);
					mWorldMenu.ResetMenu();
				}
				else if(game_values.matchtype == MATCH_TYPE_TOUR)
				{
					miTourStopExitDialogImage->Show(true);
					miTourStopExitDialogExitTourText->Show(true);
					miTourStopExitDialogYesButton->Show(true);
					miTourStopExitDialogNoButton->Show(true);

					mTourStopMenu.RememberCurrent();

					mTourStopMenu.SetHeadControl(miTourStopExitDialogNoButton);
					mTourStopMenu.SetCancelCode(MENU_CODE_NONE);
					mTourStopMenu.ResetMenu();
				}
				else if(game_values.matchtype == MATCH_TYPE_TOURNAMENT)
				{
					miGameSettingsExitDialogImage->Show(true);
					miGameSettingsExitDialogTournamentText->Show(true);
					miGameSettingsExitDialogExitText->Show(true);
					miGameSettingsExitDialogYesButton->Show(true);
					miGameSettingsExitDialogNoButton->Show(true);

					mGameSettingsMenu.RememberCurrent();

					mGameSettingsMenu.SetHeadControl(miGameSettingsExitDialogNoButton);
					mGameSettingsMenu.SetCancelCode(MENU_CODE_NONE);
					mGameSettingsMenu.ResetMenu();
				}
				else
				{
					mCurrentMenu = &mTeamSelectMenu;
					mCurrentMenu->ResetMenu();
				}
			}
			else if(MENU_CODE_SOUND_VOLUME_CHANGED == code)
			{
				game_values.sound = game_values.soundvolume > 0;
				sfx_setsoundvolume(game_values.soundvolume);
				ifsoundonplay(sfx_coin);
			}
			else if(MENU_CODE_MUSIC_VOLUME_CHANGED == code)
			{
				sfx_setmusicvolume(game_values.musicvolume);
				
				if(game_values.musicvolume == 0)
					backgroundmusic[2].stop();
				else if(game_values.musicvolume > 0 && !game_values.music)
					backgroundmusic[2].play(false, false);

				game_values.music = game_values.musicvolume > 0;
			}
			else if(MENU_CODE_START_GAME == code)
			{
				StartGame();
			}
			else if(MENU_CODE_EXIT_TOURNAMENT_YES == code || MENU_CODE_EXIT_TOURNAMENT_NO == code)
			{
				miGameSettingsExitDialogImage->Show(false);
				miGameSettingsExitDialogTournamentText->Show(false);
				miGameSettingsExitDialogExitText->Show(false);
				miGameSettingsExitDialogYesButton->Show(false);
				miGameSettingsExitDialogNoButton->Show(false);

				mGameSettingsMenu.SetHeadControl(miSettingsStartButton);
				mGameSettingsMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

				mGameSettingsMenu.RestoreCurrent();

				if(MENU_CODE_EXIT_TOURNAMENT_YES == code)
					ResetTournamentBackToMainMenu();
			}
			else if(MENU_CODE_EXIT_TOUR_YES == code || MENU_CODE_EXIT_TOUR_NO == code)
			{
				miTourStopExitDialogImage->Show(false);
				miTourStopExitDialogExitTourText->Show(false);
				miTourStopExitDialogYesButton->Show(false);
				miTourStopExitDialogNoButton->Show(false);

				mTourStopMenu.SetHeadControl(miTourStop);
				mTourStopMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

				mTourStopMenu.RestoreCurrent();

				if(MENU_CODE_EXIT_TOUR_YES == code)
					ResetTournamentBackToMainMenu();
			}
			else if(MENU_CODE_EXIT_WORLD_YES == code || MENU_CODE_EXIT_WORLD_NO == code)
			{
				miWorldExitDialogImage->Show(false);
				miWorldExitDialogExitTourText->Show(false);
				miWorldExitDialogYesButton->Show(false);
				miWorldExitDialogNoButton->Show(false);

				mWorldMenu.SetHeadControl(miWorld);
				mWorldMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

				mWorldMenu.RestoreCurrent();

				if(MENU_CODE_EXIT_WORLD_YES == code)
				{
					ResetTournamentBackToMainMenu();
					miWorldStop->Show(false);
				}
			}
			else if(MENU_CODE_BONUS_DONE == code)
			{
				if(miBonusWheel->GetPowerupSelectionDone())
				{
					if(game_values.matchtype == MATCH_TYPE_WORLD)
					{
						if(game_values.tournamentwinner == -1)
							mCurrentMenu = &mTournamentScoreboardMenu;
						else
							ResetTournamentBackToMainMenu();
					}
					else
					{
						if((game_values.matchtype == MATCH_TYPE_TOUR || game_values.matchtype == MATCH_TYPE_TOURNAMENT) && game_values.tournamentwinner == -1 &&
							((game_values.matchtype != MATCH_TYPE_TOUR && game_values.bonuswheel == 2) || (game_values.matchtype == MATCH_TYPE_TOUR && game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType)))
							mCurrentMenu = &mTournamentScoreboardMenu;
						else
							ResetTournamentBackToMainMenu();
					}
				}
			}
			else if (MENU_CODE_TO_POWERUP_SELECTION_MENU == code)
			{
				mCurrentMenu = &mPowerupSelectionMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_POWERUP_SETTINGS_MENU == code)
			{
				mCurrentMenu = &mPowerupSettingsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_RESTORE_DEFAULT_POWERUP_WEIGHTS == code)
			{
				miPowerupSelectionDialogImage->Show(true);
				miPowerupSelectionDialogTournamentText->Show(true);
				miPowerupSelectionDialogExitText->Show(true);
				miPowerupSelectionDialogYesButton->Show(true);
				miPowerupSelectionDialogNoButton->Show(true);

				mPowerupSelectionMenu.RememberCurrent();

				mPowerupSelectionMenu.SetHeadControl(miPowerupSelectionDialogNoButton);
				mPowerupSelectionMenu.SetCancelCode(MENU_CODE_POWERUP_RESET_NO);
				mPowerupSelectionMenu.ResetMenu();
			}
			else if(MENU_CODE_POWERUP_RESET_YES == code || MENU_CODE_POWERUP_RESET_NO == code)
			{
				miPowerupSelectionDialogImage->Show(false);
				miPowerupSelectionDialogTournamentText->Show(false);
				miPowerupSelectionDialogExitText->Show(false);
				miPowerupSelectionDialogYesButton->Show(false);
				miPowerupSelectionDialogNoButton->Show(false);

				mPowerupSelectionMenu.SetHeadControl(miPowerupSlider[iPowerupPositionMap[0]]);
				mPowerupSelectionMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

				mPowerupSelectionMenu.RestoreCurrent();

				if(MENU_CODE_POWERUP_RESET_YES == code)
				{
					//restore default powerup weights for powerup selection menu
					for(short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
					{
						miPowerupSlider[iPowerup]->SetKey(g_iDefaultPowerupWeights[iPowerup]);
						game_values.powerupweights[iPowerup] = g_iDefaultPowerupWeights[iPowerup];
					}
				}
			}
			else if (MENU_CODE_TO_GRAPHICS_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mGraphicsOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_SOUND_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mSoundOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_GAMEPLAY_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mGameplayOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_TEAM_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mTeamOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_PROJECTILES_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mProjectilesOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_MODE_SETTINGS_MENU == code)
			{
				for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
				{
					if(miGoalField[iGameMode]->IsVisible())
					{
						mCurrentMenu = &mModeSettingsMenu[iGameMode];
						mCurrentMenu->ResetMenu();
						break;
					}
				}
			}
			else if (MENU_CODE_MENU_GRAPHICS_PACK_CHANGED == code)
			{
				LoadStartGraphics();
				LoadMenuGraphics();

				blitdest = menu_backdrop.getSurface();
				menu_shade.setalpha(GetScreenBackgroundFade());
				menu_shade.draw(0, 0);
				blitdest = screen;
			}
			else if (MENU_CODE_GAME_GRAPHICS_PACK_CHANGED == code)
			{
				gfx_loadpalette();
				LoadGameGraphics();
			}
			else if (MENU_CODE_SOUND_PACK_CHANGED == code)
			{
				LoadGameSounds();

				if(!game_values.soundcapable)
				{
					game_values.sound = false;
					game_values.music = false;
					game_values.soundvolume = 0;
					game_values.musicvolume = 0;
				}
			}
			else if (MENU_CODE_WORLD_STAGE_START == code)
			{
				miWorldStop->Refresh(game_values.tourstopcurrent);
				miWorldStop->Show(true);

				mWorldMenu.RememberCurrent();

				mWorldMenu.SetHeadControl(miWorldStop);
				mWorldMenu.SetCancelCode(MENU_CODE_WORLD_STAGE_NO_START);
				
				mWorldMenu.ResetMenu();
			}
			else if (MENU_CODE_WORLD_STAGE_NO_START == code)
			{
				miWorldStop->Show(false);

				mWorldMenu.SetHeadControl(miWorld);
				mWorldMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

				mWorldMenu.RestoreCurrent();
			}
			else if (MENU_CODE_TOUR_STOP_CONTINUE == code || MENU_CODE_TOUR_STOP_CONTINUE_FORCED == code)
			{
				//If this tour stop is forced, we need to load the map first
				if(MENU_CODE_TOUR_STOP_CONTINUE_FORCED == code)
					miWorldStop->Refresh(game_values.tourstopcurrent);

				miWorld->ClearCloud();

				//Tour bonus house
				if(game_values.matchtype == MATCH_TYPE_WORLD && game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1)
				{
					bonushousemode->goal = 0;
					game_values.gamemode = bonushousemode;
				}
				else
				{
					short iGameMode = game_values.tourstops[game_values.tourstopcurrent]->iMode;
					gamemodes[iGameMode]->goal = game_values.tourstops[game_values.tourstopcurrent]->iGoal;
					game_values.gamemode = gamemodes[iGameMode];
				}

				StartGame();
			}
			else if (MENU_CODE_RESET_STORED_POWERUPS == code)
			{
				for(short iPlayer = 0; iPlayer < 4; iPlayer++)
					game_values.storedpowerups[iPlayer] = -1;
			}
			else if(MENU_CODE_MAP_CHANGED == code)
			{
				if(game_values.matchtype != MATCH_TYPE_TOUR)
					szCurrentMapName = miMapField->GetMapName();
			}
			else if(MENU_CODE_MAP_FILTER_EXIT == code)
			{
				maplist.ApplyFilters(game_values.pfFilters);
				
				//If the filtered map list has at least 1 map in it, then allow exiting the filter menu
				if(maplist.MapInFilteredSet())
				{
					miMapField->LoadCurrentMap();
					szCurrentMapName = miMapField->GetMapName();

					miMapFilterScroll->Show(false);
				
					mGameSettingsMenu.SetHeadControl(miSettingsStartButton);
					mGameSettingsMenu.SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

					mGameSettingsMenu.RestoreCurrent();

					iDisplayError = DISPLAY_ERROR_NONE;

					miMapFiltersOnImage->Show(game_values.fFiltersOn);
				}
				else  //otherwise display a message
				{
					iDisplayError = DISPLAY_ERROR_MAP_FILTER;
					iDisplayErrorTimer = 120;
				}
			}
			else if(MENU_CODE_TO_MAP_FILTERS == code)
			{
				miMapFilterScroll->Show(true);
				mGameSettingsMenu.RememberCurrent();

				mGameSettingsMenu.SetHeadControl(miMapFilterScroll);
				mGameSettingsMenu.SetCancelCode(MENU_CODE_NONE);
				mGameSettingsMenu.ResetMenu();
			}
			else if(MENU_CODE_TO_MAP_FILTER_EDIT == code)
			{
				miMapBrowser->Reset(0);
				
				mCurrentMenu = &mMapFilterEditMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_MAP_BROWSER_EXIT == code)
			{
				miMapField->LoadCurrentMap();
				szCurrentMapName = miMapField->GetMapName();

				mCurrentMenu = &mGameSettingsMenu;
				//mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_TO_MAP_BROWSER_THUMBNAILS == code)
			{
				miMapBrowser->Reset(1);

				mCurrentMenu = &mMapFilterEditMenu;
				mCurrentMenu->ResetMenu();
			}
			else if(MENU_CODE_SAVE_ALL_MAP_THUMBNAILS == code)
			{
				miGenerateThumbsDialogImage->Show(true);
				miGenerateThumbsDialogAreYouText->Show(true);
				miGenerateThumbsDialogSureText->Show(true);
				miGenerateThumbsDialogYesButton->Show(true);
				miGenerateThumbsDialogNoButton->Show(true);

				mOptionsMenu.RememberCurrent();

				mOptionsMenu.SetHeadControl(miGenerateThumbsDialogNoButton);
				mOptionsMenu.SetCancelCode(MENU_CODE_GENERATE_THUMBS_RESET_NO);
				mOptionsMenu.ResetMenu();
			}
			else if(MENU_CODE_GENERATE_THUMBS_RESET_YES == code || MENU_CODE_GENERATE_THUMBS_RESET_NO == code)
			{
				miGenerateThumbsDialogImage->Show(false);
				miGenerateThumbsDialogAreYouText->Show(false);
				miGenerateThumbsDialogSureText->Show(false);
				miGenerateThumbsDialogYesButton->Show(false);
				miGenerateThumbsDialogNoButton->Show(false);

				mOptionsMenu.SetHeadControl(miGameplayOptionsMenuButton);
				mOptionsMenu.SetCancelCode(MENU_CODE_TO_MAIN_MENU);

				mOptionsMenu.RestoreCurrent();

				if(MENU_CODE_GENERATE_THUMBS_RESET_YES == code)
				{
					fGenerateMapThumbs = true;
				}
			}
		}

		//--------------- draw everything ----------------------

		//Don't draw backdrop for world
		if(mCurrentMenu != &mWorldMenu)
			menu_backdrop.draw(0,0);
		else
			SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

		mCurrentMenu->Update();
		mCurrentMenu->Draw();

		if(iDisplayError > DISPLAY_ERROR_NONE)
		{
			spr_selectfield.draw(70, 400, 0, 0, 484, 32);
			spr_selectfield.draw(554, 400, 496, 0, 16, 32);

			if(iDisplayError == DISPLAY_ERROR_READ_TOUR_FILE)
				menu_font_large.drawCentered(320, 405, "Error Reading Tour File!");
			if(iDisplayError == DISPLAY_ERROR_READ_WORLD_FILE)
				menu_font_large.drawCentered(320, 405, "Error Reading World File!");
			else if(iDisplayError == DISPLAY_ERROR_MAP_FILTER)
				menu_font_large.drawCentered(320, 405, "No Maps Meet All Filter Conditions!");
			
			if(--iDisplayErrorTimer == 0)
				iDisplayError = DISPLAY_ERROR_NONE;
		}

		if(game_values.screenfadespeed != 0)
		{
			game_values.screenfade += game_values.screenfadespeed;

			if(game_values.screenfade <= 0)
			{
				game_values.screenfadespeed = 0;
				game_values.screenfade = 0;
			}
			else if(game_values.screenfade >= 255)
			{
				game_values.screenfadespeed = 0;
				game_values.screenfade = 255;
			}
		}

		if(game_values.screenfade > 0)
		{
			menu_shade.setalpha((Uint8)game_values.screenfade);
			menu_shade.draw(0, 0);
		}

		if(GS_START_GAME == game_values.gamestate && game_values.screenfade == 255)
		{
			if(game_values.matchtype == MATCH_TYPE_WORLD && game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1)
			{
				g_map.loadMap(convertPath("maps/special/bonushouse.map"), read_type_full);
				LoadCurrentMapBackground();
			}
			else
			{
				g_map.loadMap(maplist.currentFilename(), read_type_full);
				
				//Allows all players to start the game
				game_values.singleplayermode = -1;
			}

			game_values.gamestate = GS_GAME;

			/*
				game_values.gamemode = bossgamemode;  //boss type has already been set at this point

				if(bossgamemode->GetBossType() == 0)
					g_map.loadMap(convertPath("maps/special/dungeon.map"), read_type_full);
				else if(bossgamemode->GetBossType() == 1)
					g_map.loadMap(convertPath("maps/special/hills.map"), read_type_full);
				else if(bossgamemode->GetBossType() == 2)
					g_map.loadMap(convertPath("maps/special/volcano.map"), read_type_full);

				LoadCurrentMapBackground();

				g_map.predrawbackground(spr_background, spr_backmap);
				g_map.predrawforeground(spr_frontmap);
				LoadMapObjects();
			*/

			

			SetGameModeSettingsFromMenu();
			
			g_map.predrawbackground(spr_background, spr_backmap);
			g_map.predrawforeground(spr_frontmap);
			LoadMapObjects();

			if(game_values.music)
			{
				musiclist.SetRandomMusic(g_map.musicCategoryID, maplist.currentShortmapname(), g_map.szBackgroundFile);
				backgroundmusic[0].load(musiclist.GetCurrentMusic());
				backgroundmusic[0].play(game_values.playnextmusic, false);
			}

			return;
		}
		else if(game_values.screenfade == 255) //Fade to world match type
		{
			game_values.screenfadespeed = -8;

			mCurrentMenu = &mWorldMenu;
			mCurrentMenu->ResetMenu();

			backgroundmusic[2].stop();
			backgroundmusic[6].load(worldmusiclist.GetMusic(g_worldmap.GetMusicCategory()));
			backgroundmusic[6].play(false, false);
			fNeedMenuMusicReset = true;
		}

		if(fGenerateMapThumbs)
		{
			menu_font_large.drawCentered(320, 405, "Refreshing Map Thumbnails");
			menu_font_large.drawCentered(320, 430, "Please Wait...");
		}

		ticks = SDL_GetTicks() - framestart;
		if(ticks == 0)
			ticks = 1;

		if(game_values.showfps) 
			menu_font_large.drawf(0, 480 - menu_font_large.getHeight(), "Actual:%.1f/%.1f, Flip:%.1f, Potential:%.1f", realfps, 1000.0f / (float)WAITTIME, flipfps, 1000.0f / (float)ticks);

#ifdef _DEBUG
	if(g_fAutoTest)
		menu_font_small.drawRightJustified(635, 5, "Auto");

	if(g_fRecordTest)
		menu_font_small.drawRightJustified(635, 5, "Recording...");
#endif
 
		//double buffering -> flip buffers
		SDL_Flip(screen);

		flipfps = 1000.0f / (float)ticks;

		//we don't need much accuracy here, so we can stick to SDL_Delay
		delay = WAITTIME - (SDL_GetTicks() - framestart);
		if(delay < 0)
			delay = 0;
		else if(delay > WAITTIME)
			delay = WAITTIME;
		
		SDL_Delay(delay);

		realfps = 1000.0f / (float)ticks;

		if(fGenerateMapThumbs)
		{
			fGenerateMapThumbs = false;
			backgroundmusic[2].sfx_pause();

			//Reload map auto filters from live map files (don't use the cache)
			maplist.ReloadMapAutoFilters();

			//Write out all the map thumbnails for the map browser and filter editor
			char szThumbnail[256];
			std::map<std::string, MapListNode*>::iterator itr = maplist.GetIteratorAt(0, false);
		
			short iMapCount = maplist.GetCount();
			for(short iMap = 0; iMap < iMapCount; iMap++)
			{
				strcpy(szThumbnail, "maps/cache/");
				char * pszThumbnail = szThumbnail + strlen(szThumbnail);
				GetNameFromFileName(pszThumbnail, (*itr).second->filename.c_str());

#ifdef PNG_SAVE_FORMAT				
				strcat(szThumbnail, ".png");
#else
				strcat(szThumbnail, ".bmp");
#endif

				g_map.loadMap((*itr).second->filename, read_type_preview);
				g_map.saveThumbnail(szThumbnail, false);

				itr++;
			}

			backgroundmusic[2].sfx_pause();
		}
	}

	//we won't ever get here
	return;
}

bool Menu::ReadTourFile()
{
	ResetTourStops();

	FILE * fp = fopen(tourlist.GetIndex(game_values.tourindex), "r");

	char buffer[256];
	bool fReadVersion = false;
	short iVersion[4] = {0, 0, 0, 0};
	while(fgets(buffer, 256, fp) && game_values.tourstoptotal < 10)
	{
		if(buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t')
			continue;

		if(!fReadVersion)
		{
			fReadVersion = true;

			char * psz = strtok(buffer, ".\n");
			if(psz)
				iVersion[0] = atoi(psz);

			psz = strtok(NULL, ".\n");
			if(psz)
				iVersion[1] = atoi(psz);

			psz = strtok(NULL, ".\n");
			if(psz)
				iVersion[2] = atoi(psz);

			psz = strtok(NULL, ".\n");
			if(psz)
				iVersion[3] = atoi(psz);

			continue;
		}

		TourStop * ts = ParseTourStopLine(buffer, iVersion, false);
		
		game_values.tourstops.push_back(ts);
		game_values.tourstoptotal++;
	}
	
	if(game_values.tourstoptotal != 0)
	{
		miTourStop->Refresh(game_values.tourstopcurrent);

		//For old tours, turn on the bonus wheel at the end
		if(iVersion[0] == 1 && iVersion[1] == 7 && iVersion[2] == 0 && iVersion[3] <= 1)
			game_values.tourstops[game_values.tourstoptotal - 1]->iBonusType = 1;
	}

	fclose(fp);

	return game_values.tourstoptotal != 0;
}

void Menu::StartGame()
{
#ifdef _DEBUG
	iScriptState = 2;
	fScriptRunPreGameOptions = false;
#endif

	game_values.gamestate = GS_START_GAME;
	//backgroundonly = false;
	//fastmap = false;

	WriteGameOptions();

	//Load skins for players
	for(int k = 0; k < 4; k++)
	{
		if(game_values.playercontrol[k] > 0)
		{
			if(game_values.randomskin[k])
			{
				do
				{
					game_values.skinids[k] = rand() % skinlist.GetCount();
				}
				while(!LoadFullSkin(spr_player[k], game_values.skinids[k], game_values.colorids[k]));
			}
			else
			{
				LoadFullSkin(spr_player[k], game_values.skinids[k], game_values.colorids[k]);
			}
		}
	}

	//Load announcer sounds if changed
	if(game_values.loadedannouncer != announcerlist.GetCurrentIndex())
	{
		game_values.loadedannouncer = (short)announcerlist.GetCurrentIndex();

		//Delete the old sounds
		for(int k = 0; k < PANNOUNCER_SOUND_LAST; k++)
			sfx_announcer[k].reset();

		FILE * announcerfile = fopen(announcerlist.current_name(), "r");
		
		char szBuffer[256];

		int announcerIndex = 0;

		while(fgets(szBuffer, 256, announcerfile) && announcerIndex < PANNOUNCER_SOUND_LAST)
		{
			//Ignore comment lines
			if(szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
				continue;

			//Clean off carriage returns
			for(int k = 0; k < (int)strlen(szBuffer); k++)
			{
				if(szBuffer[k] == '\r' || szBuffer[k] == '\n')
				{
					szBuffer[k] = '\0';
					break;
				}
			}

			if(!strcmp(szBuffer, "[none]"))
				announcerIndex++;
			else
				sfx_announcer[announcerIndex++].init(convertPath(szBuffer));

		}

		fclose(announcerfile);
	}

	//Load soundtrack music if changed
	if(game_values.loadedmusic != musiclist.GetCurrentIndex())
	{
		game_values.loadedmusic = (short)musiclist.GetCurrentIndex();
		backgroundmusic[1].load(musiclist.GetMusic(0)); //Stage Clear
		backgroundmusic[3].load(musiclist.GetMusic(2)); //Tournament Menu
		backgroundmusic[4].load(musiclist.GetMusic(3)); //Tournament Over
	}

	backgroundmusic[2].stop();
	ifsoundonandreadyplay(sfx_announcer[11]);

	game_values.screenfade = 8;
	game_values.screenfadespeed = 8;
}

void Menu::Exit()
{
	game_values.gamestate = GS_QUIT;
	WriteGameOptions();
}

void Menu::ResetTournamentBackToMainMenu()
{
	mCurrentMenu = &mMainMenu;
	mCurrentMenu->ResetMenu();

	if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME)
	{
		if(fNeedMenuMusicReset)
		{
			backgroundmusic[3].stop();
			backgroundmusic[2].play(false, false);
			fNeedMenuMusicReset = false;
		}
	}
}


#ifdef _DEBUG

void Menu::LoadScript(char * szScriptFile)
{
	operations.clear();

	FILE * fp = fopen(szScriptFile, "r");

	if(fp != NULL)
	{	
		char buffer[256];
		while(fgets(buffer, 256, fp) != NULL)
		{
			if(buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t')
				continue;

			ScriptOperation * op = new ScriptOperation();

			//Get controller to use
			op->iController = atoi(strtok(buffer, ",\n"));

			//Get iterations on this operation
			op->iIterations = atoi(strtok(NULL, ",\n"));

			//Get input operations to perform
			for(short i = 0; i < 8; i++)
			{
				op->fInput[i] = atoi(strtok(NULL, ",\n")) != 0;
			}

			operations.push_back(op);
		}

		fclose(fp);
	}

	current = operations.begin();
	iScriptState = 0;
}

void Menu::ResetScript()
{
	std::vector<ScriptOperation*>::iterator itr = operations.begin();

	while (itr != operations.end())
	{
		(*itr)->iTimesExecuted = 0;
		itr++;
	}

	current = operations.begin();
	iScriptState = 0;
}

void Menu::GetNextScriptOperation()
{
	if(GS_MENU != game_values.gamestate)
		return;

	//static short slowdowntimer = 0;
	//if(++slowdowntimer < 30)
		//return;
	//slowdowntimer = 0;

	if(iScriptState == 1)
	{
		if(mCurrentMenu == &mGameSettingsMenu)
		{
			if(!fScriptRunPreGameOptions)
			{
				fScriptRunPreGameOptions = true;
				LoadScript("Scripts/GameModeOptionsOnly.txt");
				iScriptState = 0;
				return;
			}
			else if(game_values.gamemode->gamemode == game_mode_frag ||
			   game_values.gamemode->gamemode == game_mode_timelimit ||
			   game_values.gamemode->gamemode == game_mode_coins ||
			   game_values.gamemode->gamemode == game_mode_classic ||
			   game_values.gamemode->gamemode == game_mode_eggs ||
			   game_values.gamemode->gamemode == game_mode_owned)
			{
				LoadScript("Scripts/NoModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_jail)
			{
				LoadScript("Scripts/JailModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_stomp)
			{
				LoadScript("Scripts/StompModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_ctf)
			{
				LoadScript("Scripts/CaptureTheFlagModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_chicken)
			{
				LoadScript("Scripts/ChickenModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_tag)
			{
				LoadScript("Scripts/TagModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_star)
			{
				LoadScript("Scripts/StarModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_domination)
			{
				LoadScript("Scripts/DominationModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_koth)
			{
				LoadScript("Scripts/KingOfTheHillModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_race)
			{
				LoadScript("Scripts/RaceModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_frenzy)
			{
				LoadScript("Scripts/FrenzyModeOptions.txt");
			}
			else if(game_values.gamemode->gamemode == game_mode_survival)
			{
				LoadScript("Scripts/SurvivalModeOptions.txt");
			}

			game_values.gamemode->setdebuggoal();
		}
		else if(mCurrentMenu == &mTourStopMenu)
		{
			static int tournext = rand() % 200;

			if(--tournext < 0)
			{
				game_values.playerInput.outputControls[0].menu_select.fPressed = true;
				tournext = rand() % 200;
				game_values.gamemode->setdebuggoal();
			}
			return;
		}

		return;
	}
	else if (iScriptState == 2)
	{
		if(mCurrentMenu == &mTournamentScoreboardMenu)
		{
			static int scoreboardnext = rand() % 200;

			if(--scoreboardnext < 0)
			{
				game_values.playerInput.outputControls[0].menu_select.fPressed = true;
				iScriptState = 3;
				scoreboardnext = rand() % 200;
			}
			
			return;	
		}
		else if(mCurrentMenu == &mGameSettingsMenu)
		{
			iScriptState = 0;
			mCurrentMenu = &mMainMenu;
			mCurrentMenu->ResetMenu();
			LoadScript("Scripts/StartMenuAutomation.txt");
			fScriptRunPreGameOptions = true;
			return;
		}
		else if(mCurrentMenu == &mBonusWheelMenu)
		{
			static int bonuswheelnext = rand() % 200 + 300;

			if(--bonuswheelnext < 0)
			{
				game_values.playerInput.outputControls[0].menu_select.fPressed = true;
				//iScriptState = 3;
				bonuswheelnext = rand() % 200 + 300;
			}
			return;
		}
		else if(mCurrentMenu == &mTourStopMenu)
		{
			static int tournext = rand() % 200;

			if(--tournext < 0)
			{
				game_values.playerInput.outputControls[0].menu_select.fPressed = true;
				tournext = rand() % 200;
				game_values.gamemode->setdebuggoal();
			}
			return;
		}

		return;
	}
	else if (iScriptState == 3)
	{
		if(mCurrentMenu == &mBonusWheelMenu)
		{
			game_values.playerInput.outputControls[0].menu_select.fPressed = true;
			return;
		}
		else if(mCurrentMenu == &mGameSettingsMenu)
		{
			iScriptState = 0;
			LoadScript("Scripts/GameModeOptionsOnly.txt");
			fScriptRunPreGameOptions = true;
			return;
		}
		else if(mCurrentMenu == &mMainMenu)
		{
			iScriptState = 0;
			LoadScript("Scripts/StartMenuAutomation.txt");
			fScriptRunPreGameOptions = true;
			return;
		}
		else if(mCurrentMenu == &mTourStopMenu)
		{
			static int tournext = rand() % 200;

			if(--tournext < 0)
			{
				game_values.playerInput.outputControls[0].menu_select.fPressed = true;
				tournext = rand() % 200;
				game_values.gamemode->setdebuggoal();
			}
			return;
		}

		return;
	}

	ScriptOperation * op = *current;

	while(op->iTimesExecuted >= op->iIterations)
	{
		if(++current == operations.end())
		{
			iScriptState = 1;
			return;
		}

		op = *current;
	}

	op->iTimesExecuted++;

	short iController = op->iController;

	if(iController == 5)
		iController = rand() % 4;

	short iKeys[8];
	short iNumKeys = 0;

	for(short i = 0; i < 8; i++)
	{
		if(op->fInput[i])
			iKeys[iNumKeys++] = i;
	}

	if(iController == 4) //All controllers
	{
		game_values.playerInput.outputControls[0].keys[iKeys[rand() % iNumKeys]].fPressed = true;
		game_values.playerInput.outputControls[1].keys[iKeys[rand() % iNumKeys]].fPressed = true;
		game_values.playerInput.outputControls[2].keys[iKeys[rand() % iNumKeys]].fPressed = true;
		game_values.playerInput.outputControls[3].keys[iKeys[rand() % iNumKeys]].fPressed = true;
	}
	else
	{
		game_values.playerInput.outputControls[iController].keys[iKeys[rand() % iNumKeys]].fPressed = true;
	}
}

void Menu::StartRecordScript()
{
	operations.clear();
}

void Menu::SaveScript(char * szScriptFile)
{
	FILE * fp = fopen(szScriptFile, "w");

	if(fp)
	{
		std::vector<ScriptOperation*>::iterator itr = operations.begin();

		while (itr != operations.end())
		{
			ScriptOperation * op = *itr;

			//Add line breaks
			if(op->iController == -1)
			{
				fprintf(fp, "\n");
				itr++;
				continue;
			}

			fprintf(fp, "%d,%d", op->iController, op->iIterations);

			for(short i = 0; i < 8; i++)
			{
				fprintf(fp, ",%d", op->fInput[i]);
			}

			fprintf(fp, "\n");
			
			itr++;
		}

		fclose(fp);
	}
}

void Menu::AddEmtpyLineToScript()
{
	ScriptOperation * op = new ScriptOperation();
	op->iController = -1;
	operations.push_back(op);
}

void Menu::SetNextScriptOperation()
{
	for(short i = 0; i < 8; i++)
	{
		if(game_values.playerInput.outputControls[0].keys[i].fPressed)
		{
			ScriptOperation * op = new ScriptOperation();

			op->iController = 0;
			op->iIterations = 1;

			for(short iKey = 0; iKey < 8; iKey++)
			{
				op->fInput[iKey] = false;
			}

			op->fInput[i] = true;

			operations.push_back(op);
		}
	}
}

#endif

