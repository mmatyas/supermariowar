#include "MainMenu.h"

#include "ResourceManager.h"

extern CResourceManager* rm;

UI_MainMenu::UI_MainMenu() : UI_Menu()
{
	miSMWTitle = new MI_Image(&rm->menu_smw, smw->ScreenWidth/2 - ((short)rm->menu_smw.getWidth() >> 1), 30, 0, 0, 372, 140, 1, 1, 0);
    miSMWVersion = new MI_Image(&rm->menu_version, smw->ScreenWidth * 0.89f, 10, 0, 0, 58, 32, 1, 1, 0);
    //miSMWVersionText = new MI_Text("Beta 2", 630, 45, 0, 2, 2);

    miMainStartButton = new MI_Button(&rm->spr_selectfield, 120, 210, "Start", 310, 0);
    miMainStartButton->SetCode(MENU_CODE_TO_MATCH_SELECTION_MENU);

    miQuickGameButton = new MI_Button(&rm->spr_selectfield, 440, 210, "Go!", 80, 0);
    miQuickGameButton->SetCode(MENU_CODE_QUICK_GAME_START);

    miPlayerSelect = new MI_PlayerSelect(&rm->menu_player_select, 120, 250, "Players", 400, 140);

    miMultiplayerButton = new MI_Button(&rm->spr_selectfield, 120, 322, "Multiplayer", 400, 0);
    miMultiplayerButton->SetCode(MENU_CODE_TO_NET_SERVERS_MENU);

    miOptionsButton = new MI_Button(&rm->spr_selectfield, 120, 362, "Options", 200, 0);
    miOptionsButton->SetCode(MENU_CODE_TO_OPTIONS_MENU);

    miControlsButton = new MI_Button(&rm->spr_selectfield, 320, 362, "Controls", 200, 0);
    miControlsButton->SetCode(MENU_CODE_TO_CONTROLS_MENU);

    miExitButton = new MI_Button(&rm->spr_selectfield, 120, 402, "Exit", smw->ScreenWidth * 0.625f, 0);
    miExitButton->SetCode(MENU_CODE_EXIT_APPLICATION);

    AddControl(miMainStartButton, miExitButton, miPlayerSelect, NULL, miQuickGameButton);
    AddControl(miQuickGameButton, miExitButton, miPlayerSelect, miMainStartButton, NULL);
    AddControl(miPlayerSelect, miMainStartButton, miMultiplayerButton, NULL, NULL);
    AddControl(miMultiplayerButton, miPlayerSelect, miOptionsButton, NULL, NULL);
    AddControl(miOptionsButton, miMultiplayerButton, miExitButton, miControlsButton, miControlsButton);
    AddControl(miControlsButton, miMultiplayerButton, miExitButton, miOptionsButton, miOptionsButton);
    AddControl(miExitButton, miOptionsButton, miMainStartButton, NULL, NULL);

    SetHeadControl(miMainStartButton);
#ifndef _XBOX
    SetCancelCode(MENU_CODE_EXIT_APPLICATION);
#endif

    AddNonControl(miSMWTitle);
    AddNonControl(miSMWVersion);
    //AddNonControl(miSMWVersionText);
};

UI_MainMenu::~UI_MainMenu() {
}
