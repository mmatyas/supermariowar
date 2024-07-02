#include "TourStopMenu.h"

#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_Text.h"
#include "ui/MI_TourStop.h"

extern CResourceManager* rm;

UI_TourStopMenu::UI_TourStopMenu()
    : UI_Menu()
{
    miTourStop = new MI_TourStop(70, 45, false);

    // Exit tour dialog box
    miTourStopExitDialogImage = new MI_Image(&rm->spr_dialog, 224, 176, 0, 0, 192, 128, 1, 1, 0);
    miTourStopExitDialogExitTourText = new MI_HeaderText("Exit Tour", 320, 205);

    miTourStopExitDialogYesButton = new MI_Button(&rm->spr_selectfield, 235, 250, "Yes", 80, TextAlign::CENTER);
    miTourStopExitDialogNoButton = new MI_Button(&rm->spr_selectfield, 325, 250, "No", 80, TextAlign::CENTER);

    miTourStopExitDialogYesButton->SetCode(MENU_CODE_EXIT_TOUR_YES);
    miTourStopExitDialogNoButton->SetCode(MENU_CODE_EXIT_TOUR_NO);

    miTourStopExitDialogImage->setVisible(false);
    miTourStopExitDialogExitTourText->setVisible(false);
    miTourStopExitDialogYesButton->setVisible(false);
    miTourStopExitDialogNoButton->setVisible(false);

    AddControl(miTourStop, NULL, NULL, NULL, NULL);

    AddNonControl(miTourStopExitDialogImage);
    AddNonControl(miTourStopExitDialogExitTourText);

    AddControl(miTourStopExitDialogYesButton, NULL, NULL, NULL, miTourStopExitDialogNoButton);
    AddControl(miTourStopExitDialogNoButton, NULL, NULL, miTourStopExitDialogYesButton, NULL);

    setInitialFocus(miTourStop);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);
};

void UI_TourStopMenu::OpenExitDialog()
{
    miTourStopExitDialogImage->setVisible(true);
    miTourStopExitDialogExitTourText->setVisible(true);
    miTourStopExitDialogYesButton->setVisible(true);
    miTourStopExitDialogNoButton->setVisible(true);

    RememberCurrent();

    setInitialFocus(miTourStopExitDialogNoButton);
    SetCancelCode(MENU_CODE_NONE);
    ResetMenu();
}

void UI_TourStopMenu::CloseExitDialog()
{
    miTourStopExitDialogImage->setVisible(false);
    miTourStopExitDialogExitTourText->setVisible(false);
    miTourStopExitDialogYesButton->setVisible(false);
    miTourStopExitDialogNoButton->setVisible(false);

    setInitialFocus(miTourStop);
    SetCancelCode(MENU_CODE_BACK_TEAM_SELECT_MENU);

    RestoreCurrent();
}
