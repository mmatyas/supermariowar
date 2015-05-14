#include "WorldMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_WorldMenu::UI_WorldMenu() : UI_Menu()
{
    miWorld = new MI_World();
    miWorld->SetAutoModify(true);

    miWorldStop = new MI_TourStop(70, 45, true);
    miWorldStop->Show(false);

    //Exit tour dialog box
    miWorldExitDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miWorldExitDialogExitTourText = new MI_Text("Exit World", 320, 205, 0, 2, 1);

    miWorldExitDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, 1);
    miWorldExitDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, 1);

    miWorldExitDialogYesButton->SetCode(MENU_CODE_EXIT_WORLD_YES);
    miWorldExitDialogNoButton->SetCode(MENU_CODE_EXIT_WORLD_NO);

    miWorldExitDialogImage->Show(false);
    miWorldExitDialogExitTourText->Show(false);
    miWorldExitDialogYesButton->Show(false);
    miWorldExitDialogNoButton->Show(false);

    AddControl(miWorld, NULL, NULL, NULL, NULL);

    AddControl(miWorldStop, NULL, NULL, NULL, NULL);

    AddNonControl(miWorldExitDialogImage);
    AddNonControl(miWorldExitDialogExitTourText);

    AddControl(miWorldExitDialogYesButton, NULL, NULL, NULL, miWorldExitDialogNoButton);
    AddControl(miWorldExitDialogNoButton, NULL, NULL, miWorldExitDialogYesButton, NULL);

    SetHeadControl(miWorld);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);
};

UI_WorldMenu::~UI_WorldMenu() {
}

void UI_WorldMenu::OpenStageStart()
{
    miWorldStop->Refresh(game_values.tourstopcurrent);
    miWorldStop->Show(true);

    RememberCurrent();

    SetHeadControl(miWorldStop);
    SetCancelCode(MENU_CODE_WORLD_STAGE_NO_START);

    ResetMenu();
}

void UI_WorldMenu::CloseStageStart()
{
    miWorldStop->Show(false);

    SetHeadControl(miWorld);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}

void UI_WorldMenu::OpenExitDialog()
{
    miWorldExitDialogImage->Show(true);
    miWorldExitDialogExitTourText->Show(true);
    miWorldExitDialogYesButton->Show(true);
    miWorldExitDialogNoButton->Show(true);

    RememberCurrent();

    SetHeadControl(miWorldExitDialogNoButton);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_WorldMenu::CloseExitDialog()
{
    miWorldExitDialogImage->Show(false);
    miWorldExitDialogExitTourText->Show(false);
    miWorldExitDialogYesButton->Show(false);
    miWorldExitDialogNoButton->Show(false);

    SetHeadControl(miWorld);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}
