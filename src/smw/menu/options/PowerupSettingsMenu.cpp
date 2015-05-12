#include "PowerupSettingsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"

extern CResourceManager* rm;
extern CGameValues game_values;

UI_PowerupSettingsMenu::UI_PowerupSettingsMenu() : UI_Menu()
{
    miStoredPowerupDelayField = new MI_SelectField(&rm->spr_selectfield, 70, 100, "Item Use Speed", 500, 220);
    miStoredPowerupDelayField->Add("Very Slow", 2, "", false, false);
    miStoredPowerupDelayField->Add("Slow", 3, "", false, false);
    miStoredPowerupDelayField->Add("Moderate", 4, "", false, false);
    miStoredPowerupDelayField->Add("Fast", 5, "", false, false);
    miStoredPowerupDelayField->Add("Very Fast", 6, "", false, false);
    miStoredPowerupDelayField->SetData(&game_values.storedpowerupdelay, NULL, NULL);
    miStoredPowerupDelayField->SetKey(game_values.storedpowerupdelay);

    miItemRespawnField = new MI_SelectField(&rm->spr_selectfield, 70, 140, "Item Spawn", 500, 220);
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

    miHiddenBlockRespawnField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Hidden Block Hide", 500, 220);
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

    miSwapStyleField = new MI_SelectField(&rm->spr_selectfield, 70, 220, "Swap Style", 500, 220);
    miSwapStyleField->Add("Walk", 0, "", false, false);
    miSwapStyleField->Add("Blink", 1, "", false, false);
    miSwapStyleField->Add("Instant", 2, "", false, false);
    miSwapStyleField->SetData(&game_values.swapstyle, NULL, NULL);
    miSwapStyleField->SetKey(game_values.swapstyle);

    miBonusWheelField = new MI_SelectField(&rm->spr_selectfield, 70, 260, "Bonus Wheel", 500, 220);
    miBonusWheelField->Add("Off", 0, "", false, false);
    miBonusWheelField->Add("Tournament Win", 1, "", false, false);
    miBonusWheelField->Add("Every Game", 2, "", false, false);
    miBonusWheelField->SetData(&game_values.bonuswheel, NULL, NULL);
    miBonusWheelField->SetKey(game_values.bonuswheel);

    miKeepPowerupField = new MI_SelectField(&rm->spr_selectfield, 70, 300, "Bonus Item", 500, 220);
    miKeepPowerupField->Add("Until Next Spin", 0, "", false, false);
    miKeepPowerupField->Add("Keep Always", 1, "", true, false);
    miKeepPowerupField->SetData(NULL, NULL, &game_values.keeppowerup);
    miKeepPowerupField->SetKey(game_values.keeppowerup ? 1 : 0);
    miKeepPowerupField->SetAutoAdvance(true);

    miStoredPowerupResetButton = new MI_StoredPowerupResetButton(&rm->spr_selectfield, 70, 340, "Reset Stored Items", 500, 0);
    miStoredPowerupResetButton->SetCode(MENU_CODE_RESET_STORED_POWERUPS);

    miPowerupSettingsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, 1);
    miPowerupSettingsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miPowerupSettingsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miPowerupSettingsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miPowerupSettingsMenuHeaderText = new MI_Text("Item Settings Menu", 320, 5, 0, 2, 1);

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

UI_PowerupSettingsMenu::~UI_PowerupSettingsMenu() {
}
