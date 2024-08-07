#include "NetNewRoomSettingsMenu.h"

#include "GameMode.h"
#include "net.h"
#include "ResourceManager.h"
#include "menu/GameSettingsMenu.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_ImageSelectField.h"
#include "ui/MI_MapField.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

#include <cassert>

extern CResourceManager* rm;


UI_NetNewRoomSettingsMenu::UI_NetNewRoomSettingsMenu(const UI_GameSettingsMenu* gsm) : UI_Menu()
{
    miContinueButton = new MI_Button(&rm->spr_selectfield, 70, 45, "Continue", 500);
    miContinueButton->SetCode(MENU_CODE_TO_NET_NEW_ROOM_SETTINGS_MENU);

    miModeField = new MI_ImageSelectField(*(gsm->miModeField));
    for (short iGoalField = 0; iGoalField < GAMEMODE_LAST; iGoalField++)
        miGoalField[iGoalField] = new MI_SelectField<short>(*(gsm->miGoalField[iGoalField]));

    miMapField = new MI_MapField(&rm->spr_selectfield, 70, 165, "Map", 400, 120, true);
    netplay.mapfilepath = getCurrentMapPath();


    //AddControl(miModeField, miContinueButton, miGoalField[0], NULL, NULL);

/*    AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);

    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], NULL);

    AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miMapField, miGoalField[GAMEMODE_LAST - 2], NULL);
*/

    miModeSettingsButton = new MI_Button(&rm->spr_selectfield, 430, 125, "Settings", 140);
    miModeSettingsButton->SetCode(MENU_CODE_TO_MODE_SETTINGS_MENU);


    AddControl(miContinueButton, miMapField, miModeField, NULL, NULL);
    AddControl(miModeField, miContinueButton, miGoalField[0], NULL, NULL);
    AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);
    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        AddControl(miGoalField[iGoalField],
            miGoalField[iGoalField - 1],
            miGoalField[iGoalField + 1],
            miGoalField[iGoalField - 1],
            miModeSettingsButton);
    AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miMapField, miGoalField[GAMEMODE_LAST - 2], NULL);
    AddControl(miModeSettingsButton, miModeField, miMapField, miGoalField[GAMEMODE_LAST - 1], NULL);
    AddControl(miMapField, miGoalField[GAMEMODE_LAST - 1], miContinueButton, NULL, NULL);

    miLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miHeaderText = new MI_HeaderText("Multiplayer Level Select Menu", 320, 5);

    AddNonControl(miLeftHeaderBar);
    AddNonControl(miRightHeaderBar);
    AddNonControl(miHeaderText);

    //AddNonControl(miMapFiltersOnImage);

    setInitialFocus(miContinueButton);
    SetCancelCode(MENU_CODE_TO_NET_LOBBY_MENU);
}

void UI_NetNewRoomSettingsMenu::RefreshGameModeButtons()
{
    for (short iMode = 0; iMode < GAMEMODE_LAST; iMode++)
        miGoalField[iMode]->setVisible(miModeField->currentValue() == iMode);
}

std::string UI_NetNewRoomSettingsMenu::getCurrentMapPath() const
{
    return miMapField->GetMapFilePath();
}

short UI_NetNewRoomSettingsMenu::getSelectedGameModeID() const {
    for (short iGameMode = 0; iGameMode < GAMEMODE_LAST; iGameMode++) {
        if (miGoalField[iGameMode]->IsVisible())
            return iGameMode;
    }

    assert(false);
    return 0;
}
