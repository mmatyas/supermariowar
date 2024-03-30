#include "PowerupSettingsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_StoredPowerupResetButton.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;


UI_PowerupSettingsMenu::UI_PowerupSettingsMenu()
    : UI_Menu()
{
    miStoredPowerupDelayField = new MI_SelectField(&rm->spr_selectfield, 70, 100, "Item Use Speed", 500, 220);
    miStoredPowerupDelayField->Add("Very Slow", 2);
    miStoredPowerupDelayField->Add("Slow", 3);
    miStoredPowerupDelayField->Add("Moderate", 4);
    miStoredPowerupDelayField->Add("Fast", 5);
    miStoredPowerupDelayField->Add("Very Fast", 6);
    miStoredPowerupDelayField->SetData(&game_values.storedpowerupdelay, NULL, NULL);
    miStoredPowerupDelayField->SetKey(game_values.storedpowerupdelay);

    miItemRespawnField = new MI_SelectField(&rm->spr_selectfield, 70, 140, "Item Spawn", 500, 220);
    miItemRespawnField->Add("Off", 0);
    miItemRespawnField->Add("5 Seconds", 310);
    miItemRespawnField->Add("10 Seconds", 620);
    miItemRespawnField->Add("15 Seconds", 930);
    miItemRespawnField->Add("20 Seconds", 1240);
    miItemRespawnField->Add("25 Seconds", 1550);
    miItemRespawnField->Add("30 Seconds", 1860);
    miItemRespawnField->Add("35 Seconds", 2170);
    miItemRespawnField->Add("40 Seconds", 2480);
    miItemRespawnField->Add("45 Seconds", 2790);
    miItemRespawnField->Add("50 Seconds", 3100);
    miItemRespawnField->Add("55 Seconds", 3410);
    miItemRespawnField->Add("60 Seconds", 3720);
    miItemRespawnField->SetData(&game_values.itemrespawntime, NULL, NULL);
    miItemRespawnField->SetKey(game_values.itemrespawntime);

    miHiddenBlockRespawnField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Hidden Block Hide", 500, 220);
    miHiddenBlockRespawnField->Add("Off", 0);
    miHiddenBlockRespawnField->Add("5 Seconds", 310);
    miHiddenBlockRespawnField->Add("10 Seconds", 620);
    miHiddenBlockRespawnField->Add("15 Seconds", 930);
    miHiddenBlockRespawnField->Add("20 Seconds", 1240);
    miHiddenBlockRespawnField->Add("25 Seconds", 1550);
    miHiddenBlockRespawnField->Add("30 Seconds", 1860);
    miHiddenBlockRespawnField->Add("35 Seconds", 2170);
    miHiddenBlockRespawnField->Add("40 Seconds", 2480);
    miHiddenBlockRespawnField->Add("45 Seconds", 2790);
    miHiddenBlockRespawnField->Add("50 Seconds", 3100);
    miHiddenBlockRespawnField->Add("55 Seconds", 3410);
    miHiddenBlockRespawnField->Add("60 Seconds", 3720);
    miHiddenBlockRespawnField->SetData(&game_values.hiddenblockrespawn, NULL, NULL);
    miHiddenBlockRespawnField->SetKey(game_values.hiddenblockrespawn);

    miSwapStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 220, "Swap Style", 500, 220);
    miSwapStyleField->Add("Walk", 0);
    miSwapStyleField->Add("Blink", 1);
    miSwapStyleField->Add("Instant", 2);
    miSwapStyleField->SetData(&game_values.swapstyle, NULL, NULL);
    miSwapStyleField->SetKey(game_values.swapstyle);

    miBonusWheelField = new MI_SelectField(&rm->spr_selectfield, 70, 260, "Bonus Wheel", 500, 220);
    miBonusWheelField->Add("Off", 0);
    miBonusWheelField->Add("Tournament Win", 1);
    miBonusWheelField->Add("Every Game", 2);
    miBonusWheelField->SetData(&game_values.bonuswheel, NULL, NULL);
    miBonusWheelField->SetKey(game_values.bonuswheel);

    miKeepPowerupField = new MI_SelectField(&rm->spr_selectfield, 70, 300, "Bonus Item", 500, 220);
    miKeepPowerupField->Add("Until Next Spin", 0);
    miKeepPowerupField->Add("Keep Always", 1, "", true, false);
    miKeepPowerupField->SetData(NULL, NULL, &game_values.keeppowerup);
    miKeepPowerupField->SetKey(game_values.keeppowerup ? 1 : 0);
    miKeepPowerupField->SetAutoAdvance(true);

    miStoredPowerupResetButton = new MI_StoredPowerupResetButton(&rm->spr_selectfield, 70, 340, "Reset Stored Items", 500);
    miStoredPowerupResetButton->SetCode(MENU_CODE_RESET_STORED_POWERUPS);

    miPowerupSettingsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miPowerupSettingsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miPowerupSettingsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miPowerupSettingsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miPowerupSettingsMenuHeaderText = new MI_HeaderText("Item Settings Menu", 320, 5);

    AddControl(miStoredPowerupDelayField, miPowerupSettingsMenuBackButton, miItemRespawnField, NULL, miPowerupSettingsMenuBackButton);
    AddControl(miItemRespawnField, miStoredPowerupDelayField, miHiddenBlockRespawnField, NULL, miPowerupSettingsMenuBackButton);
    AddControl(miHiddenBlockRespawnField, miItemRespawnField, miSwapStyleField, NULL, miPowerupSettingsMenuBackButton);
    AddControl(miSwapStyleField, miHiddenBlockRespawnField, miBonusWheelField, NULL, miPowerupSettingsMenuBackButton);
    AddControl(miBonusWheelField, miSwapStyleField, miKeepPowerupField, NULL, miPowerupSettingsMenuBackButton);
    AddControl(miKeepPowerupField, miBonusWheelField, miStoredPowerupResetButton, NULL, miPowerupSettingsMenuBackButton);
    AddControl(miStoredPowerupResetButton, miKeepPowerupField, miPowerupSettingsMenuBackButton, NULL, miPowerupSettingsMenuBackButton);

    AddControl(miPowerupSettingsMenuBackButton, miStoredPowerupResetButton, miStoredPowerupDelayField, miStoredPowerupResetButton, NULL);

    AddNonControl(miPowerupSettingsMenuLeftHeaderBar);
    AddNonControl(miPowerupSettingsMenuRightHeaderBar);
    AddNonControl(miPowerupSettingsMenuHeaderText);

    SetHeadControl(miStoredPowerupDelayField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};
