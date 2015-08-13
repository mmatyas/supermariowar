#include "NetNewLevelMenu.h"

#include "GameMode.h"
#include "net.h"
#include "ResourceManager.h"

extern CResourceManager* rm;

UI_NetNewLevelMenu::UI_NetNewLevelMenu(const UI_GameSettingsMenu* gsm) : UI_Menu()
{
    miNetNewLevelContinueButton = new MI_Button(&rm->spr_selectfield, 70, 45, "Continue", 500, 0);
    miNetNewLevelContinueButton->SetCode(MENU_CODE_TO_NET_NEW_ROOM_SETTINGS_MENU);

    miNetNewLevelModeField = new MI_ImageSelectField(*(gsm->miModeField));
    for (short iGoalField = 0; iGoalField < GAMEMODE_LAST; iGoalField++)
        miNetNewLevelGoalField[iGoalField] = new MI_SelectField(*(gsm->miGoalField[iGoalField]));

    miNetNewLevelMapField = new MI_MapField(&rm->spr_selectfield, 70, 165, "Map", 400, 120, true);
    netplay.mapfilepath = getCurrentMapPath();

    AddControl(miNetNewLevelContinueButton, miNetNewLevelMapField, miNetNewLevelModeField, NULL, NULL);
    AddControl(miNetNewLevelModeField, miNetNewLevelContinueButton, miNetNewLevelGoalField[0], NULL, NULL);

    AddControl(miNetNewLevelGoalField[0], miNetNewLevelModeField, miNetNewLevelGoalField[1], NULL, /*FIXME: miModeSettingsButton*/ NULL);
    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        AddControl(miNetNewLevelGoalField[iGoalField],
            miNetNewLevelGoalField[iGoalField - 1],
            miNetNewLevelGoalField[iGoalField + 1],
            miNetNewLevelGoalField[iGoalField - 1], NULL);
    AddControl(miNetNewLevelGoalField[GAMEMODE_LAST - 1], miNetNewLevelGoalField[GAMEMODE_LAST - 2], miNetNewLevelMapField, miNetNewLevelGoalField[GAMEMODE_LAST - 2], NULL);

    //AddControl(miModeField, miNetNewLevelContinueButton, miGoalField[0], NULL, NULL);

/*    AddControl(miGoalField[0], miModeField, miGoalField[1], NULL, miModeSettingsButton);

    for (short iGoalField = 1; iGoalField < GAMEMODE_LAST - 1; iGoalField++)
        AddControl(miGoalField[iGoalField], miGoalField[iGoalField - 1], miGoalField[iGoalField + 1], miGoalField[iGoalField - 1], NULL);

    AddControl(miGoalField[GAMEMODE_LAST - 1], miGoalField[GAMEMODE_LAST - 2], miMapField, miGoalField[GAMEMODE_LAST - 2], NULL);
*/
    AddControl(miNetNewLevelMapField, miNetNewLevelGoalField[GAMEMODE_LAST - 1], miNetNewLevelContinueButton, NULL, NULL);


    miNetNewLevelLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miNetNewLevelRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miNetNewLevelHeaderText = new MI_Text("Multiplayer Level Select Menu", 320, 5, 0, 2, 1);

    AddNonControl(miNetNewLevelLeftHeaderBar);
    AddNonControl(miNetNewLevelRightHeaderBar);
    AddNonControl(miNetNewLevelHeaderText);

    //AddNonControl(miMapFiltersOnImage);

    SetHeadControl(miNetNewLevelContinueButton);
    SetCancelCode(MENU_CODE_TO_NET_LOBBY_MENU);
}

UI_NetNewLevelMenu::~UI_NetNewLevelMenu() {
}

std::string UI_NetNewLevelMenu::getCurrentMapPath()
{
    return miNetNewLevelMapField->GetMapFilePath();
}
