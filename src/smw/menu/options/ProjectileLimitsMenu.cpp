#include "ProjectileLimitsMenu.h"

#include "GameValues.h"
#include "ResourceManager.h"
#include "ui/MI_Button.h"
#include "ui/MI_Image.h"
#include "ui/MI_SelectField.h"
#include "ui/MI_Text.h"

extern CResourceManager* rm;
extern CGameValues game_values;


UI_ProjectileLimitsMenu::UI_ProjectileLimitsMenu()
    : UI_Menu()
{
    miFireballLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 60, "Fireball Limit", 500, 220);
    miFireballLimitField->add("Unlimited", 0);
    miFireballLimitField->add("2", 2);
    miFireballLimitField->add("5", 5);
    miFireballLimitField->add("8", 8);
    miFireballLimitField->add("10", 10);
    miFireballLimitField->add("12", 12);
    miFireballLimitField->add("15", 15);
    miFireballLimitField->add("20", 20);
    miFireballLimitField->add("25", 25);
    miFireballLimitField->add("30", 30);
    miFireballLimitField->add("40", 40);
    miFireballLimitField->add("50", 50);
    miFireballLimitField->setOutputPtr(&game_values.fireballlimit);
    miFireballLimitField->setCurrentValue(game_values.fireballlimit);

    miHammerLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 100, "Hammer Limit", 500, 220);
    miHammerLimitField->add("Unlimited", 0);
    miHammerLimitField->add("2", 2);
    miHammerLimitField->add("5", 5);
    miHammerLimitField->add("8", 8);
    miHammerLimitField->add("10", 10);
    miHammerLimitField->add("12", 12);
    miHammerLimitField->add("15", 15);
    miHammerLimitField->add("20", 20);
    miHammerLimitField->add("25", 25);
    miHammerLimitField->add("30", 30);
    miHammerLimitField->add("40", 40);
    miHammerLimitField->add("50", 50);
    miHammerLimitField->setOutputPtr(&game_values.hammerlimit);
    miHammerLimitField->setCurrentValue(game_values.hammerlimit);

    miBoomerangLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 140, "Boomerang Limit", 500, 220);
    miBoomerangLimitField->add("Unlimited", 0);
    miBoomerangLimitField->add("2", 2);
    miBoomerangLimitField->add("5", 5);
    miBoomerangLimitField->add("8", 8);
    miBoomerangLimitField->add("10", 10);
    miBoomerangLimitField->add("12", 12);
    miBoomerangLimitField->add("15", 15);
    miBoomerangLimitField->add("20", 20);
    miBoomerangLimitField->add("25", 25);
    miBoomerangLimitField->add("30", 30);
    miBoomerangLimitField->add("40", 40);
    miBoomerangLimitField->add("50", 50);
    miBoomerangLimitField->setOutputPtr(&game_values.boomeranglimit);
    miBoomerangLimitField->setCurrentValue(game_values.boomeranglimit);

    miFeatherLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 180, "Feather Limit", 500, 220);
    miFeatherLimitField->add("Unlimited", 0);
    miFeatherLimitField->add("2", 2);
    miFeatherLimitField->add("5", 5);
    miFeatherLimitField->add("8", 8);
    miFeatherLimitField->add("10", 10);
    miFeatherLimitField->add("12", 12);
    miFeatherLimitField->add("15", 15);
    miFeatherLimitField->add("20", 20);
    miFeatherLimitField->add("25", 25);
    miFeatherLimitField->add("30", 30);
    miFeatherLimitField->add("40", 40);
    miFeatherLimitField->add("50", 50);
    miFeatherLimitField->setOutputPtr(&game_values.featherlimit);
    miFeatherLimitField->setCurrentValue(game_values.featherlimit);

    miLeafLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 220, "Leaf Limit", 500, 220);
    miLeafLimitField->add("Unlimited", 0);
    miLeafLimitField->add("2", 2);
    miLeafLimitField->add("5", 5);
    miLeafLimitField->add("8", 8);
    miLeafLimitField->add("10", 10);
    miLeafLimitField->add("12", 12);
    miLeafLimitField->add("15", 15);
    miLeafLimitField->add("20", 20);
    miLeafLimitField->add("25", 25);
    miLeafLimitField->add("30", 30);
    miLeafLimitField->add("40", 40);
    miLeafLimitField->add("50", 50);
    miLeafLimitField->setOutputPtr(&game_values.leaflimit);
    miLeafLimitField->setCurrentValue(game_values.leaflimit);

    miPwingsLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 260, "P-Wings Limit", 500, 220);
    miPwingsLimitField->add("Unlimited", 0);
    miPwingsLimitField->add("2", 2);
    miPwingsLimitField->add("5", 5);
    miPwingsLimitField->add("8", 8);
    miPwingsLimitField->add("10", 10);
    miPwingsLimitField->add("12", 12);
    miPwingsLimitField->add("15", 15);
    miPwingsLimitField->add("20", 20);
    miPwingsLimitField->add("25", 25);
    miPwingsLimitField->add("30", 30);
    miPwingsLimitField->add("40", 40);
    miPwingsLimitField->add("50", 50);
    miPwingsLimitField->setOutputPtr(&game_values.pwingslimit);
    miPwingsLimitField->setCurrentValue(game_values.pwingslimit);

    miTanookiLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 300, "Tanooki Limit", 500, 220);
    miTanookiLimitField->add("Unlimited", 0);
    miTanookiLimitField->add("2", 2);
    miTanookiLimitField->add("5", 5);
    miTanookiLimitField->add("8", 8);
    miTanookiLimitField->add("10", 10);
    miTanookiLimitField->add("12", 12);
    miTanookiLimitField->add("15", 15);
    miTanookiLimitField->add("20", 20);
    miTanookiLimitField->add("25", 25);
    miTanookiLimitField->add("30", 30);
    miTanookiLimitField->add("40", 40);
    miTanookiLimitField->add("50", 50);
    miTanookiLimitField->setOutputPtr(&game_values.tanookilimit);
    miTanookiLimitField->setCurrentValue(game_values.tanookilimit);

    miBombLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 340, "Bomb Limit", 500, 220);
    miBombLimitField->add("Unlimited", 0);
    miBombLimitField->add("2", 2);
    miBombLimitField->add("5", 5);
    miBombLimitField->add("8", 8);
    miBombLimitField->add("10", 10);
    miBombLimitField->add("12", 12);
    miBombLimitField->add("15", 15);
    miBombLimitField->add("20", 20);
    miBombLimitField->add("25", 25);
    miBombLimitField->add("30", 30);
    miBombLimitField->add("40", 40);
    miBombLimitField->add("50", 50);
    miBombLimitField->setOutputPtr(&game_values.bombslimit);
    miBombLimitField->setCurrentValue(game_values.bombslimit);

    miWandLimitField = new MI_SelectField<short>(&rm->spr_selectfield, 70, 380, "Wand Limit", 500, 220);
    miWandLimitField->add("Unlimited", 0);
    miWandLimitField->add("2", 2);
    miWandLimitField->add("5", 5);
    miWandLimitField->add("8", 8);
    miWandLimitField->add("10", 10);
    miWandLimitField->add("12", 12);
    miWandLimitField->add("15", 15);
    miWandLimitField->add("20", 20);
    miWandLimitField->add("25", 25);
    miWandLimitField->add("30", 30);
    miWandLimitField->add("40", 40);
    miWandLimitField->add("50", 50);
    miWandLimitField->setOutputPtr(&game_values.wandlimit);
    miWandLimitField->setCurrentValue(game_values.wandlimit);

    miProjectilesLimitsMenuBackButton = new MI_Button(&rm->spr_selectfield, 544, 432, "Back", 80, TextAlign::CENTER);
    miProjectilesLimitsMenuBackButton->SetCode(MENU_CODE_BACK_TO_OPTIONS_MENU);

    miProjectilesLimitsMenuLeftHeaderBar = new MI_Image(&rm->menu_plain_field, 0, 0, 0, 0, 320, 32, 1, 1, 0);
    miProjectilesLimitsMenuRightHeaderBar = new MI_Image(&rm->menu_plain_field, 320, 0, 192, 0, 320, 32, 1, 1, 0);
    miProjectilesLimitsMenuHeaderText = new MI_HeaderText("Weapon Use Limits Menu", 320, 5);

    AddControl(miFireballLimitField, miProjectilesLimitsMenuBackButton, miHammerLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miHammerLimitField, miFireballLimitField, miBoomerangLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miBoomerangLimitField, miHammerLimitField, miFeatherLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miFeatherLimitField, miBoomerangLimitField, miLeafLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miLeafLimitField, miFeatherLimitField, miPwingsLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miPwingsLimitField, miLeafLimitField, miTanookiLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miTanookiLimitField, miPwingsLimitField, miBombLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miBombLimitField, miTanookiLimitField, miWandLimitField, NULL, miProjectilesLimitsMenuBackButton);
    AddControl(miWandLimitField, miBombLimitField, miProjectilesLimitsMenuBackButton, NULL, miProjectilesLimitsMenuBackButton);

    AddControl(miProjectilesLimitsMenuBackButton, miWandLimitField, miFireballLimitField, miWandLimitField, NULL);

    AddNonControl(miProjectilesLimitsMenuLeftHeaderBar);
    AddNonControl(miProjectilesLimitsMenuRightHeaderBar);
    AddNonControl(miProjectilesLimitsMenuHeaderText);

    SetHeadControl(miFireballLimitField);
    SetCancelCode(MENU_CODE_BACK_TO_OPTIONS_MENU);
};
