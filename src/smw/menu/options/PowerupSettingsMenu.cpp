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
    miStoredPowerupDelayField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, "Item Use Speed", 500, 220);
    miStoredPowerupDelayField->add("Very Slow", 2);
    miStoredPowerupDelayField->add("Slow", 3);
    miStoredPowerupDelayField->add("Moderate", 4);
    miStoredPowerupDelayField->add("Fast", 5);
    miStoredPowerupDelayField->add("Very Fast", 6);
    miStoredPowerupDelayField->setOutputPtr(&game_values.storedpowerupdelay);
    miStoredPowerupDelayField->setCurrentValue(game_values.storedpowerupdelay);

    miItemRespawnField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 140, "Item Spawn", 500, 220);
    miItemRespawnField->add("Off", 0);
    miItemRespawnField->add("5 Seconds", 310);
    miItemRespawnField->add("10 Seconds", 620);
    miItemRespawnField->add("15 Seconds", 930);
    miItemRespawnField->add("20 Seconds", 1240);
    miItemRespawnField->add("25 Seconds", 1550);
    miItemRespawnField->add("30 Seconds", 1860);
    miItemRespawnField->add("35 Seconds", 2170);
    miItemRespawnField->add("40 Seconds", 2480);
    miItemRespawnField->add("45 Seconds", 2790);
    miItemRespawnField->add("50 Seconds", 3100);
    miItemRespawnField->add("55 Seconds", 3410);
    miItemRespawnField->add("60 Seconds", 3720);
    miItemRespawnField->setOutputPtr(&game_values.itemrespawntime);
    miItemRespawnField->setCurrentValue(game_values.itemrespawntime);

    miHiddenBlockRespawnField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 180, "Hidden Block Hide", 500, 220);
    miHiddenBlockRespawnField->add("Off", 0);
    miHiddenBlockRespawnField->add("5 Seconds", 310);
    miHiddenBlockRespawnField->add("10 Seconds", 620);
    miHiddenBlockRespawnField->add("15 Seconds", 930);
    miHiddenBlockRespawnField->add("20 Seconds", 1240);
    miHiddenBlockRespawnField->add("25 Seconds", 1550);
    miHiddenBlockRespawnField->add("30 Seconds", 1860);
    miHiddenBlockRespawnField->add("35 Seconds", 2170);
    miHiddenBlockRespawnField->add("40 Seconds", 2480);
    miHiddenBlockRespawnField->add("45 Seconds", 2790);
    miHiddenBlockRespawnField->add("50 Seconds", 3100);
    miHiddenBlockRespawnField->add("55 Seconds", 3410);
    miHiddenBlockRespawnField->add("60 Seconds", 3720);
    miHiddenBlockRespawnField->setOutputPtr(&game_values.hiddenblockrespawn);
    miHiddenBlockRespawnField->setCurrentValue(game_values.hiddenblockrespawn);

    miSwapStyleField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 220, "Swap Style", 500, 220);
    miSwapStyleField->add("Walk", 0);
    miSwapStyleField->add("Blink", 1);
    miSwapStyleField->add("Instant", 2);
    miSwapStyleField->setOutputPtr(&game_values.swapstyle);
    miSwapStyleField->setCurrentValue(game_values.swapstyle);

    miBonusWheelField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 260, "Bonus Wheel", 500, 220);
    miBonusWheelField->add("Off", 0);
    miBonusWheelField->add("Tournament Win", 1);
    miBonusWheelField->add("Every Game", 2);
    miBonusWheelField->setOutputPtr(&game_values.bonuswheel);
    miBonusWheelField->setCurrentValue(game_values.bonuswheel);

    miKeepPowerupField = new MI_SelectField<bool>(&rm->spr_selectfield, 70, 300, "Bonus Item", 500, 220);
    miKeepPowerupField->add("Until Next Spin", false);
    miKeepPowerupField->add("Keep Always", true);
    miKeepPowerupField->setOutputPtr(&game_values.keeppowerup);
    miKeepPowerupField->setCurrentValue(game_values.keeppowerup ? 1 : 0);
    miKeepPowerupField->setAutoAdvance(true);

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

    setInitialFocus(miStoredPowerupDelayField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};
