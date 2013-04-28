
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

extern int g_iVersion[];

extern void SetGameModeSettingsFromMenu();
extern void LoadMapObjects(bool fPreview);
extern bool LoadStartGraphics();
extern bool LoadMenuGraphics();
extern bool LoadWorldGraphics();
extern bool LoadGameGraphics();
extern bool LoadGameSounds();
extern bool LoadFullSkin(gfxSprite ** sprites, short skinID, short colorID);

extern void UpdateScoreBoard();

extern Uint8 GetScreenBackgroundFade();
extern short LookupTeamID(short id);

extern const char * g_szAutoFilterNames[NUM_AUTO_FILTERS];
extern short g_iAutoFilterIcons[NUM_AUTO_FILTERS];

extern void ResetTourStops();
extern WorldMap g_worldmap;

extern void LoadCurrentMapBackground();

extern TourStop * ParseTourStopLine(char * buffer, int iVersion[4], bool fIsWorld);

extern CScore *score[4];

extern std::string stripPathAndExtension(const std::string &path);

void Menu::WriteGameOptions()
{
	FILE * fp = OpenFile("options.bin", "wb");

	if(fp != NULL)
	{
		fwrite(g_iVersion, sizeof(int), 4, fp);

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

		unsigned char abyte[35];
		abyte[0] = (unsigned char) game_values.spawnstyle;
		abyte[1] = (unsigned char) game_values.awardstyle;
		abyte[2] = (unsigned char) announcerlist.GetCurrentIndex();
		abyte[3] = (unsigned char) game_values.teamcollision;
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
		abyte[14] = (unsigned char) worldmusiclist.GetCurrentIndex();
		abyte[15] = (unsigned char) game_values.outofboundstime;
		abyte[16] = (unsigned char) game_values.cpudifficulty;
		abyte[17] = (unsigned char) menugraphicspacklist.GetCurrentIndex();
		abyte[18] = (unsigned char) soundpacklist.GetCurrentIndex();
		abyte[19] = (unsigned char) game_values.framelimiter;
		abyte[20] = (unsigned char) game_values.bonuswheel;
		abyte[21] = (unsigned char) game_values.keeppowerup;
		abyte[22] = (unsigned char) game_values.showwinningcrown;
		abyte[23] = (unsigned char) game_values.playnextmusic;
		abyte[24] = (unsigned char) game_values.pointspeed;
		abyte[25] = (unsigned char) game_values.swapstyle;
		abyte[26] = (unsigned char) gamegraphicspacklist.GetCurrentIndex();
		abyte[28] = (unsigned char) game_values.overridepowerupsettings;
		abyte[29] = (unsigned char) game_values.minigameunlocked;
		abyte[30] = (unsigned char) game_values.startgamecountdown;
		abyte[31] = (unsigned char) game_values.deadteamnotice;
		abyte[32] = (unsigned char) worldgraphicspacklist.GetCurrentIndex();
		abyte[33] = (unsigned char) game_values.tournamentcontrolstyle;
		abyte[34] = (unsigned char) game_values.startmodedisplay;
		fwrite(abyte, sizeof(unsigned char), 35, fp); 

		fwrite(&game_values.shieldtime, sizeof(short), 1, fp);
		fwrite(&game_values.shieldstyle, sizeof(short), 1, fp);
		fwrite(&game_values.itemrespawntime, sizeof(short), 1, fp);
		fwrite(&game_values.hiddenblockrespawn, sizeof(short), 1, fp);
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
		fwrite(&game_values.leaflimit, sizeof(short), 1, fp);
		fwrite(&game_values.pwingslimit, sizeof(short), 1, fp);
		fwrite(&game_values.tanookilimit, sizeof(short), 1, fp);
		fwrite(&game_values.bombslimit, sizeof(short), 1, fp);
		fwrite(&game_values.wandfreezetime, sizeof(short), 1, fp);
		fwrite(&game_values.wandlimit, sizeof(short), 1, fp);
		fwrite(&game_values.shellttl, sizeof(short), 1, fp);
		fwrite(&game_values.blueblockttl, sizeof(short), 1, fp);
		fwrite(&game_values.redblockttl, sizeof(short), 1, fp);
		fwrite(&game_values.grayblockttl, sizeof(short), 1, fp);
		fwrite(&game_values.storedpowerupdelay, sizeof(short), 1, fp);
		fwrite(&game_values.warplockstyle, sizeof(short), 1, fp);
		fwrite(&game_values.warplocktime, sizeof(short), 1, fp);
		fwrite(&game_values.suicidetime, sizeof(short), 1, fp);

		fwrite(&game_values.poweruppreset, sizeof(short), 1, fp);
		fwrite(&g_iCurrentPowerupPresets, sizeof(short), NUM_POWERUP_PRESETS * NUM_POWERUPS, fp);

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

	//Create the mode options menus
	modeOptionsMenu.CreateMenu();

	//***********************
	// Main Menu
	//***********************

	mCurrentMenu = &mMainMenu;

	miSMWTitle = new MI_Image(&menu_smw, 320 - ((short)menu_smw.getWidth() >> 1), 30, 0, 0, 372, 140, 1, 1, 0);
	miSMWVersion = new MI_Image(&menu_version, 570, 10, 0, 0, 58, 32, 1, 1, 0);
	//miSMWVersionText = new MI_Text("Beta 2", 630, 45, 0, 2, 2);
	
	miMainStartButton = new MI_Button(&spr_selectfield, 120, 210, "Start", 310, 0);
	miMainStartButton->SetCode(MENU_CODE_TO_MATCH_SELECTION_MENU);

	miQuickGameButton = new MI_Button(&spr_selectfield, 440, 210, "Go!", 80, 0);
	miQuickGameButton->SetCode(MENU_CODE_QUICK_GAME_START);

	miPlayerSelect = new MI_PlayerSelect(&menu_player_select, 120, 250, "Players", 400, 140);
	
	miOptionsButton = new MI_Button(&spr_selectfield, 120, 322, "Options", 400, 0);
	miOptionsButton->SetCode(MENU_CODE_TO_OPTIONS_MENU);

	miControlsButton = new MI_Button(&spr_selectfield, 120, 362, "Controls", 400, 0);
	miControlsButton->SetCode(MENU_CODE_TO_CONTROLS_MENU);

	miExitButton = new MI_Button(&spr_selectfield, 120, 402, "Exit", 400, 0);
	miExitButton->SetCode(MENU_CODE_EXIT_APPLICATION);

	mMainMenu.AddControl(miMainStartButton, miExitButton, miPlayerSelect, NULL, miQuickGameButton);
	mMainMenu.AddControl(miQuickGameButton, miExitButton, miPlayerSelect, miMainStartButton, NULL);
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
	//mMainMenu.AddNonControl(miSMWVersionText);

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

	miGameplayOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 40, "Gameplay", 400, 1);
	miGameplayOptionsMenuButton->SetCode(MENU_CODE_TO_GAMEPLAY_OPTIONS_MENU);
	
	miTeamOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 80, "Team", 400, 1);
	miTeamOptionsMenuButton->SetCode(MENU_CODE_TO_TEAM_OPTIONS_MENU);

	miPowerupOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 120, "Item Selection", 400, 1);
	miPowerupOptionsMenuButton->SetCode(MENU_CODE_TO_POWERUP_SELECTION_MENU);

	miPowerupSettingsMenuButton = new MI_Button(&spr_selectfield, 120, 160, "Item Settings", 400, 1);
	miPowerupSettingsMenuButton->SetCode(MENU_CODE_TO_POWERUP_SETTINGS_MENU);

	miProjectilesOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 200, "Weapons & Projectiles", 400, 1);
	miProjectilesOptionsMenuButton->SetCode(MENU_CODE_TO_PROJECTILES_OPTIONS_MENU);

	miProjectilesLimitsMenuButton = new MI_Button(&spr_selectfield, 120, 240, "Weapon Use Limits", 400, 1);
	miProjectilesLimitsMenuButton->SetCode(MENU_CODE_TO_PROJECTILES_LIMITS_MENU);

	miGraphicsOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 280, "Graphics", 400, 1);
	miGraphicsOptionsMenuButton->SetCode(MENU_CODE_TO_GRAPHICS_OPTIONS_MENU);

	miEyeCandyOptionsMenuButton = new MI_Button(&spr_selectfield, 120, 320, "Eye Candy", 400, 1);
	miEyeCandyOptionsMenuButton->SetCode(MENU_CODE_TO_EYECANDY_OPTIONS_MENU);
	
	miSoundOptionsMenuButton = new MI_Button((game_values.soundcapable ? &spr_selectfield : &spr_selectfielddisabled), 120, 360, "Music & Sound", 400, 1);
	
	if(game_values.soundcapable)
		miSoundOptionsMenuButton->SetCode(MENU_CODE_TO_SOUND_OPTIONS_MENU);

	miGenerateMapThumbsButton = new MI_Button(&spr_selectfield, 120, 400, "Refresh Maps", 400, 1);
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
	mOptionsMenu.AddControl(miProjectilesOptionsMenuButton, miPowerupSettingsMenuButton, miProjectilesLimitsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miProjectilesLimitsMenuButton, miProjectilesOptionsMenuButton, miGraphicsOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miGraphicsOptionsMenuButton, miProjectilesLimitsMenuButton, miEyeCandyOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miEyeCandyOptionsMenuButton, miGraphicsOptionsMenuButton, miSoundOptionsMenuButton, NULL, miOptionsMenuBackButton);
	mOptionsMenu.AddControl(miSoundOptionsMenuButton, miEyeCandyOptionsMenuButton, miGenerateMapThumbsButton, NULL, miOptionsMenuBackButton);
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

	miTopLayerField = new MI_SelectField(&spr_selectfield, 70, 120, "Draw Top Layer", 500, 220);
	miTopLayerField->Add("Background", 0, "", false, false);
	miTopLayerField->Add("Foreground", 1, "", true, false);
	miTopLayerField->SetData(NULL, NULL, &game_values.toplayer);
	miTopLayerField->SetKey(game_values.toplayer ? 1 : 0);
	miTopLayerField->SetAutoAdvance(true);

	miFrameLimiterField = new MI_SelectField(&spr_selectfield, 70, 160, "Frame Limit", 500, 220);
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

#ifdef _XBOX
	miScreenSettingsButton = new MI_Button(&spr_selectfield, 70, 200, "Screen Settings", 500, 0);
	miScreenSettingsButton->SetCode(MENU_CODE_TO_SCREEN_SETTINGS);
#else
	miFullscreenField = new MI_SelectField(&spr_selectfield, 70, 200, "Screen Size", 500, 220);
	miFullscreenField->Add("Windowed", 0, "", false, false);
	miFullscreenField->Add("Fullscreen", 1, "", true, false);
	miFullscreenField->SetData(NULL, NULL, &game_values.fullscreen);
	miFullscreenField->SetKey(game_values.fullscreen ? 1 : 0);
	miFullscreenField->SetAutoAdvance(true);
	miFullscreenField->SetItemChangedCode(MENU_CODE_TOGGLE_FULLSCREEN);
#endif //_XBOX

	miMenuGraphicsPackField = new MI_PacksField(&spr_selectfield, 70, 240, "Menu Graphics", 500, 220, &menugraphicspacklist, MENU_CODE_MENU_GRAPHICS_PACK_CHANGED);
	miWorldGraphicsPackField = new MI_PacksField(&spr_selectfield, 70, 280, "World Graphics", 500, 220, &worldgraphicspacklist, MENU_CODE_WORLD_GRAPHICS_PACK_CHANGED);
	miGameGraphicsPackField = new MI_PacksField(&spr_selectfield, 70, 320, "Game Graphics", 500, 220, &gamegraphicspacklist, MENU_CODE_GAME_GRAPHICS_PACK_CHANGED);

	miGraphicsOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miGraphicsOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miGraphicsOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miGraphicsOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miGraphicsOptionsMenuHeaderText = new MI_Text("Graphics Options Menu", 320, 5, 0, 2, 1);

	mGraphicsOptionsMenu.AddControl(miTopLayerField, miGraphicsOptionsMenuBackButton, miFrameLimiterField, NULL, miGraphicsOptionsMenuBackButton);

