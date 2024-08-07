#include "WorldMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_Text.h"
#include "ui/MI_TourStop.h"
#include "ui/MI_World.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_WorldMenu::UI_WorldMenu()
    : UI_Menu()
{
    miWorld = new MI_World();
    miWorld->SetAutoModify(true);

    miWorldStop = new MI_TourStop(70, 45, true);
    miWorldStop->setVisible(false);

    // Exit tour dialog box
    miWorldExitDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miWorldExitDialogExitTourText = new MI_HeaderText("Exit World", 320, 205);

    miWorldExitDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, TextAlign::CENTER);
    miWorldExitDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, TextAlign::CENTER);

    miWorldExitDialogYesButton->SetCode(MENU_CODE_EXIT_WORLD_YES);
    miWorldExitDialogNoButton->SetCode(MENU_CODE_EXIT_WORLD_NO);

    miWorldExitDialogImage->setVisible(false);
    miWorldExitDialogExitTourText->setVisible(false);
    miWorldExitDialogYesButton->setVisible(false);
    miWorldExitDialogNoButton->setVisible(false);

    AddControl(miWorld, NULL, NULL, NULL, NULL);

    AddControl(miWorldStop, NULL, NULL, NULL, NULL);

    AddNonControl(miWorldExitDialogImage);
    AddNonControl(miWorldExitDialogExitTourText);

    AddControl(miWorldExitDialogYesButton, NULL, NULL, NULL, miWorldExitDialogNoButton);
    AddControl(miWorldExitDialogNoButton, NULL, NULL, miWorldExitDialogYesButton, NULL);

    setInitialFocus(miWorld);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);
};

void UI_WorldMenu::OpenStageStart()
{
    miWorldStop->Refresh(game_values.tourstopcurrent);
    miWorldStop->setVisible(true);

    RememberCurrent();

    setInitialFocus(miWorldStop);
    SetCancelCode(MENU_CODE_WORLD_STAGE_NO_START);

    ResetMenu();
}

void UI_WorldMenu::CloseStageStart()
{
    miWorldStop->setVisible(false);

    setInitialFocus(miWorld);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}

void UI_WorldMenu::OpenExitDialog()
{
    miWorldExitDialogImage->setVisible(true);
    miWorldExitDialogExitTourText->setVisible(true);
    miWorldExitDialogYesButton->setVisible(true);
    miWorldExitDialogNoButton->setVisible(true);

    RememberCurrent();

    setInitialFocus(miWorldExitDialogNoButton);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_WorldMenu::CloseExitDialog()
{
    miWorldExitDialogImage->setVisible(false);
    miWorldExitDialogExitTourText->setVisible(false);
    miWorldExitDialogYesButton->setVisible(false);
    miWorldExitDialogNoButton->setVisible(false);

    setInitialFocus(miWorld);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}
