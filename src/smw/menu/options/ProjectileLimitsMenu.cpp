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
    miFireballLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 60, "Fireball Limit", 500, 220);
    miFireballLimitField->Add("Unlimited", 0);
    miFireballLimitField->Add("2", 2);
    miFireballLimitField->Add("5", 5);
    miFireballLimitField->Add("8", 8);
    miFireballLimitField->Add("10", 10);
    miFireballLimitField->Add("12", 12);
    miFireballLimitField->Add("15", 15);
    miFireballLimitField->Add("20", 20);
    miFireballLimitField->Add("25", 25);
    miFireballLimitField->Add("30", 30);
    miFireballLimitField->Add("40", 40);
    miFireballLimitField->Add("50", 50);
    miFireballLimitField->SetData(&game_values.fireballlimit, NULL, NULL);
    miFireballLimitField->SetKey(game_values.fireballlimit);

    miHammerLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 100, "Hammer Limit", 500, 220);
    miHammerLimitField->Add("Unlimited", 0);
    miHammerLimitField->Add("2", 2);
    miHammerLimitField->Add("5", 5);
    miHammerLimitField->Add("8", 8);
    miHammerLimitField->Add("10", 10);
    miHammerLimitField->Add("12", 12);
    miHammerLimitField->Add("15", 15);
    miHammerLimitField->Add("20", 20);
    miHammerLimitField->Add("25", 25);
    miHammerLimitField->Add("30", 30);
    miHammerLimitField->Add("40", 40);
    miHammerLimitField->Add("50", 50);
    miHammerLimitField->SetData(&game_values.hammerlimit, NULL, NULL);
    miHammerLimitField->SetKey(game_values.hammerlimit);

    miBoomerangLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 140, "Boomerang Limit", 500, 220);
    miBoomerangLimitField->Add("Unlimited", 0);
    miBoomerangLimitField->Add("2", 2);
    miBoomerangLimitField->Add("5", 5);
    miBoomerangLimitField->Add("8", 8);
    miBoomerangLimitField->Add("10", 10);
    miBoomerangLimitField->Add("12", 12);
    miBoomerangLimitField->Add("15", 15);
    miBoomerangLimitField->Add("20", 20);
    miBoomerangLimitField->Add("25", 25);
    miBoomerangLimitField->Add("30", 30);
    miBoomerangLimitField->Add("40", 40);
    miBoomerangLimitField->Add("50", 50);
    miBoomerangLimitField->SetData(&game_values.boomeranglimit, NULL, NULL);
    miBoomerangLimitField->SetKey(game_values.boomeranglimit);

    miFeatherLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 180, "Feather Limit", 500, 220);
    miFeatherLimitField->Add("Unlimited", 0);
    miFeatherLimitField->Add("2", 2);
    miFeatherLimitField->Add("5", 5);
    miFeatherLimitField->Add("8", 8);
    miFeatherLimitField->Add("10", 10);
    miFeatherLimitField->Add("12", 12);
    miFeatherLimitField->Add("15", 15);
    miFeatherLimitField->Add("20", 20);
    miFeatherLimitField->Add("25", 25);
    miFeatherLimitField->Add("30", 30);
    miFeatherLimitField->Add("40", 40);
    miFeatherLimitField->Add("50", 50);
    miFeatherLimitField->SetData(&game_values.featherlimit, NULL, NULL);
    miFeatherLimitField->SetKey(game_values.featherlimit);

    miLeafLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 220, "Leaf Limit", 500, 220);
    miLeafLimitField->Add("Unlimited", 0);
    miLeafLimitField->Add("2", 2);
    miLeafLimitField->Add("5", 5);
    miLeafLimitField->Add("8", 8);
    miLeafLimitField->Add("10", 10);
    miLeafLimitField->Add("12", 12);
    miLeafLimitField->Add("15", 15);
    miLeafLimitField->Add("20", 20);
    miLeafLimitField->Add("25", 25);
    miLeafLimitField->Add("30", 30);
    miLeafLimitField->Add("40", 40);
    miLeafLimitField->Add("50", 50);
    miLeafLimitField->SetData(&game_values.leaflimit, NULL, NULL);
    miLeafLimitField->SetKey(game_values.leaflimit);

    miPwingsLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 260, "P-Wings Limit", 500, 220);
    miPwingsLimitField->Add("Unlimited", 0);
    miPwingsLimitField->Add("2", 2);
    miPwingsLimitField->Add("5", 5);
    miPwingsLimitField->Add("8", 8);
    miPwingsLimitField->Add("10", 10);
    miPwingsLimitField->Add("12", 12);
    miPwingsLimitField->Add("15", 15);
    miPwingsLimitField->Add("20", 20);
    miPwingsLimitField->Add("25", 25);
    miPwingsLimitField->Add("30", 30);
    miPwingsLimitField->Add("40", 40);
    miPwingsLimitField->Add("50", 50);
    miPwingsLimitField->SetData(&game_values.pwingslimit, NULL, NULL);
    miPwingsLimitField->SetKey(game_values.pwingslimit);

    miTanookiLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 300, "Tanooki Limit", 500, 220);
    miTanookiLimitField->Add("Unlimited", 0);
    miTanookiLimitField->Add("2", 2);
    miTanookiLimitField->Add("5", 5);
    miTanookiLimitField->Add("8", 8);
    miTanookiLimitField->Add("10", 10);
    miTanookiLimitField->Add("12", 12);
    miTanookiLimitField->Add("15", 15);
    miTanookiLimitField->Add("20", 20);
    miTanookiLimitField->Add("25", 25);
    miTanookiLimitField->Add("30", 30);
    miTanookiLimitField->Add("40", 40);
    miTanookiLimitField->Add("50", 50);
    miTanookiLimitField->SetData(&game_values.tanookilimit, NULL, NULL);
    miTanookiLimitField->SetKey(game_values.tanookilimit);

    miBombLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 340, "Bomb Limit", 500, 220);
    miBombLimitField->Add("Unlimited", 0);
    miBombLimitField->Add("2", 2);
    miBombLimitField->Add("5", 5);
    miBombLimitField->Add("8", 8);
    miBombLimitField->Add("10", 10);
    miBombLimitField->Add("12", 12);
    miBombLimitField->Add("15", 15);
    miBombLimitField->Add("20", 20);
    miBombLimitField->Add("25", 25);
    miBombLimitField->Add("30", 30);
    miBombLimitField->Add("40", 40);
    miBombLimitField->Add("50", 50);
    miBombLimitField->SetData(&game_values.bombslimit, NULL, NULL);
    miBombLimitField->SetKey(game_values.bombslimit);

    miWandLimitField = new MI_SelectField(&rm->spr_selectfield, 70, 380, "Wand Limit", 500, 220);
    miWandLimitField->Add("Unlimited", 0);
    miWandLimitField->Add("2", 2);
    miWandLimitField->Add("5", 5);
    miWandLimitField->Add("8", 8);
    miWandLimitField->Add("10", 10);
    miWandLimitField->Add("12", 12);
    miWandLimitField->Add("15", 15);
    miWandLimitField->Add("20", 20);
    miWandLimitField->Add("25", 25);
    miWandLimitField->Add("30", 30);
    miWandLimitField->Add("40", 40);
    miWandLimitField->Add("50", 50);
    miWandLimitField->SetData(&game_values.wandlimit, NULL, NULL);
    miWandLimitField->SetKey(game_values.wandlimit);

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