#ifdef _XBOX
	mGraphicsOptionsMenu.AddControl(miFrameLimiterField, miTopLayerField, miScreenSettingsButton, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miScreenSettingsButton, miFrameLimiterField, miMenuGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miMenuGraphicsPackField, miScreenSettingsButton, miWorldGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
#else
	mGraphicsOptionsMenu.AddControl(miFrameLimiterField, miTopLayerField, miFullscreenField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miFullscreenField, miFrameLimiterField, miMenuGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miMenuGraphicsPackField, miFullscreenField, miWorldGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
#endif

	mGraphicsOptionsMenu.AddControl(miWorldGraphicsPackField, miMenuGraphicsPackField, miGameGraphicsPackField, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miGameGraphicsPackField, miWorldGraphicsPackField, miGraphicsOptionsMenuBackButton, NULL, miGraphicsOptionsMenuBackButton);
	mGraphicsOptionsMenu.AddControl(miGraphicsOptionsMenuBackButton, miGameGraphicsPackField, miTopLayerField, miGameGraphicsPackField, NULL);

	mGraphicsOptionsMenu.AddNonControl(miGraphicsOptionsMenuLeftHeaderBar);
	mGraphicsOptionsMenu.AddNonControl(miGraphicsOptionsMenuRightHeaderBar);
	mGraphicsOptionsMenu.AddNonControl(miGraphicsOptionsMenuHeaderText);

	mGraphicsOptionsMenu.SetHeadControl(miTopLayerField);
	mGraphicsOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);


	//***********************
	// Eye Candy Options
	//***********************

	miSpawnStyleField = new MI_SelectField(&spr_selectfield, 70, 80, "Spawn Style", 500, 220);
	miSpawnStyleField->Add("Instant", 0, "", false, false);
	miSpawnStyleField->Add("Door", 1, "", false, false);
	miSpawnStyleField->Add("Swirl", 2, "", false, false);
	miSpawnStyleField->SetData(&game_values.spawnstyle, NULL, NULL);
	miSpawnStyleField->SetKey(game_values.spawnstyle);

	miAwardStyleField = new MI_SelectField(&spr_selectfield, 70, 120, "Award Style", 500, 220);
	miAwardStyleField->Add("None", 0, "", false, false);
	miAwardStyleField->Add("Fireworks", 1, "", false, false);
	miAwardStyleField->Add("Spiral", 2, "", false, false);
	miAwardStyleField->Add("Ring", 3, "", false, false);
	miAwardStyleField->Add("Souls", 4, "", false, false);
	miAwardStyleField->Add("Text", 5, "", false, false);
	miAwardStyleField->SetData(&game_values.awardstyle, NULL, NULL);
	miAwardStyleField->SetKey(game_values.awardstyle);

	miScoreStyleField = new MI_SelectField(&spr_selectfield, 70, 160, "Score Location", 500, 220);
	miScoreStyleField->Add("Top", 0, "", false, false);
	miScoreStyleField->Add("Bottom", 1, "", false, false);
	miScoreStyleField->Add("Corners", 2, "", false, false);
	miScoreStyleField->SetData(&game_values.scoreboardstyle, NULL, NULL);
	miScoreStyleField->SetKey(game_values.scoreboardstyle);

	miCrunchField = new MI_SelectField(&spr_selectfield, 70, 200, "Screen Crunch", 500, 220);
	miCrunchField->Add("Off", 0, "", false, false);
	miCrunchField->Add("On", 1, "", true, false);
	miCrunchField->SetData(NULL, NULL, &game_values.screencrunch);
	miCrunchField->SetKey(game_values.screencrunch ? 1 : 0);
	miCrunchField->SetAutoAdvance(true);

	miWinningCrownField = new MI_SelectField(&spr_selectfield, 70, 240, "Leader Crown", 500, 220);
	miWinningCrownField->Add("Off", 0, "", false, false);
	miWinningCrownField->Add("On", 1, "", true, false);
	miWinningCrownField->SetData(NULL, NULL, &game_values.showwinningcrown);
	miWinningCrownField->SetKey(game_values.showwinningcrown ? 1 : 0);
	miWinningCrownField->SetAutoAdvance(true);

	miStartCountDownField = new MI_SelectField(&spr_selectfield, 70, 280, "Start Countdown", 500, 220);
	miStartCountDownField->Add("Off", 0, "", false, false);
	miStartCountDownField->Add("On", 1, "", true, false);
	miStartCountDownField->SetData(NULL, NULL, &game_values.startgamecountdown);
	miStartCountDownField->SetKey(game_values.startgamecountdown ? 1 : 0);
	miStartCountDownField->SetAutoAdvance(true);

	miStartModeDisplayField = new MI_SelectField(&spr_selectfield, 70, 320, "Show Mode", 500, 220);
	miStartModeDisplayField->Add("Off", 0, "", false, false);
	miStartModeDisplayField->Add("On", 1, "", true, false);
	miStartModeDisplayField->SetData(NULL, NULL, &game_values.startmodedisplay);
	miStartModeDisplayField->SetKey(game_values.startmodedisplay ? 1 : 0);
	miStartModeDisplayField->SetAutoAdvance(true);

	miDeadTeamNoticeField = new MI_SelectField(&spr_selectfield, 70, 360, "Dead Team Notice", 500, 220);
	miDeadTeamNoticeField->Add("Off", 0, "", false, false);
	miDeadTeamNoticeField->Add("On", 1, "", true, false);
	miDeadTeamNoticeField->SetData(NULL, NULL, &game_values.deadteamnotice);
	miDeadTeamNoticeField->SetKey(game_values.deadteamnotice ? 1 : 0);
	miDeadTeamNoticeField->SetAutoAdvance(true);

	miEyeCandyOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miEyeCandyOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miEyeCandyOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miEyeCandyOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miEyeCandyOptionsMenuHeaderText = new MI_Text("Eye Candy Options Menu", 320, 5, 0, 2, 1);

	mEyeCandyOptionsMenu.AddControl(miSpawnStyleField, miEyeCandyOptionsMenuBackButton, miAwardStyleField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miAwardStyleField, miSpawnStyleField, miScoreStyleField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miScoreStyleField, miAwardStyleField, miCrunchField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miCrunchField, miScoreStyleField, miWinningCrownField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miWinningCrownField, miCrunchField, miStartCountDownField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miStartCountDownField, miWinningCrownField, miStartModeDisplayField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miStartModeDisplayField, miStartCountDownField, miDeadTeamNoticeField, NULL, miEyeCandyOptionsMenuBackButton);
	mEyeCandyOptionsMenu.AddControl(miDeadTeamNoticeField, miStartModeDisplayField, miEyeCandyOptionsMenuBackButton, NULL, miEyeCandyOptionsMenuBackButton);

	mEyeCandyOptionsMenu.AddControl(miEyeCandyOptionsMenuBackButton, miDeadTeamNoticeField, miSpawnStyleField, miDeadTeamNoticeField, NULL);

	mEyeCandyOptionsMenu.AddNonControl(miEyeCandyOptionsMenuLeftHeaderBar);
	mEyeCandyOptionsMenu.AddNonControl(miEyeCandyOptionsMenuRightHeaderBar);
	mEyeCandyOptionsMenu.AddNonControl(miEyeCandyOptionsMenuHeaderText);

	mEyeCandyOptionsMenu.SetHeadControl(miSpawnStyleField);
	mEyeCandyOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);


	//***********************
	// Team Options
	//***********************
	
	miTeamKillsField = new MI_SelectField(&spr_selectfield, 70, 180, "Player Collision", 500, 220);
	miTeamKillsField->Add("Off", 0, "", false, false);
	miTeamKillsField->Add("Assist", 1, "", false, false);
	miTeamKillsField->Add("On", 2, "", false, false);
	miTeamKillsField->SetData(&game_values.teamcollision, NULL, NULL);
	miTeamKillsField->SetKey(game_values.teamcollision);

	miTeamColorsField = new MI_SelectField(&spr_selectfield, 70, 220, "Colors", 500, 220);
	miTeamColorsField->Add("Individual", 0, "", false, false);
	miTeamColorsField->Add("Team", 1, "", true, false);
	miTeamColorsField->SetData(NULL, NULL, &game_values.teamcolors);
	miTeamColorsField->SetKey(game_values.teamcolors ? 1 : 0);
	miTeamColorsField->SetAutoAdvance(true);

	miTournamentControlField = new MI_SelectField(&spr_selectfield, 70, 260, "Tournament Control", 500, 220);
	miTournamentControlField->Add("All", 0, "", false, false);
	miTournamentControlField->Add("Game Winner", 1, "", false, false);
	miTournamentControlField->Add("Game Loser", 2, "", false, false);
	miTournamentControlField->Add("Leading Teams", 3, "", false, false);
	miTournamentControlField->Add("Trailing Teams", 4, "", false, false);
	miTournamentControlField->Add("Random", 5, "", false, false);
	miTournamentControlField->Add("Random Loser", 6, "", false, false);
	miTournamentControlField->Add("Round Robin", 7, "", false, false);
	miTournamentControlField->SetData(&game_values.tournamentcontrolstyle, NULL, NULL);
	miTournamentControlField->SetKey(game_values.tournamentcontrolstyle);

	miTeamOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miTeamOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miTeamOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miTeamOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miTeamOptionsMenuHeaderText = new MI_Text("Team Options Menu", 320, 5, 0, 2, 1);

	mTeamOptionsMenu.AddControl(miTeamKillsField, miTeamOptionsMenuBackButton, miTeamColorsField, NULL, miTeamOptionsMenuBackButton);
	mTeamOptionsMenu.AddControl(miTeamColorsField, miTeamKillsField, miTournamentControlField, NULL, miTeamOptionsMenuBackButton);
	mTeamOptionsMenu.AddControl(miTournamentControlField, miTeamColorsField, miTeamOptionsMenuBackButton, NULL, miTeamOptionsMenuBackButton);
	mTeamOptionsMenu.AddControl(miTeamOptionsMenuBackButton, miTournamentControlField, miTeamKillsField, miTournamentControlField, NULL);

	mTeamOptionsMenu.AddNonControl(miTeamOptionsMenuLeftHeaderBar);
	mTeamOptionsMenu.AddNonControl(miTeamOptionsMenuRightHeaderBar);
	mTeamOptionsMenu.AddNonControl(miTeamOptionsMenuHeaderText);

	mTeamOptionsMenu.SetHeadControl(miTeamKillsField);
	mTeamOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Gameplay Options
	//***********************

	miRespawnField = new MI_SelectField(&spr_selectfield, 70, 60, "Respawn Time", 500, 220);
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
	
	miShieldStyleField = new MI_SelectField(&spr_selectfield, 70, 100, "Shield Style", 500, 220);
	miShieldStyleField->Add("No Shield", 0, "", false, false);
	miShieldStyleField->Add("Soft", 1, "", false, false);
	miShieldStyleField->Add("Soft with Stomp", 2, "", false, false);
	miShieldStyleField->Add("Hard", 3, "", false, false);
	miShieldStyleField->SetData(&game_values.shieldstyle, NULL, NULL);
	miShieldStyleField->SetKey(game_values.shieldstyle);

	miShieldTimeField = new MI_SelectField(&spr_selectfield, 70, 140, "Shield Time", 500, 220);
	miShieldTimeField->Add("0.5 Seconds", 31, "", false, false);
	miShieldTimeField->Add("1.0 Seconds", 62, "", false, false);
	miShieldTimeField->Add("1.5 Seconds", 93, "", false, false);
	miShieldTimeField->Add("2.0 Seconds", 124, "", false, false);
	miShieldTimeField->Add("2.5 Seconds", 155, "", false, false);
	miShieldTimeField->Add("3.0 Seconds", 186, "", false, false);
	miShieldTimeField->Add("3.5 Seconds", 217, "", false, false);
	miShieldTimeField->Add("4.0 Seconds", 248, "", false, false);
	miShieldTimeField->Add("4.5 Seconds", 279, "", false, false);
	miShieldTimeField->Add("5.0 Seconds", 310, "", false, false);
	miShieldTimeField->SetData(&game_values.shieldtime, NULL, NULL);
	miShieldTimeField->SetKey(game_values.shieldtime);

	miBoundsTimeField = new MI_SelectField(&spr_selectfield, 70, 180, "Bounds Time", 500, 220);
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

	miSuicideTimeField = new MI_SelectField(&spr_selectfield, 70, 220, "Suicide Time", 500, 220);
	miSuicideTimeField->Add("Off", 0, "", false, false);
	miSuicideTimeField->Add("3 Seconds", 186, "", false, false);
	miSuicideTimeField->Add("5 Seconds", 310, "", false, false);
	miSuicideTimeField->Add("8 Seconds", 496, "", false, false);
	miSuicideTimeField->Add("10 Seconds", 620, "", false, false);
	miSuicideTimeField->Add("15 Seconds", 930, "", false, false);
	miSuicideTimeField->Add("20 Seconds", 1240, "", false, false);
	miSuicideTimeField->SetData(&game_values.suicidetime, NULL, NULL);
	miSuicideTimeField->SetKey(game_values.suicidetime);

	miWarpLockStyleField = new MI_SelectField(&spr_selectfield, 70, 260, "Warp Lock Style", 500, 220);
	miWarpLockStyleField->Add("Entrance Only", 0, "", false, false);
	miWarpLockStyleField->Add("Exit Only", 1, "", false, false);
	miWarpLockStyleField->Add("Entrance and Exit", 2, "", false, false);
	miWarpLockStyleField->Add("Entire Connection", 3, "", false, false);
	miWarpLockStyleField->Add("All Warps", 4, "", false, false);
	miWarpLockStyleField->SetData(&game_values.warplockstyle, NULL, NULL);
	miWarpLockStyleField->SetKey(game_values.warplockstyle);

	miWarpLockTimeField = new MI_SelectField(&spr_selectfield, 70, 300, "Warp Lock Time", 500, 220);
	miWarpLockTimeField->Add("Off", 0, "", false, false);
	miWarpLockTimeField->Add("1 Second", 62, "", false, false);
	miWarpLockTimeField->Add("2 Seconds", 124, "", false, false);
	miWarpLockTimeField->Add("3 Seconds", 186, "", false, false);
	miWarpLockTimeField->Add("4 Seconds", 248, "", false, false);
	miWarpLockTimeField->Add("5 Seconds", 310, "", false, false);
	miWarpLockTimeField->Add("6 Seconds", 372, "", false, false);
	miWarpLockTimeField->Add("7 Seconds", 434, "", false, false);
	miWarpLockTimeField->Add("8 Seconds", 496, "", false, false);
	miWarpLockTimeField->Add("9 Seconds", 558, "", false, false);
	miWarpLockTimeField->Add("10 Seconds", 620, "", false, false);
	miWarpLockTimeField->SetData(&game_values.warplocktime, NULL, NULL);
	miWarpLockTimeField->SetKey(game_values.warplocktime);

	miBotsField = new MI_SelectField(&spr_selectfield, 70, 340, "Bot Difficulty", 500, 220);
	miBotsField->Add("Very Easy", 0, "", false, false);
	miBotsField->Add("Easy", 1, "", false, false);
	miBotsField->Add("Moderate", 2, "", false, false);
	miBotsField->Add("Hard", 3, "", false, false);
	miBotsField->Add("Very Hard", 4, "", false, false);
	miBotsField->SetData(&game_values.cpudifficulty, NULL, NULL);
	miBotsField->SetKey(game_values.cpudifficulty);

	miPointSpeedField = new MI_SelectField(&spr_selectfield, 70, 380, "Point Speed", 500, 220);
	miPointSpeedField->Add("Very Slow", 60, "", false, false);
	miPointSpeedField->Add("Slow", 40, "", false, false);
	miPointSpeedField->Add("Moderate", 20, "", false, false);
	miPointSpeedField->Add("Fast", 10, "", false, false);
	miPointSpeedField->Add("Very Fast", 5, "", false, false);
	miPointSpeedField->SetData(&game_values.pointspeed, NULL, NULL);
	miPointSpeedField->SetKey(game_values.pointspeed);
	
	miGameplayOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miGameplayOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miGameplayOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miGameplayOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miGameplayOptionsMenuHeaderText = new MI_Text("Gameplay Options Menu", 320, 5, 0, 2, 1);

	mGameplayOptionsMenu.AddControl(miRespawnField, miGameplayOptionsMenuBackButton, miShieldStyleField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miShieldStyleField, miRespawnField, miShieldTimeField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miShieldTimeField, miShieldStyleField, miBoundsTimeField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miBoundsTimeField, miShieldTimeField, miSuicideTimeField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miSuicideTimeField, miBoundsTimeField, miWarpLockStyleField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miWarpLockStyleField, miSuicideTimeField, miWarpLockTimeField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miWarpLockTimeField, miWarpLockStyleField, miBotsField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miBotsField, miWarpLockTimeField, miPointSpeedField, NULL, miGameplayOptionsMenuBackButton);
	mGameplayOptionsMenu.AddControl(miPointSpeedField, miBotsField, miGameplayOptionsMenuBackButton, NULL, miGameplayOptionsMenuBackButton);
	
	mGameplayOptionsMenu.AddControl(miGameplayOptionsMenuBackButton, miPointSpeedField, miRespawnField, miPointSpeedField, NULL);

	mGameplayOptionsMenu.AddNonControl(miGameplayOptionsMenuLeftHeaderBar);
	mGameplayOptionsMenu.AddNonControl(miGameplayOptionsMenuRightHeaderBar);
	mGameplayOptionsMenu.AddNonControl(miGameplayOptionsMenuHeaderText);

	mGameplayOptionsMenu.SetHeadControl(miRespawnField);
	mGameplayOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Sound Options
	//***********************

	miSoundVolumeField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 70, 100, "Sound Volume", 500, 220, 484);
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
	
	miMusicVolumeField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 70, 140, "Music Volume", 500, 220, 484);
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

	miPlayNextMusicField = new MI_SelectField(&spr_selectfield, 70, 180, "Next Music", 500, 220);
	miPlayNextMusicField->Add("Off", 0, "", false, false);
	miPlayNextMusicField->Add("On", 1, "", true, false);
	miPlayNextMusicField->SetData(NULL, NULL, &game_values.playnextmusic);
	miPlayNextMusicField->SetKey(game_values.playnextmusic ? 1 : 0);
	miPlayNextMusicField->SetAutoAdvance(true);

	miAnnouncerField = new MI_AnnouncerField(&spr_selectfield, 70, 220, "Announcer", 500, 220, &announcerlist);
	miSoundPackField = new MI_PacksField(&spr_selectfield, 70, 260, "Sound Pack", 500, 220, &soundpacklist, MENU_CODE_SOUND_PACK_CHANGED);

	miPlaylistField = new MI_PlaylistField(&spr_selectfield, 70, 300, "Game Music Pack", 500, 220);
	miWorldMusicField = new MI_SelectField(&spr_selectfield, 70, 340, "World Music Pack", 500, 220);
	
	int iCurrentMusic = worldmusiclist.GetCurrentIndex();
	worldmusiclist.SetCurrent(0);
	for(short iMusic = 0; iMusic < worldmusiclist.GetCount(); iMusic++)
	{
		miWorldMusicField->Add(worldmusiclist.current_name(), iMusic, "", false, false);
		worldmusiclist.next();
	}
	miWorldMusicField->SetKey(iCurrentMusic);
	worldmusiclist.SetCurrent(iCurrentMusic);
	miWorldMusicField->SetItemChangedCode(MENU_CODE_WORLD_MUSIC_CHANGED);
	
	miPointSpeedField->Add("Very Slow", 60, "", false, false);
	miPointSpeedField->Add("Slow", 40, "", false, false);
	miPointSpeedField->Add("Moderate", 20, "", false, false);
	miPointSpeedField->Add("Fast", 10, "", false, false);
	miPointSpeedField->Add("Very Fast", 5, "", false, false);
	miPointSpeedField->SetData(&game_values.pointspeed, NULL, NULL);
	miPointSpeedField->SetKey(game_values.pointspeed);

	miSoundOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miSoundOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miSoundOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miSoundOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miSoundOptionsMenuHeaderText = new MI_Text("Sound Options Menu", 320, 5, 0, 2, 1);

	mSoundOptionsMenu.AddControl(miSoundVolumeField, miSoundOptionsMenuBackButton, miMusicVolumeField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miMusicVolumeField, miSoundVolumeField, miPlayNextMusicField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miPlayNextMusicField, miMusicVolumeField, miAnnouncerField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miAnnouncerField, miPlayNextMusicField, miSoundPackField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miSoundPackField, miAnnouncerField, miPlaylistField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miPlaylistField, miSoundPackField, miWorldMusicField, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miWorldMusicField, miPlaylistField, miSoundOptionsMenuBackButton, NULL, miSoundOptionsMenuBackButton);
	mSoundOptionsMenu.AddControl(miSoundOptionsMenuBackButton, miWorldMusicField, miSoundVolumeField, miSoundPackField, NULL);
	
	mSoundOptionsMenu.AddNonControl(miSoundOptionsMenuLeftHeaderBar);
	mSoundOptionsMenu.AddNonControl(miSoundOptionsMenuRightHeaderBar);
	mSoundOptionsMenu.AddNonControl(miSoundOptionsMenuHeaderText);

	mSoundOptionsMenu.SetHeadControl(miSoundVolumeField);
	mSoundOptionsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Projectile Limits
	//***********************

	miFireballLimitField = new MI_SelectField(&spr_selectfield, 70, 60, "Fireball Limit", 500, 220);
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

	miHammerLimitField = new MI_SelectField(&spr_selectfield, 70, 100, "Hammer Limit", 500, 220);
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

	miBoomerangLimitField = new MI_SelectField(&spr_selectfield, 70, 140, "Boomerang Limit", 500, 220);
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

	miFeatherLimitField = new MI_SelectField(&spr_selectfield, 70, 180, "Feather Limit", 500, 220);
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

	miLeafLimitField = new MI_SelectField(&spr_selectfield, 70, 220, "Leaf Limit", 500, 220);
	miLeafLimitField->Add("Unlimited", 0, "", false, false);
	miLeafLimitField->Add("2", 2, "", false, false);
	miLeafLimitField->Add("5", 5, "", false, false);
	miLeafLimitField->Add("8", 8, "", false, false);
	miLeafLimitField->Add("10", 10, "", false, false);
	miLeafLimitField->Add("12", 12, "", false, false);
	miLeafLimitField->Add("15", 15, "", false, false);
	miLeafLimitField->Add("20", 20, "", false, false);
	miLeafLimitField->Add("25", 25, "", false, false);
	miLeafLimitField->Add("30", 30, "", false, false);
	miLeafLimitField->Add("40", 40, "", false, false);
	miLeafLimitField->Add("50", 50, "", false, false);
	miLeafLimitField->SetData(&game_values.leaflimit, NULL, NULL);
	miLeafLimitField->SetKey(game_values.leaflimit);

	miPwingsLimitField = new MI_SelectField(&spr_selectfield, 70, 260, "P-Wings Limit", 500, 220);
	miPwingsLimitField->Add("Unlimited", 0, "", false, false);
	miPwingsLimitField->Add("2", 2, "", false, false);
	miPwingsLimitField->Add("5", 5, "", false, false);
	miPwingsLimitField->Add("8", 8, "", false, false);
	miPwingsLimitField->Add("10", 10, "", false, false);
	miPwingsLimitField->Add("12", 12, "", false, false);
	miPwingsLimitField->Add("15", 15, "", false, false);
	miPwingsLimitField->Add("20", 20, "", false, false);
	miPwingsLimitField->Add("25", 25, "", false, false);
	miPwingsLimitField->Add("30", 30, "", false, false);
	miPwingsLimitField->Add("40", 40, "", false, false);
	miPwingsLimitField->Add("50", 50, "", false, false);
	miPwingsLimitField->SetData(&game_values.pwingslimit, NULL, NULL);
	miPwingsLimitField->SetKey(game_values.pwingslimit);

	miTanookiLimitField = new MI_SelectField(&spr_selectfield, 70, 300, "Tanooki Limit", 500, 220);
	miTanookiLimitField ->Add("Unlimited", 0, "", false, false);
	miTanookiLimitField ->Add("2", 2, "", false, false);
	miTanookiLimitField ->Add("5", 5, "", false, false);
	miTanookiLimitField ->Add("8", 8, "", false, false);
	miTanookiLimitField ->Add("10", 10, "", false, false);
	miTanookiLimitField ->Add("12", 12, "", false, false);
	miTanookiLimitField ->Add("15", 15, "", false, false);
	miTanookiLimitField ->Add("20", 20, "", false, false);
	miTanookiLimitField ->Add("25", 25, "", false, false);
	miTanookiLimitField ->Add("30", 30, "", false, false);
	miTanookiLimitField ->Add("40", 40, "", false, false);
	miTanookiLimitField ->Add("50", 50, "", false, false);
	miTanookiLimitField ->SetData(&game_values.tanookilimit, NULL, NULL);
	miTanookiLimitField ->SetKey(game_values.tanookilimit);

	miBombLimitField = new MI_SelectField(&spr_selectfield, 70, 340, "Bomb Limit", 500, 220);
	miBombLimitField ->Add("Unlimited", 0, "", false, false);
	miBombLimitField ->Add("2", 2, "", false, false);
	miBombLimitField ->Add("5", 5, "", false, false);
	miBombLimitField ->Add("8", 8, "", false, false);
	miBombLimitField ->Add("10", 10, "", false, false);
	miBombLimitField ->Add("12", 12, "", false, false);
	miBombLimitField ->Add("15", 15, "", false, false);
	miBombLimitField ->Add("20", 20, "", false, false);
	miBombLimitField ->Add("25", 25, "", false, false);
	miBombLimitField ->Add("30", 30, "", false, false);
	miBombLimitField ->Add("40", 40, "", false, false);
	miBombLimitField ->Add("50", 50, "", false, false);
	miBombLimitField ->SetData(&game_values.bombslimit, NULL, NULL);
	miBombLimitField ->SetKey(game_values.bombslimit);

	miWandLimitField = new MI_SelectField(&spr_selectfield, 70, 380, "Wand Limit", 500, 220);
	miWandLimitField ->Add("Unlimited", 0, "", false, false);
	miWandLimitField ->Add("2", 2, "", false, false);
	miWandLimitField ->Add("5", 5, "", false, false);
	miWandLimitField ->Add("8", 8, "", false, false);
	miWandLimitField ->Add("10", 10, "", false, false);
	miWandLimitField ->Add("12", 12, "", false, false);
	miWandLimitField ->Add("15", 15, "", false, false);
	miWandLimitField ->Add("20", 20, "", false, false);
	miWandLimitField ->Add("25", 25, "", false, false);
	miWandLimitField ->Add("30", 30, "", false, false);
	miWandLimitField ->Add("40", 40, "", false, false);
	miWandLimitField ->Add("50", 50, "", false, false);
	miWandLimitField ->SetData(&game_values.wandlimit, NULL, NULL);
	miWandLimitField ->SetKey(game_values.wandlimit);

	miProjectilesLimitsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miProjectilesLimitsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miProjectilesLimitsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miProjectilesLimitsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miProjectilesLimitsMenuHeaderText = new MI_Text("Weapon Use Limits Menu", 320, 5, 0, 2, 1);

	mProjectilesLimitsMenu.AddControl(miFireballLimitField, miProjectilesLimitsMenuBackButton, miHammerLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miHammerLimitField, miFireballLimitField, miBoomerangLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miBoomerangLimitField, miHammerLimitField, miFeatherLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miFeatherLimitField, miBoomerangLimitField, miLeafLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miLeafLimitField, miFeatherLimitField, miPwingsLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miPwingsLimitField, miLeafLimitField, miTanookiLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miTanookiLimitField, miPwingsLimitField, miBombLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miBombLimitField, miTanookiLimitField, miWandLimitField, NULL, miProjectilesLimitsMenuBackButton);
	mProjectilesLimitsMenu.AddControl(miWandLimitField, miBombLimitField, miProjectilesLimitsMenuBackButton, NULL, miProjectilesLimitsMenuBackButton);
	
	mProjectilesLimitsMenu.AddControl(miProjectilesLimitsMenuBackButton, miWandLimitField, miFireballLimitField, miWandLimitField, NULL);

	mProjectilesLimitsMenu.AddNonControl(miProjectilesLimitsMenuLeftHeaderBar);
	mProjectilesLimitsMenu.AddNonControl(miProjectilesLimitsMenuRightHeaderBar);
	mProjectilesLimitsMenu.AddNonControl(miProjectilesLimitsMenuHeaderText);

	mProjectilesLimitsMenu.SetHeadControl(miFireballLimitField);
	mProjectilesLimitsMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);


	//***********************
	// Projectile Options
	//***********************

	miFireballLifeField = new MI_SelectField(&spr_selectfield, 10, 80, "Life", 305, 120);
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

	miFeatherJumpsField = new MI_SelectField(&spr_selectfield, 10, 150, "Jumps", 305, 120);
	miFeatherJumpsField->Add("1", 1, "", false, false);
	miFeatherJumpsField->Add("2", 2, "", false, false);
	miFeatherJumpsField->Add("3", 3, "", false, false);
	miFeatherJumpsField->Add("4", 4, "", false, false);
	miFeatherJumpsField->Add("5", 5, "", false, false);
	miFeatherJumpsField->SetData(&game_values.featherjumps, NULL, NULL);
	miFeatherJumpsField->SetKey(game_values.featherjumps);

	miBoomerangStyleField = new MI_SelectField(&spr_selectfield, 10, 220, "Style", 305, 120);
	miBoomerangStyleField->Add("Flat", 0, "", false, false);
	miBoomerangStyleField->Add("SMB3", 1, "", false, false);
	miBoomerangStyleField->Add("Zelda", 2, "", false, false);
	miBoomerangStyleField->Add("Random", 3, "", false, false);
	miBoomerangStyleField->SetData(&game_values.boomerangstyle, NULL, NULL);
	miBoomerangStyleField->SetKey(game_values.boomerangstyle);

	miBoomerangLifeField = new MI_SelectField(&spr_selectfield, 10, 260, "Life", 305, 120);
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

	miHammerLifeField = new MI_SelectField(&spr_selectfield, 325, 80, "Life", 305, 120);
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

	miHammerDelayField = new MI_SelectField(&spr_selectfield, 325, 120, "Delay", 305, 120);
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

	miHammerOneKillField = new MI_SelectField(&spr_selectfield, 325, 160, "Power", 305, 120);
	miHammerOneKillField->Add("One Kill", 0, "", true, false);
	miHammerOneKillField->Add("Multiple Kills", 1, "", false, false);
	miHammerOneKillField->SetData(NULL, NULL, &game_values.hammerpower);
	miHammerOneKillField->SetKey(game_values.hammerpower ? 0 : 1);
	miHammerOneKillField->SetAutoAdvance(true);

	miShellLifeField = new MI_SelectField(&spr_selectfield, 10, 330, "Life", 305, 120);
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

	miWandFreezeTimeField = new MI_SelectField(&spr_selectfield, 10, 400, "Freeze", 305, 120);
	miWandFreezeTimeField->Add("1 Second", 62, "", false, false);
	miWandFreezeTimeField->Add("2 Seconds", 124, "", false, false);
	miWandFreezeTimeField->Add("3 Seconds", 186, "", false, false);
	miWandFreezeTimeField->Add("4 Seconds", 248, "", false, false);
	miWandFreezeTimeField->Add("5 Seconds", 310, "", false, false);
	miWandFreezeTimeField->Add("6 Seconds", 372, "", false, false);
	miWandFreezeTimeField->Add("7 Seconds", 434, "", false, false);
	miWandFreezeTimeField->Add("8 Seconds", 496, "", false, false);
	miWandFreezeTimeField->Add("9 Seconds", 558, "", false, false);
	miWandFreezeTimeField->Add("10 Seconds", 620, "", false, false);
	miWandFreezeTimeField->Add("12 Seconds", 744, "", false, false);
	miWandFreezeTimeField->Add("15 Seconds", 930, "", false, false);
	miWandFreezeTimeField->Add("18 Seconds", 1116, "", false, false);
	miWandFreezeTimeField->Add("20 Seconds", 1240, "", false, false);
	miWandFreezeTimeField->SetData(&game_values.wandfreezetime, NULL, NULL);
	miWandFreezeTimeField->SetKey(game_values.wandfreezetime);

	miBlueBlockLifeField = new MI_SelectField(&spr_selectfield, 325, 230, "Blue Life", 305, 120);
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

	miGrayBlockLifeField = new MI_SelectField(&spr_selectfield, 325, 270, "Gray Life", 305, 120);
	miGrayBlockLifeField->Add("Unlimited", 0, "", false, false);
	miGrayBlockLifeField->Add("1 Second", 62, "", false, false);
	miGrayBlockLifeField->Add("2 Seconds", 124, "", false, false);
	miGrayBlockLifeField->Add("3 Seconds", 186, "", false, false);
	miGrayBlockLifeField->Add("4 Seconds", 248, "", false, false);
	miGrayBlockLifeField->Add("5 Seconds", 310, "", false, false);
	miGrayBlockLifeField->Add("6 Seconds", 372, "", false, false);
	miGrayBlockLifeField->Add("7 Seconds", 434, "", false, false);
	miGrayBlockLifeField->Add("8 Seconds", 496, "", false, false);
	miGrayBlockLifeField->Add("9 Seconds", 558, "", false, false);
	miGrayBlockLifeField->Add("10 Seconds", 620, "", false, false);
	miGrayBlockLifeField->Add("15 Seconds", 930, "", false, false);
	miGrayBlockLifeField->Add("20 Seconds", 1240, "", false, false);
	miGrayBlockLifeField->Add("25 Seconds", 1550, "", false, false);
	miGrayBlockLifeField->Add("30 Seconds", 1860, "", false, false);
	miGrayBlockLifeField->SetData(&game_values.grayblockttl, NULL, NULL);
	miGrayBlockLifeField->SetKey(game_values.grayblockttl);

	miRedBlockLifeField = new MI_SelectField(&spr_selectfield, 325, 310, "Red Life", 305, 120);
	miRedBlockLifeField->Add("Unlimited", 0, "", false, false);
	miRedBlockLifeField->Add("1 Second", 62, "", false, false);
	miRedBlockLifeField->Add("2 Seconds", 124, "", false, false);
	miRedBlockLifeField->Add("3 Seconds", 186, "", false, false);
	miRedBlockLifeField->Add("4 Seconds", 248, "", false, false);
	miRedBlockLifeField->Add("5 Seconds", 310, "", false, false);
	miRedBlockLifeField->Add("6 Seconds", 372, "", false, false);
	miRedBlockLifeField->Add("7 Seconds", 434, "", false, false);
	miRedBlockLifeField->Add("8 Seconds", 496, "", false, false);
	miRedBlockLifeField->Add("9 Seconds", 558, "", false, false);
	miRedBlockLifeField->Add("10 Seconds", 620, "", false, false);
	miRedBlockLifeField->Add("15 Seconds", 930, "", false, false);
	miRedBlockLifeField->Add("20 Seconds", 1240, "", false, false);
	miRedBlockLifeField->Add("25 Seconds", 1550, "", false, false);
	miRedBlockLifeField->Add("30 Seconds", 1860, "", false, false);
	miRedBlockLifeField->SetData(&game_values.redblockttl, NULL, NULL);
	miRedBlockLifeField->SetKey(game_values.redblockttl);

	miProjectilesOptionsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miProjectilesOptionsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	miProjectilesOptionsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miProjectilesOptionsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miProjectilesOptionsMenuHeaderText = new MI_Text("Projectile & Weapon Options Menu", 320, 5, 0, 2, 1);

	miFireballText = new MI_Text("Fireball", 10, 50, 0, 2, 0);
	miFeatherText = new MI_Text("Feather", 10, 120, 0, 2, 0);
	miBoomerangText = new MI_Text("Boomerang", 10, 190, 0, 2, 0);
	miHammerText = new MI_Text("Hammer", 325, 50, 0, 2, 0);
	miShellText = new MI_Text("Shell", 10, 300, 0, 2, 0);
	miWandText = new MI_Text("Wand", 10, 370, 0, 2, 0);
	miBlueBlockText = new MI_Text("Throwable Blocks", 325, 200, 0, 2, 0);

	mProjectilesOptionsMenu.AddControl(miFireballLifeField, miProjectilesOptionsMenuBackButton, miFeatherJumpsField, NULL, miHammerLifeField);
	
	mProjectilesOptionsMenu.AddControl(miFeatherJumpsField, miFireballLifeField, miBoomerangStyleField, NULL, miHammerOneKillField);
	
	mProjectilesOptionsMenu.AddControl(miBoomerangStyleField, miFeatherJumpsField, miBoomerangLifeField, NULL, miBlueBlockLifeField);
	mProjectilesOptionsMenu.AddControl(miBoomerangLifeField, miBoomerangStyleField, miShellLifeField, NULL, miGrayBlockLifeField);

	mProjectilesOptionsMenu.AddControl(miShellLifeField, miBoomerangLifeField, miWandFreezeTimeField, NULL, miRedBlockLifeField);

	mProjectilesOptionsMenu.AddControl(miWandFreezeTimeField, miShellLifeField, miHammerLifeField, NULL, miProjectilesOptionsMenuBackButton);

	mProjectilesOptionsMenu.AddControl(miHammerLifeField, miWandFreezeTimeField, miHammerDelayField, miFireballLifeField, NULL);
	mProjectilesOptionsMenu.AddControl(miHammerDelayField, miHammerLifeField, miHammerOneKillField, miFireballLifeField, NULL);
	mProjectilesOptionsMenu.AddControl(miHammerOneKillField, miHammerDelayField, miBlueBlockLifeField, miFeatherJumpsField, NULL);
	
	mProjectilesOptionsMenu.AddControl(miBlueBlockLifeField, miHammerOneKillField, miGrayBlockLifeField, miBoomerangStyleField, NULL);
	mProjectilesOptionsMenu.AddControl(miGrayBlockLifeField, miBlueBlockLifeField, miRedBlockLifeField, miBoomerangLifeField, NULL);
	mProjectilesOptionsMenu.AddControl(miRedBlockLifeField, miGrayBlockLifeField, miProjectilesOptionsMenuBackButton, miShellLifeField, NULL);

	mProjectilesOptionsMenu.AddControl(miProjectilesOptionsMenuBackButton, miRedBlockLifeField, miFireballLifeField, miWandFreezeTimeField, NULL);

	mProjectilesOptionsMenu.AddNonControl(miFireballText);
	mProjectilesOptionsMenu.AddNonControl(miFeatherText);
	mProjectilesOptionsMenu.AddNonControl(miBoomerangText);
	mProjectilesOptionsMenu.AddNonControl(miHammerText);
	mProjectilesOptionsMenu.AddNonControl(miShellText);
	mProjectilesOptionsMenu.AddNonControl(miWandText);
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
	miScreenResizeButton = new MI_Button(&spr_selectfield, 70, 160, "Resize Screen", 500, 0);
	miScreenResizeButton->SetCode(MENU_CODE_TO_SCREEN_RESIZE);

	miScreenHardwareFilterField = new MI_SelectField(&spr_selectfield, 70, 200, "Screen Filter", 500, 220);
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

	miScreenFlickerFilterField = new MI_SliderField(&spr_selectfield, &menu_slider_bar, 70, 240, "Flicker Filter", 500, 220, 380);
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

	miScreenSoftFilterField = new MI_SelectField(&spr_selectfield, 70, 280, "Soften Filter", 500, 220);
	miScreenSoftFilterField->Add("Off", 0, "", false, false);
	miScreenSoftFilterField->Add("On", 1, "", true, false);
	miScreenSoftFilterField->SetData(&game_values.softfilter, NULL, NULL);
	miScreenSoftFilterField->SetKey(game_values.softfilter);
	miScreenSoftFilterField->SetAutoAdvance(true);
	miScreenSoftFilterField->SetItemChangedCode(MENU_CODE_SCREEN_SETTINGS_CHANGED);

	/*
	miScreenAspectRatioField = new MI_SelectField(&spr_selectfield, 70, 300, "10x11 Aspect", 500, 220);
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
	miMatchSelectionField->Add("Single Game", MATCH_TYPE_SINGLE_GAME, "", false, false);
	miMatchSelectionField->Add("Tournament", MATCH_TYPE_TOURNAMENT, "", false, false);
	miMatchSelectionField->Add("Tour", MATCH_TYPE_TOUR, "", false, false);
	miMatchSelectionField->Add("World", MATCH_TYPE_WORLD, "", false, false);
	miMatchSelectionField->Add("Minigame", MATCH_TYPE_MINIGAME, "", false, !game_values.minigameunlocked);
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
		GetNameFromFileName(szTemp, tourlist.GetIndex(iTour), true);
		//strcat(szTemp, " Tour");
		miTourField->Add(szTemp, iTour, "", true, false);
	}
	miTourField->SetData(&game_values.tourindex, NULL, NULL);
	miTourField->SetKey(game_values.tourindex);
	miTourField->Show(false);

	miWorldField = new MI_SelectField(&spr_selectfield, 130, 380, "World", 380, 100);
	for(short iWorld = 0; iWorld < worldlist.GetCount(); iWorld++)
	{
		GetNameFromFileName(szTemp, worldlist.GetIndex(iWorld), true);
		miWorldField->Add(szTemp, iWorld, "", true, false);
	}
	miWorldField->SetData(&game_values.worldindex, NULL, NULL);
	miWorldField->SetKey(game_values.worldindex);
	miWorldField->SetItemChangedCode(MENU_CODE_WORLD_MAP_CHANGED);
	miWorldField->Show(false);

	miMinigameField = new MI_SelectField(&spr_selectfield, 130, 380, "Game", 380, 100);
	miMinigameField->Add("Pipe Coin Game", 0, "", false, false);
	miMinigameField->Add("Hammer Boss Game", 1, "", false, false);
	miMinigameField->Add("Bomb Boss Game", 2, "", false, false);
	miMinigameField->Add("Fire Boss Game", 3, "", false, false);
	miMinigameField->Add("Boxes Game", 4, "", false, false);
	miMinigameField->SetData(&game_values.selectedminigame, NULL, NULL);
	miMinigameField->SetKey(game_values.selectedminigame);
	miMinigameField->Show(false);

	miMatchSelectionMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miMatchSelectionMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miMatchSelectionMenuHeaderText = new MI_Text("Match Type Menu", 320, 5, 0, 2, 1);

	miMatchSelectionDisplayImage = new MI_Image(&menu_match_select, 160, 80, 0, 0, 320, 240, 1, 1, 0);
	miWorldPreviewDisplay = new MI_WorldPreviewDisplay(160, 80, 20, 15);
	miWorldPreviewDisplay->Show(false);

	mMatchSelectionMenu.AddNonControl(miMatchSelectionMenuLeftHeaderBar);
	mMatchSelectionMenu.AddNonControl(miMatchSelectionMenuRightHeaderBar);
	mMatchSelectionMenu.AddNonControl(miMatchSelectionMenuHeaderText);

	mMatchSelectionMenu.AddNonControl(miWorldPreviewDisplay);
	mMatchSelectionMenu.AddNonControl(miMatchSelectionDisplayImage);

	mMatchSelectionMenu.AddControl(miMatchSelectionField, miMatchSelectionStartButton, miTournamentField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miTournamentField, miMatchSelectionField, miTourField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miTourField, miTournamentField, miWorldField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miWorldField, miTourField, miMinigameField, NULL, NULL);
	mMatchSelectionMenu.AddControl(miMinigameField, miWorldField, miMatchSelectionStartButton, NULL, NULL);
	mMatchSelectionMenu.AddControl(miMatchSelectionStartButton, miMinigameField, miMatchSelectionField, NULL, NULL);

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
		GetNameFromFileName(szTemp, filterslist.GetIndex(iFilter), true);
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
	
	for(short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
		mGameSettingsMenu.AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], miModeSettingsButton);

	mGameSettingsMenu.AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miMapField, miGoalField[GAMEMODE_LAST - 2], miModeSettingsButton);
	
	mGameSettingsMenu.AddControl(miModeSettingsButton, miModeField, miMapField, miGoalField[GAMEMODE_LAST - 1], NULL);
	mGameSettingsMenu.AddControl(miMapField, miGoalField[GAMEMODE_LAST - 1], miMapFiltersButton, NULL, NULL);
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

	miBonusWheel = new MI_BonusWheel(144, 38);

	mBonusWheelMenu.AddControl(miBonusWheel, NULL, NULL, NULL, NULL);
	mBonusWheelMenu.SetHeadControl(miBonusWheel);
	mBonusWheelMenu.SetCancelCode(MENU_CODE_BONUS_DONE);


	//***********************
	// Powerup Selection Menu
	//***********************
	
	miPowerupSelection = new MI_PowerupSelection(50, 44, 640, 8);
	miPowerupSelection->SetAutoModify(true);

	miPowerupSelectionLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miPowerupSelectionMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miPowerupSelectionMenuHeaderText = new MI_Text("Item Selection Menu", 320, 5, 0, 2, 1);

	mPowerupSelectionMenu.AddControl(miPowerupSelection, NULL, NULL, NULL, NULL);

	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionLeftHeaderBar);
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionMenuRightHeaderBar);
	mPowerupSelectionMenu.AddNonControl(miPowerupSelectionMenuHeaderText);
	
	mPowerupSelectionMenu.SetHeadControl(miPowerupSelection);
	mPowerupSelectionMenu.SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

	//***********************
	// Powerup Settings Menu
	//***********************
	
	miStoredPowerupDelayField = new MI_SelectField(&spr_selectfield, 70, 100, "Item Use Speed", 500, 220);
	miStoredPowerupDelayField->Add("Very Slow", 2, "", false, false);
	miStoredPowerupDelayField->Add("Slow", 3, "", false, false);
	miStoredPowerupDelayField->Add("Moderate", 4, "", false, false);
	miStoredPowerupDelayField->Add("Fast", 5, "", false, false);
	miStoredPowerupDelayField->Add("Very Fast", 6, "", false, false);
	miStoredPowerupDelayField->SetData(&game_values.storedpowerupdelay, NULL, NULL);
	miStoredPowerupDelayField->SetKey(game_values.storedpowerupdelay);

	miItemRespawnField = new MI_SelectField(&spr_selectfield, 70, 140, "Item Spawn", 500, 220);
	miItemRespawnField->Add("Off", 0, "", false, false);
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

	miHiddenBlockRespawnField = new MI_SelectField(&spr_selectfield, 70, 180, "Hidden Block Hide", 500, 220);
	miHiddenBlockRespawnField->Add("Off", 0, "", false, false);
	miHiddenBlockRespawnField->Add("5 Seconds", 310, "", false, false);
	miHiddenBlockRespawnField->Add("10 Seconds", 620, "", false, false);
	miHiddenBlockRespawnField->Add("15 Seconds", 930, "", false, false);
	miHiddenBlockRespawnField->Add("20 Seconds", 1240, "", false, false);
	miHiddenBlockRespawnField->Add("25 Seconds", 1550, "", false, false);
	miHiddenBlockRespawnField->Add("30 Seconds", 1860, "", false, false);
	miHiddenBlockRespawnField->Add("35 Seconds", 2170, "", false, false);
	miHiddenBlockRespawnField->Add("40 Seconds", 2480, "", false, false);
	miHiddenBlockRespawnField->Add("45 Seconds", 2790, "", false, false);
	miHiddenBlockRespawnField->Add("50 Seconds", 3100, "", false, false);
	miHiddenBlockRespawnField->Add("55 Seconds", 3410, "", false, false);
	miHiddenBlockRespawnField->Add("60 Seconds", 3720, "", false, false);
	miHiddenBlockRespawnField->SetData(&game_values.hiddenblockrespawn, NULL, NULL);
	miHiddenBlockRespawnField->SetKey(game_values.hiddenblockrespawn);

	miSwapStyleField = new MI_SelectField(&spr_selectfield, 70, 220, "Swap Style", 500, 220);
	miSwapStyleField->Add("Walk", 0, "", false, false);
	miSwapStyleField->Add("Blink", 1, "", false, false);
	miSwapStyleField->Add("Instant", 2, "", false, false);
	miSwapStyleField->SetData(&game_values.swapstyle, NULL, NULL);
	miSwapStyleField->SetKey(game_values.swapstyle);

	miBonusWheelField = new MI_SelectField(&spr_selectfield, 70, 260, "Bonus Wheel", 500, 220);
	miBonusWheelField->Add("Off", 0, "", false, false);
	miBonusWheelField->Add("Tournament Win", 1, "", false, false);
	miBonusWheelField->Add("Every Game", 2, "", false, false);
	miBonusWheelField->SetData(&game_values.bonuswheel, NULL, NULL);
	miBonusWheelField->SetKey(game_values.bonuswheel);

	miKeepPowerupField = new MI_SelectField(&spr_selectfield, 70, 300, "Bonus Item", 500, 220);
	miKeepPowerupField->Add("Until Next Spin", 0, "", false, false);
	miKeepPowerupField->Add("Keep Always", 1, "", true, false);
	miKeepPowerupField->SetData(NULL, NULL, &game_values.keeppowerup);
	miKeepPowerupField->SetKey(game_values.keeppowerup ? 1 : 0);
	miKeepPowerupField->SetAutoAdvance(true);

	miStoredPowerupResetButton = new MI_StoredPowerupResetButton(&spr_selectfield, 70, 340, "Reset Stored Items", 500, 0);
	miStoredPowerupResetButton->SetCode(MENU_CODE_RESET_STORED_POWERUPS);

	miPowerupSettingsMenuBackButton = new MI_Button(&spr_selectfield, 544, 432, "Back", 80, 1);
	miPowerupSettingsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
	
	miPowerupSettingsMenuLeftHeaderBar = new MI_Image(&menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
	miPowerupSettingsMenuRightHeaderBar = new MI_Image(&menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
	miPowerupSettingsMenuHeaderText = new MI_Text("Item Settings Menu", 320, 5, 0, 2, 1);

	mPowerupSettingsMenu.AddControl(miStoredPowerupDelayField, miPowerupSettingsMenuBackButton, miItemRespawnField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miItemRespawnField, miStoredPowerupDelayField, miHiddenBlockRespawnField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miHiddenBlockRespawnField, miItemRespawnField, miSwapStyleField, NULL, miPowerupSettingsMenuBackButton);
	mPowerupSettingsMenu.AddControl(miSwapStyleField, miHiddenBlockRespawnField, miBonusWheelField, NULL, miPowerupSettingsMenuBackButton);
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
	iUnlockMinigameOptionIndex = 0;

	iTournamentAIStep = 0;
	iTournamentAITimer = 0;

	//Reset the keys each time we switch from menu to game and back
	game_values.playerInput.ResetKeys();

	fNeedMenuMusicReset = false;

	if(game_values.gamemode->winningteam > -1 && game_values.tournamentwinner == -1 && 
		(((game_values.matchtype == MATCH_TYPE_SINGLE_GAME || game_values.matchtype == MATCH_TYPE_QUICK_GAME || game_values.matchtype == MATCH_TYPE_MINIGAME || game_values.matchtype == MATCH_TYPE_TOURNAMENT) && game_values.bonuswheel == 2) || (game_values.matchtype == MATCH_TYPE_TOUR && game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType)))
	{
		miBonusWheel->Reset(false);
		mCurrentMenu = &mBonusWheelMenu;
	}
	else if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME && game_values.matchtype != MATCH_TYPE_QUICK_GAME && game_values.matchtype != MATCH_TYPE_MINIGAME)
	{
		mCurrentMenu = &mTournamentScoreboardMenu;
	}
	else if(game_values.matchtype == MATCH_TYPE_QUICK_GAME)
	{
		//Reset back to main menu after quick game
		mCurrentMenu = &mMainMenu;
		mCurrentMenu->ResetMenu();
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

			//Clear out the stored powerups after a game that had a winner
			if(!game_values.worldskipscoreboard)
			{
				//Only clear out the stored powerup if we were allowed to use it in the game
				if(game_values.gamemode->HasStoredPowerups())
				{
					for(short iPlayer = 0; iPlayer < 4; iPlayer++)
						game_values.storedpowerups[iPlayer] = -1;
				}
			}
		}
		/*else if(game_values.tournamentwinner > -1) //World is completed
		{
			miBonusWheel->Reset(true);
			mCurrentMenu = &mBonusWheelMenu;
		}*/

		UpdateScoreBoard();

		//If we're suposed to skip the scoreboard, then reset back to the world map
		if(game_values.worldskipscoreboard)
		{
			mCurrentMenu = &mWorldMenu;
			mCurrentMenu->ResetMenu();

			game_values.worldskipscoreboard = false;
		}
		else
		{
			miTournamentScoreboard->RefreshWorldScores(game_values.gamemode->winningteam);
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
		{
			miTournamentScoreboard->RefreshTournamentScores(game_values.gamemode->winningteam);

			//Set the next controlling team
			switch(game_values.tournamentcontrolstyle)
			{
				case 1: //Winning Team
				{
					game_values.tournamentcontrolteam = game_values.gamemode->winningteam;
					break;
				}

				case 2: //Losing Team
				{
					short iNumInPlace = 0;
					short iInPlace[4];
					short iLowestPlace = 0;
					for(short iScore = 0; iScore < score_cnt; iScore++)
					{
						if(score[iScore]->place == iLowestPlace)
						{
							iInPlace[iNumInPlace++] = iScore;
						}
						else if(score[iScore]->place > iLowestPlace)
						{
							iNumInPlace = 1;
							iInPlace[0] = iScore;
							iLowestPlace = score[iScore]->place;
						}
					}

					game_values.tournamentcontrolteam = iInPlace[rand() % iNumInPlace];
					break;
				}

				case 3: //Tournament Ahead Teams
				{
					short iNumInPlace = 0;
					short iInPlace[4];
					short iMostWins = 0;
					for(short iTeam = 0; iTeam < score_cnt; iTeam++)
					{
						if(game_values.tournament_scores[iTeam].wins == iMostWins)
						{
							iInPlace[iNumInPlace++] = iTeam;
						}
						else if(game_values.tournament_scores[iTeam].wins > iMostWins)
						{
							iNumInPlace = 1;
							iInPlace[0] = iTeam;
							iMostWins = game_values.tournament_scores[iTeam].wins;
						}
					}

					game_values.tournamentcontrolteam = iInPlace[rand() % iNumInPlace];
					break;
				}

				case 4: //Tournament Behind Teams
				{
					short iNumInPlace = 0;
					short iInPlace[4];
					short iLeastWins = 20; //Most possible wins are 10

					for(short iTeam = 0; iTeam < score_cnt; iTeam++)
					{
						if(game_values.tournament_scores[iTeam].wins == iLeastWins)
						{
							iInPlace[iNumInPlace++] = iTeam;
						}
						else if(game_values.tournament_scores[iTeam].wins < iLeastWins)
						{
							iNumInPlace = 1;
							iInPlace[0] = iTeam;
							iLeastWins = game_values.tournament_scores[iTeam].wins;
						}
					}

					game_values.tournamentcontrolteam = iInPlace[rand() % iNumInPlace];
					break;
				}

				case 5: //Random
				{
					game_values.tournamentcontrolteam = rand() % score_cnt;
					break;
				}

				case 6: //Random Losing Team
				{
					short iNumInPlace = 0;
					short iInPlace[4];
					short iWinner = 0;

					for(short iTeam = 0; iTeam < score_cnt; iTeam++)
					{
						if(score[iTeam]->place == 0)
						{
							iWinner = iTeam;
							break;
						}
					}

					for(short iTeam = 0; iTeam < score_cnt; iTeam++)
					{
						if(iTeam == iWinner)
							continue;

						iInPlace[iNumInPlace++] = iTeam;
					}

					game_values.tournamentcontrolteam = iInPlace[rand() % iNumInPlace];
					break;
				}

				case 7: //Round Robin
				{
					game_values.tournamentcontrolteam = game_values.tournamentnextcontrol;

					if(++game_values.tournamentnextcontrol >= score_cnt)
						game_values.tournamentnextcontrol = 0;

					break;
				}

				default:
				{
					game_values.tournamentcontrolteam = -1;
					break;
				}
			}
		}
	}

	//Reset game mode back to the current game mode in case we came from boss mode
	game_values.gamemode = gamemodes[currentgamemode];

	//Keep track if we entered the menu loop as part of a tournament, if we exit the tournament
	//we need to reset the menu music back to normal
	if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME && game_values.matchtype != MATCH_TYPE_QUICK_GAME && game_values.matchtype != MATCH_TYPE_MINIGAME)
		fNeedMenuMusicReset = true;

	if(game_values.music)
	{
		if(game_values.tournamentwinner < 0)
		{
			if(game_values.matchtype == MATCH_TYPE_SINGLE_GAME || game_values.matchtype == MATCH_TYPE_QUICK_GAME || game_values.matchtype == MATCH_TYPE_MINIGAME)
				backgroundmusic[2].play(false, false);
			else if(game_values.matchtype == MATCH_TYPE_WORLD)
				backgroundmusic[5].play(false, false);
			else
				backgroundmusic[3].play(false, false);
		}
	}

	if(game_values.matchtype != MATCH_TYPE_WORLD && game_values.matchtype != MATCH_TYPE_QUICK_GAME)
	{
		if(mCurrentMenu == &mGameSettingsMenu || mCurrentMenu == &mTournamentScoreboardMenu)
			miMapField->LoadCurrentMap();
	}

	float realfps = 0, flipfps = 0;
	unsigned int framestart, ticks;

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

#ifdef _DEBUG
					if(event.key.keysym.sym == SDLK_F2)
					{
						game_values.frameadvance = !game_values.frameadvance;
					}
#endif
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

							miFullscreenField->SetKey(game_values.fullscreen ? 1 : 0);

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

		//If AI is controlling the tournament menu, select the options
		if(game_values.matchtype == MATCH_TYPE_TOURNAMENT && iTournamentAITimer > 0 && mCurrentMenu == &mGameSettingsMenu && mCurrentMenu->GetHeadControl() == miSettingsStartButton)
		{
			if(--iTournamentAITimer == 0)
			{
				iTournamentAIStep++;

				if(iTournamentAIStep == 1)
				{
					miMapField->ChooseRandomMap();

					iTournamentAITimer = 60;
				}
				else if(iTournamentAIStep == 2)
				{
					currentgamemode = rand() % GAMEMODE_LAST;
					game_values.gamemode = gamemodes[currentgamemode];

					miModeField->SetKey(currentgamemode);

					//Unhide/hide the settings button
					game_values.gamemode = gamemodes[miModeField->GetShortValue()];
					miModeSettingsButton->Show(miModeField->GetShortValue() != game_mode_owned);

					//Show the approprate goal field
					for(short iMode = 0; iMode < GAMEMODE_LAST; iMode++)
					{
						miGoalField[iMode]->Show(miModeField->GetShortValue() == iMode);
					}

					iTournamentAITimer = 60;
				}
				else if(iTournamentAIStep == 3)
				{
					SModeOption * options = game_values.gamemode->GetOptions();
					
					//Choose a goal from the lower values for a quicker game
					short iRandOption = (rand() % 6) + 1;
					game_values.gamemode->goal  = options[iRandOption].iValue;

					miGoalField[currentgamemode]->SetKey(gamemodes[currentgamemode]->goal);

					modeOptionsMenu.SetRandomGameModeSettings(game_values.gamemode->gamemode);
					
					iTournamentAITimer = 60;
				}
				else if(iTournamentAIStep == 4)
				{
					iTournamentAIStep = 0;
					StartGame();
				}
			}
		}

		//Watch for the konami code to unlock the minigames match type
		if(!game_values.minigameunlocked && mCurrentMenu == &mMatchSelectionMenu)
		{
			if(!mCurrentMenu->GetCurrentControl()->IsModifying())
			{
				int keymask =
					(game_values.playerInput.outputControls[0].menu_up.fPressed?1:0) |
					(game_values.playerInput.outputControls[0].menu_down.fPressed?2:0) |
					(game_values.playerInput.outputControls[0].menu_left.fPressed?4:0) |
					(game_values.playerInput.outputControls[0].menu_right.fPressed?8:0) |
					(game_values.playerInput.outputControls[0].menu_random.fPressed?16:0);

				if (iUnlockMinigameOptionIndex < 11)
				{
					static const int konami_code[11] = {1,1,2,2,4,8,4,8,16,16,16};
			        
					if (keymask & konami_code[iUnlockMinigameOptionIndex]) 
						iUnlockMinigameOptionIndex++;
					else if (keymask & ~konami_code[iUnlockMinigameOptionIndex]) 
					{
						iUnlockMinigameOptionIndex = 0;

						if (keymask & konami_code[iUnlockMinigameOptionIndex]) 
							iUnlockMinigameOptionIndex++;
					}
			        
					if (iUnlockMinigameOptionIndex == 11)
					{
						ifsoundonplay(sfx_transform); 
						game_values.minigameunlocked = true;

						miMatchSelectionField->HideItem(MATCH_TYPE_MINIGAME, false);
						miMatchSelectionField->SetKey(MATCH_TYPE_MINIGAME);

						miTournamentField->Show(false);
						miTourField->Show(false);
						miWorldField->Show(false);
						miMinigameField->Show(true);

						miMatchSelectionDisplayImage->Show(true);
						miWorldPreviewDisplay->Show(false);
						miMatchSelectionDisplayImage->SetImage(0, 240 * game_values.matchtype, 320, 240);
					}
				}
			}
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
				iUnlockMinigameOptionIndex = 0;
			}
			else if(MENU_CODE_TO_MATCH_SELECTION_MENU == code)
			{
				miWorldPreviewDisplay->SetWorld();

				mCurrentMenu = &mMatchSelectionMenu;
				mCurrentMenu->ResetMenu();
				iUnlockMinigameOptionIndex = 0;
			}
			else if(MENU_CODE_MATCH_SELECTION_START == code || MENU_CODE_QUICK_GAME_START == code)
			{
				if(MENU_CODE_QUICK_GAME_START == code)
					game_values.matchtype = MATCH_TYPE_QUICK_GAME;
				else
					game_values.matchtype = miMatchSelectionField->GetShortValue();

				miTeamSelect->Reset();
				mCurrentMenu = &mTeamSelectMenu;
				mCurrentMenu->ResetMenu();

				if(game_values.matchtype != MATCH_TYPE_TOURNAMENT)
				{
					game_values.tournamentcontrolteam = -1;
					SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, false);
				}
			}
			else if(MENU_CODE_MATCH_SELECTION_MATCH_CHANGED == code)
			{
				miTournamentField->Show(game_values.matchtype == MATCH_TYPE_TOURNAMENT);
				miTourField->Show(game_values.matchtype == MATCH_TYPE_TOUR);
				miWorldField->Show(game_values.matchtype == MATCH_TYPE_WORLD);
				miMinigameField->Show(game_values.matchtype == MATCH_TYPE_MINIGAME);

				//miMatchSelectionDisplayImage->Show(game_values.matchtype != MATCH_TYPE_WORLD);
				miWorldPreviewDisplay->Show(game_values.matchtype == MATCH_TYPE_WORLD);
				
				if(game_values.matchtype == MATCH_TYPE_WORLD)
					miMatchSelectionDisplayImage->SetImage(320, 0, 320, 240);
				else
					miMatchSelectionDisplayImage->SetImage(0, 240 * game_values.matchtype, 320, 240);
			}
			else if(MENU_CODE_WORLD_MAP_CHANGED == code)
			{
				miWorldPreviewDisplay->SetWorld();
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

				if(MATCH_TYPE_MINIGAME == game_values.matchtype)
				{
					short iMiniGameType = miMinigameField->GetShortValue();
					
					if(iMiniGameType == 0) //Pipe minigame
					{
						pipegamemode->goal = 50;
						game_values.gamemode = pipegamemode;
					}
					else if(iMiniGameType >= 1 && iMiniGameType <= 3)  //3 types of boss minigames
					{
						game_values.gamemodemenusettings.boss.bosstype = iMiniGameType - 1;
						game_values.gamemodemenusettings.boss.difficulty = 2;
						game_values.gamemodemenusettings.boss.hitpoints = 5;

						bossgamemode->goal = 5;
						game_values.gamemode = bossgamemode;
					}
					else if(iMiniGameType == 4) //boxes minigame
					{
						boxesgamemode->goal = 10;
						game_values.gamemode = boxesgamemode;
					}

					StartGame();
				}
				else if(MATCH_TYPE_QUICK_GAME == game_values.matchtype)
				{
					short iRandomMode = rand() % GAMEMODE_LAST;
					game_values.gamemode = gamemodes[iRandomMode];
						
					SModeOption * options = game_values.gamemode->GetOptions();
					
					//Choose a goal from the lower values for a quicker game
					short iRandOption = (rand() % 6) + 1;
					game_values.gamemode->goal  = options[iRandOption].iValue;

					game_values.tournamentwinner = -1;

					StartGame();
				}
				else
				{

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
						//Set who is controlling the tournament menu
						if(game_values.tournamentcontrolstyle == 5 || game_values.tournamentcontrolstyle == 6) //Random
							game_values.tournamentcontrolteam = rand() % score_cnt;
						else if(game_values.tournamentcontrolstyle == 7) //Round robin
							game_values.tournamentcontrolteam = 0;
						else //The first game of the tournament is controlled by all players
							game_values.tournamentcontrolteam = -1;
		
						game_values.tournamentnextcontrol = 1;  //if round robin is selected, choose the next team next					
						
						miTournamentScoreboard->CreateScoreboard(score_cnt, game_values.tournamentgames, &menu_mode_large);
					}
					else if(game_values.matchtype == MATCH_TYPE_WORLD)
					{
						if(!g_worldmap.Load(TILESIZE))
						{
							iDisplayError = DISPLAY_ERROR_READ_WORLD_FILE;
							iDisplayErrorTimer = 120;
							fErrorReadingTourFile = true;
						}
						else
						{
							miTournamentScoreboard->CreateScoreboard(score_cnt, 0, &spr_tour_markers);

							g_worldmap.SetInitialPowerups();

							//If a player had a stored powerup from another game, add it to their inventory
							for(short iPlayer = 0; iPlayer < 4; iPlayer++)
							{
								if(game_values.storedpowerups[iPlayer] != -1)
								{
									short iTeamId = LookupTeamID(iPlayer);
									if(game_values.worldpowerupcount[iTeamId] < 32)
										game_values.worldpowerups[iTeamId][game_values.worldpowerupcount[iTeamId]++] = game_values.storedpowerups[iPlayer];
			
									game_values.storedpowerups[iPlayer] = -1;	
								}
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
							maplist.findexact(szCurrentMapName, false);
							miMapField->LoadCurrentMap();

							game_values.gamemode = gamemodes[miModeField->GetShortValue()];
							
							for(short iMode = 0; iMode < GAMEMODE_LAST; iMode++)
							{
								gamemodes[iMode]->goal = miGoalField[iMode]->GetShortValue();
							}

							miModeSettingsButton->Show(miModeField->GetShortValue() != game_mode_owned);
							
							mCurrentMenu = &mGameSettingsMenu;
							mCurrentMenu->ResetMenu();

							//If it is a tournament, then set the controlling team 
							if(MATCH_TYPE_TOURNAMENT == game_values.matchtype)
								SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, true);
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
							game_values.gamestate = GS_START_WORLD;

							//Play enter world sound
							ifsoundonandreadyplay(sfx_enterstage);
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
			}
			else if(MENU_CODE_BACK_TO_GAME_SETUP_MENU == code)
			{
				bool fNeedTeamAnnouncement = false;
				if(game_values.matchtype == MATCH_TYPE_WORLD)
				{
					if(game_values.tournamentwinner == -2 || (game_values.tournamentwinner >= 0 && game_values.bonuswheel == 0))
					{
						ResetTournamentBackToMainMenu();
					}
					else if(game_values.tournamentwinner >= 0)
					{
						miBonusWheel->Reset(true);
						mCurrentMenu = &mBonusWheelMenu;
					}
					else
					{
						mCurrentMenu = &mWorldMenu;
						miWorldStop->Refresh(game_values.tourstopcurrent);

						fNeedTeamAnnouncement = true;
					}
				}
				else
				{
					if(game_values.tournamentwinner == -2) //Tied Tour Result
					{
						ResetTournamentBackToMainMenu();
					}
					else if(game_values.tournamentwinner >= 0) //Tournament/Tour Won and Bonus Wheel will be spun
					{
						if(game_values.bonuswheel == 0 || (game_values.matchtype == MATCH_TYPE_TOUR && !game_values.tourstops[game_values.tourstopcurrent - 1]->iBonusType))
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
						{
							mCurrentMenu = &mTourStopMenu;
						}
						else
						{
							mCurrentMenu = &mGameSettingsMenu;
						}
					}
				}

				mCurrentMenu->ResetMenu();

				//Set the controlling team for tournament mode
				if(MATCH_TYPE_TOURNAMENT == game_values.matchtype && game_values.tournamentwinner == -1)
					SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, true);

				if(fNeedTeamAnnouncement)
					miWorld->DisplayTeamControlAnnouncement();
			}
			else if(MENU_CODE_BACK_TO_GAME_SETUP_MENU_FROM_MODE_SETTINGS == code)
			{
				mCurrentMenu = &mGameSettingsMenu;
			}
			else if(MENU_CODE_MODE_CHANGED == code)
			{
				game_values.gamemode = gamemodes[miModeField->GetShortValue()];
				miModeSettingsButton->Show(miModeField->GetShortValue() != game_mode_owned);

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

					if(iTournamentAITimer > 0)
						SetControllingTeamForSettingsMenu(-1, false);
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
				else
					SetControllingTeamForSettingsMenu(game_values.tournamentcontrolteam, false);
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
					//Clear out any stored items a player might have
					for(short iPlayer = 0; iPlayer < 4; iPlayer++)
						game_values.storedpowerups[iPlayer] = -1;

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
						{
							mCurrentMenu = &mTournamentScoreboardMenu;
						}
						else if(game_values.matchtype == MATCH_TYPE_SINGLE_GAME)
						{
							mCurrentMenu = &mGameSettingsMenu;
							mCurrentMenu->ResetMenu();
							miMapField->LoadCurrentMap();
						}
						else
						{
							ResetTournamentBackToMainMenu();
						}
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
			else if (MENU_CODE_TO_GRAPHICS_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mGraphicsOptionsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_EYECANDY_OPTIONS_MENU == code)
			{
				mCurrentMenu = &mEyeCandyOptionsMenu;
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
			else if (MENU_CODE_TO_PROJECTILES_LIMITS_MENU == code)
			{
				mCurrentMenu = &mProjectilesLimitsMenu;
				mCurrentMenu->ResetMenu();
			}
			else if (MENU_CODE_TO_MODE_SETTINGS_MENU == code)
			{
				for(short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++)
				{
					if(miGoalField[iGameMode]->IsVisible())
					{
						mCurrentMenu = modeOptionsMenu.GetOptionsMenu(iGameMode);
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
			else if (MENU_CODE_WORLD_GRAPHICS_PACK_CHANGED == code)
			{
				LoadWorldGraphics();
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
			else if(MENU_CODE_WORLD_MUSIC_CHANGED == code)
			{
				worldmusiclist.SetCurrent(miWorldMusicField->GetShortValue());
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

					if(iGameMode == game_mode_pipe_minigame)
						game_values.gamemode = pipegamemode;
					else if(iGameMode == game_mode_boss_minigame)
						game_values.gamemode = bossgamemode;
					else if(iGameMode == game_mode_boxes_minigame)
						game_values.gamemode = boxesgamemode;
					else
						game_values.gamemode = gamemodes[iGameMode];

					game_values.gamemode->goal = game_values.tourstops[game_values.tourstopcurrent]->iGoal;
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
			else if(MENU_CODE_HEALTH_MODE_START_LIFE_CHANGED == code)
			{
				modeOptionsMenu.HealthModeStartLifeChanged();
			}
			else if(MENU_CODE_HEALTH_MODE_MAX_LIFE_CHANGED == code)
			{
				modeOptionsMenu.HealthModeMaxLifeChanged();
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

		if(game_values.screenfade == 255)
		{
			if(GS_START_GAME == game_values.gamestate)
			{
				if(game_values.matchtype == MATCH_TYPE_QUICK_GAME)
					modeOptionsMenu.SetRandomGameModeSettings(game_values.gamemode->gamemode);
				else
					SetGameModeSettingsFromMenu();

				if(game_values.matchtype == MATCH_TYPE_WORLD && game_values.tourstops[game_values.tourstopcurrent]->iStageType == 1)
				{
					g_map.loadMap(convertPath("maps/special/two52_special_bonushouse.map"), read_type_full);
					LoadCurrentMapBackground();

					if(game_values.music)
					{
						backgroundmusic[0].load(worldmusiclist.GetMusic(WORLDMUSICBONUS, ""));
						backgroundmusic[0].play(false, false);
					}
				}
				else
				{
					std::string sShortMapName = "";

					bool fMiniGameMapFound = false;

					if(game_values.matchtype == MATCH_TYPE_WORLD)
					{
						if(game_values.gamemode->gamemode == game_mode_pipe_minigame || 
							game_values.gamemode->gamemode == game_mode_boss_minigame ||
							game_values.gamemode->gamemode == game_mode_boxes_minigame)
						{
							fMiniGameMapFound = maplist.findexact(game_values.tourstops[game_values.tourstopcurrent]->pszMapFile, true);

							if(fMiniGameMapFound)
							{
								g_map.loadMap(maplist.currentFilename(), read_type_full);
								sShortMapName = maplist.currentShortmapname();
							}
						}
					}

					if(game_values.gamemode->gamemode == game_mode_pipe_minigame)
					{
						if(!fMiniGameMapFound)
						{
							g_map.loadMap(convertPath("maps/special/two52_special_pipe_minigame.map"), read_type_full);
							sShortMapName = "minigamepipe";
						}
					}
					else if(game_values.gamemode->gamemode == game_mode_boss_minigame)
					{
						if(!fMiniGameMapFound)
						{
							short iBossType = game_values.gamemodesettings.boss.bosstype;
							bossgamemode->SetBossType(iBossType);
							if(iBossType == 0)
								g_map.loadMap(convertPath("maps/special/two52_special_hammerboss_minigame.map"), read_type_full);
							else if(iBossType == 1)
								g_map.loadMap(convertPath("maps/special/two52_special_bombboss_minigame.map"), read_type_full);
							else if(iBossType == 2)
								g_map.loadMap(convertPath("maps/special/two52_special_fireboss_minigame.map"), read_type_full);
							
							sShortMapName = "minigameboss";
						}
					}
					else if(game_values.gamemode->gamemode == game_mode_boxes_minigame)
					{
						if(!fMiniGameMapFound)
						{
							g_map.loadMap(convertPath("maps/special/two52_special_boxes_minigame.map"), read_type_full);
							sShortMapName = "minigameboxes";
						}
					}
					else if(game_values.matchtype == MATCH_TYPE_QUICK_GAME)
					{
						//Load a random map for the quick game
						const char * szMapName = maplist.randomFilename();
						g_map.loadMap(szMapName, read_type_full);
						sShortMapName = stripPathAndExtension(szMapName);
					}
					else
					{
						g_map.loadMap(maplist.currentFilename(), read_type_full);
						sShortMapName = maplist.currentShortmapname();
					}

					LoadCurrentMapBackground();

					//Allows all players to start the game
					game_values.singleplayermode = -1;

					if(game_values.music)
					{
						musiclist.SetRandomMusic(g_map.musicCategoryID, sShortMapName.c_str(), g_map.szBackgroundFile);
						backgroundmusic[0].load(musiclist.GetCurrentMusic());
						backgroundmusic[0].play(game_values.playnextmusic, false);
					}
				}

				game_values.gamestate = GS_GAME;			

				g_map.predrawbackground(spr_background, spr_backmap[0]);
				g_map.predrawforeground(spr_frontmap[0]);
				
				g_map.predrawbackground(spr_background, spr_backmap[1]);
				g_map.predrawforeground(spr_frontmap[1]);

				g_map.SetupAnimatedTiles();
				LoadMapObjects(false);

				return;
			}
			else if(GS_START_WORLD == game_values.gamestate) //Fade to world match type
			{
				game_values.screenfadespeed = -8;

				mCurrentMenu = &mWorldMenu;
				mCurrentMenu->ResetMenu();

				backgroundmusic[2].stop();
				backgroundmusic[5].load(worldmusiclist.GetMusic(g_worldmap.GetMusicCategory(), g_worldmap.GetWorldName()));
				backgroundmusic[5].play(false, false);
				fNeedMenuMusicReset = true;

				miWorld->DisplayTeamControlAnnouncement();

				game_values.gamestate = GS_MENU;
			}
		}

		if(fGenerateMapThumbs)
		{
			menu_dialog.draw(160, 176, 0, 0, 160, 64);
			menu_dialog.draw(320, 176, 352, 0, 160, 64);
			menu_dialog.draw(160, 240, 0, 416, 160, 64);
			menu_dialog.draw(320, 240, 352, 416, 160, 64);
			menu_font_large.drawCentered(320, 215, "Refreshing Map Thumbnails");
			menu_font_large.drawCentered(320, 245, "Please Wait...");
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

		
		//Sleep for time just under what we need
		short delay = (short)(game_values.framelimiter - SDL_GetTicks() + framestart - 2);

		if(delay > 0)
		{
			if(delay > game_values.framelimiter)
				delay = game_values.framelimiter;
			
			SDL_Delay(delay);
		}

		//Fine tune wait here
		while(SDL_GetTicks() - framestart < (unsigned short)game_values.framelimiter)
			SDL_Delay(0);   //keep framerate constant at 1000/game_values.framelimiter fps

		//Debug code to slow framerate down to 1 fps to see exact movement
#ifdef _DEBUG
		if(game_values.frameadvance)
		{
			delay = (short)(1000 - SDL_GetTicks() + framestart);

			if(delay > 0)
			{
				if(delay > 1000)
					delay = 1000;
				
				SDL_Delay(delay);
			}

			while(SDL_GetTicks() - framestart < 1000)
				SDL_Delay(0);
		}
#endif

		ticks = SDL_GetTicks() - framestart;
		if(ticks == 0)
			ticks = game_values.framelimiter;

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
	int iVersion[4] = {0, 0, 0, 0};
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

			//If it is not "[none]", then add this announcer sound
			if(strcmp(szBuffer, "[none]"))
				sfx_announcer[announcerIndex].init(convertPath(szBuffer));

			announcerIndex++;
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

void Menu::SetControllingTeamForSettingsMenu(short iControlTeam, bool fDisplayMessage)
{
	mGameSettingsMenu.SetControllingTeam(iControlTeam);

	modeOptionsMenu.SetControllingTeam(iControlTeam);

	miMapFilterScroll->SetControllingTeam(iControlTeam);
	miMapBrowser->SetControllingTeam(iControlTeam);

	if(fDisplayMessage)
		DisplayControllingTeamMessage(iControlTeam);

	//Scan controlling team members and if they are only bots, then let the cpu control the selection
	iTournamentAITimer = 0;
	iTournamentAIStep = 0;
	mGameSettingsMenu.SetAllowExit(false);

	if(iControlTeam >= 0)
	{
		bool fNeedAI = true;
		for(short iPlayer = 0; iPlayer < game_values.teamcounts[iControlTeam]; iPlayer++)
		{
			if(game_values.playercontrol[game_values.teamids[iControlTeam][iPlayer]] == 1)
			{
				fNeedAI = false;
				break;
			}
		}

		if(fNeedAI)
		{
			iTournamentAITimer = 60;
			mGameSettingsMenu.SetAllowExit(true);
		}
	}
}

void Menu::DisplayControllingTeamMessage(short iControlTeam)
{
	//Display the team that is in control of selecting the next game
	char szMessage[128];
	if(iControlTeam < 0)
		sprintf(szMessage, "All Teams Are In Control");
	else if(game_values.teamcounts[iControlTeam] <= 1)
		sprintf(szMessage, "Player %d Is In Control", game_values.teamids[iControlTeam][0] + 1);
	else
		sprintf(szMessage, "Team %d Is In Control", iControlTeam + 1);

	mCurrentMenu->AddEyeCandy(new EC_Announcement(&menu_font_large, &spr_announcementicons, szMessage, iControlTeam < 0 ? 4 : game_values.colorids[game_values.teamids[iControlTeam][0]], 120, 100));
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

	if(game_values.matchtype != MATCH_TYPE_SINGLE_GAME && game_values.matchtype != MATCH_TYPE_QUICK_GAME && game_values.matchtype != MATCH_TYPE_MINIGAME)
	{
		if(fNeedMenuMusicReset)
		{
			backgroundmusic[3].stop();
			backgroundmusic[2].play(false, false);
			fNeedMenuMusicReset = false;
		}
	}
}

//Debugging code for writing and playing test scripts
#ifdef _DEBUG

void Menu::LoadScript(const char * szScriptFile)
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

void Menu::SaveScript(const char * szScriptFile)
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
		
#if defined(__MACOSX__)
		chmod(file.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
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

